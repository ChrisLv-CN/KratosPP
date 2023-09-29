#pragma once
#include <string>
#include <vector>
#include <regex>

#include "INIBuffer.h"
#include "INIBufferManager.h"

#include <Ext/Helper/StringEx.h>

class INIReader
{
public:
	inline static std::regex Number{ "^\\d+$" };
	// inline static std::regex PercentFloat{ "^\\d?\\.\\d+$" };
	// inline static std::regex PercentNumber{ "^\\d+$" };
	// inline static std::regex Brackets{ "(?is)(?<=\\()[^\\)]+(?=\\))" };

	std::vector<std::string> Dependency{};
	std::string Section{};

	INIReader(std::vector<std::string> dependency, const char* section)
	{
		this->Dependency = dependency;
		this->Section = section;
	}

	INILinkedBuffer* GetLinkedBuffer()
	{
		if (!m_LinkedBuffer || m_LinkedBuffer->IsExpired())
		{
			m_LinkedBuffer = INIBufferManager::FindLinkedBuffer(Dependency, Section);
		}
		return m_LinkedBuffer;
	}

protected:
	INILinkedBuffer* m_LinkedBuffer = nullptr;
};

/// <summary>
/// 读取指定dependency下的指定section的KV对
/// </summary>
class INIBufferReader : public INIReader
{
public:
	INIBufferReader(std::vector<std::string> dependency, const char* section) : INIReader(dependency, section) {};
	INIBufferReader(std::vector<std::string> dependency, const char* section, INILinkedBuffer* linkedBuffer) : INIReader(dependency, section)
	{
		this->m_LinkedBuffer = linkedBuffer;
	}

	template <typename OutType>
	OutType Get(const char* key, const OutType def)
	{
		OutType val{};
		if (TryGet(key, val))
		{
			return val;
		}
		return def;
	}

	template <typename OutType>
	bool TryGet(const char* key, OutType& outValue)
	{
		return GetLinkedBuffer()->GetParsed(key, outValue);
	}

	template <typename OutType>
	OutType Get(std::string key, const OutType def)
	{
		return Get(key.c_str(), def);
	}

	template <typename OutType>
	bool TryGet(std::string key, OutType& outValue)
	{
		return TryGet(key.c_str(), outValue);
	}

	template <typename OutType>
	std::vector<OutType> GetList(const char* key, std::vector<OutType> def)
	{
		std::vector<OutType> vals{};
		if (TryGetList(key, vals))
		{
			return vals;
		}
		return def;
	}

	template <typename OutType>
	bool TryGetList(const char* key, std::vector<OutType>& outValues)
	{
		return GetLinkedBuffer()->GetParsedList(key, outValues);
	}

	template <typename OutType>
	std::vector<OutType> GetList(std::string key, std::vector<OutType> def)
	{
		return GetList(key.c_str(), def);
	}

	template <typename OutType>
	bool TryGetList(std::string key, std::vector<OutType>& outValues)
	{
		return TryGetList(key.c_str(), outValues);
	}

#pragma region Special types
	int GetDir16(std::string key, const int def)
	{
		std::string tempDef{ "" };
		std::string v = Get<std::string>(key, tempDef);
		if (!IsNotNone(v))
		{
			int value = 0;
			// 是数字格式
			if (std::regex_match(v, Number))
			{
				int buffer = 0;
				const char* pFmt = "%d";
				if (sscanf_s(v.c_str(), pFmt, &value) == 1)
				{
					value = buffer;
				}
				if (value > 15)
				{
					return value % 16;
				}
			}
			else
			{
				std::string dirStr = uppercase(v);
				if (dirStr == "N" || dirStr == "NORTH")
				{
					value = 0;
				}
				else if (dirStr == "NE" || dirStr == "NORTHEAST")
				{
					value = 2;
				}
				else if (dirStr == "E" || dirStr == "EAST")
				{
					value = 4;
				}
				else if (dirStr == "SE" || dirStr == "SOUTHEAST")
				{
					value = 6;
				}
				else if (dirStr == "S" || dirStr == "SOUTH")
				{
					value = 8;
				}
				else if (dirStr == "SW" || dirStr == "SOUTHWEST")
				{
					value = 10;
				}
				else if (dirStr == "W" || dirStr == "WEST")
				{
					value = 12;
				}
				else if (dirStr == "NW" || dirStr == "NORTHWEST")
				{
					value = 14;
				}
			}
			return value;
		}
		return def;
	}

	double GetChance(std::string key, const double def)
	{
		double v = Get(key, def);
		if (v < 0)
		{
			v = 0;
		}
		else if (v > 1)
		{
			v = 1;
		}
		return v;
	}

	std::vector<double> GetChanceList(std::string key, std::vector<double> def)
	{
		std::vector<double> v = GetList(key.c_str(), def);
		if (!v.empty())
		{
			for (double& vv : v)
			{
				if (vv < 0)
				{
					vv = 0;
				}
				else if (vv > 1)
				{
					vv = 1;
				}
			}
		}
		return v;
	}
#pragma endregion

};
