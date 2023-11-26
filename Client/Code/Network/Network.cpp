#include "Network.h"
#include <iostream>
#include "../Core/Core.h"
#include "../GameObject/Character/Character.h"
#include "../GraphicEngine/GraphicEngine.h"

#define	WM_SOCKET	WM_USER + 1

INIT_INSTACNE(Network)
int Network::myId = 0;
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

	// Recv만 AsyncSelect 모델 사용
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

void Network::SendLoginPacket(const std::string& loginId, const std::string& loginPassword)
{
	CSLoginPacket packet;
	packet.size = sizeof(CSLoginPacket);
	packet.type = CS_PACKET_TYPE::CS_LOGIN;
	//strncpy(packet.loginId, loginId, sizeof(loginId));
	//strncpy(packet.loginPassword, loginPassword, sizeof(loginPassword));
	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::SendMovePacket(char dir)
{
	CSMovePacket packet;
	packet.size = sizeof(CSMovePacket);
	packet.type = CS_PACKET_TYPE::CS_MOVE;
	packet.direction = dir;

	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::SendChangeChannelPacket(char channel)
{
	CSChangeChannelPacket packet;
	packet.size = sizeof(CSChangeChannelPacket);
	packet.type = CS_PACKET_TYPE::CS_CHANGE_CHANNEL;
	packet.channel = channel;

	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::SendChatPacket(const std::wstring& chat)
{
	CSChatPacket packet;
	packet.size = sizeof(CSChatPacket);
	packet.type = CS_PACKET_TYPE::CS_CHAT;
	wcsncpy_s(packet.chat, chat.c_str(), MAX_CHAT_LENGTH);
	
	std::cout << "채팅 패킷 전송" << std::endl;
	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::processPacket()
{
	switch (mPacketBuffer[1])
	{
		case SC_PACKET_TYPE::SC_LOGIN_OK:
		{
			SCLoginOkPacket* packet = reinterpret_cast<SCLoginOkPacket*>(mPacketBuffer);
			myId = packet->id;
			if (GET_INSTANCE(Core)->AddObject(myId) == false)
			{
				GET_INSTANCE(Core)->Quit();
			}
			break;
		}

		case SC_PACKET_TYPE::SC_LOGIN_FAIL:
		{
			SCLoginFailPacket* packet = reinterpret_cast<SCLoginFailPacket*>(mPacketBuffer);
			std::cout << "로그인 실패" << std::endl;
			break;
		}

		case SC_PACKET_TYPE::SC_POSITION:
		{
			// 준비가 아직 안됬으면 걍 리턴
			if (GET_INSTANCE(Core)->GetIsReady() == false)
			{
				break;
			}

			SCPositionPacket* packet = reinterpret_cast<SCPositionPacket*>(mPacketBuffer);			
			int id = packet->id;
			int x = packet->x;
			int y = packet->y;

			if (myId == id)
			{
				GET_INSTANCE(Camera)->SetPosition(x, y);
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
			// 준비가 아직 안됬으면 걍 리턴
			if (GET_INSTANCE(Core)->GetIsReady() == false)
			{
				break;
			}

			SCAddObjectPacket* packet = reinterpret_cast<SCAddObjectPacket*>(mPacketBuffer);
			int id = packet->id;
			int x = packet->x;
			int y = packet->y;

			if (myId == id)
			{
				GET_INSTANCE(Camera)->SetPosition(x, y);
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
			// 준비가 아직 안됬으면 걍 리턴
			if (GET_INSTANCE(Core)->GetIsReady() == false)
			{
				break;
			}

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

		case SC_PACKET_TYPE::SC_CHANGE_CHANNEL:
		{
			// 준비가 아직 안됬으면 걍 리턴
			if (GET_INSTANCE(Core)->GetIsReady() == false)
			{
				break;
			}

			SCChangeChannelPacket* packet = reinterpret_cast<SCChangeChannelPacket*>(mPacketBuffer);
			if (packet->result == true)
			{
				std::cout << "채널 변경 성공" << std::endl;
			}
			else
			{
				std::cout << "채널 변경 실패" << std::endl;
			}
			break;
		}

		case SC_PACKET_TYPE::SC_CHAT:
		{
			SCChatPacket* packet = reinterpret_cast<SCChatPacket*>(mPacketBuffer);
			std::wcout << packet->chat << std::endl;
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

			// 패킷 처리
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
	// 패킷 복사
	memcpy(mSendBuffer.buffer, packet, packet[0]);

	int result = WSASend(mSocket, &mSendBuffer.wsaBuffer, 1, &iobyte, 0, nullptr, nullptr);
	if (result)
	{
		int error_code = WSAGetLastError();
		std::cout << "Error while sending packet - " << error_code << std::endl;
	}
}
