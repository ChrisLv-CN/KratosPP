#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "AutoFireAreaWeaponData.h"

/// @brief 动态载入组件
class AutoFireAreaWeapon : public TechnoScript
{
public:

	TECHNO_SCRIPT(AutoFireAreaWeapon);

	void Setup();

	virtual void Clean() override
	{
		TechnoScript::Clean();

		SkipROF = false;

		_autoAreaData = nullptr;

		_areaInitDelayTimer = {};
		_areaDelayTimer = {};
	}

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnUpdate() override;

	bool SkipROF = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->SkipROF)
			.Process(this->_areaInitDelayTimer)
			.Process(this->_areaDelayTimer)
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
		return const_cast<AutoFireAreaWeapon*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	AutoFireAreaWeaponData* _autoAreaData = nullptr;
	AutoFireAreaWeaponData* GetAutoAreaData();

	CDTimerClass _areaInitDelayTimer{};
	CDTimerClass _areaDelayTimer{};

};
