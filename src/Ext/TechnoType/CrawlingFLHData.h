#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class CrawlingFLHData : public INIConfig
{
public:
	CoordStruct PrimaryFireFLH = CoordStruct::Empty;
	CoordStruct PrimaryCrawlingFLH = CoordStruct::Empty;
	CoordStruct ElitePrimaryFireFLH = CoordStruct::Empty;
	CoordStruct ElitePrimaryCrawlingFLH = CoordStruct::Empty;
	CoordStruct SecondaryFireFLH = CoordStruct::Empty;
	CoordStruct SecondaryCrawlingFLH = CoordStruct::Empty;
	CoordStruct EliteSecondaryFireFLH = CoordStruct::Empty;
	CoordStruct EliteSecondaryCrawlingFLH = CoordStruct::Empty;

	virtual void Read(INIBufferReader* reader) override
	{
		PrimaryFireFLH = reader->Get<CoordStruct>("PrimaryFireFLH", CoordStruct::Empty);
		ElitePrimaryFireFLH = reader->Get<CoordStruct>("ElitePrimaryFireFLH", PrimaryFireFLH);

		CoordStruct primaryCrawlingDefalut = PrimaryFireFLH;
		primaryCrawlingDefalut.Z = 20;
		PrimaryCrawlingFLH = reader->Get<CoordStruct>("PrimaryCrawlingFLH", primaryCrawlingDefalut);
		CoordStruct elitePrimaryCrawlingDefalut = ElitePrimaryFireFLH;
		elitePrimaryCrawlingDefalut.Z = 20;
		ElitePrimaryCrawlingFLH = reader->Get<CoordStruct>("ElitePrimaryCrawlingFLH", elitePrimaryCrawlingDefalut);

		SecondaryFireFLH = reader->Get<CoordStruct>("SecondaryFireFLH", CoordStruct::Empty);
		EliteSecondaryFireFLH = reader->Get<CoordStruct>("EliteSecondaryFireFLH", SecondaryFireFLH);

		CoordStruct secondaryCrawlingDefalut = SecondaryFireFLH;
		secondaryCrawlingDefalut.Z = 20;
		SecondaryCrawlingFLH = reader->Get<CoordStruct>("SecondaryCrawlingFLH", secondaryCrawlingDefalut);
		CoordStruct eliteSecondaryCrawlingDefalut = EliteSecondaryFireFLH;
		eliteSecondaryCrawlingDefalut.Z = 20;
		EliteSecondaryCrawlingFLH = reader->Get<CoordStruct>("EliteSecondaryFireFLH", eliteSecondaryCrawlingDefalut);
	}

};
