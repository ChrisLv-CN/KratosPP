#pragma once

#include <Ext/Data/FilterData.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <YRPP.h>


class AttachFireData : public FilterData
{
public:
	bool UseROF = true;
	bool CheckRange = false;
	bool CheckAA = false;
	bool CheckAG = false;
	bool CheckVersus = true;

	bool RadialFire = false;
	int RadialAngle = 180;
	bool RadialZ = true;

	bool SimulateBurst = false;
	int SimulateBurstDelay = 7;
	int SimulateBurstMode = 0;

	bool Feedback = false;

	bool OnlyFireInTransport = false;
	bool UseAlternateFLH = false;

	bool CheckShooterHP = false;
	Vector2D<double> OnlyFireWhenHP = Vector2D<double>::Empty;
	bool CheckTargetHP = false;
	Vector2D<double> OnlyFireWhenTargetHP = Vector2D<double>::Empty;

	bool AffectTerrain = true;

	virtual void Read(INIBufferReader* reader) override
	{
		std::string title = "AttachFire.";

		FilterData::Read(reader, title);

		UseROF = reader->Get(title + "UseROF", UseROF);
		CheckRange = reader->Get(title + "CheckRange", CheckRange);
		CheckAA = reader->Get(title + "CheckAA", CheckAA);
		CheckAG = reader->Get(title + "CheckAG", CheckAG);
		CheckVersus = reader->Get(title + "CheckVersus", CheckVersus);

		RadialFire = reader->Get(title + "RadialFire", RadialFire);
		RadialAngle = reader->Get(title + "RadialAngle", RadialAngle);
		RadialZ = reader->Get(title + "RadialZ", RadialZ);

		SimulateBurst = reader->Get(title + "SimulateBurst", SimulateBurst);
		SimulateBurstDelay = reader->Get(title + "SimulateBurstDelay", SimulateBurstDelay);
		SimulateBurstMode = reader->Get(title + "SimulateBurstMode", SimulateBurstMode);

		Feedback = reader->Get(title + "Feedback", Feedback);

		OnlyFireInTransport = reader->Get(title + "OnlyFireInTransport", OnlyFireInTransport);
		UseAlternateFLH = reader->Get(title + "UseAlternateFLH", UseAlternateFLH);

		std::vector<double> onlyFireWhenHP{};
		if (reader->TryGetList<double>(title + "OnlyFireWhenHP", onlyFireWhenHP) && onlyFireWhenHP.size() > 1)
		{
			double min = onlyFireWhenHP[0];
			double max = onlyFireWhenHP[1];
			if (min > max)
			{
				double temp = min;
				min = max;
				max = temp;
			}
			OnlyFireWhenHP = { min, max };
			CheckShooterHP = !OnlyFireWhenHP.IsEmpty();
		}

		std::vector<double> onlyFireWhenTargetHP{};
		if (reader->TryGetList<double>(title + "OnlyFireWhenTargetHP", onlyFireWhenTargetHP) && onlyFireWhenTargetHP.size() > 1)
		{
			double min = onlyFireWhenTargetHP[0];
			double max = onlyFireWhenTargetHP[1];
			if (min > max)
			{
				double temp = min;
				min = max;
				max = temp;
			}
			OnlyFireWhenTargetHP = { min, max };
			CheckTargetHP = !OnlyFireWhenTargetHP.IsEmpty();
		}

		AffectTerrain = reader->Get(title + "AffectsTerrain", AffectTerrain);
		AffectsAllies = reader->Get(title + "AffectsAllies", AffectsAllies);
		AffectsOwner = reader->Get(title + "AffectsOwner", AffectsAllies);
		AffectsEnemies = reader->Get(title + "AffectsEnemies", AffectsEnemies);
		AffectsCivilian = reader->Get(title + "AffectsCivilian", AffectsCivilian);
	}

	bool CanFireToTarget(AbstractClass* pTarget, ObjectClass* pShooter, TechnoClass* pAttacker, HouseClass* pAttackingHouse,
		WeaponTypeClass* pWeapon)
	{
		bool canFire = true;
		// 检查发射者的血量
		if (CheckShooterHP)
		{
			double hp = 1;
			if (pAttacker)
			{
				hp = pAttacker->GetHealthPercentage();
			}
			else
			{
				hp = pShooter->GetHealthPercentage();
			}
			if (hp < OnlyFireWhenHP.X || hp > OnlyFireWhenHP.Y)
			{
				canFire = false;
			}
		}
		// 检查目标类型
		if (canFire)
		{
			AbstractType targetAbsType = pTarget->WhatAmI();
			switch (targetAbsType)
			{
			case AbstractType::Terrain:
				// 检查伐木
				if (!AffectTerrain)
				{
					canFire = false;
				}
				break;
			case AbstractType::Cell:
				// 检查A地板
				if (CheckAG && !pWeapon->Projectile->AG)
				{
					canFire = false;
				}
				break;
			case AbstractType::Building:
			case AbstractType::Infantry:
			case AbstractType::Unit:
			case AbstractType::Aircraft:
				// 检查类型
				if (!CanAffectType(targetAbsType))
				{
					canFire = false;
					break;
				}
				TechnoClass* pTargetTechno = dynamic_cast<TechnoClass*>(pTarget);
				// 检查目标血量
				if (CheckTargetHP)
				{
					double hp = pTargetTechno->GetHealthPercentage();
					if (hp < OnlyFireWhenTargetHP.X || hp > OnlyFireWhenTargetHP.Y)
					{
						canFire = false;
						break;
					}
				}
				// 检查标记
				/* TODO
				if (!IsOnMark(pTargetTechno))
				{
					canFire = false;
					break;
				}*/
				// 检查名单
				if (!CanAffectType(pTargetTechno->GetTechnoType()->ID))
				{
					canFire = false;
					break;
				}
				// 检查护甲
				/* TODO
				if (CheckVersus && pWeapon->Warhead
					&& (GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWeapon->Warhead)->GetVersus(pTargetTechno->GetTechnoType()->Armor, out bool forceFire, out bool retaliate, out bool passiveAcquire) == 0.0 || !forceFire)
				)
				{
					// 护甲为零，终止发射
					canFire = false;
					break;
				}*/
				// 检查所属
				/* TODO
				HouseClass* pTargetHouse = pTargetTechno->Owner;
				if (!pAttackingHouse.CanAffectHouse(pTargetHouse, AffectsOwner, AffectsAllies, AffectsEnemies, AffectsCivilian))
				{
					// 不可对该所属攻击，终止发射
					canFire = false;
				}*/
				break;
			}
		}
		return canFire;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->UseROF)
			.Process(this->CheckRange)
			.Process(this->CheckAA)
			.Process(this->CheckAG)
			.Process(this->CheckVersus)

			.Process(this->RadialFire)
			.Process(this->RadialAngle)
			.Process(this->RadialZ)

			.Process(this->SimulateBurst)
			.Process(this->SimulateBurstDelay)
			.Process(this->SimulateBurstMode)

			.Process(this->Feedback)

			.Process(this->OnlyFireInTransport)
			.Process(this->UseAlternateFLH)

			.Process(this->CheckShooterHP)
			.Process(this->OnlyFireWhenHP)
			.Process(this->CheckTargetHP)
			.Process(this->OnlyFireWhenTargetHP)

			.Process(this->AffectTerrain)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		FilterData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		FilterData::Save(stream);
		return const_cast<AttachFireData*>(this)->Serialize(stream);
	}
#pragma endregion
};
