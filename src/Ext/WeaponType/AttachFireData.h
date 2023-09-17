#pragma once

#include <Ext/Data/FilterData.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <YRPP.h>


class AttachFireData : public FilterData
{
public:
	bool UseROF = true;
	bool CheckRange = false;
	bool CheckAA = false;
	bool CheckAG = false;
	bool CheckVersus = true;

	bool RadialFire = false;
	int RadialAngle = 180;

	bool RadialZ = true;

	bool SimulateBurst = false;
	int SimulateBurstDelay = 7;
	int SimulateBurstMode = 0;

	bool Feedback = false;

	bool OnlyFireInTransport = false;
	bool UseAlternateFLH = false;

	bool CheckShooterHP = false;
	Vector2D<double> OnlyFireWhenHP = Vector2D<double>::Empty;
	bool CheckTargetHP = false;
	Vector2D<double> OnlyFireWhenTargetHP = Vector2D<double>::Empty;

	bool AffectTerrain = true;

	virtual void Read(INIBufferReader* reader) override
	{
		std::string title = "AttachFire.";

		FilterData::Read(reader, title);

		UseROF = reader->Get(title + "UseROF", UseROF);
		CheckRange = reader->Get(title + "CheckRange", CheckRange);
		CheckAA = reader->Get(title + "CheckAA", CheckAA);
		CheckAG = reader->Get(title + "CheckAG", CheckAG);
		CheckVersus = reader->Get(title + "CheckVersus", CheckVersus);

		RadialFire = reader->Get(title + "RadialFire", RadialFire);
		RadialAngle = reader->Get(title + "RadialAngle", RadialAngle);
		RadialZ = reader->Get(title + "RadialZ", RadialZ);

		SimulateBurst = reader->Get(title + "SimulateBurst", SimulateBurst);
		SimulateBurstDelay = reader->Get(title + "SimulateBurstDelay", SimulateBurstDelay);
		SimulateBurstMode = reader->Get(title + "SimulateBurstMode", SimulateBurstMode);

		Feedback = reader->Get(title + "Feedback", Feedback);

		OnlyFireInTransport = reader->Get(title + "OnlyFireInTransport", OnlyFireInTransport);
		UseAlternateFLH = reader->Get(title + "UseAlternateFLH", UseAlternateFLH);

		std::vector<double> onlyFireWhenHP{};
		if (reader->TryGetList<double>(title + "OnlyFireWhenHP", onlyFireWhenHP) && onlyFireWhenHP.size() > 1)
		{
			double min = onlyFireWhenHP[0];
			double max = onlyFireWhenHP[1];
			if (min > max)
			{
				double temp = min;
				min = max;
				max = temp;
			}
			OnlyFireWhenHP = { min, max };
			CheckShooterHP = !OnlyFireWhenHP.IsEmpty();
		}

		std::vector<double> onlyFireWhenTargetHP{};
		if (reader->TryGetList<double>(title + "OnlyFireWhenTargetHP", onlyFireWhenTargetHP) && onlyFireWhenTargetHP.size() > 1)
		{
			double min = onlyFireWhenTargetHP[0];
			double max = onlyFireWhenTargetHP[1];
			if (min > max)
			{
				double temp = min;
				min = max;
				max = temp;
			}
			OnlyFireWhenTargetHP = { min, max };
			CheckTargetHP = !OnlyFireWhenTargetHP.IsEmpty();
		}

		AffectTerrain = reader->Get(title + "AffectsTerrain", AffectTerrain);
		AffectsAllies = reader->Get(title + "AffectsAllies", AffectsAllies);
		AffectsOwner = reader->Get(title + "AffectsOwner", AffectsAllies);
		AffectsEnemies = reader->Get(title + "AffectsEnemies", AffectsEnemies);
		AffectsCivilian = reader->Get(title + "AffectsCivilian", AffectsCivilian);
	}

};
