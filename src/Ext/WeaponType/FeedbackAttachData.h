#pragma once
#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>

class FeedbackAttachEntity : public EffectData
{
public:

	std::vector<std::string> AttachEffects{};
	std::vector<double> AttachChances{};
	bool AttachToTransporter = false;

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		AttachEffects = reader->GetList(title + "AttachEffects", AttachEffects);
		AttachChances = reader->GetChanceList(title + "AttachChances", AttachChances);
		AttachToTransporter = reader->Get(title + "AttachToTransporter", AttachToTransporter);

		Enable = !AttachEffects.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->AttachEffects)
			.Process(this->AttachToTransporter)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		return const_cast<FeedbackAttachEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class FeedbackAttachData : public INIConfig
{
public:
	std::map<int, FeedbackAttachEntity> Datas{};

	virtual void Read(INIBufferReader* reader) override
	{
		// 读取无序号的
		std::string title = "Feedback.";
		FeedbackAttachEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Datas[0] = data;
		}
		// 读取带序号的
		for (int i = 0; i < 128; i++)
		{
			title = "Feedback" + std::to_string(i) + ".";
			FeedbackAttachEntity data2;
			data2.Read(reader, title);
			if (data2.Enable)
			{
				Datas[i] = data2;
			}
		}

		Enable = !Datas.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Datas)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		return const_cast<FeedbackAttachData*>(this)->Serialize(stream);
	}
#pragma endregion
};

