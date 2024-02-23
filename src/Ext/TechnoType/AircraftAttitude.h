#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Ext/TechnoType/TechnoStatus.h>

#include "AircraftAttitudeData.h"

/// @brief 动态载入组件
class AircraftAttitude : public TechnoScript
{
public:
	TECHNO_SCRIPT(AircraftAttitude);

	void Setup();

	bool TryGetAirportDir(int& poseDir);

	void UpdateHeadToCoord(CoordStruct headTo, bool lockAngle = false);
	void UnLock();

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

	double PitchAngle = 0.0;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(PitchAngle)

			.Process(_initFlag)
			.Process(_targetAngle)
			.Process(_smooth)
			.Process(_lockAngle)
			.Process(_location)
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
		return const_cast<AircraftAttitude*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	const double angelStep = Math::atan2(20, Unsorted::LeptonsPerCell);

	TechnoStatus* _status = nullptr;
	TechnoStatus* GetStaus()
	{
		if (!_status)
		{
			_status = _gameObject->GetComponent<TechnoStatus>();
		}
		return _status;
	}

	bool _initFlag = false;

	AircraftAttitudeData* GetAircraftAttitudeData();
	AircraftAttitudeData* _data = nullptr;

	double _targetAngle = 0.0; // 需要调整到的目标角度
	bool _smooth = true; // 平滑的改变角度，防止大幅度的变化
	bool _lockAngle = false; // 角度由外部传入，不计算

	CoordStruct _location = CoordStruct::Empty;
};
