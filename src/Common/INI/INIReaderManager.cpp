#include "INIReaderManager.h"

void INIReaderManager::ClearBuffer(EventSystem* sender, Event e, void* args)
{
	// 释放INIFileBuffer
	for (auto fileBuffer : s_File)
	{
		fileBuffer->ClearBuffer();
	}
	s_File.clear();
	// 释放LinkedBuffer
	for (auto linkedBuffer : s_LinkedBuffer)
	{
		//linkedBuffer.second->Expired();
		GameDelete(linkedBuffer.second);
	}
	s_LinkedBuffer.clear();
	// 释放Config
	for (auto configMap : s_Configs)
	{
		for (auto config : configMap.second)
		{
			// 释放config对象
			GameDelete(config.second);
		}
		configMap.second.clear();
	}
	s_Configs.clear();
}

INIFileBuffer* INIReaderManager::FindFile(std::string fileName)
{
	for (auto buffer : s_File)
	{
		if (buffer->FileName == fileName)
		{
			return buffer;
		}
	}
	// 为每个iniFile创建一个存储对象
	// INIFileBuffer在构建时会读取KV对，以字符串形式缓存
	std::string f = fileName.data();
	INIFileBuffer* buffer = GameCreate<INIFileBuffer>(f);
	s_File.push_back(buffer);
	return buffer;
}

INIBuffer* INIReaderManager::FindBuffer(std::string fileName, std::string section)
{
	// ini文件按顺序储存，在查找时先读取Map.ini，GameMode.ini，最后读取Rules.ini
	return FindFile(fileName)->GetSection(section);
}

INILinkedBuffer* INIReaderManager::FindLinkedBuffer(std::vector<std::string> dependency, std::string section)
{
	const LinkedBufferKey key{ dependency, section };
	auto it = s_LinkedBuffer.find(key);
	if (it != s_LinkedBuffer.end())
	{
		return it->second;
	}
	INILinkedBuffer* linkedBuffer{};
	for (auto iniFileName : dependency)
	{
		INIBuffer* buffer = FindBuffer(iniFileName, section);
		linkedBuffer = GameCreate<INILinkedBuffer>( buffer, linkedBuffer);
	}
	s_LinkedBuffer[key] = linkedBuffer;

	return linkedBuffer;
}

std::vector<INIFileBuffer*> INIReaderManager::s_File = {};
std::map<INIReaderManager::LinkedBufferKey, INILinkedBuffer*> INIReaderManager::s_LinkedBuffer = {};
std::map<INILinkedBuffer, std::map<std::string, INIConfigBase*>> INIReaderManager::s_Configs = {};
