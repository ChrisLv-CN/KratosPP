#pragma once
#include <string>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class TargetLaserData : public INIConfig
{
public:
	bool IsTargetLaser = false;

	int TargetLaserDuration = -1;
	CoordStruct TargetLaserOffset = CoordStruct::Empty;
	double TargetLaserLength = 0.9;
	Point2D TargetLaserShake = Point2D{ -15, 15 };
	double TargetLaserRange = 0.5;
	bool TargetLaserPoint = true;

	ColorStruct LaserInnerColor = Colors::Empty;
	bool IsHouseColor = true;

	bool BreakTargetLaser = false;

	virtual void Read(INIBufferReader* reader) override
	{
		IsTargetLaser = reader->Get("IsTargetLaser", IsTargetLaser);

		TargetLaserDuration = reader->Get("TargetLaserDuration", TargetLaserDuration);
		TargetLaserOffset = reader->Get("TargetLaserOffset", TargetLaserOffset);
		TargetLaserLength = reader->GetPercent("TargetLaserLength", TargetLaserLength);
		TargetLaserShake = reader->GetRange("TargetLaserShake", TargetLaserShake);
		TargetLaserRange = reader->Get("TargetLaserRange", TargetLaserRange);
		TargetLaserPoint = reader->Get("TargetLaserPoint", TargetLaserPoint);

		LaserInnerColor = reader->Get("LaserInnerColor", LaserInnerColor);
		IsHouseColor = reader->Get("IsHouseColor", IsHouseColor);

		BreakTargetLaser = reader->Get("BreakTargetLaser", BreakTargetLaser);

		Enable = IsTargetLaser || BreakTargetLaser;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->IsTargetLaser)
			.Process(this->TargetLaserOffset)
			.Process(this->TargetLaserLength)
			.Process(this->TargetLaserShake)
			.Process(this->TargetLaserRange)
			.Process(this->TargetLaserPoint)

			.Process(this->LaserInnerColor)
			.Process(this->IsHouseColor)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		return const_cast<TargetLaserData*>(this)->Serialize(stream);
	}
#pragma endregion
};


class TargetLaser
{
public:
	WeaponTypeClass* pWeapon = nullptr; // 照射挂载的武器
	TargetLaserData Data{}; // 配置

	AbstractClass* pTarget = nullptr; // 照射的目标

	CoordStruct FLH = CoordStruct::Empty; // 发射的FLH
	bool IsOnTurret = true; // 发射位置在炮塔上

	CoordStruct TargetOffset = CoordStruct::Empty; // 目标位置偏移
	CDTimerClass ShakeTimer{}; // 抖动延迟计时器

	int Duration = -1;
	CDTimerClass LifeTimer{}; // 生命计时器

	bool Timeup()
	{
		return Duration > 0 && LifeTimer.Expired();
	}

	void ResetTimer()
	{
		LifeTimer.Stop();
		if (Duration > 0)
		{
			LifeTimer.Start(Duration);
		}
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pWeapon)
			.Process(this->Data)

			.Process(this->pTarget)

			.Process(this->FLH)
			.Process(this->IsOnTurret)

			.Process(this->TargetOffset)
			.Process(this->ShakeTimer)

			.Process(this->Duration)
			.Process(this->LifeTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<TargetLaser*>(this)->Serialize(stream);
	}
#pragma endregion
};

