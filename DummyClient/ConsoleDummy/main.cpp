#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include "../../Server/Code/Common/Protocol.h"

using namespace std;

struct Client
{
	Client()
	{
		overEx = nullptr;
		isConnect = false;
		x = 0;
		y = 0;
		socket = 0;
		memset(packetBuf, 0, sizeof(packetBuf));
		prevSize = 0;
		sendBytes = 0;

		Initialize();
	}
	~Client()
	{
		delete overEx;
		closesocket(socket);
	}
	void Initialize()
	{
		overEx = new OverEx;
		memset(&overEx->overlapped, 0, sizeof(WSAOVERLAPPED));
		memset(overEx->messageBuffer, 0, sizeof(MAX_BUFFER));
		overEx->dataBuffer.buf = overEx->messageBuffer;
		overEx->dataBuffer.len = MAX_BUFFER;
	}

	OverEx* overEx;
	bool isConnect;
	int x;
	int y;
	SOCKET socket;

	char packetBuf[MAX_BUFFER];
	int prevSize;
	int sendBytes;
	std::mutex clientMtx;
};

HANDLE gIOCP;
std::vector<std::thread> gWorkerThreads;
std::thread gConnectThread;
std::thread gAIThread;
SOCKET gListenSocket;

Client gClients[MAX_OBJECT];
int gConnectNum = 199;

bool Initialize();
void ThreadPool();
void DoConnect();
void DoAI();
void Release();

void ErrorDisplay(const char* msg, int error);
bool ConnectServer(int id);
void RecvPacket(int id);
void SendPacket(int id, char* packet);
void ProcessPacket(int id, char* buf);
void Disconnect(int id);

void SendMovePacket(int id, char dir);

int main()
{
	_wsetlocale(LC_ALL, L"korean");
	// 에러발생시 한글로 출력되도록 명령
	std::wcout.imbue(std::locale("korean"));

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

	gConnectThread = std::thread{ DoConnect };
	gAIThread = std::thread{ DoAI };

	return true;
}

void ThreadPool()
{
	while (true)
	{
		DWORD ioByte;
		unsigned long long id;
		OverEx* overEx;
		// lpover에 recv인지 send인지 정보를 넣어야 됨.
		BOOL result = GetQueuedCompletionStatus(gIOCP, &ioByte, &id, reinterpret_cast<LPWSAOVERLAPPED*>(&overEx), INFINITE);

		//  GetQueuedCompletionStatus( )가 에러인지 아닌지 확인한다
		if (result == false)
		{
			int errorNum = WSAGetLastError();
			if (errorNum == 64)
			{
				Disconnect(static_cast<int>(id));
				continue;
			}

			else if (errorNum == 735)
			{
				break;
			}

			else
			{
				ErrorDisplay(" GetQueuedCompletionStatus()", errorNum);
			}
		}

		// 클라와 끊어졌다면 (클라가 나갔을 때)
		if (ioByte == 0)
		{
			Disconnect(static_cast<int>(id));
			continue;
		}

		switch (overEx->eventType)
		{
			case SERVER_EVENT::RECV:
			{
				// 남은 패킷 크기(들어온 패킷 크기)
				int restSize = ioByte;
				char* p = overEx->messageBuffer;
				char packetSize = 0;

				if (gClients[id].prevSize > 0)
				{
					packetSize = gClients[id].packetBuf[0];
				}

				while (restSize > 0)
				{
					// 전에 남아있던 패킷이 없었다면, 현재 들어온 패킷을 저장
					if (packetSize == 0)
						packetSize = p[0];

					// 패킷을 만들기 위한 크기?
					int required = packetSize - gClients[id].prevSize;
					// 패킷을 만들 수 있다면, (현재 들어온 패킷의 크기가 required 보다 크면)
					if (restSize >= required)
					{
						memcpy(gClients[id].packetBuf + gClients[id].prevSize, p, required);
						// 패킷 처리
						ProcessPacket(static_cast<int>(id), gClients[id].packetBuf);

						restSize -= required;
						p += required;
						packetSize = 0;

						gClients[id].prevSize = 0;
					}
					// 패킷을 만들 수 없다면,
					else
					{
						// 현재 들어온 패킷의 크기만큼, 현재 들어온 패킷을 저장시킨다.
						memcpy(gClients[id].packetBuf + gClients[id].prevSize, p, restSize);
						gClients[id].prevSize = restSize;
						restSize = 0;
						
					}
				}
				RecvPacket(static_cast<int>(id));
				break;
			}

			case SERVER_EVENT::SEND:
			{
				delete overEx;
				break;
			}

			default:
			{
				break;
			}
		}
	}
}

void DoConnect()
{
	int clientsCount = 1;
	while (clientsCount < gConnectNum)
	{
		// 서버 접속
		if (ConnectServer(clientsCount) == false)
		{
			std::cout << clientsCount << "번 클라이언트 - 서버 연결 실패" << std::endl;
			return;
		}

		++clientsCount;
	}
}

void DoAI()
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, 3);

	while (true)
	{
		std::this_thread::sleep_for(100ms);
		// move ai
		for (int i = 1; i < gConnectNum; ++i)
		{
			if (gClients[i].isConnect == false)
			{
				continue;
			}

			// 패킷전송
			char dir = uid(dre);
			SendMovePacket(i, dir);
		}
	}
}
void Release()
{
	if (gConnectThread.joinable() == true)
	{
		gConnectThread.join();
	}

	if (gAIThread.joinable() == true)
	{
		gAIThread.join();
	}

	for (auto& th : gWorkerThreads)
	{
		if (th.joinable() == true)
		{
			th.join();
		}
	}

	CloseHandle(gIOCP);
	WSACleanup();
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
	std::this_thread::sleep_for(100ms);

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

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(gClients[id].socket), gIOCP, id, 0);
	RecvPacket(id);

	gClients[id].isConnect = true;
	
	return true;
}

void RecvPacket(int id)
{
	DWORD flag = 0;

	memset(&gClients[id].overEx->overlapped, 0, sizeof(WSAOVERLAPPED));
	gClients[id].overEx->dataBuffer.buf = gClients[id].overEx->messageBuffer;
	gClients[id].overEx->dataBuffer.len = MAX_BUFFER;
	gClients[id].overEx->eventType = SERVER_EVENT::RECV;

	if (WSARecv(gClients[id].socket, &gClients[id].overEx->dataBuffer, 1, nullptr, &flag, &(gClients[id].overEx->overlapped), nullptr) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			ErrorDisplay("WSARecv() Error - ", error);
		}
	}
}

void SendPacket(int id, char* packet)
{
	SOCKET socket = gClients[id].socket;
	OverEx* over = new OverEx;

	over->dataBuffer.len = packet[0];
	over->dataBuffer.buf = over->messageBuffer;
	// 패킷의 내용을 버퍼에 복사
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

void ProcessPacket(int id, char* buf)
{
	switch (buf[1])
	{
		case SC_PACKET_TYPE::SC_POSITION:
		{
			SCPositionPacket* packet = reinterpret_cast<SCPositionPacket*>(buf);
			gClients[id].x = packet->x;
			gClients[id].y = packet->y;
			//std::cout << id << "번 클라이언트 위치 : " << gClients[id].x << ", " << gClients[id].y << std::endl;
			break;
		}

		case SC_PACKET_TYPE::SC_ADD_OBJECT:
		{
			SCAddObjectPacket* packet = reinterpret_cast<SCAddObjectPacket*>(buf);
			gClients[packet->id].x = packet->x;
			gClients[packet->id].y = packet->y;
			//std::cout << id << "번 클라이언트에서 " << packet->id << "번 클라이언트 추가" << std::endl;
			break;
		}

		case SC_PACKET_TYPE::SC_REMOVE_OBJECT:
		{
			SCRemoveObjectPacket* packet = reinterpret_cast<SCRemoveObjectPacket*>(buf);
			//std::cout << id << "번 클라이언트에서 " << packet->id << "번 클라이언트 삭제" << std::endl;
			break;
		}

		default:
		{
			break;
		}
	}
}

void Disconnect(int id)
{
	gClients[id].isConnect = false;
	std::cout << id << "번 클라이언트 연결 끊어짐" << std::endl;
}

void SendMovePacket(int id, char dir)
{
	CSMovePacket packet;
	packet.size = sizeof(CSMovePacket);
	packet.type = CS_PACKET_TYPE::CS_MOVE;
	packet.direction = dir;
	SendPacket(id, reinterpret_cast<char*>(&packet));
}