#include "Status.h"
#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/Component.h>

#include <Ext/AnimStatus.h>
#include <Ext/BulletStatus.h>
#include <Ext/TechnoStatus.h>


#pragma endregion AnimClass
AnimClass* SetAnimOwner(AnimClass* pAnim, HouseClass* pHouse)
{
	pAnim->Owner = pHouse;
	return pAnim;
}

AnimClass *SetAnimOwner(AnimClass *pAnim, TechnoClass *pTechno)
{
	pAnim->Owner = pTechno->Owner;
	return pAnim;
}

AnimClass *SetAnimOwner(AnimClass *pAnim, BulletClass *pBullet)
{
	if (BulletStatus *status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		pAnim->Owner = status->pSourceHouse;
	}
	return pAnim;
}

AnimClass *SetAnimCreater(AnimClass *pAnim, TechnoClass *pTechno)
{
	if (AnimStatus *status = GetStatus<AnimExt, AnimStatus>(pAnim))
	{
		status->pCreater = pTechno;
	}
	return pAnim;
}

AnimClass *SetAnimCreater(AnimClass *pAnim, BulletClass *pBullet)
{
	TechnoClass *Source = pBullet->Owner;
	if (!IsDead(Source))
	{
		if (AnimStatus *status = GetStatus<AnimExt, AnimStatus>(pAnim))
		{
			status->pCreater = Source;
		}
	}
	return pAnim;
}
#pragma endregion

#pragma endregion TechnoClass
bool IsDead(TechnoClass* pTechno)
{
	return !pTechno || !pTechno->GetType() || pTechno->Health <= 0 || !pTechno->IsAlive || pTechno->IsCrashing || pTechno->IsSinking;
}

bool IsDeadOrInvisible(TechnoClass* pTechno)
{
	return IsDead(pTechno) || pTechno->InLimbo;
}

bool IsCloaked(TechnoClass* pTechno, bool includeCloaking)
{
	return !pTechno || pTechno->CloakState == CloakState::Cloaked || !includeCloaking || pTechno->CloakState == CloakState::Cloaking;
}

bool IsDeadOrInvisibleOrCloaked(TechnoClass* pTechno, bool includeCloaking)
{
	return IsDeadOrInvisible(pTechno) || IsCloaked(pTechno, includeCloaking);
}

double GetROFMulti(TechnoClass* pTechno)
{
	if (IsDead(pTechno))
	{
		return 1;
	}
	bool rof = false;
	if (pTechno->Veterancy.IsElite())
	{
		rof = pTechno->GetTechnoType()->VeteranAbilities.ROF || pTechno->GetTechnoType()->EliteAbilities.ROF;
	}
	else if (pTechno->Veterancy.IsVeteran())
	{
		rof = pTechno->GetTechnoType()->VeteranAbilities.ROF;
	}
	return (!rof ? 1.0 : RulesClass::Instance->VeteranROF) * pTechno->FirepowerMultiplier * ((!pTechno->Owner || !pTechno->Owner->Type) ? 1.0 : pTechno->Owner->Type->ROFMult);
}

double GetDamageMulti(TechnoClass* pTechno)
{
	if (IsDead(pTechno))
	{
		return 1;
	}
	bool firepower = false;
	if (pTechno->Veterancy.IsElite())
	{
		firepower = pTechno->GetTechnoType()->VeteranAbilities.FIREPOWER || pTechno->GetTechnoType()->EliteAbilities.FIREPOWER;
	}
	else if (pTechno->Veterancy.IsVeteran())
	{
		firepower = pTechno->GetTechnoType()->VeteranAbilities.FIREPOWER;
	}
	return (!firepower ? 1.0 : RulesClass::Instance->VeteranCombat) * pTechno->FirepowerMultiplier * ((!pTechno->Owner || !pTechno->Owner->Type) ? 1.0 : pTechno->Owner->Type->FirepowerMult);
}
#pragma endregion

#pragma endregion BulletClass
bool IsDead(BulletClass* pBullet)
{
	BulletStatus* status = nullptr;
	return !pBullet || !pBullet->Type || pBullet->Health <= 0 || !pBullet->IsAlive || !TryGetStatus<BulletExt>(pBullet, status) || !status || status->life.IsDetonate;
}

bool IsDeadOrInvisible(BulletClass* pBullet)
{
	return IsDead(pBullet) || pBullet->InLimbo;
}

void SetSourceHouse(BulletClass* pBullet, HouseClass* pHouse)
{
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		status->pSourceHouse = pHouse;
	}
}

DirStruct Facing(BulletClass* pBullet, CoordStruct location)
{
	CoordStruct source = location;
	if (location == CoordStruct::Empty)
	{
		source = pBullet->GetCoords();
	}
	CoordStruct forward = source + ToCoordStruct(pBullet->Velocity);
	return Point2Dir(source, forward);
}

#pragma endregion
