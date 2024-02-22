#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "AircraftPutData.h"

/// @brief 动态载入组件
class AircraftPut : public TechnoScript
{
public:

	TECHNO_SCRIPT(AircraftPut);

	virtual void Awake() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnUpdate() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_aircraftPutOffset)
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
		return const_cast<AircraftPut*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	int CountAircraft();

	AircraftPutData* GetAircraftPutData();
	AircraftPutData* _data = nullptr;

	bool _aircraftPutOffset = false;
};
