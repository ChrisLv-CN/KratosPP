#pragma once

#include <string>

#include <Extension.h>
#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Extension/AnimExt.h>

#include <Ext/EffectType/Effect/StandData.h>

#include "AnimStatus.h"

class AnimStand : public AnimScript
{
public:
	ANIM_SCRIPT(AnimStand);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args)
	{
		if (args == pStand)
		{
			pStand = nullptr;
		}
	}

	virtual void Awake() override
	{
		if (!GetStandData()->Enable)
		{
			Disable();
			return;
		}
		EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &AnimStand::OnTechnoDelete);
	}

	virtual void Destroy() override
	{
		EventSystems::General.RemoveHandler(Events::ObjectUnInitEvent, this, &AnimStand::OnTechnoDelete);
	}

	virtual void OnUpdate() override;

	virtual void OnDone() override;

	TechnoClass* pStand = nullptr;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_initFlag)
			.Process(this->pStand)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &AnimStand::OnTechnoDelete);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<AnimStand*>(this)->Serialize(stream);
	}
#pragma endregion

private:

	void CreateAndPutStand();

	void SetLocation(CoordStruct location);

	bool _initFlag = false;

	StandData* _data = nullptr;
	StandData* GetStandData();
};
