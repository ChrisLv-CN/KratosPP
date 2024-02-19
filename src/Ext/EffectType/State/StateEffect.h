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

#include <Ext/StateType/State/PaintballData.h>


class PaintballEffect : public StateEffect<PaintballData>
{
public:
	STATE_EFFECT_SCRIPT(Paintball);

	// PaintballEffect() : StateEffect<PaintballData>() { this->Name = ScriptName; }

	// inline static std::string ScriptName = "PaintballEffect";

	// static Component* Create() { return static_cast<Component*>(new PaintballEffect()); }

	// inline static int g_temp_PaintballEffect = ComponentFactory::GetInstance().Register("PaintballEffect", PaintballEffect::Create);

	// virtual PaintballData GetData() override { return AEData.Paintball; }
	// __declspec(property(get = GetData)) PaintballData* Data;

	virtual IStateScript* GetState(TechnoStatus* status) override
	{
		if (status)
		{
			return status->Paintball;
		}
		return nullptr;
	}

	virtual IStateScript* GetState(BulletStatus* status) override
	{
		if (status)
		{
			return status->Paintball;
		}
		return nullptr;
	}
};




