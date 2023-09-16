#pragma once

#include <codecvt>
#include <Extension.h>
#include <TechnoClass.h>
#include <LaserDrawClass.h>
#include <Drawing.h>
#include <HouseClass.h>
#include <TacticalClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/GOExtension.h>
#include <Extension/TechnoExt.h>

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
	AttachEffect(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();
	}

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->laserColor)
			.Process(this->colorChanged)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		return const_cast<AttachEffect*>(this)->Serialize(stream);
	}
#pragma endregion

	ColorStruct laserColor = { 0, 255, 0 };

	bool colorChanged = false;
private:
	AttachEffectData _data;
};
