#include "Network.h"
#include <iostream>
#include "../Core/Core.h"
#include "../GameObject/Character/Character.h"

#define	WM_SOCKET	WM_USER + 1

INIT_INSTACNE(Network)
Network::Network()
	: mSocket(0)
{
	memset(mPacketBuffer, 0, sizeof(mPacketBuffer));
}

Network::~Network()
{
	closesocket(mSocket);
	WSACleanup();
}

bool Network::Initialize(const HWND& handle)
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int result = WSAConnect(mSocket, (sockaddr*)&serverAddr, sizeof(serverAddr), nullptr, nullptr, nullptr, nullptr);
	if (result == -1)
	{
		return false;
	}

	WSAAsyncSelect(mSocket, handle, WM_SOCKET, FD_CLOSE | FD_READ);

	mRecvBuffer.wsaBuffer.buf = mRecvBuffer.buffer;
	mRecvBuffer.wsaBuffer.len = MAX_BUFFER;
	mSendBuffer.wsaBuffer.buf = mSendBuffer.buffer;
	mSendBuffer.wsaBuffer.len = MAX_BUFFER;

	return true;
}

void Network::PreocessNetwork(unsigned long long wparam, long long lparam)
{
	if (WSAGETSELECTERROR(lparam))
	{
		closesocket(mSocket);
		//clienterror();
	}

	// Recv�� AsyncSelect �� ���
	switch (WSAGETSELECTEVENT(lparam))
	{
		case FD_READ:
		{
			recvPacket();
			break;
		}

		case FD_CLOSE:
		{
			closesocket(mSocket);
			//clienterror();
			break;
		}
		default:
			break;
	}
}

void Network::SendMovePacket(char dir)
{
	CSMovePacket packet;
	packet.size = sizeof(CSMovePacket);
	packet.type = CS_PACKET_TYPE::CS_MOVE;
	packet.direction = dir;

	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::processPacket()
{
	switch (mPacketBuffer[1])
	{
		case SC_PACKET_TYPE::SC_LOGIN_OK:
		{
			break;
		}

		case SC_PACKET_TYPE::SC_LOGIN_FAIL:
		{
			break;
		}

		case SC_PACKET_TYPE::SC_POSITION:
		{
			SCPositionPacket* packet = reinterpret_cast<SCPositionPacket*>(mPacketBuffer);
			
			int id = packet->id;
			int x = packet->x;
			int y = packet->y;
			if (GET_INSTANCE(Core)->GetPlayer()->GetId() == id)
			{
				GET_INSTANCE(Core)->GetPlayer()->SetPosition(x, y);
			}
			else
			{
				if (id < MAX_USER)
				{
					GET_INSTANCE(Core)->GetOtherPlayer(id)->SetPosition(x, y);
				}
				else
				{
					GET_INSTANCE(Core)->GetMonster(id)->SetPosition(x, y);
				}
			}
			break;
		}

		case SC_PACKET_TYPE::SC_ADD_OBJECT:
		{
			SCAddObjectPacket* packet = reinterpret_cast<SCAddObjectPacket*>(mPacketBuffer);

			int id = packet->id;
			int x = packet->x;
			int y = packet->y;
			if (GET_INSTANCE(Core)->GetPlayer()->GetId() == id)
			{
				GET_INSTANCE(Core)->GetPlayer()->SetPosition(x, y);
			}
			else
			{
				if (id < MAX_USER)
				{
					GET_INSTANCE(Core)->GetOtherPlayer(id)->SetPosition(x, y);
					GET_INSTANCE(Core)->GetOtherPlayer(id)->Visible();
				}
				else
				{
					GET_INSTANCE(Core)->GetMonster(id)->SetPosition(x, y);
					GET_INSTANCE(Core)->GetMonster(id)->Visible();
				}
			}
			break;
		}

		case SC_PACKET_TYPE::SC_REMOVE_OBJECT:
		{
			SCRemoveObjectPacket* packet = reinterpret_cast<SCRemoveObjectPacket*>(mPacketBuffer);

			int id = packet->id;
			if (id < MAX_USER)
			{
				GET_INSTANCE(Core)->GetOtherPlayer(id)->NotVisible();
			}
			else
			{
				GET_INSTANCE(Core)->GetMonster(id)->NotVisible();
			}
			break;
		}

		default:
			break;
	}
}

void Network::recvPacket()
{
	DWORD iobyte, flag = 0;

	int result = WSARecv(mSocket, &mRecvBuffer.wsaBuffer, 1, &iobyte, &flag, nullptr, nullptr);
	if (result)
	{
		int err_code = WSAGetLastError();
		std::cout << "Recv Error - " << err_code << std::endl;
	}

	BYTE* p = reinterpret_cast<BYTE*>(mRecvBuffer.buffer);
	
	while (iobyte != 0)
	{
		if (mRecvBuffer.packetSize == 0)
		{
			mRecvBuffer.packetSize = p[0];
		}

		if (iobyte + mRecvBuffer.prevSize >= mRecvBuffer.packetSize)
		{
			memcpy(mPacketBuffer + mRecvBuffer.prevSize, p, mRecvBuffer.packetSize - mRecvBuffer.prevSize);

			// ��Ŷ ó��
			processPacket();

			p += mRecvBuffer.packetSize - mRecvBuffer.prevSize;
			iobyte -= mRecvBuffer.packetSize - mRecvBuffer.prevSize;
			mRecvBuffer.packetSize = 0;
			mRecvBuffer.prevSize = 0;
		}

		else
		{
			memcpy(mPacketBuffer + mRecvBuffer.prevSize, p, iobyte);
			mRecvBuffer.prevSize += iobyte;
			iobyte = 0;
		}
	}
}

void Network::sendPacket(char* packet)
{
	DWORD iobyte = 0;
	
	mSendBuffer.wsaBuffer.buf = mSendBuffer.buffer;
	mSendBuffer.wsaBuffer.len = packet[0];
	// ��Ŷ ����
	memcpy(mSendBuffer.buffer, packet, packet[0]);

	int result = WSASend(mSocket, &mSendBuffer.wsaBuffer, 1, &iobyte, 0, nullptr, nullptr);
	if (result)
	{
		int error_code = WSAGetLastError();
		std::cout << "Error while sending packet - " << error_code << std::endl;
	}
}