#pragma once

#include <map>
#include <vector>
#include <string>

#include "INIReader.h"
#include "INIConfigReader.h"

class INIReaderManager
{
public:
	struct INIReaderKey
	{
		std::vector<std::string> dependency;
		std::string section;

		auto operator<=>(const INIReaderKey &) const = default;
	};

	static void ClearBuffer(EventSystem *sender, Event e, void *args);

	static INIBufferReader *FindBufferReader(std::vector<std::string> dependency, const char *section);

	template <typename TConfig>
	static INIConfigReader<TConfig> *FindConfigReader(std::vector<std::string> dependency, const char *section)
	{
		const INIReaderKey key{dependency, section};
		auto it = s_ConfigReader.find(key);
		if (it != s_ConfigReader.end())
		{
			return (INIConfigReader<TConfig> *)it->second;
		}
		INIBufferReader *bufferReader = FindBufferReader(dependency, section);
		INIConfigReader<TConfig> *reader = GameCreate<INIConfigReader<TConfig>>(dependency, section, bufferReader);
		s_ConfigReader[key] = reader;

		return reader;
	}

private:
	static std::map<INIReaderKey, INIBufferReader *> s_BufferReader;
	static std::map<INIReaderKey, void *> s_ConfigReader;
};
