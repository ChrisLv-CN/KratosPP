#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "DecoyMissileData.h"

class TechnoStatus;

/// @brief 动态载入组件
class DecoyMissile : public TechnoScript
{
public:

	TECHNO_SCRIPT(DecoyMissile);

	void OnMissileDelete(EventSystem* sender, Event e, void* args)
	{
		auto it = std::find(_decoys.begin(), _decoys.end(), args);
		if (it != _decoys.end())
		{
			_decoys.erase(it);
		}
	}

	virtual void Awake() override;

	virtual void Destroy() override;

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
			.Process(this->_bullets)
			.Process(this->_decoys)
			.Process(this->_delayTimer)
			.Process(this->_reloadTimer)
			.Process(this->_fire)
			.Success();
	};
	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		EventSystems::Logic.AddHandler(Events::MissileDeleteEvent, this, &DecoyMissile::OnMissileDelete);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<DecoyMissile*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	void Setup();

	void ClearDeadDecoy();

	bool DropOne(WeaponTypeClass* pWeapon);

	void Reload(WeaponTypeClass* pWeapon);

	void AddDecoy(BulletClass* pDecoy, CoordStruct launchPos, int life);

	BulletClass* RandomDecoy();

	BulletClass* CloseEnoughDecoy(CoordStruct pos, double min);

	// 热诱弹
	DecoyMissileData* _data{}; // 个体设置
	DecoyMissileData* GetDecoyMissileData();

	int _bullets = -1;
	std::vector<BulletClass*> _decoys;
	CDTimerClass _delayTimer{};
	CDTimerClass _reloadTimer{};
	bool _fire = false;
};
