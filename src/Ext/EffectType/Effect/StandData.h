#pragma once

#include <string>
#include <vector>

#include <Ext/Helper/StringEx.h>

#include "EffectData.h"
#include "OffsetData.h"

enum class StandTargeting : int
{
	BOTH = 0, LAND = 1, AIR = 2,
};

template <>
inline bool Parser<StandTargeting>::TryParse(const char* pValue, StandTargeting* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'L':
		if (outValue)
		{
			*outValue = StandTargeting::LAND;
		}
		return true;
	case 'A':
		if (outValue)
		{
			*outValue = StandTargeting::AIR;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = StandTargeting::BOTH;
		}
		return true;
	}
}

class StandData : public EffectData
{
public:
	EFFECT_DATA(Stand);

	std::string Type{ "" }; // 替身类型
	OffsetData Offset{}; // 替身相对位置

	bool LockDirection = false; // 强制朝向，不论替身在做什么
	bool FreeDirection = false; // 完全不控制朝向

	Layer DrawLayer = Layer::None; // 渲染的层
	int ZOffset = 2; // ZAdjust偏移值
	bool SameTilter = true; // 同步倾斜
	bool SameMoving = false; // 同步移动动画
	bool StickOnFloor = true; // 同步移动动画时贴在地上

	bool SameHouse = true; // 与使者同所属
	bool SameTarget = true; // 与使者同个目标
	bool SameLoseTarget = false; // 使者失去目标时替身也失去
	bool SameAmmo = false; // 与使者弹药数相同
	bool UseMasterAmmo = false; // 消耗使者的弹药
	bool SamePassengers = false; // 相同的乘客管理器

	StandTargeting Targeting = StandTargeting::BOTH; // 在什么位置可攻击
	bool ForceAttackMaster = false; // 强制选择使者为目标
	bool AttackSource = false; // 生成时选定来源为目标
	bool MobileFire = true; // 移动攻击

	bool Immune = true; // 无敌
	double DamageFromMaster = 0.0; // 分摊JOJO的伤害
	double DamageToMaster = 0.0; // 分摊伤害给JOJO
	bool AllowShareRepair = false; // 是否允许负伤害分摊

	bool Explodes = false; // 死亡会爆炸
	bool ExplodesWithMaster = false; // 使者死亡时强制替身爆炸
	bool ExplodesWithRocket = true; // 跟随子机导弹爆炸
	bool RemoveAtSinking = false; // 沉船时移除

	bool PromoteFromMaster = false; // 与使者同等级
	bool PromoteFromSpawnOwner = false; // 与火箭发射者同等级
	double ExperienceToMaster = 0.0; // 经验给使者
	bool ExperienceToSpawnOwner = false; // 经验分给火箭发射者

	bool SelectToMaster = false; // 选中替身时，改为选中使者

	bool VirtualUnit = true; // 虚单位

	bool IsVirtualTurret = true; // 虚拟炮塔

	bool IsTrain = false; // 火车类型
	bool CabinHead = false; // 插入车厢前端
	int CabinGroup = -1; // 车厢分组

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Stand.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Type = reader->Get(title + "Type", Type);

		Enable = IsNotNone(Type);

		if (Enable)
		{
			Offset.Read(reader, title);

			LockDirection = reader->Get(title + "LockDirection", LockDirection);
			FreeDirection = reader->Get(title + "FreeDirection", FreeDirection);

			DrawLayer = reader->Get(title + "DrawLayer", DrawLayer);
			ZOffset = reader->Get(title + "ZOffset", ZOffset);
			SameTilter = reader->Get(title + "SameTilter", SameTilter);
			SameMoving = reader->Get(title + "SameMoving", SameMoving);
			StickOnFloor = reader->Get(title + "StickOnFloor", StickOnFloor);

			SameHouse = reader->Get(title + "SameHouse", SameHouse);
			SameTarget = reader->Get(title + "SameTarget", SameTarget);
			SameLoseTarget = reader->Get(title + "SameLoseTarget", SameLoseTarget);
			SameAmmo = reader->Get(title + "SameAmmo", SameAmmo);
			if (SameAmmo)
			{
				UseMasterAmmo = true;
			}
			UseMasterAmmo = reader->Get(title + "UseMasterAmmo", UseMasterAmmo);
			SamePassengers = reader->Get(title + "SamePassengers", SamePassengers);

			Targeting = reader->Get(title + "Targeting", Targeting);
			ForceAttackMaster = reader->Get(title + "ForceAttackMaster", ForceAttackMaster);
			AttackSource = reader->Get(title + "AttackSource", AttackSource);
			MobileFire = reader->Get(title + "MobileFire", MobileFire);

			Immune = reader->Get(title + "Immune", Immune);
			DamageFromMaster = reader->GetPercent(title + "DamageFromMaster", DamageFromMaster);
			DamageToMaster = reader->GetPercent(title + "DamageToMaster", DamageToMaster);
			AllowShareRepair = reader->Get(title + "AllowShareRepair", AllowShareRepair);

			Explodes = reader->Get(title + "Explodes", Explodes);
			ExplodesWithMaster = reader->Get(title + "ExplodesWithMaster", ExplodesWithMaster);
			ExplodesWithRocket = reader->Get(title + "ExplodesWithRocket", ExplodesWithRocket);
			RemoveAtSinking = reader->Get(title + "RemoveAtSinking", RemoveAtSinking);

			PromoteFromMaster = reader->Get(title + "PromoteFromMaster", PromoteFromMaster);
			PromoteFromSpawnOwner = reader->Get(title + "PromoteFromSpawnOwner", PromoteFromSpawnOwner);
			ExperienceToMaster = reader->GetPercent(title + "ExperienceToMaster", ExperienceToMaster);
			ExperienceToSpawnOwner = reader->Get(title + "ExperienceToSpawnOwner", ExperienceToSpawnOwner);

			SelectToMaster = reader->Get(title + "SelectToMaster", SelectToMaster);

			VirtualUnit = reader->Get(title + "VirtualUnit", VirtualUnit);

			IsVirtualTurret = reader->Get(title + "IsVirtualTurret", IsVirtualTurret);

			IsTrain = reader->Get(title + "IsTrain", IsTrain);
			CabinHead = reader->Get(title + "CabinHead", CabinHead);
			CabinGroup = reader->Get(title + "CabinGroup", CabinGroup);
		}
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream

			.Process(this->Type) // 替身类型
			.Process(this->Offset) // 替身相对位置

			.Process(this->LockDirection) // 强制朝向，不论替身在做什么
			.Process(this->FreeDirection) // 完全不控制朝向

			.Process(this->DrawLayer) // 渲染的层
			.Process(this->ZOffset) // ZAdjust偏移值
			.Process(this->SameTilter) // 同步倾斜
			.Process(this->SameMoving) // 同步移动动画
			.Process(this->StickOnFloor) // 同步移动动画时贴在地上

			.Process(this->SameHouse) // 与使者同所属
			.Process(this->SameTarget) // 与使者同个目标
			.Process(this->SameLoseTarget) // 使者失去目标时替身也失去
			.Process(this->SameAmmo) // 与使者弹药数相同
			.Process(this->UseMasterAmmo) // 消耗使者的弹药
			.Process(this->SamePassengers) // 相同的乘客管理器

			.Process(this->Targeting) // 在什么位置可攻击
			.Process(this->ForceAttackMaster) // 强制选择使者为目标
			.Process(this->AttackSource) // 生成时选定来源为目标
			.Process(this->MobileFire) // 移动攻击

			.Process(this->Immune) // 无敌
			.Process(this->DamageFromMaster) // 分摊JOJO的伤害
			.Process(this->DamageToMaster) // 分摊伤害给JOJO
			.Process(this->AllowShareRepair) // 是否允许负伤害分摊

			.Process(this->Explodes) // 死亡会爆炸
			.Process(this->ExplodesWithMaster) // 使者死亡时强制替身爆炸
			.Process(this->ExplodesWithRocket) // 跟随子机导弹爆炸
			.Process(this->RemoveAtSinking) // 沉船时移除

			.Process(this->PromoteFromMaster) // 与使者同等级
			.Process(this->PromoteFromSpawnOwner) // 与火箭发射者同等级
			.Process(this->ExperienceToMaster) // 经验给使者
			.Process(this->ExperienceToSpawnOwner) // 经验分给火箭发射者

			.Process(this->SelectToMaster) // 选中替身时，改为选中使者

			.Process(this->VirtualUnit) // 虚单位

			.Process(this->IsVirtualTurret) // 虚拟炮塔

			.Process(this->IsTrain) // 火车类型
			.Process(this->CabinHead) // 插入车厢前端
			.Process(this->CabinGroup) // 车厢分组


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
		return const_cast<StandData*>(this)->Serialize(stream);
	}
#pragma endregion

};


