#include "AttachEffect.h"

#include <BuildingClass.h>
#include <MissionClass.h>

#include <Ext/Helper/Status.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Common/PrintTextManager.h>

#include <Ext/EffectType/AttachEffectScript.h>

OBJECT_SCRIPT_CPP(AttachEffect);

bool AttachEffect::OwnerIsDead()
{
	if (!_ownerIsDead)
	{
		if (pBullet && IsDead(pBullet))
		{
			_ownerIsDead = true;
		}
		else if (pTechno && IsDead(pTechno))
		{
			_ownerIsDead = true;
		}
	}
	return _ownerIsDead;
}

bool AttachEffect::InBuilding()
{
	if (IsBuilding() && !OwnerIsDead())
	{
		BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(pTechno);
		return pBuilding->BState == BStateType::Construction && pBuilding->CurrentMission == Mission::Selling;
	}
	return false;
}

bool AttachEffect::InSelling()
{
	if (IsBuilding())
	{
		BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(pTechno);
		return pBuilding->BState == BStateType::Construction && pBuilding->CurrentMission == Mission::Selling && pBuilding->MissionStatus > 0;
	}
	return false;
}

int AttachEffect::Count()
{
	return _children.size();
}

void AttachEffect::GetMarks(std::vector<std::string>& marks)
{
	ForeachChild([&marks](Component* c) {
		if (auto cc = dynamic_cast<AttachEffectScript*>(c)) {
			cc->GetMarks(marks);
		}
		});
}

void AttachEffect::GetAENames(std::vector<std::string>& names)
{
	ForeachChild([&names](Component* c) {
		if (auto cc = dynamic_cast<AttachEffectScript*>(c)) {
			names.push_back(cc->AEData.Name);
		}
		});
}

void AttachEffect::SetLocationSpace(int cabinLength)
{
	_locationSpace = cabinLength;
	if (cabinLength < _locationMarkDistance)
	{
		_locationMarkDistance = cabinLength;
	}
}

void AttachEffect::Attach(AttachEffectData data, ObjectClass* pSource, HouseClass* pSourceHouse, CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	if (!data.Enable)
	{
		Debug::Log("[Developer warning]Attemp to attach an invalid AE [%s] to [%s]", data.Name.c_str(), pObject->GetType()->ID);
		return;
	}
	// 检查是否穿透铁幕
	if (!data.PenetratesIronCurtain && pObject->IsIronCurtained())
	{
		return;
	}
	// 是否在白名单上
	if (!data.CanAffectType(pObject))
	{
		return;
	}
	// 是否需要标记
	if (!IsOnMark(data))
	{
		return;
	}
	// 是否有排斥的AE
	if (HasContradiction(data))
	{
		return;
	}

	TechnoClass* pAttacker = nullptr;
	HouseClass* pAttackingHouse = pSourceHouse;
	// 调整所属
	if (!pSource)
	{
		pSource = pObject;
	}
	TechnoClass* pSourceTechno = nullptr;
	BulletClass* pSourceBullet = nullptr;
	if (CastToTechno(pSource, pSourceTechno))
	{
		pAttacker = pSourceTechno;
		if (!pAttackingHouse)
		{
			pAttackingHouse = pAttacker->Owner;
		}
	}
	else if (CastToBullet(pSource, pSourceBullet))
	{
		pAttacker = pSourceBullet->Owner;
		if (!pAttackingHouse)
		{
			pAttackingHouse = GetSourceHouse(pSourceBullet);
		}
	}
	else
	{
		Debug::Log("[Developer warning]Attach AE [%s] to [%s] form a unknow source [%s]", data.Name.c_str(), pObject->GetType()->ID, pSource->WhatAmI());
		return;
	}
	// 更改所属，如果需要
	if (data.ReceiverOwn && pSource != pObject)
	{
		// 所属设置为接受者
		if (pSourceTechno)
		{
			pAttackingHouse = pSourceTechno->Owner;
		}
		else if (pSourceBullet)
		{
			pAttackingHouse = GetSourceHouse(pSourceBullet);
		}
	}
	// 调整攻击者
	if (!fromPassenger && data.FromTransporter && !IsDead(pAttacker))
	{
		pAttacker = WhoIsShooter(pAttacker);
	}
	// 检查叠加
	bool add = data.Cumulative == CumulativeMode::YES;
	if (!add)
	{
		// 不同攻击者是否叠加
		bool isAttackMark = fromPassenger || data.Cumulative == CumulativeMode::ATTACKER && !pAttacker && pAttacker->IsAlive;
		// 不同所属是否叠加
		bool isHouseMark = data.Cumulative == CumulativeMode::HOUSE;
		// 攻击者标记AE名称相同，但可以来自不同的攻击者，可以叠加，不检查Delay
		// 检查冷却计时器
		// if (!isAttackMark && !isHouseMark && DisableDelayTimers.TryGetValue(data.Name, out TimerStruct delayTimer) && delayTimer.InProgress())
		// {
		// 	// Logger.Log($"{Game.CurrentFrame} 单位 [{section}]{pObject} 添加AE类型[{data.Name}]，该类型尚在冷却中，无法添加");
		// 	return;
		// }
		bool find = false;
		CoordStruct location = _location;
		// 检查持续时间，增减Duration
		ForeachChild([&find, &add, &isAttackMark, &isHouseMark, &data, &pAttacker, &pAttackingHouse, &location](Component* c) {
			if (auto temp = dynamic_cast<AttachEffectScript*>(c))
			{
				if (data.Group < 0)
				{
					// 无分组，攻击者标记叠加，或同名重置计时器
					if (temp->AEData.Name == data.Name)
					{
						find = true;
						if (isAttackMark)
						{
							if (temp->pSource == pAttacker)
							{
								// 相同的攻击者，重置持续时间，并跳出循环
								if (temp->AEData.ResetDurationOnReapply)
								{
									temp->ResetDuration();
								}
								c->Break();
								return;
							}
							else
							{
								// 当前条的攻击者不同，设置标记后，继续循环，直到检查完所有的AE
								find = false;
							}
						}
						else if (isHouseMark)
						{
							if (temp->pSourceHouse == pAttackingHouse)
							{
								// 是所属标记，且所属相同，重置持续时间，并跳出循环
								if (temp->AEData.ResetDurationOnReapply)
								{
									temp->ResetDuration();
								}
								c->Break();
								return;
							}
							else
							{
								// 当前条的攻击者不同，设置标记后，继续循环，直到检查完所有的AE
								find = false;
							}
						}
						else
						{
							// 不是标记，重置已存在的AE的持续时间，跳出循环
							if (temp->AEData.ResetDurationOnReapply)
							{
								temp->ResetDuration();
							}
							c->Break();
							return;
						}
					}
				}
				else
				{
					// 有分组，替换或者调整持续时间
					if (temp->IsSameGroup(data))
					{
						// 找到了同组
						find = true;
						if (data.OverrideSameGroup)
						{
							// 执行替换操作，关闭所有的同组AE
							temp->Disable(location);
							add = true;
							// 继续循环直至全部关闭
						}
						else
						{
							// 调整持续时间
							temp->MergeDuration(data.Duration);
							// 继续循环直至全部调整完
						}
					}
				}
			}
			});
		// 没找到同类或同组，可以添加新的实例
		add = add || !find;
	}
	// 可以添加AE，开始执行添加动作
	if (add && data.GetDuration() != 0 && StackNotFull(data))
	{
		int index = FindInsertIndex(data);
		Component* c = AddComponent(AttachEffectScript::ScriptName); // TODO 插队
		if (c)
		{
			AttachEffectScript* ae = dynamic_cast<AttachEffectScript*>(c);
			ae->AEData = data;
			AddStackCount(data); // 叠层计数
			// 激活AE
			ae->Enable(pAttacker, pAttackingHouse, warheadLocation, aeMode, fromPassenger);
		}

	}
}

bool AttachEffect::IsOnMark(AttachEffectData data)
{
	std::vector<std::string> marks;
	GetMarks(marks);
	return data.IsOnMark(marks);
}

bool AttachEffect::HasContradiction(AttachEffectData data)
{
	std::vector<std::string> names;
	GetAENames(names);
	return data.HasContradiction(names);
}

void AttachEffect::AddStackCount(AttachEffectData data)
{

}

void AttachEffect::ReduceStackCount(AttachEffectData data)
{

}

bool AttachEffect::StackNotFull(AttachEffectData data)
{
	// TODO AE叠层计数
	return true;
}

int AttachEffect::FindInsertIndex(AttachEffectData data)
{
	// TODO 火车插位
	return -1;
}

void AttachEffect::Awake()
{
	EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
}

void AttachEffect::Destroy()
{
	EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &AttachEffect::OnGScreenRender);
	((TechnoExt::ExtData*)extData)->SetExtStatus(nullptr);
}


void AttachEffect::OnGScreenRender(EventSystem* sender, Event e, void* args)
{
	if (args && !IsDeadOrInvisible(pTechno))
	{
#ifdef DEBUG
		std::vector<std::string> names;
		_gameObject->Foreach([&](Component* c)
			{
				std::string name = c->Name;
				names.push_back(name);
			});
		std::string log = "";
		for (std::string n : names)
		{
			log.append(n).append(",");
		}
		PrintTextManager::PrintText(log, Colors::Green, pTechno->GetCoords());
#endif // DEBUG
	}
}

void AttachEffect::OnReceiveDamageDestroy()
{
	_ownerIsDead = true;
};
