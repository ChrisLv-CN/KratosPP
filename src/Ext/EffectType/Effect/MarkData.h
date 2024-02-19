#pragma once

#include <string>
#include <vector>

#include "EffectData.h"

class MarkData : public EffectData
{
public:
	EFFECT_DATA(Mark);

	std::vector<std::string> Names{};

	virtual void Read(INIBufferReader* reader) override
	{
		std::string title = "Mark.";
		EffectData::Read(reader, title);

		Names = reader->GetList(title + "Names", Names);

		Enable = !Names.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Names)
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
		return const_cast<MarkData*>(this)->Serialize(stream);
	}
#pragma endregion

};


