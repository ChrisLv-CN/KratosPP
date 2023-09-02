#pragma once

#include <map>
#include <string>

#include "INIBuffer.h"
#include "INIConfig.h"

class INIBufferReader;
class INIConfigBase;

class INIConfigManager
{
public:
	static void ClearBuffer(EventSystem *sender, Event e, void *args);

	template <typename TConfig>
	static TConfig *FindConfig(INILinkedBuffer &buffer, INIBufferReader *reader)
	{
		std::map<std::string, INIConfigBase *> configs{};
		auto it = s_Configs.find(buffer);
		if (it != s_Configs.end())
		{
			configs = it->second;
		}
		std::string key = typeid(TConfig).name();
		auto ite = configs.find(key);
		if (ite != configs.end())
		{
			return (TConfig *)ite->second;
		}
		// 实例化Config对象
		TConfig *config = GameCreate<TConfig>();
		config->Read(reader);
		configs[key] = config;
		s_Configs[buffer] = configs;
		return config;
	}

private:
	// INILinkedBuffer里的未转换的kv对转换成对象后，用类名存储
	static std::map<INILinkedBuffer, std::map<std::string, INIConfigBase *>> s_Configs;
};
