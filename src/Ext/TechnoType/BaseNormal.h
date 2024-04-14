#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "BaseNormalData.h"

/// @brief 动态载入组件
class BaseNormal : public TechnoScript
{
public:

	TECHNO_SCRIPT(BaseNormal);

	void Setup();

	virtual void Clean() override
	{
		TechnoScript::Clean();

		_baseNormalData = nullptr;
	}

	virtual void Awake() override;

	virtual void Destroy() override;

	virtual void ExtChanged() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnRemove() override;

	virtual void OnUpdate() override;

	virtual void OnReceiveDamageDestroy() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
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
		return const_cast<BaseNormal*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	BaseNormalData* _baseNormalData = nullptr;
	BaseNormalData* GetBaseNormalData();

};
