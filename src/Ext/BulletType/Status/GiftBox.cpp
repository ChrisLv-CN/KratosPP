#include "../BulletStatus.h"

#include <FootClass.h>
#include <JumpjetLocomotionClass.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/ObjectType/AttachEffect.h>
#include <Ext/TechnoType/TechnoStatus.h>

void BulletStatus::OnUpdate_GiftBox()
{
	TechnoClass* pTechno = pBullet->Owner;
	if (pTechno)
	{
		// 记录盒子的状态
		GiftBox->IsSelected = pTechno->IsSelected;
		GiftBox->Group = pTechno->Group;
	}
	if (GiftBox->IsAlive())
	{
		// 记录朝向
		GiftBox->BodyDir = Facing(pBullet);
		GiftBox->TurretDir = GiftBox->BodyDir;
		// 准备开盒
		if (GiftBox->CanOpen() && IsOnMark_GiftBox() && !GiftBox->Data.OpenWhenDestroyed && !GiftBox->Data.OpenWhenHealthPercent)
		{
			// 开盒
			GiftBox->IsOpen = true;
			GiftBoxData data = GiftBox->Data;
			// 释放礼物
			std::vector<std::string> gifts = GetGiftList(GiftBox->GetGiftData());
			if (!gifts.empty())
			{
				ReleaseGift(gifts, data);
			}
		}

		// 重置或者销毁盒子
		if (GiftBox->IsOpen)
		{
			if (GiftBox->Data.Remove)
			{
				GiftBox->End();
				bool harmless = !GiftBox->Data.Destroy;
				life.Detonate(harmless);
			}
			else
			{
				GiftBox->ResetGiftBox();
			}
		}
	}

}

bool BulletStatus::OnDetonate_GiftBox(CoordStruct* pCoords)
{
	if (GiftBox->IsAlive() && GiftBox->Data.OpenWhenDestroyed && IsOnMark_GiftBox())
	{
		// 开盒
		GiftBox->IsOpen = true;
		GiftBoxData data = GiftBox->Data;
		// 释放礼物
		std::vector<std::string> gifts = GetGiftList(GiftBox->GetGiftData());
		if (!gifts.empty())
		{
			ReleaseGift(gifts, data);
		}
	}
	return false;
}

bool BulletStatus::IsOnMark_GiftBox()
{
	std::vector<std::string> marks = GiftBox->Data.OnlyOpenWhenMarks;
	if (!marks.empty())
	{
		if (AttachEffect* aem = AEManager())
		{
			std::vector<std::string> aeMarks{};
			aem->GetMarks(aeMarks);
			if (!aeMarks.empty())
			{
				return CheckOnMarks(aeMarks, marks);
			}
		}
		return false;
	}
	return true;
}

void BulletStatus::ReleaseGift(std::vector<std::string> gifts, GiftBoxData data)
{
	HouseClass* pHouse = pSourceHouse;
	// AE ReceiverOwn
	if (!GiftBox->ReceiverOwn && GiftBox->pAEHouse != nullptr)
	{
		pHouse = GiftBox->pAEHouse;
	}
	AbstractClass* pTarget = pBullet->Target;
	AbstractClass* pFocus = nullptr; // 步兵的移动目的地
	// 取目标所在位置
	CoordStruct targetLocation = pBullet->TargetCoords;
	if (pTarget)
	{
		targetLocation = pTarget->GetCoords();
	}
	// 取目标位置所在的格子作为移动目的地
	CellClass* pTargetLocationCell = MapClass::Instance->TryGetCellAt(targetLocation);
	if (pTargetLocationCell)
	{
		pFocus = pTargetLocationCell; // 步兵的移动目的地
	}

	// 读取记录信息
	BoxStateCache boxState = GiftBox->GetGiftBoxStateCache();
	boxState.Location = pBullet->GetCoords();
	boxState.pTarget = pTarget;

	boxState.pDest = pFocus;
	boxState.pFocus = pFocus;

	boxState.pOwner = pBullet;
	boxState.pHouse = pHouse;

	// 开刷
	ReleaseGifts(gifts, GiftBox->GetGiftData(), boxState,
		[&](TechnoClass* pGift, TechnoStatus*& pGiftStatus, AttachEffect*& pGiftAEM) {});
}
