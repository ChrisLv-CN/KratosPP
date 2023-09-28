#pragma once

#include <string>
#include <map>
#include <vector>
#include <any>
#include <typeinfo>
#include <type_traits>

#include <GeneralDefinitions.h>
#include <GeneralStructures.h>

#include <Ext/Helper/StringEx.h>
#include <Utilities/Parser.h>

template <>
inline bool Parser<std::string>::TryParse(const char* pValue, std::string* outValue)
{
	outValue->assign(pValue);
	return true;
};

template <>
inline bool Parser<short>::TryParse(const char* pValue, short* outValue)
{
	const char* pFmt = nullptr;
	if (*pValue == '$') {
		pFmt = "$%d";
	}
	else if (tolower(static_cast<unsigned char>(pValue[strlen(pValue) - 1])) == 'h') {
		pFmt = "%xh";
	}
	else {
		pFmt = "%d";
	}

	short buffer = 0;
	if (sscanf_s(pValue, pFmt, &buffer) == 1) {
		if (outValue) {
			*outValue = buffer;
		}
		return true;
	}
	return false;
};

/*
enum class Mission : int
{
	None = -1,
	Sleep = 0,
	Attack = 1,
	Move = 2,
	QMove = 3,
	Retreat = 4,
	Guard = 5,
	Sticky = 6,
	Enter = 7,
	Capture = 8,
	Eaten = 9,
	Harvest = 10,
	Area_Guard = 11,
	Return = 12,
	Stop = 13,
	Ambush = 14,
	Hunt = 15,
	Unload = 16,
	Sabotage = 17,
	Construction = 18,
	Selling = 19,
	Repair = 20,
	Rescue = 21,
	Missile = 22,
	Harmless = 23,
	Open = 24,
	Patrol = 25,
	ParadropApproach = 26,
	ParadropOverfly = 27,
	Wait = 28,
	AttackMove = 29,
	SpyplaneApproach = 30,
	SpyplaneOverfly = 31
};*/

static std::map<std::string, Mission> MissionTypeStrings
{
	{ "none", Mission::None },
	{ "sleep", Mission::Sleep },
	{ "attack", Mission::Attack },
	{ "move", Mission::Move },
	{ "qmove", Mission::QMove },
	{ "retreat", Mission::Retreat },
	{ "sticky", Mission::Sticky },
	{ "enter", Mission::Enter },
	{ "capture", Mission::Capture },
	{ "eaten", Mission::Eaten },
	{ "harvest", Mission::Harvest },
	{ "area_guard", Mission::Area_Guard },
	{ "return", Mission::Return },
	{ "stop", Mission::Stop },
	{ "ambush", Mission::Ambush },
	{ "hunt", Mission::Hunt },
	{ "unload", Mission::Unload },
	{ "sabotage", Mission::Sabotage },
	{ "construction", Mission::Construction },
	{ "selling", Mission::Selling },
	{ "repair", Mission::Repair },
	{ "rescue", Mission::Rescue },
	{ "missile", Mission::Missile },
	{ "harmless", Mission::Harmless },
	{ "open", Mission::Open },
	{ "patrol", Mission::Patrol },
	{ "paradropApproach", Mission::ParadropApproach },
	{ "paradropOverfly", Mission::ParadropOverfly },
	{ "wait", Mission::Wait },
	{ "attackMove", Mission::AttackMove },
	{ "spyplaneApproach", Mission::SpyplaneApproach },
	{ "spyplaneOverfly", Mission::SpyplaneOverfly }
};

template <>
inline bool Parser<Mission>::TryParse(const char* pValue, Mission* outValue)
{
	std::string key = lowercase(std::string(pValue));
	auto it = MissionTypeStrings.find(key);
	if (it != MissionTypeStrings.end())
	{
		outValue = &it->second;
		return true;
	}
	return false;
}


/// <summary>
/// 储存一个Section在一个ini文件中的全部KV对
/// </summary>
class INIBuffer
{
public:
	INIBuffer() {}
	INIBuffer(std::string fileName, std::string section)
	{
		this->FileName = fileName;
		this->Section = section;
	}

	void ClearBuffer()
	{
		Unparsed.clear();
		Parsed.clear();
	}

	bool GetUnparsed(std::string key, std::string& outValue)
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
	bool GetParsed(std::string key, OutType& outValue)
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
	inline size_t Parse(const char* value, OutType* outValue)
	{
		return Parser<OutType>::Parse(value, outValue);
	}

	template <>
	inline size_t Parse<CoordStruct>(const char* value, CoordStruct* outValue)
	{
		return Parser<int, 3>::Parse(value, (int*)outValue);
	}

	template <>
	inline size_t Parse<ColorStruct>(const char* value, ColorStruct* outValue)
	{
		return Parser<BYTE, 3>::Parse(value, (BYTE*)outValue);
	}

	// BulletVelocity
	template <>
	inline size_t Parse<Vector3D<double>>(const char* value, Vector3D<double>* outValue)
	{
		return Parser<double, 3>::Parse(value, (double*)outValue);
	}

	template <>
	inline size_t Parse<Point2D>(const char* value, Point2D* outValue)
	{
		return Parser<int, 2>::Parse(value, (int*)outValue);
	}

	template <>
	inline size_t Parse<Vector2D<double>>(const char* value, Vector2D<double>* outValue)
	{
		return Parser<double, 2>::Parse(value, (double*)outValue);
	}

	// CellStruct
	template<>
	inline size_t Parse<Vector2D<short>>(const char* value, Vector2D<short>* outValue)
	{
		return Parser<short, 2>::Parse(value, (short*)outValue);
	}

	template <typename OutType>
	bool GetParsedList(std::string key, std::vector<OutType>& outValues)
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
			char str[_readLength];
			size_t length = v.copy(str, std::string::npos);
			str[length] = '\0';
			char* context = nullptr;
			std::vector<OutType> values = {};
			for (auto pCur = strtok_s(str, _readDelims, &context); pCur; pCur = strtok_s(nullptr, _readDelims, &context))
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
	static const size_t _readLength = 2048;
	const char _readDelims[4] = ",";
};

/// <summary>
/// 一个Section在多个ini文件中的KV对，按文件读取顺序连接。
/// 按顺序检索key，并返回首个符合的目标。
/// </summary>
class INILinkedBuffer
{
public:
	INILinkedBuffer() {}
	INILinkedBuffer(INIBuffer* buffer, INILinkedBuffer* nextBuffer)
	{
		m_Buffer = buffer;
		m_LinkedBuffer = nextBuffer;
	}

	auto operator<=>(const INILinkedBuffer&) const = default;

	std::vector<std::string> GetDependency()
	{
		std::vector<std::string> dependency;
		GetDependency(dependency);
		return dependency;
	}

	std::string GetSection()
	{
		return m_Buffer->Section;
	}

	void ClearBuffer() {}
	void Expired()
	{
		this->_expired = true;
	}
	bool IsExpired()
	{
		return this->_expired;
	}

	/// <summary>
	/// 返回首个持有key的buffer
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	INIBuffer* GetFirstOccurrence(const char* key)
	{
		auto it = m_Buffer->Unparsed.find(key);
		if (it != m_Buffer->Unparsed.end())
		{
			return m_Buffer;
		}
		if (m_LinkedBuffer)
		{
			return m_LinkedBuffer->GetFirstOccurrence(key);
		}
		return nullptr;
	}

	// 读取未经转换的value
	bool GetUnparsed(const char* key, std::string& val)
	{
		if (m_Buffer->GetUnparsed(key, val))
		{
			return true;
		}
		if (m_LinkedBuffer)
		{
			return m_LinkedBuffer->GetUnparsed(key, val);
		}
		return false;
	}

	// 将Value转换成指定的type
	template <typename T>
	bool GetParsed(const char* key, T& outValue)
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
	bool GetParsedList(const char* key, std::vector<T>& outValue)
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

	void GetDependency(std::vector<std::string>& dependency)
	{
		dependency.push_back(m_Buffer->FileName);
		if (m_LinkedBuffer)
		{
			m_LinkedBuffer->GetDependency(dependency);
		}
	}

	INIBuffer* m_Buffer{};
	INILinkedBuffer* m_LinkedBuffer{};
};
