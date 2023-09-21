#pragma once

#include <map>
#include <vector>
#include <string>
#include <typeinfo>

#include "INIReader.h"
#include "INIConfigReader.h"

#include <Common/EventSystems/EventSystem.h>

namespace INIReaderManager
{

	struct INIReaderKey
	{
		std::vector<std::string> dependency;
		std::string section;

		auto operator<=>(const INIReaderKey&) const = default;
	};

	struct INIConfigKey
	{
		std::vector<std::string> dependency;
		std::string section;

		const char* configTypeName;

		auto operator<=>(const INIConfigKey&) const = default;
	};

	static std::map<INIReaderKey, INIBufferReader*> s_BufferReader{};
	static std::map<INIConfigKey, INIReader*> s_ConfigReader{};

	static void ClearBuffer(EventSystem* sender, Event e, void* args)
	{
		// 释放ConfigReader
		for (auto configReader : s_ConfigReader)
		{
			GameDelete(configReader.second);
		}
		s_ConfigReader.clear();
		INIConfigManager::ClearBuffer(sender, e, args);

		// 释放BufferReader
		for (auto bufferReader : s_BufferReader)
		{
			GameDelete(bufferReader.second);
		}
		s_BufferReader.clear();
		INIBufferManager::ClearBuffer(sender, e, args);
	}

	static INIBufferReader* FindBufferReader(std::vector<std::string> dependency, const char* section)
	{
		const INIReaderKey key{ dependency, section };
		auto it = s_BufferReader.find(key);
		if (it != s_BufferReader.end())
		{
			return it->second;
		}
		INIBufferReader* reader = GameCreate<INIBufferReader>(dependency, section);
		s_BufferReader[key] = reader;

		return reader;
	}

	template <typename TConfig>
	static INIConfigReader<TConfig>* FindConfigReader(std::vector<std::string> dependency, const char* section)
	{
		const INIConfigKey key{ dependency, section, typeid(TConfig).name() };
		auto it = s_ConfigReader.find(key);
		if (it != s_ConfigReader.end())
		{
			return (INIConfigReader<TConfig> *)it->second;
		}
		INIBufferReader* bufferReader = FindBufferReader(dependency, section);
		INIConfigReader<TConfig>* reader = GameCreate<INIConfigReader<TConfig>>(dependency, section, bufferReader);
		s_ConfigReader[key] = reader;

		return reader;
	}
};
