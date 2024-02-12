#include "AttachEffect.h"

#include <BuildingClass.h>
#include <MissionClass.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/MathEx.h>
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

void AttachEffect::Attach(AttachEffectTypeData* typeData)
{
	if (typeData->Enable)
	{
		Attach(typeData->AttachEffectTypes, {}, _attachEffectOnceFlag, pObject);
	}
	if (typeData->StandTrainCabinLength > 0)
	{
		SetLocationSpace(typeData->StandTrainCabinLength);
	}

}

void AttachEffect::Attach(std::vector<std::string> types, std::vector<double> chances, bool attachOnceFlag,
	ObjectClass* pSource, HouseClass* pSourceHouse,
	CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	if (!types.empty())
	{
		int index = 0;
		for (std::string type : types)
		{
			if (Bingo(chances, index))
			{
				Attach(type, attachOnceFlag, pSource, pSourceHouse, warheadLocation, aeMode, fromPassenger);
			}
			index++;
		}
	}
}

void AttachEffect::Attach(std::string type, bool attachOnceFlag,
	ObjectClass* pSource, HouseClass* pSourceHouse,
	CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	if (IsNotNone(type))
	{
		AttachEffectData* data = INI::GetConfig<AttachEffectData>(INI::Rules, type.c_str())->Data;
		if (data->Enable)
		{
			if (attachOnceFlag && data->AttachOnceInTechnoType)
			{
				return;
			}
			Attach(*data, pSource, pSourceHouse, warheadLocation, aeMode, fromPassenger);
		}
	}
}

void AttachEffect::Attach(AttachEffectData data,
	ObjectClass* pSource, HouseClass* pSourceHouse,
	CoordStruct warheadLocation, int aeMode, bool fromPassenger)
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
			AddStackCount(data); // 叠层计数
			// 初始化AE
			AttachEffectScript* ae = dynamic_cast<AttachEffectScript*>(c);
			ae->AEData = data;
			// 激活AE
			ae->EnsureAwaked();
			ae->Enable(pAttacker, pAttackingHouse, warheadLocation, aeMode, fromPassenger);
		}

	}
}

void AttachEffect::RemoveDisableAE()
{
	
}

AttachEffectTypeData* AttachEffect::GetTypeData()
{
	if (!_typeData)
	{
		_typeData = INI::GetConfig<AttachEffectTypeData>(INI::Rules, pObject->GetType()->ID)->Data;
	}
	return _typeData;
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
	if (!IsDeadOrInvisible(pObject))
	{
		CoordStruct location = _location;
		if (args)
		{
			ForeachChild([&location](Component* c) {
				if (auto cc = dynamic_cast<AttachEffectScript*>(c)) { cc->OnGScreenRenderEnd(location); }
				});
#ifdef DEBUG
			// 打印Component结构
			/// GameObject
			///		|__ AttachEffect
			///				|__ AttachEffectScript#0
			///						|__ EffectScript#0
			///						|__ EffectScript#1
			///				|__ AttachEffectScript#1
			///						|__ EffectScript#0
			///						|__ EffectScript#1
			///						|__ EffectScript#2
			if (_gameObject)
			{
				std::vector<std::string> names;
				int level = 0;
				_gameObject->PrintNames(names, level);
				CoordStruct coords = pObject->GetCoords();
				Point2D pos = ToClientPos(coords);
				int offsetZ = PrintTextManager::GetFontSize().Y;
				for (std::string& n : names)
				{
					std::string log{ n };
					log.append("\n");
					pos.Y += offsetZ;
					PrintTextManager::PrintText(log, Colors::Green, pos);
				}
		}
#endif // DEBUG
	}
		else
		{
			ForeachChild([&location](Component* c) {
				if (auto cc = dynamic_cast<AttachEffectScript*>(c)) { cc->OnGScreenRender(location); }
				});
		}
}
}

void AttachEffect::OnUpdate()
{
	// 添加Section上记录的AE
	if (!_ownerIsDead)
	{
		_location = pObject->GetCoords();
		// 检查电力
		if (!IsBullet())
		{
			PowerOff = pTechno->Owner->HasLowPower();
			if (!PowerOff && IsBuilding())
			{
				// 关闭当前建筑电源
				PowerOff = !dynamic_cast<BuildingClass*>(pTechno)->HasPower;
			}
		}

		// 添加section自带AE，无分组的
		Attach(GetTypeData());
		// 检查乘客并附加乘客带来的AE
		if (pTechno)
		{
			// TODO 乘客附加的AE
		}
		// TODO 添加分组的
		this->_attachEffectOnceFlag = true;
	}
}

void AttachEffect::OnUpdateEnd()
{
	// 移除失效的AE，附加Next的AE
	RemoveDisableAE();
}

void AttachEffect::OnReceiveDamageDestroy()
{
	_ownerIsDead = true;
};
