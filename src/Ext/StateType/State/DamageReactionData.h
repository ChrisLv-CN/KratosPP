#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/Common/PrintTextData.h>
#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>



enum class DamageReactionMode : int
{
	NONE = 0,
	EVASION = 1, // 闪避
	REDUCE = 2, // 减伤
	FORTITUDE = 3, // 刚毅盾
	PREVENT = 4 // 免死
};


template <>
inline bool Parser<DamageReactionMode>::TryParse(const char* pValue, DamageReactionMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'E':
		if (outValue)
		{
			*outValue = DamageReactionMode::EVASION;
		}
		return true;
	case 'R':
		if (outValue)
		{
			*outValue = DamageReactionMode::REDUCE;
		}
		return true;
	case 'F':
		if (outValue)
		{
			*outValue = DamageReactionMode::FORTITUDE;
		}
		return true;
	case 'P':
		if (outValue)
		{
			*outValue = DamageReactionMode::PREVENT;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = DamageReactionMode::NONE;
		}
		return true;
	}
}

enum class DamageTextStyle : int
{
	AUTO = 0, DAMAGE = 1, REPAIR = 2
};


template <>
inline bool Parser<DamageTextStyle>::TryParse(const char* pValue, DamageTextStyle* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'D':
		if (outValue)
		{
			*outValue = DamageTextStyle::DAMAGE;
		}
		return true;
	case 'R':
		if (outValue)
		{
			*outValue = DamageTextStyle::REPAIR;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = DamageTextStyle::AUTO;
		}
		return true;
	}
}

class DamageReactionEntity
{
public:
	bool Enable = false;

	DamageReactionMode Mode = DamageReactionMode::EVASION;
	double Chance = 0;
	int Delay = 0;
	bool ActiveOnce = false; // 触发效果之后结束
	int TriggeredTimes = -1; // 触发次数够就结束
	bool ResetTimes = false;

	std::vector<std::string> TriggeredAttachEffects{}; // 触发后附加AE
	std::vector<double> TriggeredAttachEffectChances{}; // 附加效果的成功率
	bool TriggeredAttachEffectsFromAttacker = false; // 触发后附加的AE来源是攻击者

	std::vector<std::string> OnlyReactionWarheads{}; // 只响应某些弹头

	std::string Anim{ "" };
	CoordStruct AnimFLH = CoordStruct::Empty;
	int AnimDelay = 0;

	double ReducePercent = 1; // 伤害调整比例
	int MaxDamage = 10; // 伤害上限

	bool ActionText = true; // 显示响应DamageText
	DamageTextStyle TextStyle = DamageTextStyle::AUTO;
	LongText DefaultText = LongText::MISS; // 默认显示的内容
	std::string CustomText{ "" };
	std::string CustomSHP{ "" };
	int CustomSHPIndex = 0;


	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Mode = reader->Get(title + "Mode", Mode);
		switch (Mode)
		{
		case DamageReactionMode::EVASION:
			DefaultText = LongText::MISS; // 未命中
			break;
		case DamageReactionMode::REDUCE:
			DefaultText = LongText::HIT; // 击中
			break;
		case DamageReactionMode::FORTITUDE:
			DefaultText = LongText::GLANCING; // 偏斜
			break;
		case DamageReactionMode::PREVENT:
			DefaultText = LongText::BLOCK; // 格挡
			break;
		default:
			return;
		}
		Chance = reader->GetChance(title + "Chance", Chance);
		Delay = reader->Get(title + "Delay", Delay);
		ActiveOnce = reader->Get(title + "ActiveOnce", ActiveOnce);
		TriggeredTimes = reader->Get(title + "TriggeredTimes", TriggeredTimes);
		ResetTimes = reader->Get(title + "ResetTimes", ResetTimes);

		TriggeredAttachEffects = reader->GetList(title + "TriggeredAttachEffects", TriggeredAttachEffects);
		ClearIfGetNone(TriggeredAttachEffects);
		TriggeredAttachEffectChances = reader->GetChanceList(title + "TriggeredAttachEffectChances", TriggeredAttachEffectChances);
		TriggeredAttachEffectsFromAttacker = reader->Get(title + "TriggeredAttachEffectsFromAttacker", TriggeredAttachEffectsFromAttacker);

		OnlyReactionWarheads = reader->GetList(title + "OnlyReactionWarheads", OnlyReactionWarheads);
		ClearIfGetNone(OnlyReactionWarheads);

		Anim = reader->Get(title + "Anim", Anim);
		AnimFLH = reader->Get(title + "AnimFLH", AnimFLH);
		AnimDelay = reader->Get(title + "AnimDelay", AnimDelay);

		ReducePercent = reader->GetPercent(title + "ReducePercent", ReducePercent);
		if (Mode == DamageReactionMode::REDUCE)
		{
			double mult = abs(ReducePercent);
			if (mult > 1.0)
			{
				DefaultText = LongText::CRIT; // 暴击
			}
			else if (mult < 1.0)
			{
				DefaultText = LongText::GLANCING; // 偏斜
			}
		}

		MaxDamage = reader->Get(title + "FortitudeMax", MaxDamage);

		ActionText = reader->Get(title + "ActionText", ActionText);
		TextStyle = reader->Get(title + "ActionTextStyle", TextStyle);

		CustomText = reader->Get(title + "ActionTextCustom", CustomText);
		CustomSHP = reader->Get(title + "ActionTextSHP", CustomSHP);
		CustomSHPIndex = reader->Get(title + "ActionTextSHPIndex", CustomSHPIndex);

		Enable = Chance > 0;
	}

	bool WarheadOnMark(std::string whID)
	{
		if (!OnlyReactionWarheads.empty())
		{
			return std::find(OnlyReactionWarheads.begin(), OnlyReactionWarheads.end(), whID) != OnlyReactionWarheads.end();
		}
		return true;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Mode)
			.Process(this->Chance)
			.Process(this->Delay)
			.Process(this->ActiveOnce)
			.Process(this->TriggeredTimes)
			.Process(this->ResetTimes)

			.Process(this->TriggeredAttachEffects)
			.Process(this->TriggeredAttachEffectChances)
			.Process(this->TriggeredAttachEffectsFromAttacker)

			.Process(this->OnlyReactionWarheads)

			.Process(this->Anim)
			.Process(this->AnimFLH)
			.Process(this->AnimDelay)

			.Process(this->ReducePercent)
			.Process(this->MaxDamage)

			.Process(this->ActionText)
			.Process(this->TextStyle)
			.Process(this->DefaultText)
			.Process(this->CustomText)
			.Process(this->CustomSHP)
			.Process(this->CustomSHPIndex)

			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<DamageReactionEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};


class DamageReactionData : public EffectData
{
public:
	EFFECT_DATA(DamageReaction);

	DamageReactionEntity Data{};
	DamageReactionEntity EliteData{};

	DamageReactionData() : EffectData()
	{
		TriggeredTimes = 1;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "DamageReaction.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		DamageReactionEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Data = data;
			EliteData = Data;
		}

		DamageReactionEntity eliteData;
		eliteData.Read(reader, title + "Elite");
		if (eliteData.Enable)
		{
			EliteData = eliteData;
		}

		Enable = Data.Enable || EliteData.Enable;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)
			.Process(this->EliteData)
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
		return const_cast<DamageReactionData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
