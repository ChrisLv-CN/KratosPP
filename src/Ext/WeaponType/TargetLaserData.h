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

	ColorStruct LaserInnerColor = Colors::Empty;
	bool IsHouseColor = true;

	virtual void Read(INIBufferReader* reader) override
	{
		IsTargetLaser = reader->Get("IsTargetLaser", IsTargetLaser);
		TargetLaserOffset = reader->Get("TargetLaserOffset", TargetLaserOffset);
		TargetLaserShake = reader->GetRange("TargetLaserShake", TargetLaserShake);
		TargetLaserRange = reader->Get("TargetLaserRange", TargetLaserRange);

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

