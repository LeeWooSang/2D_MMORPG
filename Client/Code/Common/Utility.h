#pragma once
#include <iostream>
#include <string>
#include <locale>
#include <random>
#include <ctime>

inline std::wstring StringToWString(const std::string& s)
{
	std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(s);
}

inline int GetRandomNumber(int startNum, int endNum)
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(startNum, endNum);
	
	return uid(dre);
}

inline std::string GetCurrentDate()
{
	std::time_t now = std::time(nullptr);
	std::tm localTime;
	localtime_s(&localTime, &now);

	char buf[100] = { 0, };
	sprintf_s(buf, "%d-%02d-%02d", (localTime.tm_year + 1900), (localTime.tm_mon + 1), localTime.tm_mday);
	return buf;
}

inline std::string GetCurrentDateTime()
{
	std::time_t now = std::time(nullptr);
	std::tm localTime;
	localtime_s(&localTime, &now);

	char buf[100] = { 0, };
	sprintf_s(buf, "%d-%02d-%02d %02d:%02d:%02d", (localTime.tm_year + 1900), (localTime.tm_mon + 1), localTime.tm_mday, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
	return buf;
}