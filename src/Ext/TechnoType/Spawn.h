#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "SpawnData.h"

/// @brief 动态载入组件
class Spawn : public TechnoScript
{
public:

	TECHNO_SCRIPT(Spawn);

	SpawnData* GetSpawnData();

	bool TryGetSpawnType(int i, std::string& newId);

	virtual void Clean() override
	{
		TechnoScript::Clean();

		_spawnData = nullptr;
	}

	virtual void Awake() override;

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
		return const_cast<Spawn*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	// 子机管理器
	SpawnData* _spawnData = nullptr;

};
