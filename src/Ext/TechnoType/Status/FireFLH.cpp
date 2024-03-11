#include "../TechnoStatus.h"

FireFLHData* TechnoStatus::GetFireFLHData()
{
	if (!_fireFLHData)
	{
		// 取Art上的设置
		const char* section = pTechno->GetTechnoType()->ID;
		std::string image = INI::GetSection(INI::Rules, section)->Get("Image", std::string{ section });
		_fireFLHData = INI::GetConfig<FireFLHData>(INI::Art, image.c_str())->Data;
	}
	return _fireFLHData;
}

bool TechnoStatus::IsFLHOnBody(int weaponIdx)
{
	FireFLHData* data = GetFireFLHData();
	if (data->Enable)
	{
		FireFLHEntity flh = data->Data;
		if (bool isElite = pTechno->Veterancy.IsElite())
		{
			flh = data->EliteData;
		}
		if (flh.Enable)
		{
			TechnoTypeClass* pType = pTechno->GetTechnoType();
			// 是否盖特或者FV
			if (pType->IsGattling || pType->Gunner)
			{
				std::vector<int> onBodyIdxs = data->Data.OnBodyIndexs;
				return std::find(onBodyIdxs.begin(), onBodyIdxs.end(), weaponIdx) != onBodyIdxs.end();
			}
			else if (weaponIdx == 0)
			{
				return flh.PrimaryOnBody;
			}
			else if (weaponIdx == 1)
			{
				return flh.SecondaryOnBody;
			}
		}
	}
	return false;
}

bool TechnoStatus::IsFLHOnTarget()
{
	FireFLHData* data = GetFireFLHData();
	if (data->Enable)
	{
		FireFLHEntity flh = data->Data;
		if (bool isElite = pTechno->Veterancy.IsElite())
		{
			flh = data->EliteData;
		}
		if (flh.Enable)
		{
			TechnoTypeClass* pType = pTechno->GetTechnoType();
			// 是否盖特或者FV
			if (pType->IsGattling || pType->Gunner)
			{
				std::vector<int> onTargetIdxs = data->Data.OnTargetIndexs;
				return std::find(onTargetIdxs.begin(), onTargetIdxs.end(), FLHIndex) != onTargetIdxs.end();
			}
			else if (FLHIndex == 0)
			{
				return flh.PrimaryOnTarget;
			}
			else if (FLHIndex == 1)
			{
				return flh.SecondaryOnTarget;
			}
		}
	}
	return false;
}


