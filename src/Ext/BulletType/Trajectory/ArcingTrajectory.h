#pragma once

#include <GeneralDefinitions.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "TrajectoryData.h"

/// @brief 动态载入组件
class ArcingTrajectory : public BulletScript
{
public:
	BULLET_SCRIPT(ArcingTrajectory);

	void ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos);

	/**
	 *@brief 重新计算Arcing抛射体的出膛向量，精确命中目标位置
	 *
	 * @param speedMultiple 速度倍率，主要用于弹跳
	 */
	void ResetArcingVelocity(float speedMultiple = 1.0f);

	virtual void Clean() override
	{
		BulletScript::Clean();

		CanBounce = false;
		_initFlag = false;
	}

	virtual void Awake() override;

	virtual void OnUpdate() override;

	/**
	 *@brief 有弹性，撞悬崖会反弹
	 *
	 */
	bool CanBounce = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->CanBounce)
			.Process(this->_initFlag)
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
		return const_cast<ArcingTrajectory*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	// 弹道控制
	bool _initFlag = false;
};
