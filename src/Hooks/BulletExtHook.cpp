#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Utilities/Macro.h>
#include <Extension/AnimExt.h>
#include <Extension/BulletExt.h>
#include <Common/Components/Component.h>
#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/BulletStatus.h>

#include <AnimClass.h>
#include <AnimTypeClass.h>
#include <GeneralDefinitions.h>
#include <SpecificStructures.h>

// ----------------
// Extension
// ----------------

extern bool IsLoadGame;

DEFINE_HOOK(0x4664BA, BulletClass_CTOR, 0x5)
{
	// skip this Allocate just left BulletClass_Load_Suffix => LoadKey to Allocate
	// when is loading a save game.
	if (!IsLoadGame)
	{
		GET(BulletClass *, pItem, ESI);

		BulletExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x4665E9, BulletClass_DTOR, 0x5)
{
	GET(BulletClass *, pItem, ECX);

	BulletExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x46AFB0, BulletClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x46AE70, BulletClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(BulletClass *, pItem, 0x4);
	GET_STACK(IStream *, pStm, 0x8);

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
	GET(BulletClass *, pThis, ECX);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   {if (auto cc = dynamic_cast<BulletScript*>(c)) {cc->OnInit(); } });

	return 0;
}

DEFINE_HOOK(0x468B5D, BulletClass_Put, 0x6)
{
	GET(BulletClass *, pThis, EBX);
	GET_STACK(CoordStruct *, pCoord, -0x20);
	DirType faceDir = DirType::North;

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pCoord, faceDir](Component *c)
							   { if (auto cc = dynamic_cast<BulletScript*>(c)) {cc->OnPut(pCoord, faceDir); } });

	return 0;
}

DEFINE_HOOK(0x4666F7, BulletClass_Update, 0x6)
{
	GET(BulletClass *, pThis, EBP);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (c)c->OnUpdate(); });

	return 0;
}

DEFINE_HOOK_AGAIN(0x467FEE, BulletClass_UpdateEnd, 0x6)
DEFINE_HOOK(0x466781, BulletClass_UpdateEnd, 0x6)
{
	GET(BulletClass *, pThis, EBP);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (c)c->OnUpdateEnd(); });

	return 0;
}

DEFINE_HOOK(0x4690C1, BulletClass_Detonate, 0x8)
{
	GET(BulletClass *, pThis, ECX);
	GET_BASE(CoordStruct *, pPos, 0x8);

	auto pExt = BulletExt::ExtMap.Find(pThis);
	bool skip = false;
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<BulletScript*>(c)) {cc->OnDetonate(pPos, skip); } });

	if (skip)
	{
		return 0x46A2FB;
	}

	return 0;
}

// ----------------
// Feature
// ----------------

// when shooter dead, project's house will be 0
DEFINE_HOOK(0x469A75, BulletClass_Detonate_GetHouse, 0x7)
{
	GET(BulletClass *, pThis, ESI);
	GET(HouseClass *, pHouse, ECX);

	if (!pHouse)
	{
		// GetStatus save the House
		if (BulletStatus *status = GetStatus<BulletExt, BulletStatus>(pThis))
		{
			if (HouseClass *pSourceHouse = status->pSourceHouse)
			{
				R->ECX(pSourceHouse);
			}
		}
	}

	return 0;
}

// Take over to create Warhead Anim
DEFINE_HOOK(0x469C4E, BulletClass_Detonate_WHAnim_Remap, 0x5)
{
	GET(BulletClass *, pThis, ESI);
	GET(AnimTypeClass *, pAnimType, EBX);
	GET_STACK(CoordStruct, pos, 0x64);
	if (pAnimType)
	{
		AnimClass *pAnim = GameCreate<AnimClass>(pAnimType, pos, 0, 1, 0x2600, -15, false);
		SetAnimOwner(pAnim, pThis);
		SetAnimCreater(pAnim, pThis);
	}

	return 0;
}
