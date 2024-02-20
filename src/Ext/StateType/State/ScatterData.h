#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class ScatterData : public EffectData
{
public:
	EFFECT_DATA(Scatter);

	CoordStruct MoveToFLH = CoordStruct::Empty;
	bool Panic = false;

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader);
		Read(reader, "Scatter.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		// read Scatter.Enable=yes
		EffectData::Read(reader, title);

		MoveToFLH = reader->Get(title + "MoveToFLH", MoveToFLH);
		Panic = reader->Get(title + "Panic", Panic);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
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
		return const_cast<ScatterData*>(this)->Serialize(stream);
	}
#pragma endregion
};
