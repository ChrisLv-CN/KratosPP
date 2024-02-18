﻿#include "../TechnoStatus.h"

#include <Ext/Helper/Scripts.h>

bool TechnoStatus::AmIStand()
{
	// 抛射体上的替身可能会因为抛射体的攻击者已经死亡而MyMaster为空
	return pMyMaster || MyMasterIsAnim || StandData.Enable;
}

void TechnoStatus::OnPut_Stand(CoordStruct* pCoord, DirType dirType)
{
	if (AmIStand())
	{
		if (StandData.Immune)
		{
			TechnoExt::ImmuneStandArray[pTechno] = StandData;
		}
		else
		{
			TechnoExt::StandArray[pTechno] = StandData;
		}
	}
	else if (VirtualUnit)
	{
		// 单位既是替身又是虚单位，只加入替身清单
		TechnoExt::VirtualUnitArray.push_back(pTechno);
	}
	// 虚单位拔地而起
	if (VirtualUnit)
	{
		pTechno->UpdatePlacement(PlacementType::Remove);
	}
}

void TechnoStatus::OnRemove_Stand()
{
	auto it = TechnoExt::StandArray.find(pTechno);
	if (it != TechnoExt::StandArray.end())
	{
		TechnoExt::StandArray.erase(it);
	}

	auto it2 = TechnoExt::ImmuneStandArray.find(pTechno);
	if (it2 != TechnoExt::ImmuneStandArray.end())
	{
		TechnoExt::ImmuneStandArray.erase(it2);
	}

	auto it3 = std::find(TechnoExt::VirtualUnitArray.begin(), TechnoExt::VirtualUnitArray.end(), pTechno);
	if (it3 != TechnoExt::VirtualUnitArray.end())
	{
		TechnoExt::VirtualUnitArray.erase(it3);
	}
}

void TechnoStatus::OnReceiveDamage_Stand(args_ReceiveDamage* args)
{
	// 无视防御的真实伤害不做任何分摊
	// TODO 伤害分摊
}

void TechnoStatus::OnRegisterDestruction_Stand(TechnoClass* pKiller, int cost, bool& skip)
{
	if (cost != 0 && !MyMasterIsAnim && !IsDead(pKiller))
	{
		// Killer是Stand，而且Master可训练
		TechnoStatus* killerStatue = nullptr;
		if (TryGetStatus<TechnoExt>(pKiller, killerStatue) && killerStatue->AmIStand() && !IsDead(killerStatue->pMyMaster))
		{
			TechnoClass* pMaster = killerStatue->pMyMaster;
			if (killerStatue->MyMasterIsSpawned && killerStatue->StandData.ExperienceToSpawnOwner && !IsDead(pMaster->SpawnOwner))
			{
				pMaster = pMaster->SpawnOwner;
			}
			if (pMaster->GetTechnoType()->Trainable)
			{
				int transExp = 0;
				if (pKiller->GetTechnoType()->Trainable)
				{
					// 替身可以训练，经验部分转给使者
					int exp = cost;
					// 替身已经满级
					if (!pKiller->Veterancy.IsElite())
					{
						transExp = cost;
						exp = 0;
					}
					if (!pMaster->Veterancy.IsElite())
					{
						// 使者还能获得经验，转移部分给使者
						transExp = (int)(cost * killerStatue->StandData.ExperienceToMaster);
						exp -= transExp;
					}
					// 剩余部分自己享用
					if (exp != 0)
					{
						int technoCost = pKiller->GetTechnoType()->GetActualCost(pKiller->Owner);
						pKiller->Veterancy.Add(technoCost, exp);
					}
				}
				else
				{
					// 替身不能训练，经验全部转给使者
					transExp = cost;
				}
				if (transExp != 0)
				{
					int technoCost = pMaster->GetTechnoType()->GetActualCost(pMaster->Owner);
					pMaster->Veterancy.Add(technoCost, transExp);
				}

				skip = true;
			}
		}
	}
}

bool TechnoStatus::OnSelect_VirtualUnit()
{
	if (pMyMaster && StandData.Enable && StandData.SelectToMaster)
	{
		pMyMaster->Select();
	}
	return !VirtualUnit;
}

bool TechnoStatus::OnSelect_Deselect()
{
	return true;
}
