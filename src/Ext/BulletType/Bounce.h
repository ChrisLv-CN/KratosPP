#pragma once

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "BounceData.h"

/// @brief 动态载入组件
class Bounce : public BulletScript
{
public:
	BULLET_SCRIPT(Bounce);

	void SetBounceData(BounceData bounceData);

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_bounceData)
			.Process(this->_isBounceSplit)
			.Process(this->_bounceIndex)
			.Process(this->_bounceTargetPos)
			.Process(this->_bounceSpeedMultiple)
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
		return const_cast<Bounce*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	/**
	 *@brief 分裂出子抛射体
	 *
	 * @return true
	 * @return false
	 */
	bool SpawnSplitCannon();

	// 弹跳
	BounceData _bounceData{};
	bool _isBounceSplit = false; // 是弹跳抛射体分裂的子抛射体
	int _bounceIndex = 0; // 第几号子抛射体
	CoordStruct _bounceTargetPos = CoordStruct::Empty;
	float _bounceSpeedMultiple = 1.0f;

};
