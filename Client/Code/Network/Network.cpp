#include "Network.h"
#include <iostream>
#include "../Core/Core.h"

#include "../Manager/EventManager/EventManager.h"
#include "../Manager/SceneMangaer/SceneManager.h"
#include "../Scene/InGameScene/InGameScene.h"

#include "../Manager/UIManager/UIManager.h"
#include "../GameObject/UI/ChattingBox/ChattingBox.h"

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

void Network::SendBroadCastingChatPacket(const std::wstring& chat)
{
	CSBroadcastingChatPacket packet;
	packet.size = sizeof(CSBroadcastingChatPacket);
	packet.type = CS_PACKET_TYPE::CS_BROADCASTING_CHAT;
	wcsncpy_s(packet.chat, chat.c_str(), MAX_CHAT_LENGTH);
	
	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::SendWhisperingChatPacket(int id, const std::wstring& chat)
{
	CSWhisperingChatPacket packet;
	packet.size = sizeof(CSWhisperingChatPacket);
	packet.type = CS_PACKET_TYPE::CS_WHISPERING_CHAT;
	packet.id = id;

	wcsncpy_s(packet.chat, chat.c_str(), MAX_CHAT_LENGTH);

	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::SendAttackPacket()
{
	CSAttackPacket packet;
	packet.size = sizeof(CSAttackPacket);
	packet.type = CS_PACKET_TYPE::CS_ATTACK;

	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::SendChangeAvatarPacket(char slotType, int texId)
{
	CSChangeAvatarPacket packet;
	packet.size = sizeof(CSChangeAvatarPacket);
	packet.type = CS_PACKET_TYPE::CS_CHANGE_AVATAR;
	// 텍스쳐 아이디
	// 아이템 타입
	packet.slotType = slotType;
	packet.texId = texId;

	sendPacket(reinterpret_cast<char*>(&packet));
}

void Network::processPacket()
{
	switch (mPacketBuffer[1])
	{
		case SC_PACKET_TYPE::SC_LOGIN_OK:
		{		
			SCLoginOkPacket* packet = reinterpret_cast<SCLoginOkPacket*>(mPacketBuffer);

			std::shared_ptr<PacketBase> p = std::make_shared<PacketBase>();
			p->packetType = packet->type;
			p->id = packet->id;
			GET_INSTANCE(EventManager)->AddPacketEvent(p);
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
			SCPositionPacket* packet = reinterpret_cast<SCPositionPacket*>(mPacketBuffer);

			std::shared_ptr<PositionPacket> p = std::make_shared<PositionPacket>();
			p->packetType = packet->type;
			p->id = packet->id;
			p->x = packet->x;
			p->y = packet->y;
			GET_INSTANCE(EventManager)->AddPacketEvent(p);
			break;
		}

		case SC_PACKET_TYPE::SC_ADD_OBJECT:
		{
			SCAddObjectPacket* packet = reinterpret_cast<SCAddObjectPacket*>(mPacketBuffer);

			std::shared_ptr<AddPacket> p = std::make_shared<AddPacket>();
			p->packetType = packet->type;
			p->id = packet->id;
			p->x = packet->x;
			p->y = packet->y;
			p->texId = 0;
			GET_INSTANCE(EventManager)->AddPacketEvent(p);
			break;
		}

		case SC_PACKET_TYPE::SC_REMOVE_OBJECT:
		{
			SCRemoveObjectPacket* packet = reinterpret_cast<SCRemoveObjectPacket*>(mPacketBuffer);

			std::shared_ptr<PacketBase> p = std::make_shared<PacketBase>();
			p->packetType = packet->type;
			p->id = packet->id;
			GET_INSTANCE(EventManager)->AddPacketEvent(p);
			break;
		}

		case SC_PACKET_TYPE::SC_CHANGE_CHANNEL:
		{
			// 준비가 아직 안됬으면 걍 리턴
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
			ChattingBox* ui = static_cast<ChattingBox*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChattingBox"));
			if (ui != nullptr)
			{
				ui->AddChattingLog(packet->id, packet->chat);
			}
			break;
		}

		case SC_PACKET_TYPE::SC_CHANGE_AVATAR:
		{
			SCChangeAvatarPacket* packet = reinterpret_cast<SCChangeAvatarPacket*>(mPacketBuffer);

			std::shared_ptr<AvatarPacket> p = std::make_shared<AvatarPacket>();
			p->packetType = packet->type;
			p->id = packet->id;
			p->texId = packet->texId;
			GET_INSTANCE(EventManager)->AddPacketEvent(p);
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
