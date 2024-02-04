#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class DisguiseData : public INIConfig
{
public:
	std::string DefaultDisguise{ "" };
	std::string DefaultUnitDisguise{ "" };

	virtual void Read(INIBufferReader* reader) override
	{
		DefaultDisguise = reader->Get("DefaultDisguise", DefaultDisguise);

		DefaultUnitDisguise = reader->Get("DefaultUnitDisguise", DefaultUnitDisguise);

		Enable = IsNotNone(DefaultDisguise) || IsNotNone(DefaultUnitDisguise);
	}

};
