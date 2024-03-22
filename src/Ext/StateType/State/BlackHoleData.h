#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class BlackHoleEntity
{
public:
	bool Enable = false;

	double Range = 0.0;
	bool FullAirspace = false;

	int Rate = 0;

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Range = reader->Get(title + "Range", Range);
		FullAirspace = reader->Get(title + "FullAirspace", FullAirspace);

		Rate = reader->Get(title + "Rate", Rate);

		Enable = Range != 0.0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Range)
			.Process(this->FullAirspace)
			.Process(this->Rate)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<BlackHoleEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};


class BlackHoleData : public EffectData
{
public:
	EFFECT_DATA(BlackHole);

	BlackHoleEntity Data{};
	BlackHoleEntity EliteData{};

	CoordStruct Offset = CoordStruct::Empty;
	bool IsOnTurret = true;
	bool DontScan = false;

	double Weight = -1;
	bool CaptureFromSameWeight = true;
	bool CaptureFromWarhead = false;
	int CaptureSpeed = (int)(12 * 2.55);
	bool CaptureIgnoreWeight = false;
	bool AllowEscape = false;
	bool AllowCrawl = true;
	bool AllowRotateUnit = true;
	bool AllowPassBuilding = false;

	int Damage = 0;
	int DamageDelay = 0;
	std::string DamageWH{ "" };
	bool AllowFallingDestroy = false;
	int FallingDestroyHeight = 2 * Unsorted::LevelHeight;
	bool AllowDamageTechno = true;
	bool AllowDamageBullet = false;

	bool ClearTarget = false;
	bool ChangeTarget = false;
	bool OutOfControl = false;

	bool AffectBlackHole = true;

	BlackHoleData() : EffectData()
	{
		AffectsOwner = false;
		AffectsAllies = false;
		AffectsEnemies = true;
		AffectsCivilian = false;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "BlackHole.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		BlackHoleEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Data = data;
			EliteData = Data;
		}

		BlackHoleEntity eliteData;
		eliteData.Read(reader, title + "Elite");
		if (eliteData.Enable)
		{
			EliteData = eliteData;
		}

		Enable = Data.Enable || EliteData.Enable;

		Offset = reader->Get(title + "Offset", Offset);
		IsOnTurret = reader->Get(title + "IsOnTurret", IsOnTurret);
		DontScan = reader->Get(title + "DontScan", DontScan);

		Weight = reader->Get(title + "Weight", Weight);
		CaptureFromWarhead = reader->Get(title + "CaptureFromWarhead", CaptureFromWarhead);
		if (Enable)
		{
			Enable = !DontScan || CaptureFromWarhead; // 既不扫描也不从弹头捕获，黑洞无效
		}
		CaptureFromSameWeight = reader->Get(title + "CaptureFromSameWeight", CaptureFromSameWeight);
		int speed = reader->Get(title + "CaptureSpeed", 0);
		if (speed != 0)
		{
			CaptureSpeed = (int)(speed * 2.55);
		}
		CaptureIgnoreWeight = reader->Get(title + "CaptureIgnoreWeight", CaptureIgnoreWeight);
		AllowEscape = reader->Get(title + "AllowEscape", AllowEscape);
		AllowCrawl = reader->Get(title + "AllowCrawl", AllowCrawl);
		AllowRotateUnit = reader->Get(title + "AllowRotateUnit", AllowRotateUnit);
		AllowPassBuilding = reader->Get(title + "AllowPassBuilding", AllowPassBuilding);

		Damage = reader->Get(title + "Damage", Damage);
		DamageDelay = reader->Get(title + "Damage.Delay", DamageDelay);
		DamageWH = reader->Get(title + "Damage.Warhead", DamageWH);
		AllowFallingDestroy = reader->Get(title + "AllowFallingDestroy", AllowFallingDestroy);
		FallingDestroyHeight = reader->Get(title + "FallingDestroyHeight", FallingDestroyHeight);

		AllowDamageTechno = reader->Get(title + "AllowDamageTechno", AllowDamageTechno);
		AllowDamageBullet = reader->Get(title + "AllowDamageBullet", AllowDamageBullet);
		ClearTarget = reader->Get(title + "ClearTarget", ClearTarget);
		ChangeTarget = reader->Get(title + "ChangeTarget", ChangeTarget);
		OutOfControl = reader->Get(title + "OutOfControl", OutOfControl);

		AffectBlackHole = reader->Get(title + "AffectBlackHole", AffectBlackHole);

	}

	int GetCaptureSpeed(double weight)
	{
		// F = mtv, v = F/mv
		if (!CaptureIgnoreWeight && weight != 0)
		{
			return (int)(CaptureSpeed / weight);
		}
		return CaptureSpeed;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)
			.Process(this->EliteData)

			.Process(this->Offset)
			.Process(this->IsOnTurret)
			.Process(this->DontScan)

			.Process(this->Weight)
			.Process(this->CaptureFromSameWeight)
			.Process(this->CaptureFromWarhead)
			.Process(this->CaptureSpeed)
			.Process(this->CaptureIgnoreWeight)
			.Process(this->AllowEscape)
			.Process(this->AllowCrawl)
			.Process(this->AllowRotateUnit)
			.Process(this->AllowPassBuilding)

			.Process(this->Damage)
			.Process(this->DamageDelay)
			.Process(this->DamageWH)
			.Process(this->AllowFallingDestroy)
			.Process(this->FallingDestroyHeight)
			.Process(this->AllowDamageTechno)
			.Process(this->AllowDamageBullet)

			.Process(this->ClearTarget)
			.Process(this->ChangeTarget)
			.Process(this->OutOfControl)

			.Process(this->AffectBlackHole)
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
		return const_cast<BlackHoleData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
