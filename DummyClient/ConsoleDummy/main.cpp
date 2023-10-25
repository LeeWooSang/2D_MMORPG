#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <vector>
#include <thread>
#include "../../Server/Code/Common/Protocol.h"

struct Client
{
	Client()
	{
		memset(&overEx, 0, sizeof(overEx));
		isConnect = false;
		x = 0;
		y = 0;
		eventType = SERVER_EVENT::DEFAULT;
		socket = 0;
		memset(packetBuf, 0, sizeof(packetBuf));
		prevSize = 0;
		sendBytes = 0;
	}

	OverEx overEx;
	bool isConnect;
	int x;
	int y;
	SERVER_EVENT eventType;
	SOCKET socket;

	char packetBuf[MAX_BUFFER];
	int prevSize;
	int sendBytes;
};

HANDLE gIOCP;
std::vector<std::thread> gWorkerThreads;
std::thread gAIthread;
SOCKET gListenSocket;

Client gClients[MAX_USER];

bool Initialize();
void ThreadPool();
void DoAI();
void Release();

void ErrorDisplay(const char* msg, int error);
bool ConnectServer(int id);
void RecvPacket(int id);

int main()
{
	if (Initialize() == false)
	{
		return -1;
	}

	Release();
}

bool Initialize()
{
	gIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (gIOCP == nullptr)
	{
		return false;
	}

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	int workerThreadCount = 2;
	// 워커 스레드 생성
	for (int i = 0; i < workerThreadCount; ++i)
	{
		gWorkerThreads.emplace_back(std::thread{ ThreadPool });
	}

	gAIthread = std::thread{ DoAI };

	return true;
}

void ThreadPool()
{
}

void DoAI()
{
	static int clientId = 0;

	while (clientId < 5)
	{
		// 서버 접속
		if (ConnectServer(clientId) == false)
		{
			std::cout << clientId << "번 클라이언트 - 서버 연결 실패" << std::endl;
			return;
		}

		RecvPacket(clientId);
		++clientId;
	}

	while (true)
	{
		// move ai
		for (int i = 0; i < clientId; ++i)
		{
			if (gClients[i].isConnect == false)
			{
				continue;
			}

			// 패킷전송
		}
	}
}
void Release()
{
	if (gAIthread.joinable() == true)
	{
		gAIthread.join();
	}
	for (auto& th : gWorkerThreads)
	{
		if (th.joinable() == true)
		{
			th.join();
		}
	}

	CloseHandle(gIOCP);
}

void ErrorDisplay(const char* msg, int error)
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

	LocalFree(lpMsgBuf);
}

bool ConnectServer(int id)
{
	gClients[id].socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int result = WSAConnect(gClients[id].socket, (sockaddr*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);
	if (result != 0)
	{
		ErrorDisplay("WSAConnect : ", GetLastError());
		return false;
	}

	gClients[id].isConnect = true;

	return true;
}

void RecvPacket(int id)
{
	memset(&gClients[id].overEx, 0, sizeof(gClients[id].overEx));
	gClients[id].overEx.dataBuffer.buf = gClients[id].overEx.messageBuffer;
	gClients[id].overEx.dataBuffer.len = MAX_BUFFER;
	gClients[id].overEx.eventType = SERVER_EVENT::RECV;
	gClients[id].prevSize = 0;
	gClients[id].sendBytes = 0;

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(gClients[id].socket), gIOCP, id, 0);

	int result = WSARecv(gClients[id].socket, &gClients[id].overEx.dataBuffer, 1, NULL, &recv_flag, &(gClients[id].overEx.overlapped), NULL);
	if (result == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			ErrorDisplay("RECV ERROR", err_no);
		}
	}
}
