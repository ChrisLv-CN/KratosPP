#include "../TechnoStatus.h"

#include <FootClass.h>

#include <Ext/Helper/Finder.h>

bool TechnoStatus::WeaponNoAA(int weaponIdx)
{
	// 检查单位是否具有防空武器
	WeaponStruct* pPrimayStruct = pTechno->GetWeapon(0);
	WeaponTypeClass* pPrimary = pPrimayStruct ? pPrimayStruct->WeaponType : nullptr;
	bool noPrimary = !pPrimary || !pPrimary->Projectile;

	WeaponStruct* pSecondaryStruct = pTechno->GetWeapon(1);
	WeaponTypeClass* pSecondary = pSecondaryStruct ? pSecondaryStruct->WeaponType : nullptr;
	bool noSecondary = !pSecondary || !pSecondary->Projectile;

	if (weaponIdx == 0 && noPrimary)
	{
		Debug::Log("Warning: Techno [%s] has no Primary weapon, disable AntiMissile.\n", pTechno->GetTechnoType()->ID);
	}
	else if (weaponIdx == 1 && noSecondary)
	{
		Debug::Log("Warning: Techno [%s] has no Secondary weapon, disable AntiMissile.\n", pTechno->GetTechnoType()->ID);
	}
	else if (noPrimary && noSecondary)
	{
		Debug::Log("Warning: Techno [%s] has no Any weapon, disable AntiMissile.\n", pTechno->GetTechnoType()->ID);
	}
	else
	{
		bool primaryAA = !noPrimary && pPrimary->Projectile->AA;
		bool secondaryAA = !noSecondary && pSecondary->Projectile->AA;
		if (weaponIdx == 0 && !primaryAA)
		{
			Debug::Log("Warning: Techno [%s] Primary weapon has no AA, disable AntiMissile.\n", pTechno->GetTechnoType()->ID);
		}
		else if (weaponIdx == 1 && !secondaryAA)
		{
			Debug::Log("Warning: Techno [%s] Secondary weapon has no AA, disable AntiMissile.\n", pTechno->GetTechnoType()->ID);
		}
		else if (!primaryAA && !secondaryAA)
		{
			Debug::Log("Warning: Techno [%s] All weapon has no AA, disable AntiMissile.\n", pTechno->GetTechnoType()->ID);
		}
		else
		{
			return false;
		}
	}
	return true;
}

void TechnoStatus::OnUpdate_AntiBullet()
{
	if (!IsDeadOrInvisible(pTechno) && AntiBullet->IsAlive())
	{
		AntiBulletData data = AntiBullet->Data;
		// 没有乘客也没有防空武器，关闭反抛射体搜索
		if (!data.Self || WeaponNoAA(data.Weapon) && (!data.ForPassengers || pTechno->GetTechnoType()->Passengers == 0))
		{
			AntiBullet->End();
			return;
		}
		if (AntiBullet->CanSearchBullet())
		{
			double scanRange = data.Range;
			if (pTechno->Veterancy.IsElite())
			{
				scanRange = data.EliteRange;
			}
			std::string range = std::to_string(scanRange);
			// 扫描附近的抛射体
			TechnoClass* pSelf = pTechno;
			CoordStruct location = pTechno->GetCoords();
			FindObject<BulletClass>(BulletClass::Array.get(),
				[data, pSelf](BulletClass* pBullet)->bool {
					bool targetComfirm = false;
					if (data.ScanAll)
					{
						// 检查抛射体的目标的所属是否是友军
						AbstractClass* pTarget = pBullet->Target;
						TechnoClass* pBulletTarget = nullptr;
						if (CastToTechno(pTarget, pBulletTarget))
						{
							HouseClass* pBulletTargetHouse = pBulletTarget->Owner;
							targetComfirm = pSelf->Owner == pBulletTargetHouse || pSelf->Owner->IsAlliedWith(pBulletTargetHouse);
						}
						else
						{
							// 检查抛射体自身的所属是否是敌军
							HouseClass* pBulletHouse = GetHouse(pBullet);
							targetComfirm = !pBulletHouse || !pSelf->Owner->IsAlliedWith(pBulletHouse);
						}
					}
					else
					{
						// 抛射体是否正在攻击自己
						targetComfirm = pBullet->Target == pSelf;
					}
					if (targetComfirm)
					{
						// 确认目标
						if (data.ForPassengers)
						{
							pSelf->SetTargetForPassengers(pBullet);
						}
						if (data.Self && (!pSelf->Target || IsDeadOrInvisible(static_cast<ObjectClass*>(pSelf->Target))))
						{
							pSelf->SetTarget(pBullet);
						}
						return true;
					}

					return false;
				}, location, scanRange, 0);
		}
	}
}
