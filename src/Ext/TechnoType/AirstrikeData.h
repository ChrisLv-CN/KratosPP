#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/CommonStatus.h>

class AirstrikeData : public INIConfig
{
public:

	bool AirstrikeDisableLine = false;
	bool AirstrikeDisableColor = false;
	bool AirstrikeDisableBlink = false;

	virtual void Read(INIBufferReader* reader) override
	{
		AirstrikeDisableLine = reader->Get("AirstrikeDisableLine", AirstrikeDisableLine);
		AirstrikeDisableColor = reader->Get("AirstrikeDisableColor", AirstrikeDisableColor);
		AirstrikeDisableBlink = reader->Get("AirstrikeDisableBlink", AirstrikeDisableBlink);
	}
};
