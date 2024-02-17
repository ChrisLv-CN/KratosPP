#pragma once

#include <Windows.h>
#include <algorithm>
#include <string>
#include <vector>


//
//  Lowercases string
//
template <typename T>
static std::basic_string<T> lowercase(const std::basic_string<T>& s)
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
static std::basic_string<T> uppercase(const std::basic_string<T>& s)
{
	std::basic_string<T> s2 = s;
	std::transform(s2.begin(), s2.end(), s2.begin(),
		[](const T v) { return static_cast<T>(std::toupper(v)); });
	return s2;
}

static std::string& trim(std::string& s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

static void split(std::string& s, std::string& delim, std::vector<std::string>* result)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		result->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		result->push_back(s.substr(last, index - last));
	}
}

static int Wchar2Char(const wchar_t* wcharStr, char* charStr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wcharStr, wcslen(wcharStr), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wcharStr, wcslen(wcharStr), charStr, len, NULL, NULL);
	charStr[len] = '\0';
	return len;
}

static std::string WString2String(std::wstring wstr)
{
	std::string res{};
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	if (len <= 0)
	{
		return res;
	}
	char* buffer = new char[len + 1];
	if (buffer == nullptr)
	{
		return res;
	}
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	res.append(buffer);
	delete buffer;
	return res;
}

static int Char2Wchar(const char* charStr, wchar_t* wcharStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, charStr, strlen(charStr), NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, charStr, strlen(charStr), wcharStr, len);
	wcharStr[len] = '\0';
	return len;
}

static std::wstring String2WString(std::string str)
{
	std::wstring res{};
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	if (len <= 0)
	{
		return res;
	}
	wchar_t* buffer = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';
	res.append(buffer);
	delete buffer;
	return res;
}

static bool IsNotNone(std::string val)
{
	return !val.empty() && lowercase(trim(val)) != "none";
}

static std::string GetUUID()
{
    std::string strUUID;
    GUID guid;
    if (!CoCreateGuid(&guid)) {
        char buffer[64] = { 0 };
        _snprintf_s(buffer, sizeof(buffer),
            "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",    //大写
            //"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",  //小写
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2],
            guid.Data4[3], guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
        strUUID = buffer;
    }
    return strUUID;
}
