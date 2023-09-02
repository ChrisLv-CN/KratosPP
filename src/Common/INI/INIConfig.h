#pragma once

#include <typeinfo>

#include "INIReader.h"

#include <Utilities/INIParser.h>
#include <Utilities/TemplateDef.h>

class INIBufferReader;

class INIConfigBase
{
public:
	virtual void Read(INIBufferReader *ini) = 0;

};

template<typename T>
class INIConfig : public INIConfigBase
{
public:
	std::string Name = typeid(T).name();
};

