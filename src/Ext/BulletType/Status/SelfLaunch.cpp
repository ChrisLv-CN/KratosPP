﻿#include "../BulletStatus.h"

#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Physics.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/TechnoType/TechnoStatus.h>

void BulletStatus::OnUpdate_SelfLaunchOrPumpAction()
{
	if (!_movingSelfInitFlag)
	{
		_movingSelfInitFlag = true;
		if (WeaponTypeClass* pWeapon = pBullet->WeaponType)
		{
			WeaponTypeExt::TypeData* weaponData = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon);
			TechnoStatus* status = nullptr;
			if (weaponData && (weaponData->SelfLaunch || weaponData->PumpAction || weaponData->HumanCannon > -1)
				&& !IsDeadOrInvisible(pSource)
				&& TryGetStatus<TechnoExt>(pSource, status)
				&& pSource->WhatAmI() != AbstractType::Building)
			{
				if (weaponData->SelfLaunch)
				{
					_selfLaunch = !AmIStand(pSource);
					if (_selfLaunch && pSource->IsSelected && pSource->Owner && pSource->Owner->IsInPlayerControl)
					{
						pSource->WasSelected = true;
						_shooterIsSelected = true;
					}
				}
				else if (weaponData->PumpAction)
				{
					status->PumpAction(pBullet->TargetCoords, pWeapon->Lobber);
				}
				// 人间大炮可以和自身移动一起生效
				if (weaponData->HumanCannon > -1)
				{
					status->HumanCannon(pBullet->SourceCoords, pBullet->TargetCoords, weaponData->HumanCannon, pWeapon->Lobber);
				}
			}
		}
	}
	if (_selfLaunch && !_limboFlag)
	{
		_limboFlag = true;
		// 抛射体生成时没有消耗弹药，所以在抛射体开始运行后，再移除发射者
		if (!IsDeadOrInvisible(pSource))
		{
			CoordStruct sourcePos = pSource->GetCoords();
			// 从占据的格子中移除自己
			pSource->UnmarkAllOccupationBits(sourcePos);
			// 强令停止移动
			ForceStopMoving(dynamic_cast<FootClass*>(pSource));
			// 清除移动目的地
			pSource->SetDestination(nullptr, true);
			// Limbo发射者
			pSource->Limbo();
		}
	}
}

bool BulletStatus::OnDetonate_SelfLaunch(CoordStruct* pCoords)
{
	if (_selfLaunch && !IsDead(pSource))
	{
		// 发射者Limbo会移除抛射体的所属，要加回去
		pBullet->Owner = pSource;
		// 移动发射者到爆点
		if (pSource->InLimbo)
		{
			// Put到目的地
			CoordStruct targetPos = *pCoords;
			if (TryPutTechno(pSource, targetPos))
			{
				if (_shooterIsSelected)
				{
					pSource->Select();
				}
			}
		}
		else
		{
			// 直接拉过来
			CoordStruct sourcePos = pSource->GetCoords();
			CoordStruct nextPos = *pCoords;
			// 从占据的格子中移除自己
			pSource->UnmarkAllOccupationBits(sourcePos);
			// 强令停止移动
			ForceStopMoving(dynamic_cast<FootClass*>(pSource));
			// 清除移动目的地
			pSource->SetDestination(nullptr, true);
			bool onBridge = false;
			if (CellClass* pCell = MapClass::Instance->TryGetCellAt(nextPos))
			{
				onBridge = pCell->ContainsBridge();
			}
			// 被黑洞吸走
			pSource->UpdatePlacement(PlacementType::Remove);
			// 是否在桥上
			pSource->OnBridge = onBridge;
			pSource->SetLocation(nextPos);
			pSource->UpdatePlacement(PlacementType::Put);
			// 移除黑幕
			MapClass::Instance->RevealArea2(&nextPos, pSource->LastSightRange, pSource->Owner, false, false, false, true, 0);
			MapClass::Instance->RevealArea2(&nextPos, pSource->LastSightRange, pSource->Owner, false, false, false, true, 1);
			// 恢复选择
			if (_shooterIsSelected)
			{
				pSource->Select();
			}
		}
		if (!FallingDown(pSource, 0, false))
		{
			if (pSource->Target)
			{
				pSource->QueueMission(Mission::Attack, false);
			}
		}
	}
	return false;
}
