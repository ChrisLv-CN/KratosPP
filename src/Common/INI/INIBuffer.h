#pragma once

#include <string>
#include <map>
#include <vector>

#include <TacticalClass.h>
#include <CCINIClass.h>

#include <Utilities/INIParser.h>
#include <Utilities/TemplateDef.h>

/// <summary>
/// ����һ��Section��һ��ini�ļ��е�ȫ��KV��
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
	// û��ת�����͵�kv��
	std::map<std::string, std::string> Unparsed{};
	// �Ѿ�װ�������͵�kv��
	std::map<std::string, void*> Parsed{};
private:
};

/// <summary>
/// һ��Section�ڶ��ini�ļ��е�KV�ԣ����ļ���ȡ˳�����ӡ�
/// ��˳�����key���������׸����ϵ�Ŀ�ꡣ
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
	/// �����׸�����key��buffer
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	INIBuffer* GetFirstOccurrence(const char* key);
	// ��ȡδ��ת����value
	bool GetUnparsed(const char* key, std::string& val);

	// ��Valueת����ָ����type
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

