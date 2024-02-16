#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "MissileHomingData.h"

/// @brief 动态载入组件
class MissileHoming : public TechnoScript
{
public:

	TECHNO_SCRIPT(MissileHoming);

	void Setup();

	virtual void Awake() override;

	virtual void OnUpdate() override;

	// 子机导弹跟踪，标记可由Hook强制开启
	bool IsHoming = false;
	CoordStruct HomingTargetLocation = CoordStruct::Empty;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->IsHoming)
			.Process(this->HomingTargetLocation)
			.Process(this->_initHomingFlag)
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
		return const_cast<MissileHoming*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	MissileHomingData* _homingData = nullptr;
	MissileHomingData* GetHomingData();

	bool _initHomingFlag = false;

};
