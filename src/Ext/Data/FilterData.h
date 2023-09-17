#pragma once

#include <Ext/Data/FilterData.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIreader.h>

#include <YRPP.h>


class FilterData : public INIConfig
{
public:
	std::vector<std::string> AffectTypes{};
	std::vector<std::string> NotAffectTypes{};

	bool AffectTechno = true;
	bool AffectBuilding = true;
	bool AffectInfantry = true;
	bool AffectUnit = true;
	bool AffectAircraft = true;

	bool AffectBullet = true;
	bool AffectMissile = true;
	bool AffectTorpedo = true;
	bool AffectCannon = true;
	bool AffectBomb = true;

	bool AffectStand = false;
	bool AffectSelf = false;
	bool AffectInAir = true;
	std::vector<std::string> NotAffectMarks{};
	std::vector<std::string> OnlyAffectMarks{};

	bool AffectsOwner = true;
	bool AffectsAllies = true;
	bool AffectsEnemies = true;
	bool AffectsCivilian = true;

	virtual void Read(INIBufferReader* ini) override
	{ }

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		AffectTypes = reader->GetList<std::string>(title + "AffectTypes", AffectTypes);
		NotAffectTypes = reader->GetList<std::string>(title + "NotAffectTypes", NotAffectTypes);

		AffectTechno = reader->Get(title + "AffectTechno", AffectTechno);
		AffectBuilding = reader->Get(title + "AffectBuilding", AffectBuilding);
		AffectInfantry = reader->Get(title + "AffectInfantry", AffectInfantry);
		AffectUnit = reader->Get(title + "AffectUnit", AffectUnit);
		AffectAircraft = reader->Get(title + "AffectAircraft", AffectAircraft);
		if (!AffectBuilding && !AffectInfantry && !AffectUnit && !AffectAircraft)
		{
			AffectTechno = false;
		}

		AffectBullet = reader->Get(title + "AffectBullet", AffectBullet);
		AffectMissile = reader->Get(title + "AffectMissile", AffectMissile);
		AffectTorpedo = reader->Get(title + "AffectTorpedo", AffectTorpedo);
		AffectCannon = reader->Get(title + "AffectCannon", AffectCannon);
		AffectBomb = reader->Get(title + "AffectBomb", AffectBomb);
		if (!AffectMissile && !AffectCannon && !AffectBomb)
		{
			AffectBullet = false;
		}

		AffectStand = reader->Get(title + "AffectStand", AffectStand);
		AffectSelf = reader->Get(title + "AffectSelf", AffectSelf);
		AffectInAir = reader->Get(title + "AffectInAir", AffectInAir);
		NotAffectMarks = reader->GetList(title + "NotAffectMarks", NotAffectMarks);
		OnlyAffectMarks = reader->GetList(title + "OnlyAffectMarks", OnlyAffectMarks);

		bool affectsAllies = true;
		if (reader->TryGet(title + "AffectsAllies", affectsAllies))
		{
			AffectsAllies = affectsAllies;
			AffectsOwner = affectsAllies;
		}

		bool affectsOwner = true;
		if (reader->TryGet(title + "AffectsOwner", affectsOwner))
		{
			AffectsOwner = affectsOwner;
		}
		AffectsEnemies = reader->Get(title + "AffectsEnemies", AffectsEnemies);
		AffectsCivilian = reader->Get(title + "AffectsCivilian", AffectsCivilian);
	}
};
