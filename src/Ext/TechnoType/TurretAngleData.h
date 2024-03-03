#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

enum class DeathZoneAction : int
{
	CLEAR = 0, BLOCK = 1, TURN = 2
};

template <>
inline bool Parser<DeathZoneAction>::TryParse(const char* pValue, DeathZoneAction* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'B':
		if (outValue)
		{
			*outValue = DeathZoneAction::BLOCK;
		}
		return true;
	case 'T':
		if (outValue)
		{
			*outValue = DeathZoneAction::TURN;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = DeathZoneAction::CLEAR;
		}
		return true;
	}
}

class TurretAngleData : public INIConfig
{
public:
	int DefaultAngle = 0;

	bool AngleLimit = false;
	Point2D Angle{};
	DeathZoneAction Action = DeathZoneAction::CLEAR;

	bool AutoTurn = false;
	Point2D SideboardAngleL{};
	Point2D SideboardAngleR{};

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Turret.");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		DefaultAngle = reader->Get(title + "DefaultAngle", DefaultAngle);
		if (DefaultAngle < 0)
		{
			DefaultAngle = -DefaultAngle;
		}
		if (DefaultAngle >= 360)
		{
			DefaultAngle = 0;
		}


		Angle = reader->Get(title + "Angle", Angle);
		// 计算死区
		if (!Angle.IsEmpty())
		{
			FormatAngle360(Angle);
		}
		AngleLimit = !Angle.IsEmpty() && (Angle.Y - Angle.X) > 0;

		Action = reader->Get(title + "Action", Action);
		SideboardAngleL = reader->Get(title + "SideboardAngleL", SideboardAngleL);
		if (!SideboardAngleL.IsEmpty())
		{
			FormatAngle180(SideboardAngleL);
			// 换算成180-360，顺时针
			int min = SideboardAngleL.X;
			int max = SideboardAngleL.Y;
			min = 360 - min;
			max = 360 - max;
			SideboardAngleL.X = max;
			SideboardAngleL.Y = min;
			if (AngleLimit)
			{
				// 存在死区，可以区域不能覆盖死区
				if (SideboardAngleL.X < Angle.Y)
				{
					SideboardAngleL.X = Angle.Y;
				}
				if (SideboardAngleL.Y < Angle.Y)
				{
					SideboardAngleL.Y = Angle.Y;
				}
			}
		}
		SideboardAngleR = reader->Get(title + "SideboardAngleR", SideboardAngleR);
		if (!SideboardAngleR.IsEmpty())
		{
			FormatAngle180(SideboardAngleR);
			if (AngleLimit)
			{
				// 存在死区，可以区域不能覆盖死区
				if (SideboardAngleR.X > Angle.X)
				{
					SideboardAngleR.X = Angle.X;
				}
				if (SideboardAngleR.Y > Angle.X)
				{
					SideboardAngleR.Y = Angle.X;
				}
			}
		}
		AutoTurn = (!SideboardAngleL.IsEmpty() && (SideboardAngleL.X > 180 || SideboardAngleL.Y < 360))
			|| (!SideboardAngleR.IsEmpty() && (SideboardAngleR.X > 0 || SideboardAngleR.Y < 180));

		Enable = DefaultAngle > 0 || AngleLimit || AutoTurn;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->DefaultAngle)
			.Process(this->AngleLimit)
			.Process(this->Angle)
			.Process(this->Action)
			.Process(this->AutoTurn)
			.Process(this->SideboardAngleL)
			.Process(this->SideboardAngleR)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		INIConfig::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		INIConfig::Save(stream);
		return const_cast<TurretAngleData*>(this)->Serialize(stream);
	}
#pragma endregion
private:

	void FormatAngle180(Point2D& angle)
	{
		int min = angle.X;
		int max = angle.Y;
		if (min < 0)
		{
			min = -min;
		}
		if (min > 180)
		{
			min = 0;
		}
		if (max < 0)
		{
			max = -max;
		}
		if (max > 180 || max == 0)
		{
			max = 180;
		}
		if (max < min)
		{
			angle.X = max;
			angle.Y = min;
		}
		else
		{
			angle.X = min;
			angle.Y = max;
		}
	}

	/// <summary>
	/// 将X=[0,180]，Y=[0,180]，换算成[0,360]，重新分配给X=min，Y=max
	/// 顺时针旋转，所以X是右，Y是左，与设定值颠倒
	/// </summary>
	/// <param name="angle"></param>
	void FormatAngle360(Point2D& angle)
	{
		// 最大值
		int max = angle.X;
		if (max < 0)
		{
			max = -max;
		}
		if (max > 180)
		{
			max = 180;
		}
		else if (max == 0)
		{
			max = 360;
		}
		else
		{
			max = 360 - max;
		}
		// 最小值
		int min = angle.Y;
		if (min < 0)
		{
			min = -min;
		}
		if (min > 180)
		{
			min = 180;
		}
		angle.X = min;
		angle.Y = max;
	}

};

