#pragma once

#include <string>
#include <vector>

#include <Ext/Helper/StringEx.h>

#include <Ext/AnimType/RelationData.h>
#include "EffectData.h"
#include "OffsetData.h"


class AnimationEntity
{
public:
	bool Enable = false;

	std::string Type{ "" }; // 动画类型
	OffsetData Offset{}; // 动画相对位置

	bool RemoveInCloak = true; // 隐形时移除
	bool TranslucentInCloak = false; // 隐形时调整透明度为50
	Relation Visibility = Relation::All; // 谁能看见持续动画

	void Read(INIBufferReader* reader, std::string title, std::string anim)
	{
		Type = reader->Get(title + "Type", anim);
		if (IsNotNone(Type))
		{
			Enable = true;
			Offset.Read(reader, title);

			RemoveInCloak = reader->Get(title + "RemoveInCloak", RemoveInCloak);
			TranslucentInCloak = reader->Get(title + "TranslucentInCloak", TranslucentInCloak);
			Visibility = reader->Get(title + "Visibility", Visibility);
		}
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Type)
			.Process(this->Offset)
			.Process(this->RemoveInCloak)
			.Process(this->TranslucentInCloak)
			.Process(this->Visibility)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<AnimationEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class AnimationData : public EffectData
{
public:
	std::string ScriptName = "Animation";

	AnimationEntity IdleAnim{}; // 持续动画
	AnimationEntity ActiveAnim{}; // 激活时播放的动画
	AnimationEntity HitAnim{}; // 被击中时播放的动画
	AnimationEntity DoneAnim{}; // 结束时播放的动画

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		std::string idle = reader->Get<std::string>("Animation", "");
		AnimationEntity idleAnim{};
		idleAnim.Read(reader, "Anim.", idle);
		idleAnim.Read(reader, "Anim.Idle.", idle);
		if (idleAnim.Enable)
		{
			IdleAnim = idleAnim;
		}

		std::string active = reader->Get<std::string>("ActiveAnim", "");
		AnimationEntity activeAnim{};
		activeAnim.Read(reader, "Anim.Active.", active);
		if (activeAnim.Enable)
		{
			ActiveAnim = activeAnim;
		}

		std::string hit = reader->Get<std::string>("HitAnim", "");
		AnimationEntity hitAnim{};
		hitAnim.Read(reader, "Anim.Hit.", hit);
		if (hitAnim.Enable)
		{
			HitAnim = hitAnim;
		}

		std::string done = reader->Get<std::string>("DoneAnim", "");
		AnimationEntity doneAnim{};
		doneAnim.Read(reader, "Anim.Done.", done);
		if (doneAnim.Enable)
		{
			DoneAnim = doneAnim;
		}

		Enable = IdleAnim.Enable || ActiveAnim.Enable || HitAnim.Enable || DoneAnim.Enable;

	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->IdleAnim)
			.Process(this->ActiveAnim)
			.Process(this->HitAnim)
			.Process(this->DoneAnim)
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
		return const_cast<AnimationData*>(this)->Serialize(stream);
	}
#pragma endregion

};


