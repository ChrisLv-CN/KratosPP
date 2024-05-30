#pragma once

#include <Windows.h>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>


//
// Lowercases string
//
template <typename T>
std::basic_string<T> lowercase(const std::basic_string<T>& s)
{
	std::basic_string<T> s2 = s;
	std::transform(s2.begin(), s2.end(), s2.begin(),
		[](const T v) { return static_cast<T>(std::tolower(v)); });
	return s2;
}

//
// Uppercases string
//
template <typename T>
std::basic_string<T> uppercase(const std::basic_string<T>& s)
{
	std::basic_string<T> s2 = s;
	std::transform(s2.begin(), s2.end(), s2.begin(),
		[](const T v) { return static_cast<T>(std::toupper(v)); });
	return s2;
}

std::string& trim(std::string& s);

void split(std::string& s, std::string& delim, std::vector<std::string>* result);

std::string subreplace(std::string resource, std::string sub, std::string replace);

int Wchar2Char(const wchar_t* wcharStr, char* charStr);
std::string WString2String(std::wstring wstr);

int Char2Wchar(const char* charStr, wchar_t* wcharStr);
std::wstring String2WString(std::string str);

bool IsNotNone(std::string val);

void ClearIfGetNone(std::vector<std::string>& value);

std::string GetUUID();
std::string GetUUIDShort();
