#pragma once

#include <typeinfo>

#include "INIReader.h"

#include <Utilities/INIParser.h>
#include <Utilities/TemplateDef.h>

class INIBufferReader;

class INIConfig
{
public:
	virtual void Read(INIBufferReader *ini) = 0;

};
