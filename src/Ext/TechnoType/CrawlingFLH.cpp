#include "../TechnoStatus.h"

#include <InfantryClass.h>

#include <Ext/Helper.h>


CrawlingFLHData* TechnoStatus::GetCrawlingFLHData()
{
	if (!_crawlingFLHData)
	{
		// 获取image
		const char* section = pTechno->GetTechnoType()->ID;
		std::string image = INI::GetSection(INI::Rules, section)->Get("Image", std::string{ section });
		_crawlingFLHData = INI::GetConfig<CrawlingFLHData>(INI::Art, image.c_str())->Data;
	}
	return _crawlingFLHData;
}

void TechnoStatus::OnUpdate_CrawlingFLH()
{
	if (IsInfantry() && !static_cast<InfantryClass*>(pTechno)->Type->Fearless)
	{
		WeaponStruct* primary = pTechno->GetWeapon(0);
		WeaponStruct* secondary = pTechno->GetWeapon(1);

		bool isElite = pTechno->Veterancy.IsElite();
		if (static_cast<InfantryClass*>(pTechno)->Crawling)
		{
			if (isElite)
			{
				if (primary)
				{
					primary->FLH = GetCrawlingFLHData()->ElitePrimaryCrawlingFLH;
				}
				if (secondary)
				{
					secondary->FLH = GetCrawlingFLHData()->EliteSecondaryCrawlingFLH;
				}
			}
			else
			{
				if (primary)
				{
					primary->FLH = GetCrawlingFLHData()->PrimaryCrawlingFLH;
				}
				if (secondary)
				{
					secondary->FLH = GetCrawlingFLHData()->SecondaryCrawlingFLH;
				}
			}
		}
		else
		{
			if (isElite)
			{
				if (primary)
				{
					primary->FLH = GetCrawlingFLHData()->ElitePrimaryFireFLH;
				}
				if (secondary)
				{
					secondary->FLH = GetCrawlingFLHData()->EliteSecondaryFireFLH;
				}
			}
			else
			{
				if (primary)
				{
					primary->FLH = GetCrawlingFLHData()->PrimaryFireFLH;
				}
				if (secondary)
				{
					secondary->FLH = GetCrawlingFLHData()->SecondaryFireFLH;
				}
			}
		}
	}
}
