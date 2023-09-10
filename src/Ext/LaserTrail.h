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

class LaserTrailData : public INIConfig
{
public:
	virtual void Read(INIBufferReader *ini) override
	{
		ColorChanged = ini->Get("ColorChanged", false);
	}

	Valueable<bool> ColorChanged{true};
};

class LaserTrail : public TechnoScript
{
public:
	LaserTrail(Extension<TechnoClass> *ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();
	}

	virtual void Awake() override
	{
		// _gameObject->RemoveComponent(this);
	}

	virtual void Destroy() override
	{
	}

#pragma region Save/Load
	template <typename T>
	void Serialize(T &stream){
		stream
			.Process(this->laserColor)
			.Process(this->colorChanged)
			;
	};

	virtual void LoadFromStream(ExStreamReader &stream) override
	{
		Component::LoadFromStream(stream);
		this->Serialize(stream);
	}
	virtual void SaveToStream(ExStreamWriter &stream) override
	{
		Component::SaveToStream(stream);
		this->Serialize(stream);
	}
#pragma endregion

	ColorStruct laserColor = {0, 255, 0};

	bool colorChanged = false;

	virtual void OnUpdate() override
	{
		CoordStruct sourcePos = _owner->GetCoords();
		CoordStruct targetPos = GetFLHAbsoluteCoords(sourcePos, { 1024, 0, 0 }, _owner->PrimaryFacing.Current());
		CoordStruct turretPos = GetFLHAbsoluteCoords(sourcePos, { 2048, 0, 0 }, _owner->SecondaryFacing.Current());
		LaserHelper::RedLine(sourcePos, targetPos);
		LaserHelper::RedLineZ(targetPos, 512);
		LaserHelper::GreenLine(sourcePos, turretPos);
		LaserHelper::GreenLineZ(turretPos, 512);
	}

private:
	LaserTrailData _data;
};
