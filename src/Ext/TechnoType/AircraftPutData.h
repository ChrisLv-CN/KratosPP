#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/CommonStatus.h>

class AircraftPutData : public GeneralData
{
public:

	virtual void Read(INIBufferReader* reader) override
	{
		INIBufferReader* generalReader = INI::GetSection(INI::Rules, INI::SectionGeneral);
		GeneralData::Read(generalReader);

		NoHelipadPutOffset = reader->Get("NoHelipadPutOffset", NoHelipadPutOffset);

		ForcePutOffset = reader->Get("ForcePutOffset", ForcePutOffset);

		Enable = !NoHelipadPutOffset.IsEmpty();
	}

	/**
	 * @brief 检查飞机是不是需要机场
	 *
	 * @param aircraftId
	 * @return true
	 * @return false
	 */
	bool NeedPad(std::string aircraftId)
	{
		if (PadAircraft.empty())
		{
			return false;
		}
		auto it = std::find(PadAircraft.begin(), PadAircraft.end(), aircraftId);
		return it != PadAircraft.end();
	}
};
