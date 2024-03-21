#include "Core.h"
#include "../GameTimer/GameTimer.h"
#include "../Database/Database.h"
#include "../Trade/Trade.h"
#include "../../../Client/Code/Common/Utility.h"

INIT_INSTACNE(Core)
Core::Core()
	: mIOCP(nullptr), mIsRun(false)
{
	mListenSocket = 0;
	_wsetlocale(LC_ALL, L"korean");
	// 에러발생시 한글로 출력되도록 명령
	std::wcout.imbue(std::locale("korean"));

	//_CrtSetBreakAlloc(727);

	mUsers = nullptr;
	mTrades = nullptr;
}

Core::~Core()
{
	std::cout << "Core 소멸자" << std::endl;
	mIOCP = nullptr;

	GET_INSTANCE(GameTimer)->Release();
	GET_INSTANCE(Database)->Release();

	if (mAcceptThread.joinable() == true)
	{
		mAcceptThread.join();
	}

	for (auto& th : mWorkerThreads)
	{
		if (th.joinable() == true)
		{
			th.join();
		}
	}

	for (auto& data : mEventDatas)
	{
		delete data.second;
	}
	mEventDatas.clear();
	mSendDatas.clear();

	delete[] mUsers;
	delete[] mTrades;

	WSACleanup();
}

void Core::errorDisplay(const char* msg, int error)
{
	WCHAR* lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	std::cout << msg;
	std::wcout << L"설명 : " << lpMsgBuf << std::endl;

	// 에러발생시 무한루프로 멈추게함
	while (true);

	LocalFree(lpMsgBuf);
}

void Core::acceptClient()
{
	// Winsock Start - windock.dll 로드
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "Error - Can not load 'winsock.dll' file" << std::endl;
		return;
	}

	mListenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mListenSocket == INVALID_SOCKET)
	{
		std::cout << "Error - Invalid Socket" << std::endl;
		WSACleanup();
		return;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	if (::bind(mListenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		std::cout << "Error - Fail bind" << std::endl;
		closesocket(mListenSocket);
		WSACleanup();
		return;
	}

	// 3. 수신대기열생성
	if (listen(mListenSocket, 5) == SOCKET_ERROR)
	{
		std::cout << "Error - Fail listen" << std::endl;
		closesocket(mListenSocket);
		WSACleanup();
		return;
	}

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;

	while (true)
	{
		clientSocket = accept(mListenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			if (error == 10004)
			{
				std::cout << "AcceptThread is quit" << std::endl;
				break;
			}

			std::cout << "Error - Accept Failure" << std::endl;
			break;
		}

		// 빈 아이디를 생성해줌
		int id = createPlayerId();
		mUsers[id].SetSocket(clientSocket);
		mUsers[id].PlayerConnect();
		//std::cout << "Accept - " << id << "번 Client" << std::endl;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), mIOCP, id, 0);
		recvPacket(id);
	}
}

void Core::threadPool()
{
	while (mIsRun == true)
	{
		DWORD ioByte;
		unsigned long long id;
		Over* over;
		// lpover에 recv인지 send인지 정보를 넣어야 됨.
		BOOL result = GetQueuedCompletionStatus(mIOCP, &ioByte, &id, reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);

		//  GetQueuedCompletionStatus( )가 에러인지 아닌지 확인한다
		if (result == false)
		{
			int errorNum = WSAGetLastError();
			if (errorNum == 64)
			{
				disconnect(static_cast<int>(id));
				continue;
			}

			else if (errorNum == 735)
			{
				break;
			}

			else
			{
				errorDisplay(" GetQueuedCompletionStatus()", errorNum);
			}
		}

		// 클라와 끊어졌다면 (클라가 나갔을 때)
		if (ioByte == 0)
		{
			disconnect(static_cast<int>(id));
			continue;
		}

		switch (over->eventType)
		{
			case SERVER_EVENT::RECV:
			{
				if (id >= MONSTER_START_ID)
				{
					break;
				}

				OverEx* overEx = static_cast<OverEx*>(over);
				// 남은 패킷 크기(들어온 패킷 크기)
				int restSize = ioByte;
				char* p = overEx->messageBuffer;
				int packetSize = 0;

				if (mUsers[id].GetPrevSize() > 0)
				{
					packetSize = mUsers[id].GetPacketBuf()[0];
				}

				while (restSize > 0)
				{
					// 전에 남아있던 패킷이 없었다면, 현재 들어온 패킷 크기를 저장
					if (packetSize == 0)
					{
						packetSize = p[0];
					}

					// 패킷을 만들기 위한 크기?
					int required = packetSize - mUsers[id].GetPrevSize();
					// 패킷을 만들 수 있다면, (현재 들어온 패킷의 크기가 required 보다 크면)
					if (required >= 0 && restSize >= required)
					{
						mUsers[id].SetPacketBuf(p, required);
						// 패킷 처리
						processPacket(static_cast<int>(id), mUsers[id].GetPacketBuf());

						restSize -= required;
						p += required;
						packetSize = 0;

						mUsers[id].SetPrevSize(0);
					}
					// 패킷을 만들 수 없다면,
					else
					{
						// 현재 들어온 패킷의 크기만큼, 현재 들어온 패킷을 저장시킨다.
						mUsers[id].SetPacketBuf(p, restSize);
						mUsers[id].SetPrevSize(restSize);
						restSize = 0;
					}
				}
				recvPacket(static_cast<int>(id));
				break;
			}

			case SERVER_EVENT::SEND:
			{
				//delete over;
				//over = nullptr;
				//popSendData(over);
				mSendDatas.erase(over->key);
				break;
			}

			default:
			{
				processEvent(over);
				break;
			}
		}
	}
}

void Core::disconnect(int id)
{
	mUsers[id].PlayerDisconnect();
	mTrades[id].Reset();
	std::cout << id << "번 클라이언트 연결 끊어짐" << std::endl;
}

void Core::recvPacket(int id)
{
	DWORD flag = 0;
	SOCKET socket = mUsers[id].GetSocket();

	OverEx* over = static_cast<OverEx*>(mUsers[id].GetOver());	
	// WSASend(응답에 대한)의 콜백일 경우
	over->dataBuffer.len = MAX_BUFFER;
	over->dataBuffer.buf = over->messageBuffer;
	memset(&(over->overlapped), 0, sizeof(WSAOVERLAPPED));
	over->eventType = SERVER_EVENT::RECV;

	if (WSARecv(socket, &over->dataBuffer, 1, nullptr, &flag, &(over->overlapped), nullptr) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		if (error != WSA_IO_PENDING)
		{
			errorDisplay("WSARecv() Error - ", error);
		}
	}
}

void Core::sendPacket(int id, char* packet)
{
	SOCKET socket = mUsers[id].GetSocket();
	//OverEx* over = new OverEx;
	std::shared_ptr<OverEx> over = std::make_shared<OverEx>();

	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = packet[0];

	// 패킷의 내용을 버퍼에 복사
	memcpy(over->messageBuffer, packet, packet[0]);
	ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->eventType = SERVER_EVENT::SEND;

	void* key = &over;
	over->key = key;
	{
		tbb::concurrent_hash_map<void*, std::shared_ptr<OverEx>>::accessor acc;
		mSendDatas.insert(acc, key);
		acc->second = over;
		acc.release();
	}

	if (WSASend(socket, &over->dataBuffer, 1, nullptr, 0, &(over->overlapped), nullptr) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "Error - Fail WSASend(error_code : " << WSAGetLastError() << std::endl;
		}
	}
}

void Core::processPacket(int id, char* buf)
{
	//auto start = std::chrono::high_resolution_clock::now();
	switch (buf[1])
	{
		case CS_PACKET_TYPE::CS_SERVER_SELECT:
		{
			CSServerSelectPacket* packet = reinterpret_cast<CSServerSelectPacket*>(buf);
			packet->type;			
			short channelUserSize[MAX_CHANNEL] = { 0. };
			for (int i = 0; i < MAX_CHANNEL; ++i)
			{
				channelUserSize[i] = mChannels[i].GetChannelUserSize();
			}
			SendServerSelectPacket(id, channelUserSize);
			break;
		}

		case CS_PACKET_TYPE::CS_LOGIN:
		{
			CSLoginPacket* packet = reinterpret_cast<CSLoginPacket*>(buf);
			// 아이디, 비번 비교
			SendLoginOkPacket(id);
			break;
		}

		case CS_PACKET_TYPE::CS_DUMMY_LOGIN:
		{
			int channel = FindChannel();
			int channelIndex = mChannels[channel].PushUser(id);
			mUsers[id].SetChannelIndex(channelIndex);
			mUsers[id].SetChannel(channel);

			int x = mUsers[id].GetX();
			int y = mUsers[id].GetY();

			// 섹터 찾아서 넣기
			mChannels[channel].PushSectorObject(x, y, id);
			mUsers[id].ProcessLoginViewList();

			SendDummyLoginPacket(id, channel);
			std::cout << id << " dummy client is " << channel << " channel login ok" << std::endl;
			break;
		}

		case CS_PACKET_TYPE::CS_CHANNEL_LOGIN:
		{
			CSChannelLoginPacket* packet = reinterpret_cast<CSChannelLoginPacket*>(buf);
			// 채널 먼저 접속
			int channel = packet->channel;
			if (mChannels[channel].IsFull() == false)
			{
				int channelIndex = mChannels[channel].PushUser(id);
				mUsers[id].SetChannelIndex(channelIndex);
				mUsers[id].SetChannel(channel);

				SendChannelLoginPacket(id, channel);
				std::cout << id << " client is " << channel << " channel login ok" << std::endl;

				int x = mUsers[id].GetX();
				int y = mUsers[id].GetY();

				x = 0;
				y = 0;
				mUsers[id].SetPosition(x, y);

				// 섹터 찾아서 넣기
				mChannels[channel].PushSectorObject(x, y, id);
				mUsers[id].ProcessLoginViewList();
			}
			else
			{
				std::cout << id << " client is " << channel << " channel login fail" << std::endl;
				SendChannelLoginPacket(id, -1);
			}
			break;
		}

		case CS_PACKET_TYPE::CS_MOVE:
		{
			CSMovePacket* packet = reinterpret_cast<CSMovePacket*>(buf);
			mUsers[id].ProcessMove(packet->direction);
		
			mUsers[id].CheckViewList();
			SendPositionPacket(id, id);
			break;
		}

		case CS_PACKET_TYPE::CS_CHANGE_CHANNEL:
		{
			CSChangeChannelPacket* packet = reinterpret_cast<CSChangeChannelPacket*>(buf);
			bool result = false;

			int oldChannel = mUsers[id].GetChannel();
			int newChannel = packet->channel;

			// 기존 채널과 새로운 채널이 같은지 먼저 확인
			if (oldChannel == newChannel)
			{
				std::cout << id << " client change channel fail : " << oldChannel << " --> " << newChannel << std::endl;
				SendChangeChannelPacket(id, result, oldChannel);
				break;
			}
			// 새로운 채널이 범위를 벗어났는지 확인
			if (newChannel >= MAX_CHANNEL)
			{
				std::cout << id << " client change channel fail : 최대 채널 초과" << std::endl;
				SendChangeChannelPacket(id, result, oldChannel);
				break;
			}
			// 채널의 유저가 가득 찼는지 확인
			if (mChannels[newChannel].IsFull() == false)
			{
				int x = mUsers[id].GetX();
				int y = mUsers[id].GetY();
				int oldChannelIndex = mUsers[id].GetChannelIndex();

				// 플레이어 기존 채널, 섹터에서 제거
				mChannels[oldChannel].PopUser(oldChannelIndex);
				mChannels[oldChannel].PopSectorObject(x, y, id);
				// 새로운 채널, 섹터로 추가
				int newChannelIndex = mChannels[newChannel].PushUser(id);
				mChannels[newChannel].PushSectorObject(x, y, id);

				mUsers[id].SetChannelIndex(newChannelIndex);
				mUsers[id].SetChannel(newChannel);
				// 채널이동시 기존 채널의 오브젝트와 새로운 채널 오브젝트들의 뷰리스트들을 처리한다.
				mUsers[id].ProcessChangeChannelViewList(oldChannel, newChannel);

				result = true;
				std::cout << id << " client change channel ok: " << oldChannel << " --> " << newChannel << std::endl;
				SendChangeChannelPacket(id, result, newChannel);
			}
			else
			{
				std::cout << id << " client change channel fail : " << oldChannel << " --> " << newChannel << std::endl;
				std::cout << newChannel << " channel is full" << std::endl;
				SendChangeChannelPacket(id, result, oldChannel);
			}
			break;
		}

		case CS_PACKET_TYPE::CS_BROADCASTING_CHAT:
		{
			CSBroadcastingChatPacket* packet = reinterpret_cast<CSBroadcastingChatPacket*>(buf);
			SendChatPacket(id, id, packet->chat);
			// 주변 사람들에게 채팅보냄
			mUsers[id].ProcessBroadcastingChat(packet->chat);
			break;
		}

		case CS_PACKET_TYPE::CS_WHISPERING_CHAT:
		{
			CSWhisperingChatPacket* packet = reinterpret_cast<CSWhisperingChatPacket*>(buf);
			SendChatPacket(id, id, packet->chat);
			// 주변 사람들에게 채팅보냄
			mUsers[id].ProcessWhisperingChat(packet->id, packet->chat);
			break;
		}

		case CS_PACKET_TYPE::CS_ATTACK:
		{
			CSAttackPacket* packet = reinterpret_cast<CSAttackPacket*>(buf);
			// 몬스터에게 공격했는지 체크
			mUsers[id].ProcessAttack();
			break;
		}

		case CS_PACKET_TYPE::CS_CHANGE_AVATAR:
		{
			CSChangeAvatarPacket* packet = reinterpret_cast<CSChangeAvatarPacket*>(buf);
			// 주변 다른 유저들에게 아바타 바꼈다고 전송
			mUsers[id].ProcessChangeAvatar(packet->slotType, packet->texId, true);
			break;
		}
		
		case CS_PACKET_TYPE::CS_TAKE_OFF_EQUIP_ITEM:
		{
			CSTakeOffEquipItemPacket* packet = reinterpret_cast<CSTakeOffEquipItemPacket*>(buf);
			// 주변 다른 유저들에게 아바타 바꼈다고 전송
			mUsers[id].ProcessChangeAvatar(packet->slotType, 0, false);
			break;
		}

		case CS_PACKET_TYPE::CS_REQUEST_TRADE:
		{
			CSRequestTradePacket* packet = reinterpret_cast<CSRequestTradePacket*>(buf);
			mTrades[id].StartTrade(packet->id);
			break;
		}

		case CS_PACKET_TYPE::CS_ADD_TRADE_ITEM:
		{
			CSAddTradeItemPacket* packet = reinterpret_cast<CSAddTradeItemPacket*>(buf);
			// 교환 id를 알아야 한다.
			int tradeId = mUsers[id].GetTradeId();
			if (tradeId != -1)
			{
				mTrades[tradeId].AddItem(id, packet->slotNum, packet->texId);
			}
			break;
		}

		case CS_PACKET_TYPE::CS_ADD_TRADE_MESO:
		{
			CSAddTradeMesoPacket* packet = reinterpret_cast<CSAddTradeMesoPacket*>(buf);
			// 교환 id를 알아야 한다.
			int tradeId = mUsers[id].GetTradeId();
			if (tradeId != -1)
			{
				mTrades[tradeId].AddMeso(id, packet->meso);
			}
			break;
		}

		case CS_PACKET_TYPE::CS_TRADE:
		{
			CSTradePacket* packet = reinterpret_cast<CSTradePacket*>(buf);
			int tradeId = mUsers[id].GetTradeId();
			if (tradeId != -1)
			{
				mTrades[tradeId].ProcessTrade(id);
			}
			break;
		}

		case CS_PACKET_TYPE::CS_TRADE_CANCEL:
		{
			CSTradeCancelPacket* packet = reinterpret_cast<CSTradeCancelPacket*>(buf);
			int tradeId = mUsers[id].GetTradeId();
			if (tradeId != -1)
			{
				mTrades[tradeId].ProcessCancel();
			}
			break;
		}

		default:
		{
			break;
		}
	}
	//auto end = std::chrono::high_resolution_clock::now() - start;
	//auto time = std::chrono::duration<double>(end).count();
	//std::cout << "패킷 처리 시간 : " << time << "초" << std::endl;
}

void Core::processEvent(Over* over)
{
	switch (over->eventType)
	{
		case SERVER_EVENT::MONSTER_MOVE:
		{
			int index = mChannels[over->channel].FindSectorObjectIndexById(over->sectorXId, over->sectorYId, over->myId);
			Monster& monster = mChannels[over->channel].FindSectorById(over->sectorXId, over->sectorYId).GetMonster(index);
			monster.SetState(MONSTER_STATE::MOVE);

			char dir = monster.RandomDirection();
			monster.ProcessMove(dir);
			monster.ProcessMoveViewList();

			mEventDatas.erase(over->key);
			delete over;
			break;
		}

		default:
		{
			break;
		}
	}

	//popEventData(over);
}

int Core::createPlayerId() const
{
	// 아이디가 있을 때까지, 루프돌면서 기다리게함
	while (true)
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (mUsers[i].GetIsConnect() == false)
			{
				mUsers[i].PlayerConnect();
				return i;
			}
		}
	}

	return -1;
}

void Core::popSendData(Over* over)
{
	//void* key = over->key;

	//mSendMtx.lock();
	//mSendDatas[key].reset();
	//mSendDatas.erase(key);
	//mSendMtx.unlock();
}

void Core::popEventData(Over* over)
{

}

bool Core::Initialize()
{
	mIsRun = true;

	std::cout << "Core 초기화" << std::endl;

	// IOCP 객체 생성
	mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (mIOCP == nullptr)
	{
		return false;
	}

	for (int i = 0; i < mChannels.size(); ++i)
	{
		if (mChannels[i].Initialize(i) == false)
		{
			return false;
		}
	}

	//int objectId = 0;
	mUsers = new Player[MAX_USER];
	mTrades = new Trade[MAX_USER];
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (mUsers[i].Inititalize(i) == false)
		{
			return false;
		}
		mTrades[i].Initialize(i);
	}

	int workerThreadCount = 6;
	// 워커 스레드 생성
	for (int i = 0; i < workerThreadCount; ++i)
	{
		mWorkerThreads.emplace_back(std::thread{ &Core::threadPool, this });
	}

	// accept 스레드 생성
	mAcceptThread = std::thread{ &Core::acceptClient, this };

	if (GET_INSTANCE(GameTimer)->Initialize() == false)
	{
		std::cout << "GameTimer Initialize Fail!!" << std::endl;
		return false;
	}

	if (GET_INSTANCE(Database)->Initialize() == false)
	{
		std::cout << "Database Initialize Fail!!" << std::endl;
		return false;
	}

	std::cout << "Max Channel Size : " << MAX_CHANNEL << std::endl;
	std::cout << "MAX Monster : " << (MAX_OBJECT - MAX_USER) * MAX_CHANNEL << std::endl;
	std::cout << "MAX User : " << MAX_USER << std::endl;
	std::cout << "Server Initialize OK!!" << std::endl;

	return true;
}

void Core::ServerQuit()
{
	mIsRun = false;

	CloseHandle(mIOCP);

	PostQueuedCompletionStatus(mIOCP, 1, NULL, nullptr);

	closesocket(mListenSocket);

	Release();
}

int Core::FindChannel()
{
	while (true)
	{
		int randomChannel = GetRandomNumber(0, MAX_CHANNEL - 1);
		if (mChannels[randomChannel].IsFull() == false)
		{
			return randomChannel;
		}
	}

	return -1;
}

void Core::SendServerSelectPacket(int to, short* size)
{
	SCServerSelectPacket packet;
	packet.size = sizeof(SCServerSelectPacket);
	packet.type = SC_PACKET_TYPE::SC_SERVER_SELECT;
	memcpy(packet.channelUserSize, size, sizeof(short) * MAX_CHANNEL);
	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendLoginOkPacket(int to)
{
	SCLoginOkPacket packet;
	packet.size = sizeof(SCLoginOkPacket);
	packet.type = SC_PACKET_TYPE::SC_LOGIN_OK;
	packet.id = to;
	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendLoginFailPacket(int to)
{
	SCLoginFailPacket packet;
	packet.size = sizeof(SCLoginFailPacket);
	packet.type = SC_PACKET_TYPE::SC_LOGIN_FAIL;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendDummyLoginPacket(int to, int channel)
{
	SCDummyLoginPacket packet;
	packet.size = sizeof(SCDummyLoginPacket);
	packet.type = SC_PACKET_TYPE::SC_DUMMY_LOGIN;
	packet.id = to;
	packet.channel = channel;
	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendChannelLoginPacket(int to, int channel)
{
	SCChannelLoginPacket packet;
	packet.size = sizeof(SCChannelLoginPacket);
	packet.type = SC_PACKET_TYPE::SC_CHANNEL_LOGIN;
	packet.id = to;
	packet.channel = channel;
	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendPositionPacket(int to, int obj)
{
	int x = mUsers[obj].GetX();
	int y = mUsers[obj].GetY();
	char dir = mUsers[obj].GetDirection();

	SCPositionPacket packet;
	packet.size = sizeof(SCPositionPacket);
	packet.type = SC_PACKET_TYPE::SC_POSITION;
	packet.id = obj;
	packet.x = x;
	packet.y = y;
	packet.dir = dir;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendAddPlayerPacket(int to, int obj, int* texIds)
{
	int x = mUsers[obj].GetX();
	int y = mUsers[obj].GetY();
	char dir = mUsers[obj].GetDirection();
	char animationType = static_cast<char>(mUsers[obj].GetAnimationType());

	SCAddPlayerPacket packet;
	packet.size = sizeof(SCAddPlayerPacket);
	packet.type = SC_PACKET_TYPE::SC_ADD_PLAYER;
	packet.id = obj;
	packet.x = x;
	packet.y = y;
	packet.dir = dir;
	packet.animationType = animationType;
	memcpy(packet.texIds, texIds, sizeof(packet.texIds));

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendAddObjectPacket(int to, int obj)
{
	int x = mUsers[obj].GetX();
	int y = mUsers[obj].GetY();

	SCAddObjectPacket packet;
	packet.size = sizeof(SCAddObjectPacket);
	packet.type = SC_PACKET_TYPE::SC_ADD_OBJECT;
	packet.id = obj;
	packet.x = x;
	packet.y = y;
	packet.texId = 0;
	//packet.texId = mUsers[obj].GetTexId();

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendAddMonsterPacket(int to, int obj, int x, int y)
{
	SCAddObjectPacket packet;
	packet.size = sizeof(SCAddObjectPacket);
	packet.type = SC_PACKET_TYPE::SC_ADD_OBJECT;
	packet.id = obj;
	packet.x = x;
	packet.y = y;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendMonsterPositionPacket(int to, int obj, int x, int y)
{
	SCPositionPacket packet;
	packet.size = sizeof(SCPositionPacket);
	packet.type = SC_PACKET_TYPE::SC_POSITION;
	packet.id = obj;
	packet.x = x;
	packet.y = y;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendRemoveObjectPacket(int to, int obj)
{
	SCRemoveObjectPacket packet;
	packet.size = sizeof(SCRemoveObjectPacket);
	packet.type = SC_PACKET_TYPE::SC_REMOVE_OBJECT;
	packet.id = obj;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendRemoveAllObjectPacket(int to)
{
	SCRemoveAllObjectPacket packet;
	packet.size = sizeof(SCRemoveAllObjectPacket);
	packet.type = SC_PACKET_TYPE::SC_REMOVE_ALL_OBJECT;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendChangeChannelPacket(int to, bool result, int channel)
{
	SCChangeChannelPacket packet;
	packet.size = sizeof(SCChangeChannelPacket);
	packet.type = SC_PACKET_TYPE::SC_CHANGE_CHANNEL;
	packet.result = result;
	packet.channel = channel;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendChatPacket(int to, int obj, wchar_t* chat)
{
	SCChatPacket packet;
	packet.size = sizeof(SCChatPacket);
	packet.type = SC_PACKET_TYPE::SC_CHAT;
	packet.id = obj;
	wcsncpy_s(packet.chat, chat, MAX_CHAT_LENGTH);

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendChangeAvatarPacket(int to, int obj, int slot, int texId)
{
	SCChangeAvatarPacket packet;
	packet.size = sizeof(SCChangeAvatarPacket);
	packet.type = SC_PACKET_TYPE::SC_CHANGE_AVATAR;
	packet.id = obj;
	packet.slotType = slot;
	packet.texId = texId;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendTakeOffEquipItemPacket(int to, int obj, int slot)
{
	SCTakeOffEquipItemPacket packet;
	packet.size = sizeof(SCTakeOffEquipItemPacket);
	packet.type = SC_PACKET_TYPE::SC_TAKE_OFF_EQUIP_ITEM;
	packet.id = obj;
	packet.slotType = slot;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendRequestTradePacket(int to, int obj)
{
	SCRequestTradePacket packet;
	packet.size = sizeof(SCRequestTradePacket);
	packet.type = SC_PACKET_TYPE::SC_REQUEST_TRADE;
	packet.id = obj;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendAddTradeItemPacket(int to, int texId, int slotNum)
{
	SCAddTradeItemPacket packet;
	packet.size = sizeof(SCAddTradeItemPacket);
	packet.type = SC_PACKET_TYPE::SC_ADD_TRADE_ITEM;
	packet.texId = texId;
	packet.slotNum = slotNum;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendAddTradeMesoPacket(int to, long long meso)
{
	SCAddTradeMesoPacket packet;
	packet.size = sizeof(SCAddTradeMesoPacket);
	packet.type = SC_PACKET_TYPE::SC_ADD_TRADE_MESO;
	packet.meso = meso;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendTradePacket(int to, int* items, long long meso)
{
	SCTradePacket packet;
	packet.size = sizeof(SCTradePacket);
	packet.type = SC_PACKET_TYPE::SC_TRADE;
	memcpy(packet.items, items, sizeof(packet.items));
	packet.meso = meso;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}

void Core::SendTradeCancelPacket(int to)
{
	SCTradeCancelPacket packet;
	packet.size = sizeof(SCTradeCancelPacket);
	packet.type = SC_PACKET_TYPE::SC_TRADE_CANCEL;

	sendPacket(to, reinterpret_cast<char*>(&packet));
}