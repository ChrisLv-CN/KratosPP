#pragma once
#include <string>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class TargetLaserData : public INIConfig
{
public:
	bool IsTargetLaser = false;
	CoordStruct TargetLaserOffset = CoordStruct::Empty;
	Point2D TargetLaserShake = Point2D{ -15, 15 };
	double TargetLaserRange = 0.5;
	bool TargetLaserPoint = true;

	ColorStruct LaserInnerColor = Colors::Empty;
	bool IsHouseColor = true;

	virtual void Read(INIBufferReader* reader) override
	{
		IsTargetLaser = reader->Get("IsTargetLaser", IsTargetLaser);
		TargetLaserOffset = reader->Get("TargetLaserOffset", TargetLaserOffset);
		TargetLaserShake = reader->GetRange("TargetLaserShake", TargetLaserShake);
		TargetLaserRange = reader->Get("TargetLaserRange", TargetLaserRange);
		TargetLaserPoint = reader->Get("TargetLaserPoint", TargetLaserPoint);

		LaserInnerColor = reader->Get("LaserInnerColor", LaserInnerColor);
		IsHouseColor = reader->Get("IsHouseColor", IsHouseColor);

		Enable = IsTargetLaser;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->IsTargetLaser)
			.Process(this->TargetLaserOffset)
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
	TargetLaserData Data{}; // 配置

	AbstractClass* Target = nullptr; // 照射的目标

	int RangeLimit = -1; // 射程限制
	CoordStruct FLH = CoordStruct::Empty; // 发射的FLH
	bool IsOnTurret = true; // 发射位置在炮塔上

	CoordStruct TargetOffset = CoordStruct::Empty; // 目标位置偏移
	CDTimerClass ShakeTimer{}; // 抖动延迟计时器

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)

			.Process(this->Target)

			.Process(this->RangeLimit)
			.Process(this->FLH)
			.Process(this->IsOnTurret)

			.Process(this->TargetOffset)
			.Process(this->ShakeTimer)
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

