#pragma once

#include <map>
#include <string>

#include "INIBuffer.h"
#include "INIConfig.h"

class INIBufferReader;
class INIConfig;

class INIConfigManager
{
public:
	static void ClearBuffer(EventSystem* sender, Event e, void* args)
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

	template <typename TConfig>
	static TConfig* FindConfig(INILinkedBuffer& buffer, INIBufferReader* reader)
	{
		std::map<std::string, INIConfig*> configs{};
		auto it = s_Configs.find(buffer);
		if (it != s_Configs.end())
		{
			configs = it->second;
		}
		std::string key = typeid(TConfig).name();
		auto ite = configs.find(key);
		if (ite != configs.end())
		{
			return (TConfig*)ite->second;
		}
		// 实例化Config对象
		TConfig* config = GameCreate<TConfig>();
		config->Read(reader);
		configs[key] = config;
		s_Configs[buffer] = configs;
		return config;
	}

private:
	// INILinkedBuffer里的未转换的kv对转换成对象后，用类名存储
	inline static std::map<INILinkedBuffer, std::map<std::string, INIConfig*>> s_Configs{};

};
