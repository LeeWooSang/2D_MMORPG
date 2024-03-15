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
struct ServerSelectPacket : public PacketBase
{
	short channelUserSize[MAX_CHANNEL];
};
struct ChannelLoginPacket : public PacketBase
{
	int channel;
};
struct PositionPacket : public PacketBase
{
	int x;
	int y;
	char dir;
};
struct AddPlayerPacket : public PacketBase
{
	int x;
	int y;
	char dir;
	int animationType;
	int texIds[21];
};
struct AddPacket : public PacketBase
{
	int x;
	int y;
	int texId;
};
struct AvatarPacket : public PacketBase
{
	int slotType;
	int texId;
};
struct TakeOffEquipItemPacket : public PacketBase
{
	int slotType;
};
struct AddTradeItemPacket : public PacketBase
{
	int texId;
	int slotNum;
};
struct AddTradeMesoPacket : public PacketBase
{
	long long meso;
};
struct TradePacket : public PacketBase
{
	int items[9];
	long long meso;
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