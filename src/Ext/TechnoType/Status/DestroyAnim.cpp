#include "../TechnoStatus.h"

#include <Ext/Common/CommonStatus.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Gift.h>

void TechnoStatus::OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse)
{
	// 记录下击杀方
	DestroyAnim->pKillerHouse = pAttackingHouse ? pAttackingHouse : HouseClass::FindSpecial();
	// 读取弹头的击杀设置
	DestroyAnimData* data = INI::GetConfig<DestroyAnimData>(INI::Rules, pWH->ID)->Data;
	if (data->Enable && data->CanAffectType(pTechno))
	{
		DestroyAnim->Replace(data);
	}
};


bool TechnoStatus::PlayDestroyAnims()
{
	if (DestroyAnim->IsAlive())
	{
		DestroyAnimData data = DestroyAnim->Data;
		if (data.Enable && data.CanAffectType(pTechno))
		{
			HouseClass* pKillerHouse = DestroyAnim->pKillerHouse;
			if (!pKillerHouse)
			{
				pKillerHouse = pTechno->Owner;
			}
			// 刷残骸
			if (IsNotNone(data.WreckType) && !pTechno->IsInAir())
			{
				CoordStruct location = pTechno->GetCoords();
				HouseClass* pHouse = pTechno->Owner;
				switch (data.Owner)
				{
				case WreckOwner::KILLER:
					pHouse = pKillerHouse;
					break;
				case WreckOwner::NEUTRAL:
					pHouse = HouseClass::FindSpecial();
					break;
				}
				if (!pHouse)
				{
					pHouse = HouseClass::FindSpecial();
				}
				if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
				{
					CoordStruct putLocation = pCell->GetCoordsWithBridge();
					// 生成残骸
					TechnoClass* pWreak = CreateAndPutTechno(data.WreckType, pHouse, putLocation, pCell);
					if (pWreak)
					{
						// 设置朝向和任务
						pWreak->PrimaryFacing.SetCurrent(pTechno->PrimaryFacing.Current());
						pWreak->SecondaryFacing.SetCurrent(pTechno->SecondaryFacing.Current());
						// 调整任务
						pWreak->QueueMission(data.WreckMission, true);
						return true;
					}
				}
			}
			else
			{
				// 绘制动画
				if (!data.Anims.empty() && (data.PlayInAir || !pTechno->IsInAir()))
				{
					int facing = data.Anims.size();
					int index = 0;
					if (!data.Random && facing % 8 == 0)
					{
						// 0的方向是游戏中的北方，是↗，素材0帧是朝向0点，是↑
						index = Dir2FrameIndex(pTechno->PrimaryFacing.Current(), facing);
					}
					else
					{
						index = Random::RandomRanged(0, facing - 1);
					}
					std::string animId = data.Anims[index];
					AnimTypeClass* pAnimType = AnimTypeClass::Find(animId.c_str());
					if (pAnimType)
					{
						CoordStruct location = pTechno->GetCoords();
						AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
						HouseClass* pHouse = pTechno->Owner;
						switch (data.Owner)
						{
						case WreckOwner::KILLER:
							pHouse = pKillerHouse;
							break;
						case WreckOwner::NEUTRAL:
							pHouse = HouseClass::FindSpecial();
							break;
						}
						if (!pHouse)
						{
							pHouse = HouseClass::FindSpecial();
						}
						pAnim->Owner = pHouse;
						return true;
					}
				}
			}
		}
	}
	return false;
}
