#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class BaseNormalData : public INIConfig
{
public:
	bool BaseNormal = false; // 做基地建造节点
	bool EligibileForAllyBuilding = false; // 做友军的基地建造节点

	virtual void Read(INIBufferReader* reader) override
	{
		EligibileForAllyBuilding = reader->Get("EligibileForAllyBuilding", EligibileForAllyBuilding);
		BaseNormal = reader->Get("BaseNormal", EligibileForAllyBuilding);

		Enable = BaseNormal;
	}

};
