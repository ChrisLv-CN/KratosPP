#pragma once

#include <string>
#include <map>
#include <vector>

#include <TacticalClass.h>
#include <CCINIClass.h>

#include <Utilities/INIParser.h>
#include <Utilities/TemplateDef.h>

/// <summary>
/// 储存一个Section在一个ini文件中的全部KV对
/// </summary>
class INIBuffer
{
public:
	INIBuffer();
	INIBuffer(std::string fileName, std::string section);

	void ClearBuffer();

	template<typename T>
	bool GetParsed(const char* key, T* val)
	{
		auto it = Parsed.find(key);
		if (it != Parsed.end())
		{
			val = &it->second;
			return true;
		}
		T buffer;
		auto ite = Unparsed.find(key);
		if (ite!= Unparsed.end())
		{
			const char* v = ite->second;
			if (Parser<T, 1>::Parse(v, val) == 1)
			{
				Parsed[key] = val;
				return true;
			}
		}
		return false;
	}

	std::string FileName{};
	std::string Section{};
	// 没有转换类型的kv对
	std::map<std::string, std::string> Unparsed{};
	// 已经装换了类型的kv对
	std::map<std::string, void*> Parsed{};
private:
};

/// <summary>
/// 一个Section在多个ini文件中的KV对，按文件读取顺序连接。
/// 按顺序检索key，并返回首个符合的目标。
/// </summary>
class INILinkedBuffer
{
public:
	INILinkedBuffer();
	INILinkedBuffer(INIBuffer* buffer, INILinkedBuffer* nextBuffer);
	
	std::vector<std::string> GetDependency();
	std::string GetSection();

	void ClearBuffer();
	void Expired();
	bool IsExpired();
	
	/// <summary>
	/// 返回首个持有key的buffer
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	INIBuffer* GetFirstOccurrence(const char* key);
	// 读取未经转换的value
	bool GetUnparsed(const char* key, std::string& val);

	// 将Value转换成指定的type
	template<typename T>
	bool GetParsed(const char* key, T* val)
	{
		std::string buffer;
		if (m_Buffer->GetParsed<T>(key, val))
		{
			return true;
		}
		if (m_LinkedBuffer)
		{
			return m_LinkedBuffer->GetParsed<T>(key, val);
		}
		return false;
	}
	template<typename T>
	bool GetParsedList(const char* key, std::vector<T>* vals)
	{

	}
private:
	bool _expired = false;

	void GetDependency(std::vector<std::string>& dependency);

	INIBuffer* m_Buffer{};
	INILinkedBuffer* m_LinkedBuffer{};
};

