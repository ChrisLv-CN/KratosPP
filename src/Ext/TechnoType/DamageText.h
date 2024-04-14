#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "DamageTextData.h"

/// @brief 动态载入组件
class DamageText : public TechnoScript
{
public:

	TECHNO_SCRIPT(DamageText);

	/**
	 * @brief 是否打印本次伤害数字
	 *
	 * @param pWH 弹头
	 * @param damageTextType 格式
	 * @return true
	 * @return false
	 */
	bool SkipDrawDamageText(WarheadTypeClass* pWH, DamageTextData*& damageTextType);
	/**
	 *@brief 打印伤害数字
	 *
	 * @param text 内容
	 * @param location 位置
	 * @param data 格式
	 */
	void OrderDamageText(std::wstring text, CoordStruct location, DamageTextEntity*& data);

	virtual void Clean() override
	{
		TechnoScript::Clean();

		SkipDamageText = false;
		_damageCache.clear();
		_repairCache.clear();
	}

	virtual void Awake() override;

	virtual void OnUpdate() override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse) override;

	// 本次伤害不记录伤害数字
	bool SkipDamageText = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->SkipDamageText)
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
		return const_cast<DamageText*>(this)->Serialize(stream);
	}
#pragma endregion

private:

	std::map<DamageTextEntity*, DamageTextCache> _damageCache{};
	std::map<DamageTextEntity*, DamageTextCache> _repairCache{};

};
