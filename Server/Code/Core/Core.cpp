#include "../Code/pch.h"
#include "../Common/Protocol.h"
#include "Core.h"
#include "../Database/Database.h"

INIT_INSTACNE(Core)
Core::Core()
	: mIOCP(nullptr), mIsRun(false)
{
	_wsetlocale(LC_ALL, L"korean");
	// �����߻��� �ѱ۷� ��µǵ��� ���
	std::wcout.imbue(std::locale("korean"));
}

Core::~Core()
{
	std::cout << "Core �Ҹ���" << std::endl;
	mIOCP = nullptr;

	GET_INSTANCE(Database)->Release();

	if (mAcceptThread.joinable() == true)
	{
		mAcceptThread.join();
	}

	if (mTimerThread.joinable() == true)
	{
		mTimerThread.join();
	}

	for (auto& th : mWorkerThreads)
	{
		if (th.joinable() == true)
		{
			th.join();
		}
	}
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

	bool check = true;

	int workerThreadCount = 4;
	// ��Ŀ ������ ����
	for (int i = 0; i < workerThreadCount; ++i)
	{
		mWorkerThreads.emplace_back(std::thread{ &Core::threadPool, this });
	}

	// accept ������ ����
	mAcceptThread = std::thread{ &Core::acceptClient, this };

	mTimerThread = std::thread{ &Core::timer, this };

	if (GET_INSTANCE(Database)->Initialize() == false)
	{
		std::cout << "DB Initialize Fail!!" << std::endl;
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
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
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
	serverAddr.sin_family = PF_INET;
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

		Player* player = mUsers[id];
		player->SetSocket(clientSocket);
		player->PlayerConnect();

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
		OverEx* overEx;
		// lpover�� recv���� send���� ������ �־�� ��.
		BOOL result = GetQueuedCompletionStatus(mIOCP, &ioByte, &id, reinterpret_cast<LPWSAOVERLAPPED*>(&overEx), INFINITE);

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

		switch (overEx->eventType)
		{
			case SERVER_EVENT::RECV:
			{
				Player* player = mUsers[id];
				// ���� ��Ŷ ũ��(���� ��Ŷ ũ��)
				int restSize = ioByte;
				char* p = overEx->messageBuffer;
				char packetSize = 0;

				if (player->GetPrevSize() > 0)
				{
					packetSize = player->GetPacketBuf()[0];
				}

				while (restSize > 0)
				{
					// ���� �����ִ� ��Ŷ�� �����ٸ�, ���� ���� ��Ŷ�� ����
					if (packetSize == 0)
						packetSize = p[0];

					// ��Ŷ�� ����� ���� ũ��?
					int required = packetSize - player->GetPrevSize();
					// ��Ŷ�� ���� �� �ִٸ�, (���� ���� ��Ŷ�� ũ�Ⱑ required ���� ũ��)
					if (restSize >= required)
					{
						player->SetPacketBuf(p, required);
						// ��Ŷ ó��
						processPacket(static_cast<int>(id), player->GetPacketBuf());

						restSize -= required;
						p += required;
						packetSize = 0;
					}
					// ��Ŷ�� ���� �� ���ٸ�,
					else
					{
						// ���� ���� ��Ŷ�� ũ�⸸ŭ, ���� ���� ��Ŷ�� �����Ų��.
						player->SetPacketBuf(p, restSize);
						restSize = 0;
					}
				}
				recvPacket(static_cast<int>(id));
				break;
			}

			case SERVER_EVENT::SEND:
			{
				delete overEx;
				break;
			}

			default:
			{
				processEvent(overEx->eventType, static_cast<int>(id));
				delete overEx;
				break;
			}
		}
	}
}

void Core::timer()
{
	while (mIsRun == true)
	{

	}

	std::cout << "Ÿ�̸� ����" << std::endl;
}

void Core::disconnect(int id)
{
	std::cout << id << "�� Ŭ���̾�Ʈ ���� ������" << std::endl;
}

void Core::recvPacket(int id)
{
	DWORD flags = 0;
	SOCKET socket = mUsers[id]->GetSocket();
	OverEx* over = mUsers[id]->GetOverEx();

	// WSASend(���信 ����)�� �ݹ��� ���
	over->dataBuffer.len = MAX_BUFFER;
	over->dataBuffer.buf = over->messageBuffer;
	memset(&(over->overlapped), 0, sizeof(WSAOVERLAPPED));

	if (WSARecv(socket, &over->dataBuffer, 1, nullptr, &flags, &(over->overlapped), nullptr) == SOCKET_ERROR)
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
	SOCKET socket = mUsers[id]->GetSocket();
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

}

void Core::processEvent(SERVER_EVENT eventType, int id)
{
}

int Core::createPlayerId() const
{
	// ���̵� ���� ������, �������鼭 ��ٸ�����
	while (true)
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (mUsers[i]->GetIsConnect() == false)
			{
				mUsers[i]->PlayerConnect();
				return i;
			}
		}
	}

	return -1;
}
