#pragma once

#include <typeinfo>

#include "INIReader.h"

class INIBufferReader;

class INIConfig
{
public:
	virtual void Read(INIBufferReader *ini) = 0;

};
