#include "../BulletStatus.h"

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/Scripts.h>

void BulletStatus::CanAffectAndDamageBullet(BulletClass* pTarget, WarheadTypeClass* pWH)
{
	if (pTarget && pTarget != pBullet)
	{
		if (BulletStatus* targetStatus = GetStatus<BulletExt, BulletStatus>(pTarget))
		{
			targetStatus->TakeDamage(damage);
		}
	}
}

bool BulletStatus::OnDetonate_AntiBullet(CoordStruct* pCoords)
{
	// 若目标非抛射体，跳过
	BulletClass* pTarget = nullptr;
	if (CastToBullet(pBullet->Target, pTarget))
	{
		if (BulletStatus* targetStatus = GetStatus<BulletExt, BulletStatus>(pTarget))
		{
			if (targetStatus->life.Interceptable)
			{
				// 目标抛射体可以被摧毁，检索爆炸范围内是否还有其他的抛射体，一并炸掉
				WarheadTypeClass* pWH = pBullet->WH;
				if (pWH->CellSpread > 0)
				{
					// 范围伤害
					FindObject<BulletClass>(BulletClass::Array.get(), [&](BulletClass* pTargetBullet)->bool {
						CanAffectAndDamageBullet(pTargetBullet, pWH);
						return false;
						}, * pCoords, pWH->CellSpread, 0, false, pSourceHouse);
				}
				else
				{
					// 单体伤害
					CoordStruct targetPos = pTarget->GetCoords();
					double dist = pCoords->DistanceFrom(targetPos);
					if (isnan(dist) || dist <= pBullet->Type->Arm + 256)
					{
						CanAffectAndDamageBullet(pTarget, pWH);
					}
				}
			}
		}
	}
	return false;
};

