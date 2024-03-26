#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Extension/TechnoTypeExt.h>

#include "TechnoStatus.h"

/// @brief 动态载入组件
class JumpjetCarryall : public TechnoScript
{
public:
	TECHNO_SCRIPT(JumpjetCarryall);

	enum class CarryStatus : int
	{
		STOP = 0, READY = 1, FLYTO = 2, LAND = 3, TAKEOFF = 4
	};

	bool InMission();

	bool CanLift(TechnoClass* pTarget, bool& toPayload);
	bool CanLift(TechnoClass* pTarget);

	void ActionClick(Action action, FootClass* pTarget);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args)
	{
		if (args == _pTarget)
		{
			_pTarget = nullptr;
		}
		if (args == pPayload)
		{
			pPayload = nullptr;
		}
	}

	virtual void Awake() override;

	virtual void Destroy() override;

	virtual void ExtChanged() override;

	virtual void OnUpdate() override;

	virtual void OnGuardCommand() override;
	virtual void OnStopCommand() override;

	FootClass* pPayload = nullptr; // 装载的载具

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pPayload)

			.Process(this->_pTarget)
			.Process(this->_pTargetCell)
			.Process(this->_toPayload)

			.Process(this->_remainingSpace)

			.Process(this->_status)
			.Success();
	};
	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &JumpjetCarryall::OnTechnoDelete);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<JumpjetCarryall*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	TechnoStatus* GetTechnoStatus();

	void SetupJJCarryall();

	void StartMission(FootClass* pTarget, bool toPayload);
	void CancelMission(bool reset = false);

	bool NotToTarget();

	// JJCarryall
	TechnoTypeExt::TypeData* _jjCarryallData = nullptr;
	TechnoTypeExt::TypeData* GetJJCarryallData();

	FootClass* _pTarget = nullptr; // 待抓取的目标
	CellClass* _pTargetCell = nullptr; // 目标所处的格子，用于判断是否脱离任务
	bool _toPayload = false; // 放吊舱

	int _remainingSpace = -1; // 剩余乘客空间

	CarryStatus _status = CarryStatus::STOP;
};
