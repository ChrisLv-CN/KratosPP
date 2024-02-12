#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <set>

#include <Ext/ObjectType/FilterData.h>
#include "Effect/AnimationData.h"
#include "Effect/MarkData.h"

enum class CumulativeMode
{
	NO = 0, YES = 1, ATTACKER = 2, HOUSE = 3,
};

template <>
inline bool Parser<CumulativeMode>::TryParse(const char* pValue, CumulativeMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case '1':
	case 'T':
	case 'Y':
		if (outValue)
		{
			*outValue = CumulativeMode::YES;
		}
		return true;
	case '0':
	case 'F':
	case 'N':
		if (outValue)
		{
			*outValue = CumulativeMode::NO;
		}
		return true;
	case 'A':
		if (outValue)
		{
			*outValue = CumulativeMode::ATTACKER;
		}
		return true;
	case 'H':
		if (outValue)
		{
			*outValue = CumulativeMode::HOUSE;
		}
		return true;
	}
	return false;
}

enum class AttachOwnerType
{
	TECHONO = 0, BULLET = 1,
};

class AttachEffectData : public FilterData
{
public:
	std::string Name{ "" };

	int Duration = -1; // 持续时间
	bool HoldDuration = true; // 无限时间

	int Delay = 0; // 不可获得同名的延迟
	Point2D RandomDelay{ 0, 0 }; // 随机最小值

	int InitialDelay = -1; // 生效前的初始延迟
	Point2D InitialRandomDelay{ 0, 0 }; // 随机初始延迟

	bool DiscardOnEntry = false; // 离开地图则失效
	bool DiscardOnTransform = true; // 发生类型改变时失效
	bool PenetratesIronCurtain = false; // 弹头附加，影响铁幕
	bool FromTransporter = true; // 弹头附加，乘客附加时，视为载具
	bool ReceiverOwn = false; // 弹头附加，属于被赋予对象

	CumulativeMode Cumulative = CumulativeMode::NO; // 可叠加
	int MaxStack = -1; // 叠加上限
	bool ResetDurationOnReapply = false; // 不可叠加时，重复获得时是否重置计时器
	int Group = -1; // 分组，同一个分组的效果互相影响，削减或增加持续时间
	bool OverrideSameGroup = false; // 是否覆盖同一个分组
	std::string Next{ "" }; // 结束后播放下一个AE

	std::set<std::string> AttachWithOutTypes{}; // 有这些AE存在则不可赋予
	bool AttachOnceInTechnoType = false; // 写在TechnoType上只在创建时赋予一次
	bool Inheritable = true; // 是否可以被礼盒礼物继承

	// TODO Effects
	AnimationData Animation{};
	MarkData Mark{};

	std::vector<std::string> EffectScriptNames{}; // 存放效果组件的名字用于初始化实例

	AttachEffectData() : FilterData()
	{
		this->AffectBullet = false;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		FilterData::Read(reader);
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		Name = reader->Section;


		int druation = reader->Get("Duration", 0);
		if (druation != 0)
		{
			Enable = true;
			Duration = druation;

			FilterData::Read(reader, title);

			// TODO Read Effects
			Animation.Read(reader, title);
			if (Animation.Enable)
			{
				EffectScriptNames.push_back(Animation.ScriptName);
			}

			Mark.Read(reader);
			if (Mark.Enable)
			{
				EffectScriptNames.push_back(Mark.ScriptName);
			}

			// 至少要有一个效果组件
			Enable = !EffectScriptNames.empty();
			if (Enable)
			{
				HoldDuration = Duration <= 0;
				HoldDuration = reader->Get("HoldDuration", HoldDuration);

				Delay = reader->Get("Delay", Delay);
				RandomDelay = reader->Get("RandomDelay", RandomDelay);
				InitialDelay = reader->Get("InitialDelay", InitialDelay);
				InitialRandomDelay = reader->Get("InitialRandomDelay", InitialRandomDelay);

				DiscardOnEntry = reader->Get("DiscardOnEntry", DiscardOnEntry);
				DiscardOnTransform = reader->Get("DiscardOnTransform", DiscardOnTransform);
				PenetratesIronCurtain = reader->Get("PenetratesIronCurtain", PenetratesIronCurtain);
				FromTransporter = reader->Get("FromTransporter", FromTransporter);
				ReceiverOwn = reader->Get("ReceiverOwn", ReceiverOwn);

				Cumulative = reader->Get("Cumulative", Cumulative);
				MaxStack = reader->Get("MaxStack", MaxStack);
				ResetDurationOnReapply = reader->Get("ResetDurationOnReapply", ResetDurationOnReapply);
				Group = reader->Get("Group", Group);
				OverrideSameGroup = reader->Get("OverrideSameGroup", OverrideSameGroup);
				Next = reader->Get("Next", Next);

				std::vector<std::string> attachWithOutTypes{};
				attachWithOutTypes = reader->GetList("AttachWithOutTypes", attachWithOutTypes);
				for (std::string& n : attachWithOutTypes)
				{
					AttachWithOutTypes.insert(n);
				}
				AttachOnceInTechnoType = reader->Get("AttachOnceInTechnoType", AttachOnceInTechnoType);
				Inheritable = reader->Get("Inheritable", Inheritable);
			}
		}
	}

	bool HasContradiction(std::vector<std::string> AENames)
	{
		bool has = !AttachWithOutTypes.empty() && !AENames.empty();
		if (has)
		{
			std::set<std::string> n(AENames.begin(), AENames.end());
			std::set<std::string> v;
			// 取交集
			std::set_intersection(n.begin(), n.end(), AttachWithOutTypes.begin(), AttachWithOutTypes.end(), std::inserter(v, v.begin()));
			has = !v.empty();
		}
		return has;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Name)

			.Process(this->Duration)
			.Process(this->HoldDuration)

			.Process(this->Delay)
			.Process(this->RandomDelay)
			.Process(this->InitialDelay)
			.Process(this->InitialRandomDelay)

			.Process(this->DiscardOnEntry)
			.Process(this->DiscardOnTransform)
			.Process(this->PenetratesIronCurtain)
			.Process(this->FromTransporter)
			.Process(this->ReceiverOwn)

			.Process(this->Cumulative)
			.Process(this->MaxStack)
			.Process(this->ResetDurationOnReapply)
			.Process(this->Group)
			.Process(this->OverrideSameGroup)
			.Process(this->Next)

			.Process(this->AttachWithOutTypes)
			.Process(this->AttachOnceInTechnoType)
			.Process(this->Inheritable)

			.Process(this->Animation)

			.Process(this->EffectScriptNames)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		FilterData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		FilterData::Save(stream);
		return const_cast<AttachEffectData*>(this)->Serialize(stream);
	}
#pragma endregion

	int GetDuration()
	{
		return this->HoldDuration ? -1 : this->Duration;
	}

};


