#pragma once
#include "../../Common/Macro.h"
#include "../../../../Server/Code/Common/Protocol.h"
#include <unordered_map>
#include <queue>

struct PacketEvent
{
	PacketEvent(char _packetType, int _id, int _x, int _y)
		: packetType(_packetType), id(_id), x(_x), y(_y) {}

	char packetType;
	int id;
	int x;
	int y;
};

class EventManager
{
	SINGLE_TONE(EventManager)

public:
	void AddPacketEvent(char packetType, int id, int x, int y) { mEventQueue.push(PacketEvent(packetType, id, x, y)); }
	void AddPacketEvent(PacketEvent p) { mEventQueue.push(p); }

	void Update();

private:
	void processPacketEvent();

private:
	static std::queue<PacketEvent> mEventQueue;
};