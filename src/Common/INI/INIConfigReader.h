#pragma once

#include <string>
#include <vector>

#include "INIReader.h"
#include "INIConfigManager.h"

template <typename TConfig>
class INIConfigReader : public INIReader
{
public:
	INIConfigReader(std::vector<std::string> dependency, const char* section, INIBufferReader* reader) : INIReader(dependency, section)
	{
		this->m_BufferReader = reader;
	};

	TConfig* GetData()
	{
		if (!m_Config)
		{
			// 实例化config
			m_Config = INIConfigManager::template FindConfig<TConfig>(*GetLinkedBuffer(), m_BufferReader);
		}
		if (GetLinkedBuffer()->IsExpired())
		{
			m_Config = nullptr;
			return GetData();
		}
		return m_Config;
	}
	__declspec(property(get = GetData)) TConfig* Data;

private:
	TConfig* m_Config = nullptr;
	INIBufferReader* m_BufferReader = nullptr;
};
