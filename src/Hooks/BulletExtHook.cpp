﻿#include <exception>
#include <Windows.h>

#include <GeneralDefinitions.h>
#include <SpecificStructures.h>
#include <AnimClass.h>
#include <AnimTypeClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/AnimExt.h>
#include <Extension/BulletExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/Common/CommonStatus.h>
#include <Ext/Common/ExpandAnimsManager.h>


// ----------------
// Extension
// ----------------


DEFINE_HOOK(0x4664BA, BulletClass_CTOR, 0x5)
{
	// skip this Allocate just left BulletClass_Load_Suffix => LoadKey to Allocate
	// when is loading a save game.
	if (!Common::IsLoadGame)
	{
		GET(BulletClass*, pItem, ESI);

		BulletExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x4665E9, BulletClass_DTOR, 0xA)
{
	GET(BulletClass*, pItem, ECX);
	BulletExt::ExtData* ext = BulletExt::ExtMap.Find(pItem);
	if (ext)
	{
		ext->SetExtStatus(nullptr);
		ext->_GameObject->Foreach([](Component* c)
			{if (auto cc = dynamic_cast<IBulletScript*>(c)) { cc->OnUnInit(); } });
	}
	BulletExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x46AFB0, BulletClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x46AE70, BulletClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(BulletClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	BulletExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK_AGAIN(0x46AF97, BulletClass_Load_Suffix, 0x7)
DEFINE_HOOK(0x46AF9E, BulletClass_Load_Suffix, 0x7)
{
	BulletExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x46AFC4, BulletClass_Save_Suffix, 0x5)
{
	BulletExt::ExtMap.SaveStatic();

	return 0;
}

// ----------------
// Component
// ----------------

DEFINE_HOOK(0x466556, BulletClass_Init, 0x6)
{
	GET(BulletClass*, pThis, ECX);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{if (auto cc = dynamic_cast<IBulletScript*>(c)) { cc->OnInit(); } });

	return 0;
}

DEFINE_HOOK(0x468B5D, BulletClass_Put, 0x6)
{
	GET(BulletClass*, pThis, EBX);
	GET_STACK(CoordStruct*, pCoord, -0x20);
	DirType faceDir = DirType::North;

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pCoord, faceDir](Component* c)
		{ if (auto cc = dynamic_cast<IBulletScript*>(c)) { cc->OnPut(pCoord, faceDir); } });

	return 0;
}

DEFINE_HOOK(0x4666F7, BulletClass_Update, 0x6)
{
	GET(BulletClass*, pThis, EBP);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ c->OnUpdate(); });

	return 0;
}

DEFINE_HOOK_AGAIN(0x467FEE, BulletClass_UpdateEnd, 0x6)
DEFINE_HOOK(0x466781, BulletClass_UpdateEnd, 0x6)
{
	GET(BulletClass*, pThis, EBP);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ c->OnUpdateEnd(); });

	return 0;
}

DEFINE_HOOK(0x4690C1, BulletClass_Detonate, 0x8)
{
	GET(BulletClass*, pThis, ECX);
	GET_BASE(CoordStruct*, pPos, 0x8);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	bool skip = false;
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<IBulletScript*>(c)) { cc->OnDetonate(pPos, skip); } });

	if (skip)
	{
		return 0x46A2FB;
	}

	return 0;
}

// ----------------
// Feature
// ----------------

#pragma region Remap

// when shooter dead, project's house will be 0
DEFINE_HOOK(0x469A75, BulletClass_Detonate_GetHouse, 0x7)
{
	GET(BulletClass*, pBullet, ESI);
	GET(HouseClass*, pHouse, ECX);

	if (!pHouse)
	{
		// GetStatus save the House
		if (HouseClass* pSourceHouse = GetSourceHouse(pBullet))
		{
			R->ECX(pSourceHouse);
		}
	}
	return 0;
}

// Take over to create Warhead Anim
DEFINE_HOOK(0x469C4E, BulletClass_Detonate_WHAnim_Remap, 0x5)
{
	GET(BulletClass*, pBullet, ESI);
	GET(AnimTypeClass*, pAnimType, EBX);
	GET_STACK(CoordStruct, pos, 0x64);
	if (pAnimType)
	{
		AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, pos, 0, 1, 0x2600, -15, false);
		SetAnimOwner(pAnim, pBullet);
		SetAnimCreater(pAnim, pBullet);
		return 0x469D06;
	}

	return 0;
}

// Take over to create Warhead VxlAnim
DEFINE_HOOK(0x469D5C, BulletClass_Detonate_WHVxlDebris_Remap, 0x6)
{
	if (AudioVisual::Data()->AllowMakeVoxelDebrisByKratos)
	{
		GET(BulletClass*, pBullet, ESI);
		GET(WarheadTypeClass*, pWH, EAX);
		GET(int, times, EBX);
		HouseClass* pHouse = GetSourceHouse(pBullet);
		TechnoClass* pCreate = pBullet->Owner;
		CoordStruct location = pBullet->GetCoords();
		ExpandAnimsManager::PlayExpandDebirs(pWH->DebrisTypes, pWH->DebrisMaximums, times, location, pHouse, pCreate);
		R->EBX(0);
		return 0x469E18;
	}
	return 0;
}

DEFINE_HOOK(0x469EB4, BulletClass_Detonate_WHDebris_Remap, 0x6)
{
	GET(BulletClass*, pBullet, ESI);
	GET(AnimClass*, pAnim, EDI);
	if (pAnim)
	{
		SetAnimOwner(pAnim, pBullet);
		SetAnimCreater(pAnim, pBullet);
	}
	GET(int, i, EBX);
	if (i > 0)
	{
		return 0x469E34;
	}
	return 0;
}

DEFINE_HOOK(0x4683E2, BulletClass_DrawSHP_Colour, 0x5)
{
	GET(BulletClass*, pBullet, ESI);
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		bool changeColor = false;
		bool changeBright = false;
		if (status->Paintball->NeedPaint(changeColor, changeBright) && changeColor)
		{
			R->Stack(0, status->Paintball->Data.Color2);
		}
	}
	return 0;
}

DEFINE_HOOK(0x4683E7, BulletClass_DrawSHP_Bright, 0x9)
{
	GET(BulletClass*, pBullet, ESI);
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		bool changeColor = false;
		bool changeBright = false;
		if (status->Paintball->NeedPaint(changeColor, changeBright) && changeBright)
		{
			R->Stack(0, GetBright(1000, status->Paintball->Data.BrightMultiplier));
		}
	}
	return 0;
}

DEFINE_HOOK(0x46B201, BulletClass_DrawVXL_Colour, 0x7)
{
	R->EDI(BlitterFlags::None);
	R->Stack(0, Add2RGB565(ToColorAdd(Colors::Red)));
	GET_STACK(BulletClass*, pBullet, 0x10 - 0x4);
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		status->DrawVXL_Paintball(R);
	}
	return 0;
}
#pragma endregion

#pragma region Trajectory
// 除 ROT>0 和 Vertical 之外的抛射体会在此根据重力对储存的向量变量进行运算
// 对Arcing抛射体的重力进行削减
DEFINE_HOOK(0x46745C, BulletClass_Update_ChangeVelocity, 0x7)
{
	GET(BulletClass*, pBullet, EBP);
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		if (status->IsArcing() && status->SpeedChanged)
		{
			LEA_STACK(BulletVelocity*, pVelocity, 0x90);
			BulletVelocity velocity = pBullet->Velocity;
			pVelocity->X = velocity.X;
			pVelocity->Y = velocity.Y;
			pVelocity->Z = status->LocationLocked ? 0 : velocity.Z;
		}
	}
	return 0;
}

// 除 ROT>0 和 Vertical 之外的抛射体会在Label_158根据速度向量获取坐标
// Arcing抛射体即使向量非常小也会试图移动1点
DEFINE_HOOK(0x4677C7, BulletClass_Update_ChangeVelocity_Locked, 0x8)
{
	GET(BulletClass*, pBullet, EBP);
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		if (status->IsArcing() && status->SpeedChanged && status->LocationLocked)
		{
			CoordStruct location = pBullet->Location;
			R->ESI(location.X);
			R->EDI(location.Y);
			R->EAX(location.Z);
		}
	}
	return 0;
}

// 导弹类抛射体当高度低于地面高度时强制引爆
// 让直线导弹可以潜地
DEFINE_HOOK(0x466E18, BulletClass_CheckHight_UnderGround, 0x6)
{
	GET(BulletClass*, pBullet, ECX);
	if (pBullet->GetHeight() <= 0)
	{
		if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
		{
			if (!status->SubjectToGround)
			{
				R->Stack<bool>(0x18, false);
				R->Stack<unsigned int>(0x20, 0);
			}
		}
	}
	return 0;
}
#pragma endregion
