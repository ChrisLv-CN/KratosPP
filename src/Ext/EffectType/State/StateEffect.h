#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <type_traits>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>

#include "StateEffectScript.h"

#include <Ext/StateType/State/AntiBulletData.h>
#include <Ext/StateType/State/DestroyAnimData.h>
#include <Ext/StateType/State/DestroySelfData.h>
#include <Ext/StateType/State/GiftBoxData.h>
#include <Ext/StateType/State/PaintballData.h>
#include <Ext/StateType/State/TransformData.h>


#define STATE_EFFECT_DEFINE(STATE_NAME) \
class STATE_NAME ## Effect : public StateEffect<STATE_NAME ## Data> \
{ \
public: \
	STATE_EFFECT_SCRIPT(STATE_NAME); \
	virtual IStateScript* GetState(TechnoStatus* status) override \
	{ \
		IStateScript* state = nullptr; \
		if (status) \
		{ \
			status->TryGetState<STATE_NAME ## State>(state); \
		} \
		return state; \
	} \
	virtual IStateScript* GetState(BulletStatus* status) override \
	{ \
		IStateScript* state = nullptr; \
		if (status) \
		{ \
			status->TryGetState<STATE_NAME ## State>(state); \
		} \
		return state; \
	} \
}; \

// TODO Add State effects

STATE_EFFECT_DEFINE(AntiBullet);
STATE_EFFECT_DEFINE(Deselect);
STATE_EFFECT_DEFINE(DestroyAnim);
STATE_EFFECT_DEFINE(DestroySelf);
STATE_EFFECT_DEFINE(GiftBox);
STATE_EFFECT_DEFINE(Paintball);
STATE_EFFECT_DEFINE(Transform);



