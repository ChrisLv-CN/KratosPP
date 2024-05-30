﻿#include "StringEx.h"


std::string& trim(std::string& s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

void split(std::string& s, std::string& delim, std::vector<std::string>* result)
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

std::string subreplace(std::string resource, std::string sub, std::string replace)
{
	std::string dst_str = resource;
	std::string::size_type;
	size_t pos = 0;
    while((pos = dst_str.find(sub)) != std::string::npos)
    {
        dst_str.replace(pos, sub.length(), replace);
    }
    return dst_str;
}

int Wchar2Char(const wchar_t* wcharStr, char* charStr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wcharStr, wcslen(wcharStr), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wcharStr, wcslen(wcharStr), charStr, len, NULL, NULL);
	charStr[len] = '\0';
	return len;
}

std::string WString2String(std::wstring wstr)
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
	delete[] buffer;
	return res;
}

int Char2Wchar(const char* charStr, wchar_t* wcharStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, charStr, strlen(charStr), NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, charStr, strlen(charStr), wcharStr, len);
	wcharStr[len] = '\0';
	return len;
}

std::wstring String2WString(std::string str)
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
	delete[] buffer;
	return res;
}

bool IsNotNone(std::string val)
{
	return !val.empty() && lowercase(trim(val)) != "none";
}

void ClearIfGetNone(std::vector<std::string>& value)
{
	if (value.size() == 1 && !IsNotNone(value[0]))
	{
		value.clear();
	}
}

std::string GetUUID()
{
    std::string strUUID;
    GUID guid;
    if (SUCCEEDED(CoCreateGuid(&guid))) {
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

std::string GetUUIDShort()
{
    std::string strUUID;
    GUID guid;
    if (SUCCEEDED(CoCreateGuid(&guid))) {
        char buffer[64] = { 0 };
        _snprintf_s(buffer, sizeof(buffer),
            "%02X%02X%02X%02X%02X%02X",    //大写
            //"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",  //小写
            guid.Data4[2],
            guid.Data4[3], guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
        strUUID = buffer;
    }
    return strUUID;
}
