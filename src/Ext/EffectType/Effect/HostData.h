#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/StateType/State/GiftBoxData.h>
#include <Ext/Helper/CastEx.h>

class HostData : public GiftBoxData
{
public:
	EFFECT_DATA(Host);

	HostData() : GiftBoxData()
	{
		RandomRange = 1;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Host.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		GiftBoxData::Read(reader, title);

		Remove = false;
		Destroy = false;
		EmptyCell = true;

		ForceTransform = false;
		IsTransform = false;
		InheritPassenger = false;
		InheritROF = false;
		InheritAmmo = false;
		InheritAE = false;
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
		GiftBoxData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		GiftBoxData::Save(stream);
		return const_cast<HostData*>(this)->Serialize(stream);
	}
#pragma endregion
};
