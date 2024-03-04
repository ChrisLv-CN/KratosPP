#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "SupportSpawnsData.h"

/// @brief 动态载入组件
class SupportSpawns : public TechnoScript
{
public:

	TECHNO_SCRIPT(SupportSpawns);

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnPut(CoordStruct* pCoord, DirType dirType) override;

	virtual void OnRemove() override;

	virtual void OnUpdate() override;

	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(_alwaysFire)
			.Process(_rof)
			.Process(_flipY)
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
		return const_cast<SupportSpawns*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	void Setup();

	void FireSupportWeaponToSpawn(bool checkROF = false);

	SupportSpawnsData* _data = nullptr;
	SupportSpawnsData* GetSupportSpawnsData();

	SupportSpawnsFLHData* _flhData = nullptr;
	SupportSpawnsFLHData* GetSupportSpawnsFLHData();

	bool _alwaysFire = false;
	std::map<std::string, CDTimerClass> _rof{};
	int _flipY = -1;
};
