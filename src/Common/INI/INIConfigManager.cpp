#include "INIConfigManager.h"

void INIConfigManager::ClearBuffer(EventSystem* sender, Event e, void* args)
{
	// 释放Config
	for (auto configMap : s_Configs)
	{
		for (auto config : configMap.second)
		{
			// 释放config对象
			GameDelete(config.second);
		}
		configMap.second.clear();
	}
	s_Configs.clear();
}

std::map<INILinkedBuffer, std::map<std::string, INIConfigBase*>> INIConfigManager::s_Configs = {};
