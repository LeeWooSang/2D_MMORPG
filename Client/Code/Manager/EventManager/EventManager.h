#pragma once
#include "../../Common/Macro.h"
#include "../../../../Server/Code/Common/Protocol.h"
#include <unordered_map>
#include <queue>
#include <memory>

struct PacketBase
{
	char packetType;
	int id;
};
struct PositionPacket : public PacketBase
{
	int x;
	int y;
};
struct AddPacket : public PacketBase
{
	int x;
	int y;
	int texId;
};
struct AvatarPacket : public PacketBase
{
	int texId;
};

class EventManager
{
	SINGLE_TONE(EventManager)

public:
	void AddPacketEvent(std::shared_ptr< PacketBase> packet) { mEventQueue.push(packet); }

	void Update();

private:
	void processPacketEvent();

private:
	static std::queue<std::shared_ptr<PacketBase>> mEventQueue;
};