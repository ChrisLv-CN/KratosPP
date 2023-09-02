#pragma once

#include <string>
#include <map>
#include <vector>
#include <any>
#include <typeinfo>
#include <type_traits>

#include <GeneralStructures.h>
#include <BulletClass.h>

#include <Utilities/INIParser.h>
#include <Utilities/TemplateDef.h>

template <>
inline bool Parser<std::string>::TryParse(const char *pValue, std::string *outValue)
{
	outValue->assign(pValue);
	return true;
}

/// <summary>
/// 储存一个Section在一个ini文件中的全部KV对
/// </summary>
class INIBuffer
{
public:
	INIBuffer();
	INIBuffer(std::string fileName, std::string section);

	void ClearBuffer();

	bool GetUnparsed(std::string key, std::string &outValue)
	{
		auto it = Unparsed.find(key);
		if (it != Unparsed.end())
		{
			outValue = it->second;
			return true;
		}
		return false;
	}

	template <typename OutType>
	bool GetParsed(std::string key, OutType &outValue)
	{
		auto it = Parsed.find(key);
		if (it != Parsed.end())
		{
			outValue = std::any_cast<OutType>(it->second);
			return true;
		}
		auto ite = Unparsed.find(key);
		if (ite != Unparsed.end())
		{
			std::string value = ite->second;
			OutType buffer = {};
			if (Parse<OutType>(value.c_str(), &buffer))
			{
				std::any v = buffer;
				Parsed[key] = v;
				outValue = buffer;
				return true;
			}
		}
		return false;
	}

	// ----------------
	// 类型转换模板
	// ----------------

	template <typename OutType>
	inline size_t Parse(const char *value, OutType *outValue)
	{
		return Parser<OutType>::Parse(value, outValue);
	}

	template <>
	inline size_t Parse<CoordStruct>(const char *value, CoordStruct *outValue)
	{
		return Parser<int, 3>::Parse(value, (int *)outValue);
	}

	template <>
	inline size_t Parse<ColorStruct>(const char *value, ColorStruct *outValue)
	{
		return Parser<BYTE, 3>::Parse(value, (BYTE *)outValue);
	}
	template <>
	inline size_t Parse<BulletVelocity>(const char *value, BulletVelocity *outValue)
	{
		return Parser<double, 3>::Parse(value, (double *)outValue);
	}

	template <>
	inline size_t Parse<Point2D>(const char *value, Point2D *outValue)
	{
		return Parser<int, 2>::Parse(value, (int *)outValue);
	}

	/*
	template<>
	inline size_t Parse<CellStruct>(const char* value, CellStruct* outValue)
	{
		return Parser<short, 2>::Parse(value, (short*)outValue);
	}
	*/
	template <typename OutType>
	bool GetParsedList(std::string key, std::vector<OutType> &outValues)
	{
		auto it = Parsed.find(key);
		if (it != Parsed.end())
		{
			outValues = std::any_cast<std::vector<OutType>>(it->second);
			return true;
		}
		auto ite = Unparsed.find(key);
		if (ite != Unparsed.end())
		{
			std::string v = ite->second;
			char str[Common::readLength];
			size_t length = v.copy(str, std::string::npos);
			str[length] = '\0';
			char *context = nullptr;
			std::vector<OutType> values = {};
			for (auto pCur = strtok_s(str, Common::readDelims, &context); pCur; pCur = strtok_s(nullptr, Common::readDelims, &context))
			{
				OutType buffer = {};
				if (Parser<OutType>::Parse(pCur, &buffer))
				{
					values.push_back(buffer);
				}
			}
			if (!values.empty())
			{
				std::any any = values;
				Parsed[key] = any;
				outValues = values;
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
	std::map<std::string, std::any> Parsed{};

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
	INILinkedBuffer(INIBuffer *buffer, INILinkedBuffer *nextBuffer);

	auto operator<=>(const INILinkedBuffer &) const = default;

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
	INIBuffer *GetFirstOccurrence(const char *key);

	// 读取未经转换的value
	bool GetUnparsed(const char *key, std::string &val);

	// 将Value转换成指定的type
	template <typename T>
	bool GetParsed(const char *key, T &outValue)
	{
		if (m_Buffer->GetParsed<T>(key, outValue))
		{
			return true;
		}
		if (m_LinkedBuffer)
		{
			return m_LinkedBuffer->GetParsed<T>(key, outValue);
		}
		return false;
	}

	// 读取多次value
	template <typename T>
	bool GetParsedList(const char *key, std::vector<T> &outValue)
	{
		if (m_Buffer->GetParsedList<T>(key, outValue))
		{
			return true;
		}
		if (m_LinkedBuffer)
		{
			return m_LinkedBuffer->GetParsedList<T>(key, outValue);
		}
		return false;
	}

private:
	bool _expired = false;

	void GetDependency(std::vector<std::string> &dependency);

	INIBuffer *m_Buffer{};
	INILinkedBuffer *m_LinkedBuffer{};
};
