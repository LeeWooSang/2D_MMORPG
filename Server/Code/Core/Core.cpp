#include "Core.h"
#include "../GameTimer/GameTimer.h"
#include "../Database/Database.h"
//#include "../Character/Character.h"


INIT_INSTACNE(Core)
Core::Core()
	: mIOCP(nullptr), mIsRun(false)
{
	mListenSocket = 0;
	_wsetlocale(LC_ALL, L"korean");
	// �����߻��� �ѱ۷� ��µǵ��� ���
	std::wcout.imbue(std::locale("korean"));

	//_CrtSetBreakAlloc(727);
}

Core::~Core()
{
	std::cout << "Core �Ҹ���" << std::endl;
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

	// ���� �۾��� �ڿ��� ����
	while (popLeafWork() == true);

	delete[] mUsers;

	WSACleanup();
}

bool Core::Initialize()
{
	mIsRun = true;

	std::cout << "Core �ʱ�ȭ" << std::endl;

	// IOCP ��ü ����
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
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (mUsers[i].Inititalize(i) == false)
		{
			return false;
		}
	}

	int workerThreadCount = 6;
	// ��Ŀ ������ ����
	for (int i = 0; i < workerThreadCount; ++i)
	{
		mWorkerThreads.emplace_back(std::thread{ &Core::threadPool, this });
	}

	// accept ������ ����
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

void Core::SendPositionPacket(int to, int obj)
{
	int x = mUsers[obj].GetX();
	int y = mUsers[obj].GetY();

	SCPositionPacket packet;
	packet.size = sizeof(SCPositionPacket);
	packet.type = SC_PACKET_TYPE::SC_POSITION;
	packet.id = obj;
	packet.x = x;
	packet.y = y;

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

void Core::SendChangeChannelPacket(int to, bool result)
{
	SCChangeChannelPacket packet;
	packet.size = sizeof(SCChangeChannelPacket);
	packet.type = SC_PACKET_TYPE::SC_CHANGE_CHANNEL;
	packet.result = result;

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
	std::wcout << L"���� : " << lpMsgBuf << std::endl;

	// �����߻��� ���ѷ����� ���߰���
	while (true);

	LocalFree(lpMsgBuf);
}

void Core::acceptClient()
{
	// Winsock Start - windock.dll �ε�
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

	// 2. ���ϼ���
	if (::bind(mListenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		std::cout << "Error - Fail bind" << std::endl;
		closesocket(mListenSocket);
		WSACleanup();
		return;
	}

	// 3. ���Ŵ�⿭����
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

		// �� ���̵� ��������
		int id = createPlayerId();
		mUsers[id].SetSocket(clientSocket);
		mUsers[id].PlayerConnect();
		std::cout << "Accept - " << id << "�� Client" << std::endl;

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
		// lpover�� recv���� send���� ������ �־�� ��.
		BOOL result = GetQueuedCompletionStatus(mIOCP, &ioByte, &id, reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);

		//  GetQueuedCompletionStatus( )�� �������� �ƴ��� Ȯ���Ѵ�
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

		// Ŭ��� �������ٸ� (Ŭ�� ������ ��)
		if (ioByte == 0)
		{
			disconnect(static_cast<int>(id));
			continue;
		}

		switch (over->eventType)
		{
			case SERVER_EVENT::RECV:
			{
				OverEx* overEx = static_cast<OverEx*>(over);
				// ���� ��Ŷ ũ��(���� ��Ŷ ũ��)
				int restSize = ioByte;
				char* p = overEx->messageBuffer;
				char packetSize = 0;

				if (mUsers[id].GetPrevSize() > 0)
				{
					packetSize = mUsers[id].GetPacketBuf()[0];
				}

				while (restSize > 0)
				{
					// ���� �����ִ� ��Ŷ�� �����ٸ�, ���� ���� ��Ŷ ũ�⸦ ����
					if (packetSize == 0)
					{
						packetSize = p[0];
					}

					// ��Ŷ�� ����� ���� ũ��?
					int required = packetSize - mUsers[id].GetPrevSize();
					// ��Ŷ�� ���� �� �ִٸ�, (���� ���� ��Ŷ�� ũ�Ⱑ required ���� ũ��)
					if (restSize >= required)
					{
						mUsers[id].SetPacketBuf(p, required);
						// ��Ŷ ó��
						processPacket(static_cast<int>(id), mUsers[id].GetPacketBuf());

						restSize -= required;
						p += required;
						packetSize = 0;

						mUsers[id].SetPrevSize(0);
					}
					// ��Ŷ�� ���� �� ���ٸ�,
					else
					{
						// ���� ���� ��Ŷ�� ũ�⸸ŭ, ���� ���� ��Ŷ�� �����Ų��.
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
				delete over;
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
	std::cout << id << "�� Ŭ���̾�Ʈ ���� ������" << std::endl;
}

void Core::recvPacket(int id)
{
	DWORD flag = 0;
	SOCKET socket = mUsers[id].GetSocket();

	OverEx* over = static_cast<OverEx*>(mUsers[id].GetOver());	
	// WSASend(���信 ����)�� �ݹ��� ���
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
	OverEx* over = new OverEx;

	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = packet[0];

	// ��Ŷ�� ������ ���ۿ� ����
	memcpy(over->messageBuffer, packet, packet[0]);
	ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->eventType = SERVER_EVENT::SEND;

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
		case CS_PACKET_TYPE::CS_LOGIN:
		{
			CSLoginPacket* packet = reinterpret_cast<CSLoginPacket*>(buf);
			// ���̵�, ��� ��

			// ä�� ���� ����
			int channel = FindChannel();
			int channelIndex = mChannels[channel].PushUser(id);
			mUsers[id].SetChannelIndex(channelIndex);
			mUsers[id].SetChannel(channel);
			SendLoginOkPacket(id);

			mUsers[id].SetPosition(0, 0);

			int x = mUsers[id].GetX();
			int y = mUsers[id].GetY();

			// ���� ã�Ƽ� �ֱ�
			mChannels[channel].PushSectorObject(x, y, id);
			mUsers[id].ProcessLoginViewList();			
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

			volatile int oldChannel = mUsers[id].GetChannel();
			int newChannel = packet->channel;
			std::cout << "oldChannel : " << oldChannel << ", newChannel : " << newChannel << std::endl;
			if (oldChannel == newChannel)
			{
				SendChangeChannelPacket(id, result);
				break;
			}

			if (mChannels[newChannel].IsFull() == false)
			{
				int oldChannelIndex = mUsers[id].GetChannelIndex();
				// ���� ä�ο��� pop
				mChannels[oldChannel].PopUser(oldChannelIndex);
				// ���ο� ä�η� insert
				int newChannelIndex = mChannels[newChannel].PushUser(id);

				mUsers[id].SetChannelIndex(newChannelIndex);
				mUsers[id].SetChannel(newChannel);
				
				mUsers[id].ProcessChangeChannelViewList(oldChannel, newChannel);

				result = true;
				std::cout << id << " Ŭ���̾�Ʈ ä�κ��� : " << oldChannel << " --> " << newChannel << std::endl;
			}
			SendChangeChannelPacket(id, result);
			break;
		}

		case CS_PACKET_TYPE::CS_CHAT:
		{
			CSChatPacket* packet = reinterpret_cast<CSChatPacket*>(buf);
			SendChatPacket(id, id, packet->chat);
			// �ֺ� ����鿡�� ä�ú���
			mUsers[id].ProcessChat(packet->chat);
			break;
		}

		case CS_PACKET_TYPE::CS_ATTACK:
		{
			CSAttackPacket* packet = reinterpret_cast<CSAttackPacket*>(buf);
			// ���Ϳ��� �����ߴ��� üũ
			mUsers[id].ProcessAttack();
			break;
		}

		default:
		{
			break;
		}
	}
	//auto end = std::chrono::high_resolution_clock::now() - start;
	//auto time = std::chrono::duration<double>(end).count();
	//std::cout << "��Ŷ ó�� �ð� : " << time << "��" << std::endl;
}

void Core::processEvent(Over* over)
{
	//std::cout << "������Ʈ id : " << id << " �̺�Ʈ �߻�" << std::endl;
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

			delete over;
			break;
		}

		default:
		{
			break;
		}
	}
	//popLeafWork();
}

int Core::FindChannel()
{
	while (true)
	{
		for (int i = 0; i < mChannels.size(); ++i)
		{
			if (mChannels[i].IsFull() == false)
			{
				return i;
			}
		}
	}

	return -1;
}

int Core::createPlayerId() const
{
	// ���̵� ���� ������, �������鼭 ��ٸ�����
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

bool Core::popLeafWork()
{
	Over* over;
	if (mLeafWorks.try_pop(over) == false)
	{
		return false;
	}

	delete over;
	return true;
}
