#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "TurretAngleData.h"

class TechnoStatus;

/// @brief 动态载入组件
class TurretAngle : public TechnoScript
{
public:

	TECHNO_SCRIPT(TurretAngle);

	bool DefaultAngleIsChange(DirStruct bodyDir);

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnRemove() override;

	virtual void OnUpdate() override;

	DirStruct LockTurretDir{};
	bool ChangeDefaultDir = false;
	bool LockTurret = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->LockTurretDir)
			.Process(this->ChangeDefaultDir)
			.Process(this->LockTurret)
			.Process(this->_isMoving)
			.Process(this->_isPut)
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
		return const_cast<TurretAngle*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	void Setup();

	int Dir2FacingIndex180to360(DirStruct dir);

	bool TryGetDefaultAngle(int& bodyDirIndex, DirStruct& newDefaultDir);

	bool InDeadZone(int bodyTargetDelta, int min, int max);

	void BlockTurretFacing(DirStruct bodyDir, int bodyDirIndex, int min, int max, int bodyTargetDelta);

	bool ForceTurretToForward(DirStruct bodyDir, int bodyDirIndex, int min, int max, int bodyTargetDelta);

	void TurnToLeft(int turretAngle, int bodyDirIndex, DirStruct bodyDir);

	void TurnToRight(int turretAngle, int bodyDirIndex, DirStruct bodyDir);

	/**
	 *@brief 如果启用侧舷接敌，则计算侧舷的可用角度，否则直接把头怼过去
	 * 
	 * @param targetDir 
	 * @param bodyDirIndex 
	 * @param bodyTargetDelta 
	 * @return true 
	 * @return false 
	 */
	bool TryTurnBodyToAngle(DirStruct targetDir, int bodyDirIndex, int bodyTargetDelta);

	// 炮塔限界
	TurretAngleData* _data{}; // 个体设置
	TurretAngleData* GetTurretAngleData();

	TechnoStatus* _status = nullptr;
	TechnoStatus* GetTechnoStatue();

	bool _isMoving = false;
	bool _isPut = false;
};
