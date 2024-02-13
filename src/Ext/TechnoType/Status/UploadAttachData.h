#pragma once
#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>

#include <Ext/ObjectType/FilterData.h>

class UploadAttachEntity : public FilterData
{
public:
	std::vector<std::string> AttachEffects{};
	bool SourceIsPassenger = true;

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		FilterData::Read(reader, title);

		AttachEffects = reader->GetList(title + "AttachEffects", AttachEffects);
		SourceIsPassenger = reader->Get(title + "SourceIsPassenger", SourceIsPassenger);

		Enable = !AttachEffects.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->AttachEffects)
			.Process(this->SourceIsPassenger)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		return const_cast<UploadAttachEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class UploadAttachData : public INIConfig
{
public:
	std::map<int, UploadAttachEntity> Datas{};

	virtual void Read(INIBufferReader* reader) override
	{
		// 读取无序号的
		std::string title = "Upload.";
		UploadAttachEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Datas[0] = data;
		}
		// 读取带序号的
		for (int i = 0; i < 128; i++)
		{
			title = "Upload" + std::to_string(i) + ".";
			UploadAttachEntity data2;
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
		return const_cast<UploadAttachData*>(this)->Serialize(stream);
	}
#pragma endregion
};

