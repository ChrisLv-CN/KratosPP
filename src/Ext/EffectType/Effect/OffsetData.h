#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class OffsetData : public INIConfig
{
public:
	CoordStruct Offset = CoordStruct::Empty; // 偏移FLH

	CoordStruct StackOffset = CoordStruct::Empty; // 堆叠偏移
	int StackGroup = -1; // 分组堆叠
	CoordStruct StackGroupOffset = CoordStruct::Empty; // 分组堆叠偏移

	bool IsOnTurret = false; // 相对炮塔或者身体
	bool IsOnWorld = false; // 相对世界

	int Direction = 0; // 相对朝向，16分圆，[0-15]

	OffsetData() : INIConfig() {}
	OffsetData(int temp) : OffsetData() {}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Offset = reader->Get(title + "Offset", Offset);

		StackOffset = reader->Get(title + "StackOffset", StackOffset);
		StackGroup = reader->Get(title + "StackGroup", StackGroup);
		StackGroupOffset = reader->Get(title + "StackGroupOffset", StackGroupOffset);

		IsOnTurret = reader->Get(title + "IsOnTurret", IsOnTurret);
		IsOnWorld = reader->Get(title + "IsOnWorld", IsOnWorld);

		Direction = reader->GetDir16(title + "Direction", Direction);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Offset)

			.Process(this->StackOffset)
			.Process(this->StackGroup)
			.Process(this->StackGroupOffset)

			.Process(this->IsOnTurret)
			.Process(this->IsOnWorld)

			.Process(this->Direction)
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
		return const_cast<OffsetData*>(this)->Serialize(stream);
	}
#pragma endregion
};

