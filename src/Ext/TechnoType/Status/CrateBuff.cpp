#include "../TechnoStatus.h"

#include <Ext/Common/CommonStatus.h>

CrateBuffData* TechnoStatus::GetCrateBuffData()
{
	if (!_crateBuffData)
	{
		_crateBuffData = INI::GetConfig<CrateBuffData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _crateBuffData;
}

void TechnoStatus::InitState_CrateBuff()
{
	CrateBuffData* data = GetCrateBuffData();
	if (data->Enable)
	{
		CrateBuffState.Enable(*data);
	}
}

void TechnoStatus::RecalculateStatus()
{
	if (!IsDead(pTechno))
	{
		// 获取箱子加成
		double firepowerMult = CrateBuff.FirepowerMultiplier;
		double armorMult = CrateBuff.ArmorMultiplier;
		double speedMult = CrateBuff.SpeedMultiplier;
		double rofMult = CrateBuff.ROFMultiplier;
		bool cloakable = CanICloakByDefault() || CrateBuff.Cloakable;
		// 算上AE加成
		/* TODO
		AttachEffectScript ae = GameObject.GetComponent<AttachEffectScript>();
		if (null != ae)
		{
			CrateBuffData aeMultiplier = ae.CountAttachStatusMultiplier();
			firepowerMult *= aeMultiplier.FirepowerMultiplier;
			armorMult *= aeMultiplier.ArmorMultiplier;
			cloakable |= aeMultiplier.Cloakable;
			speedMult *= aeMultiplier.SpeedMultiplier;
		}*/
		// 赋予单位
		pTechno->FirepowerMultiplier = firepowerMult;
		pTechno->ArmorMultiplier = armorMult;
		pTechno->Cloakable = cloakable;
		FootClass* pFoot = nullptr;
		if (CastToFoot(pTechno, pFoot))
		{
			pFoot->SpeedMultiplier = speedMult;
		}
	}
}

bool TechnoStatus::CanICloakByDefault()
{
	return pTechno && pTechno->GetTechnoType() && (pTechno->GetTechnoType()->Cloakable || pTechno->HasAbility(Ability::Cloak));
}
