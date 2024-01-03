#pragma once
#include <iostream>
#include <string>
#include <locale>

inline std::wstring StringToWString(const std::string& s)
{
	std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(s);
}