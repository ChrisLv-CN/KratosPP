#pragma once

#include <string>
#include <vector>
#include <map>
#include <type_traits>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Extension/BulletExt.h>
#include <Extension/TechnoExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>

#include "../EffectScript.h"
#include <Ext/StateType/StateScript.h>

#define STATE_EFFECT_SCRIPT(EFFECT_NAME) \
	EFFECT_SCRIPT_BASE(EFFECT_NAME, StateEffect) \

template <typename TEffectData>
class StateEffect : public EffectScript
{
public:

	virtual void ResetDuration() override final
	{
		if (State)
		{
			EffectData* pData = GetData();
			switch (pData->AffectWho)
			{
			case AffectWho::MASTER:
				State->ResetDuration(AE->GetDuration(), AE->Token);
				break;
			case AffectWho::STAND:
				AEStateToStand(true);
				// 关闭AE
				End(CoordStruct::Empty);
				break;
			default:
				State->ResetDuration(AE->GetDuration(), AE->Token);
				AEStateToStand(true);
				break;
			}
		}
	};

	virtual void Start() override final
	{
		if (State)
		{
			EffectData* pData = GetData();
			switch (pData->AffectWho)
			{
			case AffectWho::MASTER:
				State->Replace(pData, AE->GetDuration(), AE->Token, AE);
				break;
			case AffectWho::STAND:
				AEStateToStand(false);
				// 关闭AE
				End(CoordStruct::Empty);
				break;
			default:
				State->Replace(pData, AE->GetDuration(), AE->Token, AE);
				AEStateToStand(false);
				break;
			}
		}
	}

	virtual void End(CoordStruct location) override final
	{
		if (State)
		{
			State->End(AE->Token);
		}
		Deactivate();
		AE->TimeToDie();
	};

	virtual void Pause() override final
	{
		End(CoordStruct::Empty);
	}

	virtual void Recover() override final
	{
		Start();
	}

	virtual void Clean() override
	{
		EffectScript::Clean();

		_state = nullptr;
	}

	virtual IStateScript* GetState(TechnoStatus* status) = 0;
	virtual IStateScript* GetState(BulletStatus* status) = 0;

protected:
	void AEStateToStand(bool resetDuration)
	{
		EffectData* pData = GetData();
		int duration = AE->GetDuration();
		std::string token = AE->Token;
		AE->AEManager->AEStateToStand(pData, duration, token, resetDuration);
	}

	IStateScript* _state = nullptr;
	IStateScript* GetState()
	{
		if (!_state)
		{
			if (pTechno)
			{
				TechnoStatus* statue = GetStatus<TechnoExt, TechnoStatus>(pTechno);
				_state = GetState(statue);
			}
			else if (pBullet)
			{
				BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet);
				_state = GetState(status);
			}
		}
		return _state;
	}
	__declspec(property(get = GetState)) IStateScript* State;
};
