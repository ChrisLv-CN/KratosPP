#pragma once

#include <GeneralStructures.h>
#include <AbstractClass.h>
#include <HouseClass.h>
#include <TechnoClass.h>

#include <Ext/Helper/Weapon.h>

struct SimulateBurst
{
public:
	AbstractClass* pTarget = nullptr;

	TechnoClass* pAttacker = nullptr;
	HouseClass* pAttackingHouse = nullptr;

	WeaponTypeClass* pWeapon = nullptr;
	WeaponTypeExt::TypeData WeaponTypeData{};
	int MinRange = 0;
	int MaxRange = 0;

	CoordStruct FLH = CoordStruct::Empty;
	bool IsOnBody = false;
	bool IsOnTarget = false;
	FireBulletToTarget Callback = nullptr;

	// burst 控制
	int Burst = 1; // 总数
	int Index = 0; // 当前发射的序号
	int FlipY = 1; // 翻转FLH

	bool Invalid = true;

	SimulateBurst() {}

	SimulateBurst(AbstractClass* pTarget, TechnoClass* pAttacker, HouseClass* pAttackingHouse,
		WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData weaponTypeData, int minRange = -1, int maxRange = -1,
		CoordStruct flh = CoordStruct::Empty, bool isOnBody = false, bool isOnTarget = false,
		int burst = 1, int flipY = 1, FireBulletToTarget callback = nullptr)
	{
		this->pTarget = pTarget;
		this->pAttacker = pAttacker;
		this->pAttackingHouse = pAttackingHouse;

		this->pWeapon = pWeapon;
		this->WeaponTypeData = weaponTypeData;
		this->MinRange = minRange;
		this->MaxRange = maxRange;

		this->FLH = flh;
		this->IsOnBody = isOnBody;
		this->IsOnTarget = IsOnTarget;

		this->Burst = burst;
		this->FlipY = flipY;
		this->Callback = callback;

		this->_delay = weaponTypeData.SimulateBurstDelay;
		this->_timer.Start(_delay);

		this->Invalid = false;
	}

	bool CanFire()
	{
		if (_timer.Expired())
		{
			_timer.Start(_delay);
			return true;
		}
		return false;
	}

	void CountOne()
	{
		Index++;
		switch (WeaponTypeData.SimulateBurstMode)
		{
		case 1:
			// 左右切换
			FlipY *= -1;
			break;
		case 2:
			// 左一半右一半
			FlipY = (Index < Burst / 2.0f) ? _flag : -_flag;
			break;
		default:
			break;
		}
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pTarget)
			.Process(this->pAttacker)
			.Process(this->pAttackingHouse)

			.Process(this->pWeapon)
			.Process(this->WeaponTypeData)
			.Process(this->MinRange)
			.Process(this->MaxRange)

			.Process(this->FLH)
			.Process(this->IsOnBody)
			.Process(this->IsOnTarget)

			.Process(this->Callback)

			.Process(this->Burst)
			.Process(this->Index)
			.Process(this->FlipY)

			.Process(this->Invalid)

			.Process(this->_delay)
			.Process(this->_timer)
			.Process(this->_flag)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<SimulateBurst*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	int _delay = 0; // burst延迟
	CDTimerClass _timer; // 延迟计时器
	int _flag = 1; // 翻转FLH的记录
};
