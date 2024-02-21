#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <WarheadTypeClass.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>

class RevengeData : public EffectData
{
public:
	EFFECT_DATA(Revenge);

	double Chance = 1;

	// 使用武器反击
	std::vector<std::string> Types{};
	int WeaponIndex = -1;

	// 使用AE反击
	std::vector<std::string> AttachEffects{};
	std::vector<double> AttachChances{};

	// 伤害分配
	double DamageSelfPercent = 1; // 自身吃到的伤害比例
	double DamageEnemyPercent = 0; // 敌人吃到的伤害比例

	CoordStruct FireFLH = CoordStruct::Empty;
	bool IsOnTurret = true;
	bool IsOnTarget = false;
	bool Realtime = false;
	bool FromSource = false;
	bool ToSource = false;

	bool ActiveOnce = false;

	std::vector<std::string> OnlyReactionWarheads{};
	std::vector<std::string> NotReactionWarheads{};

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader);
		Read(reader, "Revenge.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Chance = reader->GetChance(title + "Chance", Chance);

		Types = reader->GetList(title + "Types", Types);
		WeaponIndex = reader->Get(title + "WeaponIndex", WeaponIndex);

		AttachEffects = reader->GetList(title + "AttachEffects", AttachEffects);
		AttachChances = reader->GetChanceList(title + "AttachChances", AttachChances);

		DamageSelfPercent = reader->GetPercent(title + "DamageSelfPercent", DamageSelfPercent);
		DamageEnemyPercent = reader->GetPercent(title + "DamageEnemyPercent", DamageEnemyPercent);

		Enable = Chance > 0 && (!Types.empty() || WeaponIndex > -1 || !AttachEffects.empty() || DamageSelfPercent != 1 || DamageEnemyPercent != 0);
		if (Enable)
		{
			Enable = AffectTechno;
		}
		FireFLH = reader->Get(title + "FireFLH", FireFLH);
		IsOnTurret = reader->Get(title + "IsOnTurret", IsOnTurret);
		IsOnTarget = reader->Get(title + "IsOnTarget", IsOnTarget);
		Realtime = reader->Get(title + "Realtime", Realtime);
		FromSource = reader->Get(title + "FromSource", FromSource);
		ToSource = reader->Get(title + "ToSource", ToSource);

		ActiveOnce = reader->Get(title + "ActiveOnce", ActiveOnce);

		OnlyReactionWarheads = reader->GetList(title + "OnlyReactionWarheads", OnlyReactionWarheads);
		if (OnlyReactionWarheads.size() == 1 && !IsNotNone(OnlyReactionWarheads[0]))
		{
			OnlyReactionWarheads.clear();
		}

		NotReactionWarheads = reader->GetList(title + "NotReactionWarheads", NotReactionWarheads);
		if (NotReactionWarheads.size() == 1 && !IsNotNone(NotReactionWarheads[0]))
		{
			NotReactionWarheads.clear();
		}
	}

	bool OnMark(std::string warheadId)
	{
		bool mark = true;
		bool hasWhiteList = !OnlyReactionWarheads.empty();
		bool hasBlackList = !NotReactionWarheads.empty();
		if (hasWhiteList)
		{
			auto it = std::find(OnlyReactionWarheads.begin(), OnlyReactionWarheads.end(), warheadId);
			mark = it != OnlyReactionWarheads.end();
		}
		if (!mark && hasBlackList)
		{
			auto it = std::find(NotReactionWarheads.begin(), NotReactionWarheads.end(), warheadId);
			mark = it == NotReactionWarheads.end();
		}
		return mark;
	}

	bool OnMark(WarheadTypeClass* pWH)
	{
		return OnMark(pWH->ID);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Chance)

			.Process(this->Types)
			.Process(this->WeaponIndex)

			.Process(this->AttachEffects)
			.Process(this->AttachChances)

			.Process(this->DamageSelfPercent)
			.Process(this->DamageEnemyPercent)

			.Process(this->FireFLH)
			.Process(this->IsOnTurret)
			.Process(this->IsOnTarget)
			.Process(this->Realtime)
			.Process(this->FromSource)
			.Process(this->ToSource)
			.Process(this->ActiveOnce)
			.Process(this->OnlyReactionWarheads)
			.Process(this->NotReactionWarheads)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		EffectData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectData::Save(stream);
		return const_cast<RevengeData*>(this)->Serialize(stream);
	}
#pragma endregion
};
