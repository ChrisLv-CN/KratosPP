#pragma once

#include <typeinfo>

#include <Utilities/INIParser.h>
#include <Utilities/TemplateDef.h>

class ConfigReader
{

};

class INIConfigBase
{
public:
	virtual void Read(INI_EX ini) = 0;

};

template<typename T>
class INIConfig : public INIConfigBase
{
	std::string Name = typeid(T).name();
};

class INIConfigManager
{

};
