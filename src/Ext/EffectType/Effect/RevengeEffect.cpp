#include "RevengeEffect.h"

#include <Extension/WarheadTypeExt.h>
#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/ObjectType/AttachFire.h>

bool RevengeEffect::CanRevenge(TechnoClass*& pRevenger, HouseClass*& pRevengerHouse, TechnoClass*& pRevengeTarget,
	WarheadTypeClass* pWH, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	// 检查弹头
	WarheadTypeExt::TypeData* warheadTypeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
	if (warheadTypeData->IgnoreRevenge || !Data->OnMark(pWH))
	{
		return false;
	}

	// 过滤平民
	HouseClass* pHouse = pTechno->Owner;
	if (Data->DeactiveWhenCivilian && pHouse && IsCivilian(pHouse))
	{
		return false;
	}
	// 检查复仇者
	if (Data->FromSource)
	{
		pRevenger = AE->pSource;
		pRevengerHouse = AE->pSourceHouse;

		if (IsDeadOrInvisible(pRevenger))
		{
			// 复仇者不存在，复个屁
			Deactivate();
			AE->TimeToDie();
			return false;
		}
	}
	// 检查报复对象
	TechnoClass* pAttackerTechno = nullptr;
	// 向AE的来源复仇
	if (Data->ToSource)
	{
		pRevengeTarget = AE->pSource;
	}
	else if (pAttacker && CastToTechno(pAttacker, pAttackerTechno))
	{
		pRevengeTarget = pAttackerTechno;
	}
	return true;
}

void RevengeEffect::OnReceiveDamage(args_ReceiveDamage* args)
{
	_ignoreDefenses = args->IgnoreDefenses;
}

void RevengeEffect::OnReceiveDamageReal(int* pRealDamage, WarheadTypeClass* pWH, TechnoClass* pAttacker, HouseClass* pAttackingHouse)
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
			_skip = true;
			Deactivate();
			AE->TimeToDie();
			return;
		}
	}
	// 检查复仇者
	pRevenger = pTechno; // 复仇者
	pRevengerHouse = pTechno->Owner; // 复仇者的阵营
	// 检查报复对象
	pRevengeTarget = nullptr; // 报复对象
	_skip = !CanRevenge(pRevenger, pRevengerHouse, pRevengeTarget, pWH, pAttacker, pAttackingHouse);
	if (!_skip)
	{
		_bingo = Bingo(Data->Chance);
		// 准备报复
		if (_bingo && !_ignoreDefenses)
		{
			int damage = *pRealDamage;
			// 减伤
			if (Data->DamageSelfPercent != 1)
			{
				*pRealDamage = (int)(damage * Data->DamageSelfPercent);
			}
			// 反伤
			if (Data->DamageEnemyPercent != 0)
			{
				// 反射给攻击者
				if (!IsDeadOrInvisible(pRevengeTarget) && !IsImmune(pRevengeTarget, true))
				{
					int refDamage = (int)(damage * Data->DamageEnemyPercent);
					pRevengeTarget->TakeDamage(refDamage, pRevengeTarget->GetTechnoType()->Crewed);
				}
			}
		}
	}
}

void RevengeEffect::OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse)
{
	_ignoreDefenses = false;
	if (_skip)
	{
		return;
	}
	// 准备报复
	if (Data->Realtime || damageState == DamageState::NowDead)
	{
		if (_bingo)
		{
			// 需要报复对象存在的复仇
			if (!IsDeadOrInvisible(pRevengeTarget))
			{
				// 过滤浮空
				if (Data->AffectInAir || !pRevengeTarget->IsInAir())
				{
					// 可以影响复仇对象
					if (Data->CanAffectHouse(pRevengerHouse, pAttackingHouse) && Data->CanAffectType(pRevengeTarget) && IsOnMark(pRevengeTarget, *Data))
					{
						// 使用武器复仇
						if ((!Data->Types.empty() || Data->WeaponIndex > -1) && pRevenger)
						{
							TechnoClass* pShooter = WhoIsShooter(pRevenger);
							if (AttachFire* attachFire = FindOrAttachScript<TechnoExt, AttachFire>(pShooter))
							{
								// 发射自定义武器
								for (std::string weaponId : Data->Types)
								{
									if (IsNotNone(weaponId))
									{
										attachFire->FireCustomWeapon(pRevenger, pRevengeTarget, pRevengerHouse, weaponId, Data->FireFLH, !Data->IsOnTurret, Data->IsOnTarget);
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
										attachFire->FireCustomWeapon(pRevenger, pRevengeTarget, pRevengerHouse, pWeapon, *weaponTypeData, Data->FireFLH);
									}
								}
							}
						}
						// 使用AE复仇
						AttachEffect* targetAEM = nullptr;
						if (!Data->AttachEffects.empty() && TryGetAEManager<TechnoExt>(pRevengeTarget, targetAEM))
						{
							targetAEM->Attach(Data->AttachEffects, Data->AttachChances, false, pRevenger, pRevengerHouse);
						}
					}
				}
			}
			// 检查触发次数
			if (Data->TriggeredTimes > 0 && ++_count >= Data->TriggeredTimes)
			{
				Deactivate();
				AE->TimeToDie();
			}
		}
	}
}
