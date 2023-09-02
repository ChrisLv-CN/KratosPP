#include "INIReaderManager.h"

void INIReaderManager::ClearBuffer(EventSystem *sender, Event e, void *args)
{
	INIBufferManager::ClearBuffer(sender, e, args);
	INIConfigManager::ClearBuffer(sender, e, args);
	// 释放BufferReader
	for (auto bufferReader : s_BufferReader)
	{
		GameDelete(bufferReader.second);
	}
	s_BufferReader.clear();
	// 释放ConfigReader
	for (auto configReader : s_ConfigReader)
	{
		GameDelete(configReader.second);
	}
	s_ConfigReader.clear();
}

INIBufferReader *INIReaderManager::FindBufferReader(std::vector<std::string> dependency, const char *section)
{
	const INIReaderKey key{dependency, section};
	auto it = s_BufferReader.find(key);
	if (it != s_BufferReader.end())
	{
		return it->second;
	}
	INIBufferReader *reader = GameCreate<INIBufferReader>(dependency, section);
	s_BufferReader[key] = reader;

	return reader;
}

std::map<INIReaderManager::INIReaderKey, INIBufferReader *> INIReaderManager::s_BufferReader = {};
std::map<INIReaderManager::INIReaderKey, void *> INIReaderManager::s_ConfigReader = {};
