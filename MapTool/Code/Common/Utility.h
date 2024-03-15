#pragma once
#include <iostream>
#include <string>
#include <locale>
#include <random>

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