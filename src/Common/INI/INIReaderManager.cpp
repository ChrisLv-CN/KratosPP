#include "INIReaderManager.h"

void INIReaderManager::ClearBuffer(EventSystem* sender, Event e, void* args)
{
	// �ͷ�INIFileBuffer
	for (auto fileBuffer : s_File)
	{
		fileBuffer->ClearBuffer();
	}
	s_File.clear();
	// �ͷ�LinkedBuffer
	for (auto linkedBuffer : s_LinkedBuffer)
	{
		//linkedBuffer.second->Expired();
		GameDelete(linkedBuffer.second);
	}
	s_LinkedBuffer.clear();
	// �ͷ�Config
	for (auto configMap : s_Configs)
	{
		for (auto config : configMap.second)
		{
			// �ͷ�config����
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
	// Ϊÿ��iniFile����һ���洢����
	// INIFileBuffer�ڹ���ʱ���ȡKV�ԣ����ַ�����ʽ����
	std::string f = fileName.data();
	INIFileBuffer* buffer = GameCreate<INIFileBuffer>(f);
	s_File.push_back(buffer);
	return buffer;
}

INIBuffer* INIReaderManager::FindBuffer(std::string fileName, std::string section)
{
	// ini�ļ���˳�򴢴棬�ڲ���ʱ�ȶ�ȡMap.ini��GameMode.ini������ȡRules.ini
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
