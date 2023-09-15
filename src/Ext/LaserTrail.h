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
	virtual void Read(INIBufferReader* ini) override
	{
		ColorChanged = ini->Get("ColorChanged", false);
	}

	Valueable<bool> ColorChanged{ true };
};

class LaserTrail : public TechnoScript
{
public:
	LaserTrail(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();
	}

	virtual void Awake() override
	{
#ifdef DEBUG
		Debug::Log("LaserTrail [%s]%s is calling awake to init data.\n", thisName.c_str(), thisId.c_str());
#endif // DEBUG
		// _gameObject->RemoveComponent(this);
	}

	virtual void Start() override
	{
#ifdef DEBUG
		Debug::Log("LaserTrail [%s]%s is calling start to init data.\n", thisName.c_str(), thisId.c_str());
#endif // DEBUG
		_gameObject->RemoveComponent(this);
	}

	virtual void Destroy() override
	{
#ifdef DEBUG
		Debug::Log("LaserTrail [%s]%s is calling destroy.\n", thisName.c_str(), thisId.c_str());
#endif // DEBUG
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
		return const_cast<LaserTrail*>(this)->Serialize(stream);
	}
#pragma endregion

	ColorStruct laserColor = { 0, 255, 0 };

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
