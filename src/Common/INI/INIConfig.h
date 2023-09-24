#pragma once

#include <typeinfo>

#include "INIReader.h"

class INIBufferReader;

class INIConfig
{
public:
	bool Enable = false;

	virtual void Read(INIBufferReader* ini) = 0;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<INIConfig*>(this)->Serialize(stream);
	}
#pragma endregion
};
