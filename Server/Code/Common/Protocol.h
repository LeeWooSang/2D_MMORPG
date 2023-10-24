#pragma once

constexpr int SERVER_PORT = 9000;
constexpr int MAX_USER = 3000;
constexpr int MAX_BUFFER = 1024;

enum class SERVER_EVENT
{
	RECV = 0,
	SEND = 1,
};
