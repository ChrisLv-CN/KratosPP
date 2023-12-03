#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/Data/EffectData.h>
#include <Ext/Helper/CastEx.h>


class TransformData : public EffectData
{
public:
	std::string TransformToType{ "" };

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, TITLE);
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		TransformToType = reader->Get(title + "Type", TransformToType);

		Enable = IsNotNone(TransformToType);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->TransformToType)
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
		return const_cast<TransformData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "Transform.";
};
