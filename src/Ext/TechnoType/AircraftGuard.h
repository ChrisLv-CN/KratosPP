#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Ext/TechnoType/TechnoStatus.h>

#include "AircraftGuardData.h"

/// @brief 动态载入组件
class AircraftGuard : public TechnoScript
{
public:
	TECHNO_SCRIPT(AircraftGuard);

	enum class AircraftGuardStatus : int
	{
		STOP = 0, READY = 1, GUARD = 2, ROLLING = 3, ATTACK = 4, RELOAD = 5
	};

	void Setup();

	bool IsAreaGuardRolling();

	/**
	 *@brief 按下Ctrl+Alt，获取移动目的地，设定航点，进入警戒巡航状态
	 *
	 */
	void StartAreaGuard();

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnUpdate() override;

	virtual void OnStopCommand() override;

	AircraftGuardStatus State = AircraftGuardStatus::STOP;

	bool Clockwise = false; // 顺时针巡航

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(State)
			.Process(Clockwise)
			.Process(_destCenter)
			.Process(_destList)
			.Process(_destIndex)
			.Process(_onStopCommand)
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
		return const_cast<AircraftGuard*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	bool SetupDestination();
	bool SetupDestination(CoordStruct location);

	void CancelAreaGuard();

	void BackToAirport();

	bool FoundAndAttack(CoordStruct location);

	bool CheckTarget(TechnoClass* pTarget);

	TechnoStatus* _status = nullptr;
	TechnoStatus* GetTechnoStatus()
	{
		if (!_status)
		{
			_status = _gameObject->GetComponent<TechnoStatus>();
		}
		return _status;
	}

	AircraftGuardData* GetAircraftGuardData();
	AircraftGuardData* _data = nullptr;

	CoordStruct _destCenter = CoordStruct::Empty; // 航点
	std::vector<CoordStruct> _destList{}; // 巡航的下一个实际坐标点
	int _destIndex = 0; // 巡航点序号

	bool _onStopCommand = false;

};
