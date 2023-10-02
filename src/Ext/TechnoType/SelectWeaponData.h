#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>
#include <Ext/Helper/CastEx.h>

class SelectWeaponData : public INIConfig
{
public:
	virtual void Read(INIBufferReader* reader) override
	{
		Enable = reader->Get("SelectWeaponUseRange", Enable);
	}

	bool UseSecondary(TechnoClass* pTechno, AbstractClass* pTarget, WeaponTypeClass* pPrimary, WeaponTypeClass* pSecondary)
	{
		if (Enable && !pTechno->IsCloseEnough(pTarget, 0))
		{
			// 检查副武器射程
			if (pTechno->IsCloseEnough(pTarget, 1)
				|| (pSecondary->Range > pPrimary->Range))
			{
				// 返回副武器
				return true;
			}
		}
		return false;
	}
};
