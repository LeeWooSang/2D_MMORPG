#include "Core.h"
#include "../GameTimer/GameTimer.h"
#include "../Database/Database.h"
#include "../Character/Character.h"


INIT_INSTACNE(Core)
Core::Core()
	: mIOCP(nullptr), mIsRun(false)
{
	_wsetlocale(LC_ALL, L"korean");
	// �����߻��� �ѱ۷� ��µǵ��� ����
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

	mUsers = new Player[MAX_USER];
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (mUsers[i].Inititalize() == false)
		{
			return false;
		}
	}

	int workerThreadCount = 4;
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

void Core::SendPositionPacket(int id)
{
	SCPositionPacket packet;
	packet.size = sizeof(SCPositionPacket);
	packet.type = SC_PACKET_TYPE::SC_POSITION;
	packet.id = id;
	packet.x = mUsers[id].GetX();
	packet.y = mUsers[id].GetY();

	sendPacket(id, reinterpret_cast<char*>(&packet));
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

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), mIOCP, id, 0);
		recvPacket(id);

		std::cout << id << "�� Ŭ���̾�Ʈ ����" << std::endl;
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
					// ���� �����ִ� ��Ŷ�� �����ٸ�, ���� ���� ��Ŷ�� ����
					if (packetSize == 0)
						packetSize = p[0];

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
					}
					// ��Ŷ�� ���� �� ���ٸ�,
					else
					{
						// ���� ���� ��Ŷ�� ũ�⸸ŭ, ���� ���� ��Ŷ�� �����Ų��.
						mUsers[id].SetPacketBuf(p, restSize);
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
				processEvent(over->eventType, static_cast<int>(id));
				popLeafWork();
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

	over->dataBuffer.len = packet[0];
	over->dataBuffer.buf = over->messageBuffer;
	// ��Ŷ�� ������ ���ۿ� ����
	memcpy(over->messageBuffer, packet, packet[0]);
	ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->eventType = SERVER_EVENT::SEND;

	if (WSASend(socket, &over->dataBuffer, 1, nullptr, 0, &(over->overlapped), nullptr) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "Error - Fail WSARecv(error_code : " << WSAGetLastError() << std::endl;
		}
	}
}

void Core::processPacket(int id, char* buf)
{
	switch (buf[1])
	{
		case CS_PACKET_TYPE::CS_MOVE:
		{
			CSMovePacket* packet = reinterpret_cast<CSMovePacket*>(buf);
			mUsers[id].ProcessMove(packet->direction);
			SendPositionPacket(id);
			break;
		}

		default:
		{
			break;
		}
	}
}

void Core::processEvent(SERVER_EVENT eventType, int id)
{
	std::cout << "������Ʈ id : " << id << " �̺�Ʈ �߻�" << std::endl;
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
	OverEx* overEx;
	if (mLeafWorks.try_pop(overEx) == false)
	{
		return false;
	}

	delete overEx;
	return true;
}