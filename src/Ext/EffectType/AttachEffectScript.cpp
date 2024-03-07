﻿#include "AttachEffectScript.h"

#include <Ext/Helper/Status.h>
#include <Ext/Common/PrintTextManager.h>

#include "EffectScript.h"
#include "Effect/AnimationEffect.h"
#include "Effect/StandEffect.h"

int AttachEffectScript::GetDuration()
{
	int duration = _immortal ? -1 : AEData.GetDuration();
	int timeLeft = -1;
	if (duration > -1 && TryGetDurationTimeLeft(timeLeft))
	{
		duration = timeLeft;
	}
	return duration;
}

bool AttachEffectScript::TryGetInitDelayTimeLeft(int& timeLeft)
{
	timeLeft = -1;
	if (_delayToEnable)
	{
		timeLeft = _initialDelayTimer.GetTimeLeft();
	}
	return timeLeft > -1;
}

bool AttachEffectScript::TryGetDurationTimeLeft(int& timeLeft)
{
	timeLeft = -1;
	if (!_immortal)
	{
		timeLeft = _lifeTimer.GetTimeLeft();
	}
	return timeLeft > -1;
}

void AttachEffectScript::MergeDuration(int otherDuration)
{
	if (!_started || otherDuration == 0)
	{
		// 延迟激活中，不接受时延修改
		return;
	}
	// 修改时间，正数加，负数减
	if (otherDuration < 0)
	{
		// 剩余时间
		int timeLeft = _immortal ? _duration : _lifeTimer.GetTimeLeft();
		// 削减生命总长
		_duration += otherDuration;
		if (_duration <= 0 || timeLeft <= 0)
		{
			// 削减的时间超过总长度，直接减没了
			Deactivate();
		}
		else
		{
			timeLeft += otherDuration;
			if (timeLeft <= 0)
			{
				// 削减完后彻底没了
				Deactivate();
			}
			else
			{
				// 还有剩
				// 重设时间
				ForceStartLifeTimer(timeLeft);
			}
		}
	}
	else
	{
		// 累加持续时间
		_duration += otherDuration;
		if (!_immortal)
		{
			int timeLeft = _lifeTimer.GetTimeLeft();
			timeLeft += otherDuration;
			ForceStartLifeTimer(timeLeft);
		}
	}
}

void AttachEffectScript::ForceStartLifeTimer(int timeLeft)
{
	_immortal = false;
	_lifeTimer.Start(timeLeft);
	ResetEffectsDuration();
}

void AttachEffectScript::ResetDuration()
{
	SetupLifeTimer();
	ResetEffectsDuration();
}

void AttachEffectScript::ResetEffectsDuration()
{
	ForeachChild([](Component* c) {
		if (auto cc = dynamic_cast<IAEScript*>(c)) { cc->ResetDuration(); }
		});
}

void AttachEffectScript::TimeToDie()
{
	_hold = false;
	_immortal = false;
	_lifeTimer.Stop();
}

void AttachEffectScript::SetupInitTimer()
{
	if (_initDelay > 0)
	{
		_delayToEnable = true;
		_initialDelayTimer.Start(_initDelay); // 下一帧开始计时
	}
}

void AttachEffectScript::SetupLifeTimer()
{
	if (!_immortal)
	{
		_lifeTimer.Start(_duration);
	}
}

bool AttachEffectScript::IsSameGroup(AttachEffectData otherType)
{
	return this->AEData.Group > -1 && otherType.Group > -1
		&& this->AEData.Group == otherType.Group;
}

void AttachEffectScript::GetMarks(std::vector<std::string>& marks)
{
	if (AEData.Mark.Enable)
	{
		if (_started)
		{
			for (std::string mark : AEData.Mark.Names)
			{
				marks.push_back(mark);
			}
		}
	}
}

bool AttachEffectScript::OwnerIsDead()
{
	return AEManager->OwnerIsDead();
}

void AttachEffectScript::OnTechnoDelete(EventSystem* sender, Event e, void* args)
{
	if (args == pSource)
	{
		pSource = nullptr;
	}
}

void AttachEffectScript::UpdateStandLocation(LocationMark locationMark)
{
	if (StandEffect* c = GetComponent<StandEffect>())
	{
		c->UpdateLocation(locationMark);
	}
}

void AttachEffectScript::UpdateAnimOffset(CoordStruct offset)
{
	if (AnimationEffect* c = GetComponent<AnimationEffect>())
	{
		c->UpdateLocationOffset(offset);
	}
}

bool AttachEffectScript::IsAlive()
{
#ifdef DEBUG
	int timeLeft = GetDuration();
	Tag = std::to_string(timeLeft) + "#" + AEData.Name;
#endif
	if (!_started)
	{
		if (!InDelayToEnable())
		{
			EnableEffects();
		}
		else
		{
			return true;
		}
	}
	if (IsActive())
	{
		// AE来源于乘客，检查乘客是否已经下车
		if (FromPassenger)
		{
			if (IsDead(pSource) || !pSource->Transporter)
			{
				// 乘客已经下车
				Deactivate();
			}
		}
		// 检查AEMode
		if (IsActive() && AEMode >= 0)
		{
			std::vector<int> ids = AEManager->PassengerIds;
			auto it = std::find(ids.begin(), ids.end(), AEMode);
			if (ids.empty() || it == ids.end())
			{
				// 乘客已经下车
				Deactivate();
			}
		}
		// 检查是否有Effect失活
		if (IsActive() && !_hold)
		{
			bool hasDead = false;
			ForeachChild([&hasDead](Component* c) {
				if (auto e = dynamic_cast<EffectScript*>(c))
				{
					hasDead = !e->IsActive() || !e->IsAlive();
					if (hasDead)
					{
						c->Break();
					}
				}
				});
			if (hasDead)
			{
				Deactivate();
			}
		}
		// 检查计时器
		if (IsActive() && !_immortal && _lifeTimer.Expired())
		{
			Deactivate();
		}
	}
	return IsActive();
}

void AttachEffectScript::Awake()
{
	Tag = AEData.Name;

	_duration = AEData.Duration;
	_immortal = AEData.HoldDuration;

	// 初始化效果
	InitEffects();
}

void AttachEffectScript::Destroy()
{
	EventSystems::Logic.RemoveHandler(Events::TechnoDeleteEvent, this, &AttachEffectScript::OnTechnoDelete);
}

void AttachEffectScript::Start(TechnoClass* pSource, HouseClass* pSourceHouse, CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	Activate();
	this->pSource = pSource;
	this->pSourceHouse = pSourceHouse;
	if (pSource != pObject)
	{
		this->_diffSource = true;
		EventSystems::Logic.AddHandler(Events::TechnoDeleteEvent, this, &AttachEffectScript::OnTechnoDelete);
	}
	if (this->FromWarhead = !warheadLocation.IsEmpty())
	{
		this->WarheadLocation = warheadLocation;
	}
	this->AEMode = aeMode;
	this->FromPassenger = fromPassenger;

	this->_inBuilding = AEManager->InBuilding();

	// 初始延迟
	this->_initDelay = GetRandomValue(AEData.InitialRandomDelay, AEData.InitialDelay);
	// 非建造状态，并且有初始延迟，开启初始延迟计时器
	if (!_inBuilding && _initDelay > 0)
	{
		_initDelay += 1; // 当前帧不算，多延迟一帧
		SetupInitTimer();
	}

	// Start函数由AttachEffect的Attach函数调用，新添加的AE都应该延迟一帧激活
	// AE管理器在每一帧的开始时检查AE的有效，AE的真正激活，将有IsAlive函数进行
	this->_delayToEnable = true;

	// 添加后立刻激活，会导致AE无法被检测到
	//
	// [Kratos PP](315) AE [STACK2] OnUpdate 监视 ReplicantBusterAE
	// [Kratos PP](315) AE [STACK] OnUpdate 监视 ZZZZZ
	// [Kratos PP](315) 添加AE[ReplicantBusterAE] <--- STACK 成功触发
	// [Kratos PP](315) AE [ZZZZZ] OnUpdate
	// [Kratos PP](315) AE [ReplicantBusterAE] OnUpdate
	//
	// [Kratos PP](316) 移除AE[ZZZZZ]
	// [Kratos PP](316) 移除AE[ReplicantBusterAE]
	// [Kratos PP](316) AE [STACK2] OnUpdate  监视 ReplicantBusterAE (不会触发)
	// [Kratos PP](316) AE [STACK] OnUpdate
	// this->_isDelayToEnable = InDelayToEnable();
	// if (_started)
	// {
	// 	EnableEffects();
	// }
}

void AttachEffectScript::End(CoordStruct location)
{
	Deactivate();
	if (!_started)
	{
		return;
	}
	ForeachChild([&location](Component* c) {
		if (auto cc = dynamic_cast<IAEScript*>(c)) { cc->End(location); }
		});
}

void AttachEffectScript::OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	if (!_started)
	{
		return;
	}
	if (damageState != DamageState::NowDead && !OwnerIsDead())
	{
		double healthPercent = pTechno->GetHealthPercentage();
		double min = AEData.DeactiveWhenHealthPrecent;
		double max = AEData.ActiveWhenHealthPrecent;
		if (max <= min)
		{
			Debug::Log("Warning: AE [%s] Active when health precent setup error.\n", AEData.Name.c_str());
			return;
		}
		if (healthPercent <= max && healthPercent >= min)
		{
			// 恢复Effects
			for (Component* c : _children)
			{
				if (auto cc = dynamic_cast<IAEScript*>(c))
				{
					if (!c->IsActive())
					{
						c->Activate();
						cc->Recover();
					}
				}
			}
		}
		else
		{
			// 暂停Effects
			for (Component* c : _children)
			{
				if (auto cc = dynamic_cast<IAEScript*>(c))
				{
					if (c->IsActive())
					{
						c->Deactivate();
						cc->Pause();
					}
				}
			}
		}
	}
};


void AttachEffectScript::OnGScreenRender(CoordStruct location)
{
	if (!_started)
	{
		return;
	}
	ForeachChild([&location](Component* c) {
		if (auto cc = dynamic_cast<IAEScript*>(c)) { cc->OnGScreenRender(location); }
		});
}

void AttachEffectScript::OnGScreenRenderEnd(CoordStruct location)
{
	if (!_started)
	{
		return;
	}
	ForeachChild([&location](Component* c) {
		if (auto cc = dynamic_cast<IAEScript*>(c)) { cc->OnGScreenRenderEnd(location); }
		});
}

void AttachEffectScript::InitEffects()
{
	std::set<std::string> scriptNames = AEData.GetScriptNames();
	for (std::string scriptName : scriptNames)
	{
		Component* c = AddComponent(scriptName);
		if (c)
		{
			EffectScript* e = dynamic_cast<EffectScript*>(c);
			// 初始化Effect
			e->AEData = this->AEData;
			e->EnsureAwaked();
			e->Deactivate(); // 令其失活等待唤醒
		}
	}
}

bool AttachEffectScript::InDelayToEnable()
{
	// 检查初始延迟计时器
	if (_delayToEnable)
	{
		_delayToEnable = _initialDelayTimer.InProgress();
	}
	// 检查建筑状态
	if (_inBuilding)
	{
		_inBuilding = AEManager->InBuilding();
		// 是建筑状态，并且有初始延迟，在建筑状态结束后，启动初始延迟计时器
		if (!_inBuilding && _initDelay > 0)
		{
			// 启动初始延迟计时器
			SetupInitTimer();
		}
	}
	return _delayToEnable || _inBuilding;
}

void AttachEffectScript::EnableEffects()
{
	_started = true;
	SetupLifeTimer();
	if (pTechno)
	{
		// 需要满足血量触发
		double healthPrecent = pTechno->GetHealthPercentage();
		double min = AEData.DeactiveWhenHealthPrecent;
		double max = AEData.ActiveWhenHealthPrecent;
		if (healthPrecent > max || healthPrecent < min)
		{
			_hold = true;
			return;
		}
	}
	_hold = false;
	// Effect is disable stats, so there cannot use ForeachChild function
	for (Component* c : _children)
	{
		if (EffectScript* effect = dynamic_cast<EffectScript*>(c))
		{
			effect->Activate();
			effect->Start();
		}
	}
}

AttachEffect* AttachEffectScript::GetAEManager()
{
	if (!_aeManager)
	{
		_aeManager = dynamic_cast<AttachEffect*>(_parent);
	}
	return _aeManager;
}


