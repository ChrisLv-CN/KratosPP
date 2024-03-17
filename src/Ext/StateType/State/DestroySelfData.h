#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/StringEx.h>


class DestroySelfData : public EffectData
{
public:
	EFFECT_DATA(DestroySelf);

	int Delay = -1;
	bool Peaceful = false;
	bool InTransport = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, TITLE);
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		std::string destorySelf{ "" };
		destorySelf = reader->Get("DestroySelf", destorySelf);
		if (IsNotNone(destorySelf))
		{
			// 是数字格式
			if (std::regex_match(destorySelf, INIReader::Number))
			{
				int buffer = 0;
				const char* pFmt = "%d";
				if (sscanf_s(destorySelf.c_str(), pFmt, &buffer) == 1)
				{
					Delay = buffer;
				}
			}
			else
			{
				const char v = *uppercase(destorySelf).substr(0, 1).c_str();
				switch (v)
				{
				case 'Y':
				case 'T':
					Delay = 0;
					break;
				}
			}
		}
		else
		{
			Delay = reader->Get(title + "Delay", Delay);
		}

		Enable = Delay >= 0;

		Peaceful = reader->Get(title + "Peaceful", Peaceful);
		InTransport = reader->Get(title + "InTransport", InTransport);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Delay)
			.Process(this->Peaceful)
			.Process(this->InTransport)
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
