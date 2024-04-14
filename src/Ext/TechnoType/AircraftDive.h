#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "AircraftAttitude.h"
#include "AircraftDiveData.h"

/// @brief 动态载入组件
class AircraftDive : public TechnoScript
{
public:

	enum class AircraftDiveStatus : int
	{
		NONE = 0, DIVEING = 1, PULLUP = 2
	};

	TECHNO_SCRIPT(AircraftDive);

	virtual void Clean() override
	{
		TechnoScript::Clean();

		DiveStatus = AircraftDiveStatus::NONE;

		_attitude = nullptr;

		_data = nullptr;

		_activeDive = false;
	}

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

	virtual void OnUpdate() override;

	AircraftDiveStatus DiveStatus = AircraftDiveStatus::NONE;

	AircraftDiveData* GetAircraftDiveData();

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(DiveStatus)
			.Process(this->_activeDive)
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
		return const_cast<AircraftDive*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	void Setup();

	AircraftAttitude* _attitude = nullptr;
	AircraftAttitude* Attitude()
	{
		if (!_attitude)
		{
			_attitude = _gameObject->GetComponent<AircraftAttitude>();
		}
		return _attitude;
	}

	AircraftDiveData* _data = nullptr;

	bool _activeDive = false;
};
