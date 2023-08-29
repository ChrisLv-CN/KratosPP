#include "INI.h"

void INI::ClearBuffer(EventSystem* sender, Event e, void* args)
{
	INIReaderManager::ClearBuffer(sender, e, args);
	s_Rules.clear();
	s_Art.clear();
	s_AI.clear();
}

bool INI::ICaseCompare(std::string_view a, std::string_view b)
{
	if (a.length() == b.length())
	{
		return std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
	}
	return false;
}

Dependency INI::GetDependency(std::string_view iniFileName)
{
	std::string fileName = iniFileName.data();
	auto it = s_Dependency.find(fileName);
	if (it != s_Dependency.end())
	{
		return it->second;
	}
	std::string rulesName = INIConstant::GetRulesName().data();
	if (ICaseCompare(fileName, rulesName))
	{
		if (SessionClass::Instance->GameMode == GameMode::Campaign)
		{
			// Map.ini, rulesmd.ini
			const std::vector<std::string> d{ INIConstant::GetMapName().data(), rulesName };
			s_Dependency[rulesName] = d;
			return d;
		}
		// Map.ini, GameMode.ini, rulesmd.ini
		const std::vector<std::string> d{ INIConstant::GetMapName().data(), INIConstant::GetGameModeName().data(), rulesName };
		s_Dependency[rulesName] = d;
		return d;
	}
	std::string artName = INIConstant::GetArtName().data();
	if (ICaseCompare(fileName, artName))
	{
		// artmd.ini
		const std::vector<std::string> d{ artName };
		s_Dependency[artName] = d;
		return d;
	}
	std::string aiName = INIConstant::GetAIName().data();
	if (ICaseCompare(fileName, aiName))
	{
		// Map.ini, aimd.ini
		const std::vector<std::string> d{ INIConstant::GetMapName().data(), aiName };
		s_Dependency[aiName] = d;
		return d;
	}
	const std::vector<std::string> d{ fileName };
	return d;
}

Dependency INI::GetRules()
{
	if (s_Rules.empty())
	{
		s_Rules = GetDependency(INIConstant::GetRulesName());
	}
	return s_Rules;
}

Dependency INI::GetArt()
{
	if (s_Art.empty())
	{
		s_Art = GetDependency(INIConstant::GetArtName());
	}
	return s_Art;
}

Dependency INI::GetAI()
{
	if (s_AI.empty())
	{
		s_AI = GetDependency(INIConstant::GetAIName());
	}
	return s_AI;
}

std::map<std::string, Dependency> INI::s_Dependency{};

GetDependency INI::Rules = INI::GetRules;
GetDependency INI::Art = INI::GetArt;
GetDependency INI::AI = INI::GetAI;

Dependency INI::s_Rules{};
Dependency INI::s_Art{};
Dependency INI::s_AI{};
