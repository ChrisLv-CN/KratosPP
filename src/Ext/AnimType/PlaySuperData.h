#pragma once

#include <Common/INI/INIConfig.h>
#include <Ext/State/FireSuperData.h>

enum class PlaySuperWeaponMode
{
	CUSTOM = 0, DONE = 1, LOOP = 2
};

template <>
inline bool Parser<PlaySuperWeaponMode>::TryParse(const char* pValue, PlaySuperWeaponMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'D':
		if (outValue)
		{
			*outValue = PlaySuperWeaponMode::DONE;
		}
		return true;
	case 'L':
		if (outValue)
		{
			*outValue = PlaySuperWeaponMode::LOOP;
		}
		return true;
	case 'C':
		if (outValue)
		{
			*outValue = PlaySuperWeaponMode::CUSTOM;
		}
		return true;
	}
	return false;
}

class PlaySuperData : public FireSuperData
{
public:
	PlaySuperWeaponMode LaunchMode = PlaySuperWeaponMode::DONE;

	virtual void Read(INIBufferReader* reader) override
	{
		FireSuperData::Read(reader);

		LaunchMode = reader->Get(TITLE + "LaunchMode", LaunchMode);
		Enable = Data.Enable;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->LaunchMode)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		FireSuperData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		FireSuperData::Save(stream);
		return const_cast<PlaySuperData*>(this)->Serialize(stream);
	}
#pragma endregion
};
