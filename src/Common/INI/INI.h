#pragma once

#include "INIReaderManager.h"

#include <string>
#include <vector>

#include <Common/EventSystems/EventSystem.h>

using Dependency = std::vector<std::string>;
using GetDependency = Dependency(*)();

/// <summary>
/// INI操作类，读取配置信息
/// INI.GetConfig<T>(INI.Rules, section);
/// </summary>
class INI
{
public:
	static void ClearBuffer(EventSystem* sender, Event e, void* args);

	static bool HasSection(Dependency dependency, const char* section)
	{
		return (new INIBufferReader(dependency, section))->HasSection(section);
	}

	static INIBufferReader* GetSection(GetDependency dependency, const char* section)
	{
		// 序列化并储存
		INIBufferReader* reader = new INIBufferReader(dependency(), section);
		return reader;
	}

	template<typename T>
	static T* GetConfig(GetDependency dependency, const char* section)
	{
		Dependency d = dependency();
		return nullptr;
	}

	static GetDependency Rules;
	static GetDependency Art;
	static GetDependency AI;
private:
	static bool ICaseCompare(std::string_view a, std::string_view b);
	
	/// <summary>
	/// 获取ini文件名列表，如：
	/// rules = [Map.ini, GameMode.ini, Rulesmd.ini]
	/// art = [artmd.ini]
	/// ai = [Map.ini, Aimd.ini]
	/// </summary>
	/// <param name="iniName"></param>
	/// <returns></returns>
	static Dependency GetDependency(std::string_view iniFileName);

	// ini文件名，文件名列表
	static std::map<std::string, Dependency> s_Dependency;

	static Dependency GetRules();
	static Dependency GetArt();
	static Dependency GetAI();

	static Dependency s_Rules;
	static Dependency s_Art;
	static Dependency s_AI;
};

