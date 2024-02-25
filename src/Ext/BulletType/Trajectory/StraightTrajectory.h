#pragma once

#include <GeneralDefinitions.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Ext/Helper/Weapon.h>

#include "TrajectoryData.h"

/// @brief 动态载入组件
class StraightTrajectory : public BulletScript
{
public:
	/// @brief 直线导弹的状态数据
	struct StraightBullet
	{
	public:
		CoordStruct sourcePos;
		CoordStruct targetPos;
		BulletVelocity Velocity;

		void ResetVelocity(BulletClass* pBullet)
		{
			this->Velocity = RecalculateBulletVelocity(pBullet, sourcePos, targetPos);
		}
	};

	BULLET_SCRIPT(StraightTrajectory);

	void ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos);

	void ResetStraightMissileVelocity();

	virtual void Awake() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnUpdate() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_straightBullet)
			.Process(this->_resetTargetFlag)
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
		return const_cast<StraightTrajectory*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void Setup();

	// 记录直线导弹的位置和速度
	StraightBullet _straightBullet{};
	bool _resetTargetFlag = false;

};
