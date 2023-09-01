#pragma once

#include "INIBuffer.h"
#include "INIReaderManager.h"


#include <CCINIClass.h>

/// <summary>
/// 读取指定dependency下的指定section的KV对
/// </summary>
class INIBufferReader
{
public:
	INIBufferReader(std::vector<std::string> dependency, const char* section);

	bool HasSection(const char* section);

	template<typename OutType>
	OutType Get(const char* key, const OutType def)
	{
		OutType val{};
		if (TryGet(key, val))
		{
			return val;
		}
		return def;
	}

	template<typename OutType>
	bool TryGet(const char* key, OutType& outValue)
	{
		return GetBuffer()->GetParsed(key, outValue);
	}

	template<typename OutType>
	std::vector<OutType> GetList(const char* key, std::vector<OutType> def)
	{
		std::vector<OutType> vals{};
		if (TryGetList(key, vals))
		{
			return vals;
		}
		return def;
	}

	template<typename OutType>
	bool TryGetList(const char* key, std::vector<OutType>& outValues)
	{
		return GetBuffer()->GetParsedList(key, outValues);
	}

private:
	INILinkedBuffer* GetBuffer();

	std::vector<std::string> m_Dependency;
	std::string m_Section;
	INILinkedBuffer* m_LinkedBuffer;
};
