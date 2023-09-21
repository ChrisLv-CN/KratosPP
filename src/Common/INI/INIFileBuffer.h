#pragma once

#include <string>
#include <vector>
#include <map>

#include "INIBuffer.h"

/// <summary>
/// 从指定的ini文件读取所有的KV对，用string储存value
/// </summary>
class INIFileBuffer
{
public:
	std::string FileName{};

	INIFileBuffer() {}
	INIFileBuffer(std::string fileName)
	{
		FileName = fileName;
		InitCache();
	}

	// 游戏开局清空所有的缓存，重新读取
	void ClearBuffer()
	{
		for (auto buffer : m_Section)
		{
			INIBuffer* b = buffer.second;
			b->ClearBuffer();
			GameDelete(b);
		}
		m_Section.clear();
		for (auto cache : m_SectionCache)
		{
			std::vector<Buffer>* pairs = cache.second;
			GameDelete(pairs);
		}
		m_SectionCache.clear();
	}

	bool HasSection(std::string section)
	{
		auto it = m_SectionCache.find(section);
		return it != m_SectionCache.end();
	}

	INIBuffer *GetSection(std::string section)
	{
		auto it = m_Section.find(section);
		if (it != m_Section.end())
		{
			return it->second;
		}
		INIBuffer* buffer = GameCreate<INIBuffer>(FileName, section);
		auto ite = m_SectionCache.find(section);
		if (ite != m_SectionCache.end())
		{
			for (Buffer pair : *ite->second)
			{
				buffer->Unparsed[pair.Key] = pair.Value;
			}
		}
		m_Section[section] = buffer;
		return buffer;
	}

private:
	// 字符串格式的一行KV对
	struct Buffer
	{
		std::string Key;
		std::string Value;
	};

	void InitCache()
	{
		CCINIClass* pINI = GameCreate<CCINIClass>();
		CCFileClass* pFile = GameCreate<CCFileClass>(FileName.c_str());
		INI_EX reader(pINI);
		// 读入ini
		pINI->ReadCCFile(pFile);
		GameDelete(pFile);
		// 用string缓存所有的KV对
		auto pSection = pINI->Sections.First();
		while (pSection)
		{
			char* _section = pSection->Name;
			if (_section)
			{
				int keyCount = pINI->GetKeyCount(_section);
				std::vector<Buffer>* pairs = GameCreate<std::vector<Buffer>>();
				for (int i = 0; i < keyCount; i++)
				{
					const char* k = pINI->GetKeyName(_section, i);
					if (k)
					{
						// deep copy key
						std::string _key(k);
						std::string key = _key.c_str();
						// deep copy value
						if (reader.ReadString(_section, key.c_str()))
						{
							std::string _value(reader.value());
							std::string value = _value.c_str();
							const Buffer b{ key, value };
							pairs->push_back(b);
						}
					}
				}
				// deep copy section
				std::string section = _section;
				m_SectionCache[section] = pairs;
			}
			pSection = pSection->Next();
		}
		GameDelete(pINI);
	}

	/// <summary>
	/// 当需要获取某个section的时候，再读取Cache中的内容存入。
	/// Key是Section
	/// </summary>
	std::map<std::string, INIBuffer *> m_Section{};
	/// <summary>
	/// 从文件中读入ini文件，即刻读取section的所有KV对进行缓存
	/// Key是Section
	/// </summary>
	std::map<std::string, std::vector<Buffer> *> m_SectionCache{};
};
