#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/FLH.h>

class ExpandAnimsData : public INIConfig
{
public:
	std::vector<std::string> Anims{};
	std::vector<int> Nums{};
	std::vector<double> Chances{};
	bool RandomType = false;
	std::vector<int> RandomWeights{};

	CoordStruct Offset = CoordStruct::Empty;
	bool UseRandomOffset = false;
	Point2D RandomOffset = Point2D::Empty;
	bool UseRandomOffsetFLH = false;
	Point2D RandomOffsetF = Point2D::Empty;
	Point2D RandomOffsetL = Point2D::Empty;
	Point2D RandomOffsetH = Point2D::Empty;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Anims = reader->GetList(title + "Anims", Anims);
		Nums = reader->GetList(title + "Nums", Nums);
		Chances = reader->GetChanceList(title + "Chances", Chances);

		RandomType = reader->Get(title + "RandomType", RandomType);
		RandomWeights = reader->GetList(title + "RandomWeights", RandomWeights);

		Offset = reader->Get(title + "Offset", Offset);
		RandomOffset = reader->GetRange(title + "RandomOffset", RandomOffset);
		UseRandomOffset = !RandomOffset.IsEmpty();
		RandomOffsetF = reader->GetRange(title + "RandomOffsetF", RandomOffsetF);
		RandomOffsetL = reader->GetRange(title + "RandomOffsetL", RandomOffsetL);
		RandomOffsetH = reader->GetRange(title + "RandomOffsetH", RandomOffsetH);
		UseRandomOffsetFLH = !RandomOffsetF.IsEmpty() || !RandomOffsetL.IsEmpty() || !RandomOffsetH.IsEmpty();

		Enable = !Anims.empty();
	}

	CoordStruct GetOffset()
	{
		if (UseRandomOffsetFLH)
		{
			int f = GetRandomValue(RandomOffsetF, 0);
			int l = GetRandomValue(RandomOffsetL, 0);
			int h = GetRandomValue(RandomOffsetH, 0);
			return CoordStruct{ f, l, h };
		}
		else if (UseRandomOffset)
		{
			int min = RandomOffset.X;
			int max = RandomOffset.Y;
			if (max > 0)
			{
				return GetRandomOffset(min, max);
			}
		}
		return Offset;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Anims)
			.Process(this->Nums)
			.Process(this->Chances)
			.Process(this->RandomType)
			.Process(this->RandomWeights)

			.Process(this->Offset)

			.Process(this->UseRandomOffset)
			.Process(this->RandomOffset)

			.Process(this->UseRandomOffsetFLH)
			.Process(this->RandomOffsetF)
			.Process(this->RandomOffsetL)
			.Process(this->RandomOffsetH)
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
		return const_cast<ExpandAnimsData*>(this)->Serialize(stream);
	}
#pragma endregion
};

