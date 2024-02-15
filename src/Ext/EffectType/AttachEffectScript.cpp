#include "AttachEffectScript.h"

#include <Ext/Helper/Status.h>
#include <Ext/Common/PrintTextManager.h>

#include "EffectScript.h"
#include "Effect/Animation.h"
#include "Effect/Stand.h"

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
	if (_immortal)
	{
		timeLeft = _lifeTimer.GetTimeLeft();
	}
	return timeLeft > -1;
}

void AttachEffectScript::MergeDuration(int otherDuration)
{
	if (_isDelayToEnable || otherDuration == 0)
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
		if (auto cc = dynamic_cast<EffectScript*>(c)) { cc->ResetDuration(); }
		});
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
		if (!_isDelayToEnable)
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

void AttachEffectScript::UpdateStandLocation(LocationMark locationMark)
{
	if (Stand* c = GetComponent<Stand>())
	{
		c->UpdateLocation(locationMark);
	}
}

void AttachEffectScript::UpdateAnimOffset(CoordStruct offset)
{
	if (Animation* c = GetComponent<Animation>())
	{
		c->UpdateLocationOffset(offset);
	}
}

bool AttachEffectScript::IsAlive()
{
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
		if (IsActive())
		{
			bool hasDead = false;
			ForeachChild([&hasDead](Component* c) {
				hasDead = !c->IsActive();
				if (hasDead)
				{
					c->Break();
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
	// 延迟启用AE
	int initDelay = GetRandomValue(AEData.InitialRandomDelay, AEData.InitialDelay);
	_delayToEnable = initDelay > 0;
	if (_delayToEnable)
	{
		Component::Deactivate();
		_initialDelayTimer.Start(initDelay);
	}
	_duration = AEData.Duration;
	_immortal = AEData.HoldDuration;

	// 初始化效果
	InitEffects();
}

void AttachEffectScript::Destroy()
{

}

void AttachEffectScript::Start(TechnoClass* pSource, HouseClass* pSourceHouse, CoordStruct warheadLocation, int aeMode, bool fromPassenger)
{
	Activate();
	this->pSource = pSource;
	this->pSourceHouse = pSourceHouse;
	if (this->FromWarhead = !warheadLocation.IsEmpty())
	{
		this->WarheadLocation = warheadLocation;
	}
	this->AEMode = aeMode;
	this->FromPassenger = fromPassenger;
	this->_inBuilding = AEManager->InBuilding();
	this->_isDelayToEnable = InDelayToEnable();
	if (!_isDelayToEnable)
	{
		EnableEffects();
	}
}

void AttachEffectScript::End(CoordStruct location)
{
	Deactivate();
	if (_isDelayToEnable)
	{
		return;
	}
	ForeachChild([&location](Component* c) {
		if (auto cc = dynamic_cast<EffectScript*>(c)) { cc->End(location); }
		});
}

void AttachEffectScript::OnGScreenRender(CoordStruct location)
{
	ForeachChild([&location](Component* c) {
		if (auto cc = dynamic_cast<EffectScript*>(c)) { cc->OnGScreenRender(location); }
		});
}

void AttachEffectScript::OnGScreenRenderEnd(CoordStruct location)
{
	ForeachChild([&location](Component* c) {
		if (auto cc = dynamic_cast<EffectScript*>(c)) { cc->OnGScreenRenderEnd(location); }
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
	}
	return _delayToEnable || _inBuilding;
}

void AttachEffectScript::EnableEffects()
{
	_isDelayToEnable = false;
	SetupLifeTimer();
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


