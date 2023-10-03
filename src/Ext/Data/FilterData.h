#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <HouseClass.h>
#include <ObjectClass.h>
#include <TechnoClass.h>
#include <BulletClass.h>

#include <Common/INI/INIConfig.h>
#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/Status.h>

class FilterData : public INIConfig
{
public:
	std::vector<std::string> AffectTypes{};
	std::vector<std::string> NotAffectTypes{};

	bool AffectTechno = true;
	bool AffectBuilding = true;
	bool AffectInfantry = true;
	bool AffectUnit = true;
	bool AffectAircraft = true;

	bool AffectBullet = true;
	bool AffectMissile = true;
	bool AffectTorpedo = true;
	bool AffectCannon = true;
	bool AffectBomb = true;

	bool AffectStand = false;
	bool AffectSelf = false;
	bool AffectInAir = true;
	std::vector<std::string> NotAffectMarks{};
	std::vector<std::string> OnlyAffectMarks{};

	bool AffectsOwner = true;
	bool AffectsAllies = true;
	bool AffectsEnemies = true;
	bool AffectsCivilian = true;

	virtual void Read(INIBufferReader* ini) override
	{ }

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		AffectTypes = reader->GetList<std::string>(title + "AffectTypes", AffectTypes);
		NotAffectTypes = reader->GetList<std::string>(title + "NotAffectTypes", NotAffectTypes);

		AffectTechno = reader->Get(title + "AffectTechno", AffectTechno);
		AffectBuilding = reader->Get(title + "AffectBuilding", AffectBuilding);
		AffectInfantry = reader->Get(title + "AffectInfantry", AffectInfantry);
		AffectUnit = reader->Get(title + "AffectUnit", AffectUnit);
		AffectAircraft = reader->Get(title + "AffectAircraft", AffectAircraft);
		if (!AffectBuilding && !AffectInfantry && !AffectUnit && !AffectAircraft)
		{
			AffectTechno = false;
		}

		AffectBullet = reader->Get(title + "AffectBullet", AffectBullet);
		AffectMissile = reader->Get(title + "AffectMissile", AffectMissile);
		AffectTorpedo = reader->Get(title + "AffectTorpedo", AffectTorpedo);
		AffectCannon = reader->Get(title + "AffectCannon", AffectCannon);
		AffectBomb = reader->Get(title + "AffectBomb", AffectBomb);
		if (!AffectMissile && !AffectCannon && !AffectBomb)
		{
			AffectBullet = false;
		}

		AffectStand = reader->Get(title + "AffectStand", AffectStand);
		AffectSelf = reader->Get(title + "AffectSelf", AffectSelf);
		AffectInAir = reader->Get(title + "AffectInAir", AffectInAir);
		NotAffectMarks = reader->GetList(title + "NotAffectMarks", NotAffectMarks);
		OnlyAffectMarks = reader->GetList(title + "OnlyAffectMarks", OnlyAffectMarks);

		bool affectsAllies = true;
		if (reader->TryGet(title + "AffectsAllies", affectsAllies))
		{
			AffectsAllies = affectsAllies;
			AffectsOwner = affectsAllies;
		}

		bool affectsOwner = true;
		if (reader->TryGet(title + "AffectsOwner", affectsOwner))
		{
			AffectsOwner = affectsOwner;
		}
		AffectsEnemies = reader->Get(title + "AffectsEnemies", AffectsEnemies);
		AffectsCivilian = reader->Get(title + "AffectsCivilian", AffectsCivilian);
	}

	bool CanAffectHouse(HouseClass* pHouse, HouseClass* pTargetHouse)
	{
		return !pHouse || !pTargetHouse || (pTargetHouse == pHouse ? AffectsOwner : (IsCivilian(pTargetHouse) ? AffectsCivilian : pTargetHouse->IsAlliedWith(pHouse) ? AffectsAllies : AffectsEnemies));
	}

	bool CanAffectType(const char* ID)
	{
		if (!NotAffectTypes.empty() && std::find(NotAffectTypes.begin(), NotAffectTypes.end(), ID) != NotAffectTypes.end())
		{
			return false;
		}
		bool can = AffectTypes.empty();
		if (!can)
		{
			can = std::find(AffectTypes.begin(), AffectTypes.end(), ID) != AffectTypes.end();
		}
		return can;
	}

	bool CanAffectType(AbstractType absType)
	{
		switch (absType)
		{
		case AbstractType::Building:
			return AffectBuilding;
		case AbstractType::Infantry:
			return AffectInfantry;
		case AbstractType::Unit:
			return AffectUnit;
		case AbstractType::Aircraft:
			return AffectAircraft;
		}
		return false;
	}

	bool CanAffectType(BulletType bulletType, bool isLevel)
	{
		switch (bulletType)
		{
		case BulletType::INVISO:
			return true;
		case BulletType::ARCING:
			return AffectCannon;
		case BulletType::BOMB:
			return AffectBomb;
		case BulletType::ROCKET:
		case BulletType::MISSILE:
			// 导弹和直线导弹都算Missile
			if (isLevel)
			{
				return AffectTorpedo;
			}
			return AffectMissile;
		}
		return false;
	}

	bool CanAffectType(BulletClass* pBullet)
	{
		return CanAffectType(pBullet->Type->ID) && CanAffectType(WhatAmI(pBullet), pBullet->Type->Level);
	}

	bool CanAffectType(TechnoClass* pTechno)
	{
		return CanAffectType(pTechno->GetTechnoType()->ID) && CanAffectType(pTechno->WhatAmI());
	}

	bool CanAffectType(ObjectClass* pObject)
	{
		TechnoClass* pTechno = nullptr;
		BulletClass* pBullet = nullptr;
		if (CastToTechno(pObject, pTechno))
		{
			return CanAffectType(pTechno);
		}
		else if (CastToBullet(pObject, pBullet))
		{
			return CanAffectType(pBullet);
		}
		return false;
	}
/* TODO AE标记
	bool IsOnMark(Pointer<ObjectClass> pObject)
	{
		return (null == OnlyAffectMarks || !OnlyAffectMarks.Any())
			&& (null == NotAffectMarks || !NotAffectMarks.Any())
			|| (pObject.TryGetAEManager(out AttachEffectScript aeManager) && IsOnMark(aeManager));
	}

	bool IsOnMark(Pointer<BulletClass> pBullet)
	{
		return (null == OnlyAffectMarks || !OnlyAffectMarks.Any())
			&& (null == NotAffectMarks || !NotAffectMarks.Any())
			|| (pBullet.TryGetAEManager(out AttachEffectScript aeManager) && IsOnMark(aeManager));
	}

	bool IsOnMark(Pointer<TechnoClass> pTechno)
	{
		return (null == OnlyAffectMarks || !OnlyAffectMarks.Any())
			&& (null == NotAffectMarks || !NotAffectMarks.Any())
			|| (pTechno.TryGetAEManager(out AttachEffectScript aeManager) && IsOnMark(aeManager));
	}

	bool IsOnMark(AttachEffectScript aeManager)
	{
		bool hasWhiteList = null != OnlyAffectMarks && OnlyAffectMarks.Any();
		bool hasBlackList = null != NotAffectMarks && NotAffectMarks.Any();
		HashSet<string> marks = null;
		return (!hasWhiteList && !hasBlackList)
			|| aeManager.TryGetMarks(out marks) ? (
				(!hasWhiteList || OnlyAffectMarks.Intersect(marks).Count() > 0)
				&& (!hasBlackList || NotAffectMarks.Intersect(marks).Count() <= 0)
				) : !hasWhiteList;
	}
*/

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->AffectTypes)
			.Process(this->NotAffectTypes)

			.Process(this->AffectTechno)
			.Process(this->AffectBuilding)
			.Process(this->AffectInfantry)
			.Process(this->AffectUnit)
			.Process(this->AffectAircraft)

			.Process(this->AffectBullet)
			.Process(this->AffectMissile)
			.Process(this->AffectTorpedo)
			.Process(this->AffectCannon)
			.Process(this->AffectBomb)

			.Process(this->AffectStand)
			.Process(this->AffectSelf)
			.Process(this->AffectInAir)
			.Process(this->NotAffectMarks)
			.Process(this->OnlyAffectMarks)

			.Process(this->AffectsOwner)
			.Process(this->AffectsAllies)
			.Process(this->AffectsEnemies)
			.Process(this->AffectsCivilian)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		INIConfig::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		INIConfig::Save(stream);
		return const_cast<FilterData*>(this)->Serialize(stream);
	}
#pragma endregion
};
