#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <set>

#include <Ext/ObjectType/FilterData.h>
// TODO Add new Effects
#include "Effect/AnimationData.h"
#include "Effect/BroadcastData.h"
#include "Effect/CrateBuffData.h"
#include "Effect/FireSuperData.h"
#include "Effect/ImmuneData.h"
#include "Effect/InfoData.h"
#include "Effect/MarkData.h"
#include "Effect/RevengeData.h"
#include "Effect/StackData.h"
#include "Effect/StandData.h"
#include "Effect/VampireData.h"
// State Effects
#include <Ext/StateType/State/AntiBulletData.h>
#include <Ext/StateType/State/DestroyAnimData.h>
#include <Ext/StateType/State/DestroySelfData.h>
#include <Ext/StateType/State/GiftBoxData.h>
#include <Ext/StateType/State/PaintballData.h>
#include <Ext/StateType/State/TransformData.h>


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

// 定义变量
#define EFFECT_VAR_DEFINE(EFFECT_DATA_NAME) \
	EFFECT_DATA_NAME ## Data EFFECT_DATA_NAME{}; \

// 从ini中读取内容
#define EFFECT_VAR_READ(EFFECT_DATA_NAME) \
	EFFECT_DATA_NAME.Read(reader); \

// 读取脚本名称
#define EFFECT_VAR_SCRIPT_NAME(EFFECT_DATA_NAME) \
	if (EFFECT_DATA_NAME.Enable) \
	{ \
		names.insert(EFFECT_DATA_NAME.GetEffectScriptName()); \
	} \

// 保存读取
#define EFFECT_VAR_PROCESS(EFFECT_DATA_NAME) \
	.Process(this->EFFECT_DATA_NAME) \

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

	std::vector<std::string> AttachWithOutTypes{}; // 有这些AE存在则不可赋予
	bool AttachOnceInTechnoType = false; // 写在TechnoType上只在创建时赋予一次
	bool Inheritable = true; // 是否可以被礼盒礼物继承

	// TODO Add new Effects
	EFFECT_VAR_DEFINE(Animation);
	EFFECT_VAR_DEFINE(Broadcast);
	EFFECT_VAR_DEFINE(CrateBuff);
	EFFECT_VAR_DEFINE(FireSuper);
	EFFECT_VAR_DEFINE(Immune);
	EFFECT_VAR_DEFINE(Info);
	EFFECT_VAR_DEFINE(Mark);
	EFFECT_VAR_DEFINE(Revenge);
	EFFECT_VAR_DEFINE(Stack);
	EFFECT_VAR_DEFINE(Stand);
	EFFECT_VAR_DEFINE(Vampire);
	// State Effects
	EFFECT_VAR_DEFINE(AntiBullet);
	EFFECT_VAR_DEFINE(DestroyAnim);
	EFFECT_VAR_DEFINE(DestroySelf);
	EFFECT_VAR_DEFINE(GiftBox);
	EFFECT_VAR_DEFINE(Paintball);
	EFFECT_VAR_DEFINE(Transform);

	void ReadEffects(INIBufferReader* reader)
	{
		EFFECT_VAR_READ(Animation);
		EFFECT_VAR_READ(Broadcast);
		EFFECT_VAR_READ(CrateBuff);
		EFFECT_VAR_READ(FireSuper);
		EFFECT_VAR_READ(Immune);
		EFFECT_VAR_READ(Info);
		EFFECT_VAR_READ(Mark);
		EFFECT_VAR_READ(Revenge);
		EFFECT_VAR_READ(Stack);
		EFFECT_VAR_READ(Stand);
		EFFECT_VAR_READ(Vampire);
		// State Effects
		EFFECT_VAR_READ(AntiBullet);
		EFFECT_VAR_READ(DestroyAnim);
		EFFECT_VAR_READ(DestroySelf);
		EFFECT_VAR_READ(GiftBox);
		EFFECT_VAR_READ(Paintball);
		EFFECT_VAR_READ(Transform);
	}

	std::set<std::string> GetScriptNames()
	{
		std::set<std::string> names{};
		EFFECT_VAR_SCRIPT_NAME(Animation);
		EFFECT_VAR_SCRIPT_NAME(Broadcast);
		EFFECT_VAR_SCRIPT_NAME(CrateBuff);
		EFFECT_VAR_SCRIPT_NAME(FireSuper);
		EFFECT_VAR_SCRIPT_NAME(Immune);
		EFFECT_VAR_SCRIPT_NAME(Info);
		EFFECT_VAR_SCRIPT_NAME(Mark);
		EFFECT_VAR_SCRIPT_NAME(Revenge);
		EFFECT_VAR_SCRIPT_NAME(Stack);
		EFFECT_VAR_SCRIPT_NAME(Stand);
		EFFECT_VAR_SCRIPT_NAME(Vampire);
		// State Effects
		EFFECT_VAR_SCRIPT_NAME(AntiBullet);
		EFFECT_VAR_SCRIPT_NAME(DestroyAnim);
		EFFECT_VAR_SCRIPT_NAME(DestroySelf);
		EFFECT_VAR_SCRIPT_NAME(GiftBox);
		EFFECT_VAR_SCRIPT_NAME(Paintball);
		EFFECT_VAR_SCRIPT_NAME(Transform);

		return names;
	}

	template <typename T>
	void ProcessEffects(T& stream)
	{
		stream
			EFFECT_VAR_PROCESS(Animation)
			EFFECT_VAR_PROCESS(Broadcast)
			EFFECT_VAR_PROCESS(CrateBuff)
			EFFECT_VAR_PROCESS(FireSuper)
			EFFECT_VAR_PROCESS(Immune)
			EFFECT_VAR_PROCESS(Info)
			EFFECT_VAR_PROCESS(Mark)
			EFFECT_VAR_PROCESS(Revenge)
			EFFECT_VAR_PROCESS(Stack)
			EFFECT_VAR_PROCESS(Stand)
			EFFECT_VAR_PROCESS(Vampire)
			// State Effects
			EFFECT_VAR_PROCESS(AntiBullet)
			EFFECT_VAR_PROCESS(DestroyAnim)
			EFFECT_VAR_PROCESS(DestroySelf)
			EFFECT_VAR_PROCESS(GiftBox)
			EFFECT_VAR_PROCESS(Paintball)
			EFFECT_VAR_PROCESS(Transform)
			;
	}

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


		int druation = reader->Get("Duration", Duration);
		if (druation != 0)
		{
			Duration = druation;

			FilterData::Read(reader, title);

			ReadEffects(reader);

			Enable = !GetScriptNames().empty();

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

			AttachWithOutTypes = reader->GetList("AttachWithOutTypes", AttachWithOutTypes);

			AttachOnceInTechnoType = reader->Get("AttachOnceInTechnoType", AttachOnceInTechnoType);
			Inheritable = reader->Get("Inheritable", Inheritable);
		}
	}

	bool HasContradiction(std::vector<std::string> AENames)
	{
		bool has = !AttachWithOutTypes.empty() && !AENames.empty();
		if (has)
		{
			std::set<std::string> n(AENames.begin(), AENames.end());
			std::set<std::string> t(AttachWithOutTypes.begin(), AttachWithOutTypes.end());
			std::set<std::string> v;
			// 取交集
			std::set_intersection(n.begin(), n.end(), t.begin(), t.end(), std::inserter(v, v.begin()));
			has = !v.empty();
		}
		return has;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		stream
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
			.Process(this->Inheritable);
		ProcessEffects(stream);
		return stream.Success();
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


