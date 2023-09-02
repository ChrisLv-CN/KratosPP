#pragma once

#include "INIBuffer.h"
#include "INIBufferManager.h"
#include "INIConfigManager.h"

#include <CCINIClass.h>

class INIReader
{
public:
	INIReader(std::vector<std::string> dependency, const char *section);

	bool HasSection(const char *section);

	INILinkedBuffer *GetLinkedBuffer();

protected:
	std::vector<std::string> m_Dependency{};
	std::string m_Section{};
	INILinkedBuffer *m_LinkedBuffer = nullptr;
};

/// <summary>
/// 读取指定dependency下的指定section的KV对
/// </summary>
class INIBufferReader : public INIReader
{
public:
	INIBufferReader(std::vector<std::string> dependency, const char *section) : INIReader(dependency, section){};
	INIBufferReader(std::vector<std::string> dependency, const char *section, INILinkedBuffer *linkedBuffer) : INIReader(dependency, section)
	{
		this->m_LinkedBuffer = linkedBuffer;
	}

	template <typename OutType>
	OutType Get(const char *key, const OutType def)
	{
		OutType val{};
		if (TryGet(key, val))
		{
			return val;
		}
		return def;
	}

	template <typename OutType>
	bool TryGet(const char *key, OutType &outValue)
	{
		return GetLinkedBuffer()->GetParsed(key, outValue);
	}

	template <typename OutType>
	std::vector<OutType> GetList(const char *key, std::vector<OutType> def)
	{
		std::vector<OutType> vals{};
		if (TryGetList(key, vals))
		{
			return vals;
		}
		return def;
	}

	template <typename OutType>
	bool TryGetList(const char *key, std::vector<OutType> &outValues)
	{
		return GetLinkedBuffer()->GetParsedList(key, outValues);
	}
};
