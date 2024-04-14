#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <AnimClass.h>

#include "../EffectScript.h"
#include "RevengeData.h"


/// @brief EffectScript
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class RevengeEffect : public EffectScript
{
public:
	EFFECT_SCRIPT(Revenge);

	virtual void Clean() override
	{
		EffectScript::Clean();

		pRevenger = nullptr; // 复仇者
		pRevengerHouse = nullptr; // 复仇者的阵营
		// 检查报复对象
		pRevengeTarget = nullptr; // 报复对象

		_ignoreDefenses = false; // 本次伤害是否真伤

		_skip = false;
		_bingo = false;

		_count = 0;
		_markFrame = 0;
	}

	virtual void OnReceiveDamage(args_ReceiveDamage* args) override;

	virtual void OnReceiveDamageReal(int* pRealDamage, WarheadTypeClass* pWH, TechnoClass* pAttacker, HouseClass* pAttackingHouse) override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse) override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->pRevenger)
			.Process(this->pRevengerHouse)
			.Process(this->pRevengeTarget)
			.Process(this->_ignoreDefenses)
			.Process(this->_skip)
			.Process(this->_bingo)
			.Process(this->_count)
			.Process(this->_markFrame)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		EffectScript::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectScript::Save(stream);
		return const_cast<RevengeEffect*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	/**
	 *@brief 是否可以报复，并获得报复对象
	 *
	 * @param pRevenger 复仇者
	 * @param pRevengerHouse 复仇者所属
	 * @param pRevengeTarget 报复对象
	 * @param pWH 弹头类型
	 * @param pAttacking 攻击者
	 * @param pAttackingHouse 攻击者所属
	 * @return true 可以报复
	 * @return false 不能报复
	 */
	bool CanRevenge(TechnoClass*& pRevenger, HouseClass*& pRevengerHouse, TechnoClass*& pRevengeTarget,
		WarheadTypeClass* pWH, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

	TechnoClass* pRevenger = nullptr; // 复仇者
	HouseClass* pRevengerHouse = nullptr; // 复仇者的阵营
	// 检查报复对象
	TechnoClass* pRevengeTarget = nullptr; // 报复对象

	bool _ignoreDefenses = false; // 本次伤害是否真伤

	bool _skip = false;
	bool _bingo = false;

	int _count = 0;
	int _markFrame = 0;
};
