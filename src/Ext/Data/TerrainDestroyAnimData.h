#pragma once

#include <Common/INI/INIConfig.h>
#include <Ext/ExpandAnims/ExpandAnimsData.h>

class TerrainDestroyAnimData : public ExpandAnimsData
{
public:
	virtual void Read(INIBufferReader* reader) override
	{
		ExpandAnimsData::Read(reader, TITLE);
	}
private:
	inline static std::string TITLE = "Destroy.";
};
