﻿#include "RevengeEffect.h"

#include <Extension/WarheadTypeExt.h>
#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/ObjectType/AttachFire.h>

void RevengeEffect::OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	// 检查弹头
	WarheadTypeExt::TypeData* warheadTypeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
	if (warheadTypeData->IgnoreRevenge || !Data->OnMark(pWH))
	{
		return;
	}

	// 过滤平民
	HouseClass* pHouse = pTechno->Owner;
	if (Data->DeactiveWhenCivilian && pHouse && IsCivilian(pHouse))
	{
		return;
	}
	// 检查复仇者
	TechnoClass* pRevenger = pTechno; // 复仇者
	HouseClass* pRevengerHouse = pHouse; // 复仇者的阵营
	if (Data->FromSource)
	{
		pRevenger = AE->pSource;
		pRevengerHouse = AE->pSourceHouse;

		if (IsDeadOrInvisible(pRevenger))
		{
			// 复仇者不存在，复个屁
			End(CoordStruct::Empty);
			return;
		}
	}
	// 检查报复对象
	TechnoClass* pRevengeTargetTechno = nullptr; // 报复对象
	TechnoClass* pAttackerTechno = nullptr;
	// 向AE的来源复仇
	if (Data->ToSource)
	{
		pRevengeTargetTechno = AE->pSource;
	}
	else if (pAttacker && CastToTechno(pAttacker, pAttackerTechno))
	{
		pRevengeTargetTechno = pAttackerTechno;
	}
	if (IsDeadOrInvisible(pRevengeTargetTechno))
	{
		// 报复对象不存在
		return;
	}
	// 准备报复
	if (Data->Realtime || damageState == DamageState::NowDead)
	{
		// 过滤浮空
		if (!Data->AffectInAir && pRevengeTargetTechno->IsInAir())
		{
			return;
		}
		// 发射武器复仇
		if (Data->CanAffectHouse(pRevengerHouse, pAttackingHouse) && Data->CanAffectType(pRevengeTargetTechno) && IsOnMark(pRevengeTargetTechno, *Data))
		{
			// 检查持续帧内触发
			if (Data->ActiveOnce)
			{
				int currentFrame = Unsorted::CurrentFrame;
				if (_markFrame == 0)
				{
					_markFrame = currentFrame;
				}
				if (currentFrame != _markFrame)
				{
					End(CoordStruct::Empty);
					return;
				}
			}

			if (Bingo(Data->Chance))
			{
				// 使用武器复仇
				if (!Data->Types.empty() || Data->WeaponIndex > -1)
				{
					Component* rgo = nullptr;
					AttachFire* attachFire = nullptr;
					if (TryGetScript<TechnoExt, Component>(pRevenger, rgo))
					{
						attachFire = rgo->FindOrAttach<AttachFire>();
					}
					if (attachFire)
					{
						// 发射自定义武器
						for (std::string weaponId : Data->Types)
						{
							if (IsNotNone(weaponId))
							{
								attachFire->FireCustomWeapon(pRevenger, pRevengeTargetTechno, pRevengerHouse, weaponId, Data->FireFLH, !Data->IsOnTurret, Data->IsOnTarget);
							}
						}
						// 使用自身武器
						if (Data->WeaponIndex > -1)
						{
							WeaponStruct* ws = pRevenger->GetWeapon(Data->WeaponIndex);
							if (ws && ws->WeaponType)
							{
								WeaponTypeClass* pWeapon = ws->WeaponType;
								WeaponTypeExt::TypeData* weaponTypeData = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon);
								// 发射武器
								attachFire->FireCustomWeapon(pRevenger, pRevengeTargetTechno, pRevengerHouse, pWeapon, *weaponTypeData, Data->FireFLH);
							}
						}
					}
				}
				// 使用AE复仇
				AttachEffect* targetAEM = nullptr;
				if (!Data->AttachEffects.empty() && TryGetAEManager<TechnoExt>(pRevengeTargetTechno, targetAEM))
				{
					targetAEM->Attach(Data->AttachEffects, Data->AttachChances, false, pRevenger, pRevengerHouse);
				}
			}
			// 检查触发次数
			if (Data->TriggeredTimes > 0 && ++_count >= Data->TriggeredTimes)
			{
				End(CoordStruct::Empty);
			}
		}
	}
}
