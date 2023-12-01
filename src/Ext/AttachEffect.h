#pragma once

#include <codecvt>

#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/State/AntiBulletState.h>
#include <Ext/State/CrateBuffData.h>
#include <Ext/State/DestroyAnimData.h>
#include <Ext/State/DestroySelfState.h>
#include <Ext/State/FireSuperData.h>
#include <Ext/State/GiftBoxState.h>
#include <Ext/State/PaintballState.h>

#include <Ext/TechnoStatus.h>

#include <Ext/TechnoType/AutoFireAreaWeaponData.h>
#include <Ext/TechnoType/BaseNormalData.h>
#include <Ext/TechnoType/CrawlingFLHData.h>
#include <Ext/TechnoType/DamageTextData.h>
#include <Ext/TechnoType/HealthTextData.h>
#include <Ext/TechnoType/JumpjetFacingData.h>
#include <Ext/TechnoType/MissileHomingData.h>
#include <Ext/TechnoType/SpawnData.h>

class AttachEffectData : public INIConfig
{
public:
	virtual void Read(INIBufferReader* ini) override
	{
		ColorChanged = ini->Get("ColorChanged", false);
	}

	Valueable<bool> ColorChanged{ true };
};

class AttachEffect : public TechnoScript
{
public:
	TECHNO_SCRIPT(AttachEffect);


#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<AttachEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
