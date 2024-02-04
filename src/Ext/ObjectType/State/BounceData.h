#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>
#include <CellClass.h>
#include <MapClass.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/Physics.h>


class BounceData : public EffectData
{
public:
	double Chance = 0; // 发生弹跳的概率

	double Elasticity = 0.5; // 弹性衰减系数
	int Limit = 128; // 衰减后，可以跳的距离
	int Times = -1; // 跳几次
	bool ExplodeOnHit = true; // 命中时是否爆炸
	std::string ExpireAnim{ "" }; // 命中时播放动画

	std::string Weapon{ "" }; // 跳弹发生时使用自定义武器

	bool OnWater = false; // 打水漂
	std::vector<LandType> OnLands{};
	std::vector<TileType> OnTiles{};

	bool StopOnBuilding = true;
	bool ReboundOnBuilding = true;
	bool StopOnInfantry = false;
	bool ReboundOnInfantry = false;
	bool StopOnUnit = true;
	bool ReboundOnUnit = false;

	BounceData()
	{
		AffectTechno = false;
	}


	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, TITLE);
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Chance = reader->GetChance(title + "Chance", Chance);
		Elasticity = reader->Get(title + "Elasticity", Elasticity);
		Enable = Chance > 0 && Elasticity > 0;
		if (Enable)
		{
			Enable = AffectBullet && AffectCannon;
		}

		Limit = reader->Get(title + "Limit", Limit);
		Times = reader->Get(title + "Times", Times);
		ExplodeOnHit = reader->Get(title + "ExplodeOnHit", ExplodeOnHit);
		ExpireAnim = reader->Get(title + "ExpireAnim", ExpireAnim);
		ExpireAnim = reader->Get(title + "ExpireAnim", ExpireAnim);

		Weapon = reader->Get(title + "Weapon", Weapon);

		OnWater = reader->Get(title + "OnWater", OnWater);
		OnLands = reader->GetList<LandType>(title + "OnLands", OnLands);
		OnTiles = reader->GetList<TileType>(title + "OnTiles", OnTiles);

		StopOnBuilding = reader->Get(title + "StopOnBuilding", StopOnBuilding);
		ReboundOnBuilding = reader->Get(title + "ReboundOnBuilding", ReboundOnBuilding);
		StopOnInfantry = reader->Get(title + "StopOnInfantry", StopOnInfantry);
		ReboundOnInfantry = reader->Get(title + "ReboundOnInfantry", ReboundOnInfantry);
		StopOnUnit = reader->Get(title + "StopOnUnit", StopOnUnit);
		ReboundOnUnit = reader->Get(title + "ReboundOnUnit", ReboundOnUnit);
	}

	bool IsOnLandType(CellClass* pCell, LandType& landType)
	{
		landType = pCell->LandType;
		if (landType == LandType::Water && pCell->Flags & CellFlags::Bridge)
		{
			// 将水面上的桥判定为路面
			landType = LandType::Road;
		}
		auto it = std::find(OnLands.begin(), OnLands.end(), landType);
		if (it != OnLands.end())
		{
			return true;
		}
		return OnWater || landType != LandType::Water;
	}

	bool IsOnTileType(CellClass* pCell, TileType& tileType)
	{
		tileType = pCell->GetTileType();
		auto it = std::find(OnTiles.begin(), OnTiles.end(), tileType);
		if (it != OnTiles.end())
		{
			return true;
		}
		return OnWater || tileType != TileType::Water;
	}

	bool Stop(CellClass* pCell, bool& rebound)
	{
		bool stop = false;
		rebound = false;
		ObjectClass* pObject = pCell->GetContent();
		do
		{
			if (pObject)
			{
				switch (pObject->WhatAmI())
				{
				case AbstractType::Building:
					stop = StopOnBuilding;
					rebound = ReboundOnBuilding;
					break;
				case AbstractType::Infantry:
					stop = StopOnInfantry;
					rebound = ReboundOnInfantry;
					break;
				case AbstractType::Unit:
					stop = StopOnUnit;
					rebound = ReboundOnUnit;
					break;
				}
			}
		} while (!stop && pObject && (pObject = pObject->NextObject) != nullptr);
		return stop;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Chance)
			.Process(this->Elasticity)

			.Process(this->Limit)
			.Process(this->Times)
			.Process(this->ExplodeOnHit)
			.Process(this->ExpireAnim)

			.Process(this->Weapon)

			.Process(this->OnWater)
			.Process(this->OnLands)
			.Process(this->OnTiles)

			.Process(this->StopOnBuilding)
			.Process(this->ReboundOnBuilding)
			.Process(this->StopOnInfantry)
			.Process(this->ReboundOnInfantry)
			.Process(this->StopOnUnit)
			.Process(this->ReboundOnUnit)
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
		return const_cast<BounceData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "Bounce.";
};

class BounceLandData : public INIConfig
{
public:
	double Elasticity = 1.0; // 弹性削减比例

	virtual void Read(INIBufferReader* reader) override
	{
		Elasticity = reader->Get("Bounce.Elasticity", Elasticity);

		Enable = Elasticity != 1.0;
	}
};

