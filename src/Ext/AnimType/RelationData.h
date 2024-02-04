#pragma once

#include <Common/INI/INIConfig.h>

enum class Relation : unsigned int
{
	NONE = 0x0, OWNER = 0x1, ALLIES = 0x2, ENEMIES = 0x4,

	Team = OWNER | ALLIES,
	NotAllies = OWNER | ENEMIES,
	NotOwner = ALLIES | ENEMIES,
	All = OWNER | ALLIES | ENEMIES
};

MAKE_ENUM_FLAGS(Relation);

template <>
inline bool Parser<Relation>::TryParse(const char* pValue, Relation* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'O':
		if (outValue)
		{
			*outValue = Relation::OWNER;
		}
		return true;
	case 'T':
		if (outValue)
		{
			*outValue = Relation::Team;
		}
		return true;
	case 'E':
		if (outValue)
		{
			*outValue = Relation::ENEMIES;
		}
		return true;
	}
	return false;
}

class AnimVisibilityData : public INIConfig
{
public:
	Relation Visibility = Relation::All;

	virtual void Read(INIBufferReader* reader) override
	{
		Visibility = reader->Get("Visibility", Visibility);
	}

};

