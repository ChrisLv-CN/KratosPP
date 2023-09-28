#pragma once

#include <GeneralStructures.h>
#include <AbstractClass.h>
#include <WeaponTypeClass.h>
#include <Extension/WeaponTypeExt.h>

struct DelayFire
{
public:
	AbstractClass* pTarget = nullptr;
	CoordStruct FLH = CoordStruct::Empty;
	bool FireOwnWeapon = false;
	int WeaponIndex = -1;
	WeaponTypeClass* pWeapon = nullptr;
	WeaponTypeExt::TypeData WeaponTypeData{};

	bool Invalid = true;

	DelayFire() {};

	DelayFire(AbstractClass* pTarget, int delay = 0, int count = 1)
	{
		this->pTarget = pTarget;
		this->_delay = delay;
		this->_timer.Start(delay);
		this->_count = count;

		this->Invalid = pTarget == nullptr;
	}

	DelayFire(int weaponIndex, AbstractClass* pTarget, int delay = 0, int count = 1) : DelayFire(pTarget, delay, count)
	{
		this->FireOwnWeapon = true;
		this->WeaponIndex = weaponIndex;
		this->pWeapon = nullptr;
	}

	DelayFire(WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData weaponTypeData, AbstractClass* pTarget, int delay = 0, int count = 1) : DelayFire(pTarget, delay, count)
	{
		this->FireOwnWeapon = false;
		this->WeaponIndex = -1;
		this->pWeapon = pWeapon;
		this->WeaponTypeData = weaponTypeData;
	}

	bool TimesUp()
	{
		return _timer.Expired();
	}

	void RecuceOnce()
	{
		_count--;
		_timer.Start(_delay);
	}

	void Done()
	{
		_count = -1;
	}

	bool NotDone()
	{
		return _count > 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pTarget)
			.Process(this->FLH)
			.Process(this->FireOwnWeapon)
			.Process(this->WeaponIndex)
			.Process(this->pWeapon)
			.Process(this->WeaponTypeData)
			.Process(this->Invalid)

			.Process(this->_delay)
			.Process(this->_timer)
			.Process(this->_count)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<DelayFire*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	int _delay = 0;
	CDTimerClass _timer{};
	int _count = 0;
};
