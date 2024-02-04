#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class DestroySelfData : public EffectData
{
public:
	int Delay = -1;
	bool Peaceful = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, TITLE);
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		bool destroySelf = false;
		destroySelf = reader->Get("DestroySelf", false);
		if (destroySelf)
		{
			Delay = 0;
		}
		else
		{
			Delay = reader->Get(title + "Delay", Delay);
		}

		Enable = Delay >= 0;

		Peaceful = reader->Get(title + "Peaceful", Peaceful);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Delay)
			.Process(this->Peaceful)
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
		return const_cast<DestroySelfData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "DestroySelf.";
};
