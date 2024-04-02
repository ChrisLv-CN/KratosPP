#pragma once
#include <string>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>

class PaintballData : public EffectData
{
public:
	EFFECT_DATA(Paintball);

	ColorStruct Color = Colors::Empty;
	bool IsHouseColor = false; // 使用所属色
	float BrightMultiplier = 1.0f; // 亮度系数

	unsigned int Color2 = 0; // RGB565
	bool ChangeColor = false;
	bool ChangeBright = false;

	void SetColor(ColorStruct color)
	{
		Color = color;
		Color2 = Add2RGB565(ToColorAdd(Color));
		ChangeColor = Color2 != 0 || IsHouseColor;

		Enable = ChangeColor || ChangeBright;
	}

	void SetBrightMultiplier(float brightMultiplier)
	{
		BrightMultiplier = brightMultiplier;
		ChangeBright = BrightMultiplier != 1.0f;

		Enable = ChangeColor || ChangeBright;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader, TITLE);

		Color = reader->Get(TITLE + "Color", Color);
		IsHouseColor = reader->Get(TITLE + "IsHouseColor", IsHouseColor);

		SetColor(Color);

		BrightMultiplier = reader->Get(TITLE + "BrightMultiplier", BrightMultiplier);
		if (BrightMultiplier < 0.0f)
		{
			BrightMultiplier = 0.0f;
		}
		else if (BrightMultiplier > 2.0f)
		{
			BrightMultiplier = 2.0f;
		}
		ChangeBright = BrightMultiplier != 1.0f;

		Enable = ChangeColor || ChangeBright;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Color)
			.Process(this->IsHouseColor)
			.Process(this->BrightMultiplier)
			.Process(this->Color2)
			.Process(this->ChangeColor)
			.Process(this->ChangeBright)
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
		return const_cast<PaintballData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "Paintball.";
};

struct PaintData
{
	bool ChangeColor = false;
	bool ChangeBright = false;
	PaintballData Data{};

	void Reset()
	{
		ChangeColor = false;
		ChangeBright = false;
	}
};
