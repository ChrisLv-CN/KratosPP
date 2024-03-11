#pragma once

#include <GeneralDefinitions.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "TrajectoryData.h"

class BulletStatus;

/// @brief 动态载入组件
class MissileTrajectory : public BulletScript
{
public:
	BULLET_SCRIPT(MissileTrajectory);

	virtual void Awake() override;

	virtual void Destroy() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnUpdate() override;

	bool IsDecoy = false; // 热诱弹
	CoordStruct LaunchPos = CoordStruct::Empty;
	CDTimerClass LifeTimer{};

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->IsDecoy)
			.Process(this->LaunchPos)
			.Process(this->LifeTimer)

			.Process(this->_targetHasDecoyFlag)
			.Process(this->_missileShakeVelocityFlag)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		Component::Save(stream);
		return const_cast<MissileTrajectory*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void CheckTargetHasDecoy();

	BulletStatus* _status = nullptr;
	BulletStatus* GetBulletStatus();

	bool _targetHasDecoyFlag = false;
	bool _missileShakeVelocityFlag = false;
};
