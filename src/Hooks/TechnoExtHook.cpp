﻿#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <SpecificStructures.h>
#include <TechnoClass.h>
#include <VoxelAnimTypeClass.h>
#include <JumpjetLocomotionClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/INI/INI.h>

#include <Extension/TechnoExt.h>
#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/Common/CommonStatus.h>
#include <Ext/Common/ExpandAnimsManager.h>
#include <Ext/ObjectType/AttachEffect.h>
#include <Ext/TechnoType/AutoFireAreaWeapon.h>
#include <Ext/TechnoType/TechnoStatus.h>

#include <Ext/TechnoType/SelectWeaponData.h>

// ----------------
// Extension
// ----------------


DEFINE_HOOK(0x6F3260, TechnoClass_CTOR, 0x5)
{
	// skip this Allocate just left TechnoClass_Load_Suffix => LoadKey to Allocate
	// when is loading a save game.
	if (!Common::IsLoadGame)
	{
		GET(TechnoClass*, pItem, ESI);

		TechnoExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x6F4500, TechnoClass_DTOR, 0x5)
{
	GET(TechnoClass*, pItem, ECX);
	// 广播TechnoDelete
	EventSystems::Logic.Broadcast(Events::TechnoDeleteEvent, pItem);
	// 从ExtMap中删除
	TechnoExt::ExtData* ext = TechnoExt::ExtMap.Find(pItem);
	if (ext)
	{
		ext->SetExtStatus(nullptr);
		ext->_GameObject->Foreach([](Component* c)
			{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnUnInit(); } });
	}
	TechnoExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x70C250, TechnoClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x70BF50, TechnoClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(TechnoClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	TechnoExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x70C249, TechnoClass_Load_Suffix, 0x5)
{
	TechnoExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x70C264, TechnoClass_Save_Suffix, 0x5)
{
	TechnoExt::ExtMap.SaveStatic();

	return 0;
}

// ----------------
// Component
// ----------------

DEFINE_HOOK(0x6F42ED, TechnoClass_Init, 0xA)
{
	GET(TechnoClass*, pThis, ESI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnInit(); } });

	return 0;
}

DEFINE_HOOK(0x6F6CA0, TechnoClass_Put, 0x7)
{
	GET(TechnoClass*, pThis, ECX);
	GET_STACK(CoordStruct*, pCoord, 0x4);
	GET_STACK(DirType, faceDir, 0x8);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pCoord, faceDir](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnPut(pCoord, faceDir); } });

	return 0;
}

// avoid hook conflict with phobos feature -- shield
//[Hook(HookType.AresHook, Address = 0x6F6AC0, Size = 5)]
DEFINE_HOOK(0x6F6AC4, TechnoClass_Remove, 0x5)
{
	GET(TechnoClass*, pThis, ECX);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnRemove(); } });

	return 0;
}

DEFINE_HOOK(0x6F9E50, TechnoClass_Update, 0x5)
{
	GET(TechnoClass*, pThis, ECX);

	// Do not search this up again in any functions called here because it is costly for performance - Starkku
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ c->OnUpdate(); });

	return 0;
}

DEFINE_HOOK_AGAIN(0x6FAFFD, TechnoClass_UpdateEnd, 0x7)
DEFINE_HOOK(0x6FAF7A, TechnoClass_UpdateEnd, 0x7)
{
	GET(TechnoClass*, pThis, ESI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ c->OnUpdateEnd(); });

	return 0;
}

// If pObject.Is_Being_Warped() is ture, will skip Foot::AI and Techno::AI
DEFINE_HOOK_AGAIN(0x44055D, TechnoClass_WarpUpdate, 0x6) // Building
DEFINE_HOOK_AGAIN(0x51BBDF, TechnoClass_WarpUpdate, 0x6) // Infantry
DEFINE_HOOK_AGAIN(0x736321, TechnoClass_WarpUpdate, 0x6) // Unit
DEFINE_HOOK(0x414CF2, TechnoClass_WarpUpdate, 0x6)		 // Aircraft
{
	GET(TechnoClass*, pThis, ESI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ c->OnWarpUpdate(); });

	return 0;
}

DEFINE_HOOK(0x71A88D, TemporalClass_Update, 0x0)
{
	GET(TemporalClass*, pTemporal, ESI);

	TechnoClass* pThis = pTemporal->Target;
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pTemporal](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnTemporalUpdate(pTemporal); } });

	GET(int, eax, EAX);
	GET(int, ebx, EBX);
	if (eax <= ebx)
	{
		return 0x71A895;
	}
	return 0x71AB08;
}

DEFINE_HOOK(0x71A917, TemporalClass_Update_Eliminate, 0x5)
{
	GET(TemporalClass*, pTemporal, ESI);

	TechnoClass* pThis = pTemporal->Target;
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pTemporal](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnTemporalEliminate(pTemporal); } });

	return 0;
}

bool DamageByToyWH = false;

DEFINE_HOOK(0x701900, TechnoClass_ReceiveDamage, 0x6)
{
	GET(TechnoClass*, pThis, ECX);
	LEA_STACK(args_ReceiveDamage*, args, 0x4);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([args](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnReceiveDamage(args); } });

	// Toy warhead
	WarheadTypeExt::TypeData* whData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(args->WH);
	if (whData->IsToy)
	{
		*args->Damage = 0;
	}
	if (*args->Damage == 0)
	{
		// Immune this damage, nothing happend;
		DamageByToyWH = true;
	}
	return 0;
}

DEFINE_HOOK(0x7019D8, TechnoClass_ReceiveDamage_At_Least1, 0x5)
{
	if (DamageByToyWH)
	{
		DamageByToyWH = false;
	}
	else
	{
		// Restore overridden instructions
		GET(int*, pDamage, EBX);
		if (*pDamage < 1)
		{
			*pDamage = 1;
		}
	}
	return 0x7019E3;
}

#pragma region ImmuneToOOXX
// Ares hook in 471C96 and return 471D2E
DEFINE_HOOK(0x471D2E, CaptureManagerClass_Is_Controllable, 0x7)
{
	GET(TechnoClass*, pTechno, ESI);
	AttachEffect* aem = nullptr;
	if (TryGetAEManager<TechnoExt>(pTechno, aem) && aem->GetImmuneData().Psionics)
	{
		return 0x471D35;
	}
	return 0;
}

// Ares skip this whole function, here can do anything.
DEFINE_HOOK(0x53B233, IonStormClass_Dominator_Activate, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AttachEffect* aem = nullptr;
	if (TryGetAEManager<TechnoExt>(pTechno, aem) && aem->GetImmuneData().Psionics)
	{
		return 0x53B364;
	}
	return 0;
}

/* Cannot hook in those address, Not Ares or Phobos */
/* Modify Damage number in AE's ReceiveDamage function
DEFINE_HOOK(0x701C45, TechnoClass_ReceiveDamage_PsionicWeapons, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AttachEffect* aem = nullptr;
	if (TryGetAEManager<TechnoExt>(pTechno, aem) && aem->GetImmuneData().PsionicWeapons)
	{
		return 0x701C4F;
	}
	return 0;
}

DEFINE_HOOK(0x701C08, TechnoClass_ReceiveDamage_Radiation, 0xA)
{
	GET(TechnoClass*, pTechno, ESI);
	AttachEffect* aem = nullptr;
	if (TryGetAEManager<TechnoExt>(pTechno, aem) && aem->GetImmuneData().Radiation)
	{
		return 0x701C1C;
	}
	return 0;
}

DEFINE_HOOK(0x701C78, TechnoClass_ReceiveDamage_Poison, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	AttachEffect* aem = nullptr;
	if (TryGetAEManager<TechnoExt>(pTechno, aem) && aem->GetImmuneData().Poison)
	{
		return 0x701C82;
	}
	return 0;
}

*/
#pragma endregion

// modify the real damage
DEFINE_HOOK(0x5F5498, ObjectClass_ReceiveDamage_RealDamage, 0xC)
{
	GET(ObjectClass*, pThis, ESI);
	GET(int*, pRealDamage, EDI);
	GET_STACK(WarheadTypeClass*, pWH, 0x34 - 0x4);
	GET_STACK(TechnoClass*, pAttacker, 0x34);
	GET_STACK(HouseClass*, pAttackingHouse, 0x40);

	TechnoClass* pTechno = nullptr;
	if (CastToTechno(pThis, pTechno))
	{
		auto pExt = TechnoExt::ExtMap.Find(pTechno);
		pExt->_GameObject->Foreach([&](Component* c)
			{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnReceiveDamageReal(pRealDamage, pWH, pAttacker, pAttackingHouse); } });
		R->ECX(*pRealDamage);
		if (*pRealDamage < 0)
		{
			return 0x5F546A;
		}
	}
	return 0;
}

DEFINE_HOOK(0x701DFF, TechnoClass_ReceiveDamageEnd, 0x7)
{
	GET(TechnoClass*, pThis, ESI);
	GET(int*, pRealDamage, EBX);
	GET(WarheadTypeClass*, pWH, EBP);
	GET(DamageState, damageState, EDI);
	GET_STACK(ObjectClass*, pAttacker, 0xD4);
	GET_STACK(HouseClass*, pAttackingHouse, 0xE0);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnReceiveDamageEnd(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse); } });

	return 0;
}

DEFINE_HOOK(0x702050, TechnoClass_ReceiveDamage_Destroy, 0x6)
{
	GET(TechnoClass*, pThis, ESI);

	EventSystems::Logic.Broadcast(Events::TechnoDestroyEvent, pThis);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnReceiveDamageDestroy(); } });

	return 0;
}

DEFINE_HOOK(0x702E9D, TechnoClass_RegisterDestruction, 0x6)
{
	GET(TechnoClass*, pThis, ESI);
	GET(TechnoClass*, pKiller, EDI);
	GET(int, cost, EBP);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	bool skip = false;
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnRegisterDestruction(pKiller, cost, skip); if (skip) c->Break(); } });

	// skip the entire veterancy
	if (skip)
	{
		return 0x702FF5;
	}
	return 0;
}

DEFINE_HOOK(0x6FC339, TechnoClass_CanFire, 0x6)
{
	GET(TechnoClass*, pThis, ESI);
	GET(WeaponTypeClass*, pWeapon, EDI);
	GET_STACK(AbstractClass*, pTarget, 0x20 - (-0x4));

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	bool ceaseFire = false;
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->CanFire(pTarget, pWeapon, ceaseFire); if (ceaseFire) c->Break(); } });

	// return FireError::ILLEGAL
	if (ceaseFire)
	{
		return 0x6FCB7E;
	}
	return 0;
}

DEFINE_HOOK(0x6FDD50, TechnoClass_Fire, 0x6)
{
	GET(TechnoClass*, pThis, ECX);
	GET_STACK(AbstractClass*, pTarget, 0x4);
	GET_STACK(int, weaponIdx, 0x8);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnFire(pTarget, weaponIdx); } });

	return 0;
}

DEFINE_HOOK(0x6F65D1, TechnoClass_DrawHealthBar_Building, 0x6)
{
	GET(TechnoClass*, pThis, ESI);
	GET(int, barLength, EBX);
	GET_STACK(Point2D*, pPos, 0x4C - (-0x4));
	GET_STACK(RectangleStruct*, pBound, 0x4C - (-0x8));

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->DrawHealthBar(barLength, pPos, pBound, true); } });

	return 0;
}

DEFINE_HOOK(0x6F683C, TechnoClass_DrawHealthBar_Other, 0x7)
{
	GET(TechnoClass*, pThis, ESI);
	int barLength = pThis->WhatAmI() == AbstractType::Infantry ? 8 : 17;
	GET_STACK(Point2D*, pPos, 0x4C - (-0x4));
	GET_STACK(RectangleStruct*, pBound, 0x4C - (-0x8));

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->DrawHealthBar(barLength, pPos, pBound, false); } });

	return 0;
}

DEFINE_HOOK(0x5F45A0, TechnoClass_Select, 0x5)
{
	GET(TechnoClass*, pThis, EDI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	bool selectable = true;
	pExt->_GameObject->Foreach([&](Component* c)
		{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnSelect(selectable); } });

	if (!selectable)
	{
		return 0x5F45A9;
	}
	return 0;
}

DEFINE_HOOK_AGAIN(0x730DEB, ObjectClass_GuardCommand, 0x6) // Building
DEFINE_HOOK(0x730E56, ObjectClass_GuardCommand, 0x6)
{
	GET(ObjectClass*, pThis, ESI);

	TechnoClass* pTechno = nullptr;
	if (CastToTechno(pThis, pTechno))
	{
		auto pExt = TechnoExt::ExtMap.Find(pTechno);
		pExt->_GameObject->Foreach([](Component* c)
			{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnGuardCommand(); } });
	}
	return 0;
}

DEFINE_HOOK(0x730EEB, ObjectClass_StopCommand, 0x6)
{
	GET(ObjectClass*, pThis, ESI);

	TechnoClass* pTechno = nullptr;
	if (CastToTechno(pThis, pTechno))
	{
		auto pExt = TechnoExt::ExtMap.Find(pTechno);
		pExt->_GameObject->Foreach([](Component* c)
			{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnStopCommand(); } });
	}
	return 0;
}

DEFINE_HOOK(0x6F9039, TechnoClass_Greatest_Threat_HealWeaponRange, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);
	int guardRange = pTechno->GetTechnoType()->GuardRange;
	WeaponStruct* pirmary = pTechno->GetTurretWeapon();
	if (pirmary && pirmary->WeaponType)
	{
		int range = pirmary->WeaponType->Range;
		if (range > guardRange)
		{
			guardRange = range;
		}
	}
	WeaponStruct* secondary = pTechno->GetWeapon(1);
	if (secondary && secondary->WeaponType)
	{
		int range = secondary->WeaponType->Range;
		if (range > guardRange)
		{
			guardRange = range;
		}
	}
	R->EDI((unsigned int)guardRange);
	return 0x6F903E;
}

DEFINE_HOOK(0x6F72E3, TechnoClass_IsRange, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(int, range, EDI);
	AttachEffect* aeManager = nullptr;
	if (TryGetAEManager<TechnoExt>(pTechno, aeManager))
	{
		CrateBuffData data = aeManager->CountAttachStatusMultiplier();
		range += (int)(data.RangeCell * Unsorted::LeptonsPerCell);
		range *= data.RangeMultiplier;
	}
	R->EDI(range);
	return 0;
}

DEFINE_HOOK(0x7067F1, TechnoClass_DrawVxl_DisableCache, 0x6)
{
	GET(unsigned int, esi, ESI);
	GET(unsigned int, eax, EAX);

	if (esi != eax)
	{
		GET(TechnoClass*, pTechno, ECX);
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status) && status->DisableVoxelCache)
		{
			// 强制禁用缓存
			return 0x706875;
		}
		return 0x7067F7;
	}
	return 0x706879;
}

DEFINE_HOOK(0x6FC016, TechnoClass_Select_SkipVoice, 0x8)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->DisableSelectVoice)
	{
		return 0x6FC01E;
	}
	return 0;
}

// Can't do anything, like EMP impact
DEFINE_HOOK(0x70EFD0, TechnoClass_IsUnderEMP_CantMove, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->Freezing)
	{
		R->AL(true);
		return 0x70EFDA;
	}
	return 0;
}

#pragma region Draw Colour
DEFINE_HOOK(0x7063FF, TechnoClass_DrawSHP_Colour, 0x7)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status))
	{
		status->DrawSHP_Paintball(R);
		status->DrawSHP_Colour(R);
	}
	return 0;
}

// vxl turret of building
DEFINE_HOOK(0x706640, TechnoClass_DrawVXL_Colour_BuildingTurret, 0x5)
{
	GET(TechnoClass*, pTechno, ECX);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->IsBuilding())
	{
		status->DrawVXL_Paintball(R, true);
	}
	return 0;
}

// after Techno_DrawVXL change berzerk color
DEFINE_HOOK(0x73C15F, UnitClass_DrawVXL_Colour, 0x7)
{
	GET(TechnoClass*, pTechno, EBP);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status))
	{
		status->DrawVXL_Paintball(R, false);
	}
	return 0;
}
#pragma endregion

#pragma region Techno Destroy Debris
// Take over to make vxl debirs
DEFINE_HOOK(0x702299, TechnoClass_Destroy_VxlDebris_Remap, 0xA)
{
	if (AudioVisual::Data()->AllowMakeVoxelDebrisByKratos)
	{
		GET(TechnoClass*, pTechno, ESI);
		TechnoTypeClass* pType = pTechno->GetTechnoType();
		// Phobos hook 这个地址，要自己算随机数
		int max = pType->MaxDebris;
		int min = pType->MinDebris;
		if (min < 0)
		{
			Debug::Log("Warning: TechnoType [%s] MinDebris = %d\n", pType->ID, min);
			min = 0;
		}
		if (max < min)
		{
			Debug::Log("Warning: TechnoType [%s] MinDebris = %d, MaxDebris = %d\n", pType->ID, min, max);
			max = min;
		}
		int times = Random::RandomRanged(min, max);
		if (times > 0)
		{
			DynamicVectorClass<VoxelAnimTypeClass*> debrisTypes = pType->DebrisTypes;
			if (debrisTypes.Count > 0)
			{
				HouseClass* pHouse = pTechno->Owner;
				CoordStruct location = pTechno->GetCoords();
				ExpandAnimsManager::PlayExpandDebirs(debrisTypes, pType->DebrisMaximums, times, location, pHouse, pTechno);
			}
			R->EBX(times);
			return 0x7023E5;
		}
	}
	return 0;
}

DEFINE_HOOK(0x70256C, TechnoClass_Destroy_Debris_Remap, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EDI);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	GET(int, i, EBX);
	if (i > 0)
	{
		return 0x7024E0;
	}
	return 0;
}

DEFINE_HOOK(0x7024B0, TechnoClass_Destroy_Debris_Remap2, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AnimClass*, pAnim, EBX);
	if (pAnim)
	{
		pAnim->Owner = pTechno->Owner;
	}
	GET(int, i, EBP);
	if (i > 0)
	{
		return 0x70240C;
	}
	return 0;
}
#pragma endregion

#pragma region Select weapon
// Can not shoot to water when NavalTargeting = 6
DEFINE_HOOK(0x6FC833, TechnoClass_NavalTargeting, 0x7)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(CellClass*, pTarget, EAX);
	if (pTarget->LandType == LandType::Water && pTechno->GetTechnoType()->NavalTargeting == NavalTargetingType::Naval_None)
	{
		return 0x6FC86A;
	}
	return 0;
}

DEFINE_HOOK(0x6F36DB, TechnoClass_SelectWeapon_AntiMissile, 0xA)
{
	GET(TechnoClass*, pTechno, ESI);
	GET_STACK(AbstractClass*, pTarget, 0x1C);
	GET_STACK(WeaponTypeClass*, pPrimary, 0x14);
	GET_STACK(WeaponTypeClass*, pSecondary, 0x10);
	GET(unsigned int, ebp, EBP);
	if (ebp != 0)
	{
		// 攻击的是单位
		return 0x6F36E3; // 继续检查护甲
	}
	else
	{
		// 攻击的是没有护甲的玩意儿，格子，覆盖物，抛射体等等
		AbstractType abstractType = pTarget->WhatAmI();
		switch (abstractType)
		{
		case AbstractType::Bullet:
			// AntiBullet
			// 检查是自己捕获的目标还是由载具传递给乘客的
			if (!pTechno->Transporter)
			{
				if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
				{
					if (status->AntiBullet->IsAlive())
					{
						// 自己捕获的目标，按设置选择武器
						if (status->AntiBullet->Data.Weapon == 1)
						{
							return 0x6F3807; // 返回副武器
						}
						else
						{
							return 0x6F37AD; // 返回主武器
						}
					}
				}
			}
			// 自动选择可以使用的武器
			if (pSecondary->Projectile->AA && (!pPrimary->Projectile->AA || pTechno->IsCloseEnough(pTarget, 1)))
			{
				return 0x6F3807; // 返回副武器
			}
			break;
		case AbstractType::Terrain:
		case AbstractType::Cell:
			SelectWeaponData* data = INI::GetConfig<SelectWeaponData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
			if (pSecondary->Projectile->AG && data->UseSecondary(pTechno, pTarget, pPrimary, pSecondary))
			{
				return 0x6F3807; // 返回副武器
			}
			break;
		}
	}
	return 0x6F37AD; // 返回主武器
}

// change form Otamaa
DEFINE_HOOK(0x6F37EB, TechnoClass_SelectWeapon_SecondaryCheckAA_SwitchByRange, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(AbstractClass*, pTarget, EBP);
	GET_STACK(WeaponTypeClass*, pPrimary, 0x18 - 0x4);
	GET(WeaponTypeClass*, pSecondary, EAX);
	// check AA
	if (pSecondary->Projectile->AA && pTarget->IsInAir())
	{
		return 0x6F3807; // 返回副武器
	}
	else
	{
		SelectWeaponData* data = INI::GetConfig<SelectWeaponData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
		if (data->UseSecondary(pTechno, pTarget, pPrimary, pSecondary))
		{
			return 0x6F3807; // 返回副武器
		}
	}
	return 0x6F37AD; // 返回主武器
}

DEFINE_HOOK(0x6FDD61, TechnoClass_Fire_OverrideWeapon, 0x5)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->OverrideWeapon->IsAlive())
	{
		WeaponTypeClass* pOverrideWeapon = nullptr;
		if (status->OverrideWeapon->TryGetOverrideWeapon(pTechno->Veterancy.IsElite(), false, pOverrideWeapon))
		{
			R->EBX(pOverrideWeapon);
			return 0x6FDD71;
		}
	}
	return 0;
}

DEFINE_HOOK(0x70D6B4, TechnoClass_Fire_DeathWeapon_OverrideWeapon, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->OverrideWeapon->IsAlive())
	{
		WeaponTypeClass* pOverrideWeapon = nullptr;
		if (status->OverrideWeapon->TryGetOverrideWeapon(pTechno->Veterancy.IsElite(), true, pOverrideWeapon))
		{
			R->EDI(pOverrideWeapon);
			return 0x70D6BA;
		}
	}
	return 0;
}

DEFINE_HOOK(0x70D773, TechnoClass_Fire_DeathWeapon_OverrideWeapon_Stand, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand() && !IsDead(status->pMyMaster))
	{
		// 修改死亡武器的发射者是JOJO
		TechnoClass* pMaster = status->pMyMaster;
		if (status->MyMasterIsSpawned && status->StandData.ExperienceToSpawnOwner && !IsDead(pMaster->SpawnOwner))
		{
			pMaster = pMaster->SpawnOwner;
		}
		if (pMaster->GetTechnoType()->Trainable)
		{
			GET(BulletClass*, pBullet, EBX);
			pBullet->Owner = pMaster;
		}
	}
	return 0;
}
#pragma endregion

DEFINE_HOOK(0x6FF29E, TechnoClass_Fire_ROFMultiplier, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	GET(int, rof, EAX);
	// check skipRof
	AutoFireAreaWeapon* autoArea = GetScript<TechnoExt, AutoFireAreaWeapon>(pTechno);
	if (autoArea && autoArea->SkipROF)
	{
		// skip ROF
		R->EAX(0);
	}
	else
	{
		// 计算ROF
		GET(WeaponTypeClass*, pWeapon, EBX);
		AttachEffect* aem = nullptr;
		if (pTechno->CurrentBurstIndex >= pWeapon->Burst && TryGetAEManager<TechnoExt>(pTechno, aem))
		{
			double rofMult = aem->CountAttachStatusMultiplier().ROFMultiplier;
			R->EAX(rof * rofMult);
		}
	}
	return 0;
}

DEFINE_HOOK(0x736A95, UnitClass_Rotation_Update_Freezing_TurretSpins, 0x6)
{
	GET(TechnoClass*, pTechno, ESI);
	TechnoStatus* status = nullptr;
	if (pTechno && TryGetStatus<TechnoExt>(pTechno, status) && (status->Freezing || status->Teleport->IsFreezing()))
	{
		return 0x736AD5;
	}
	return 0;
}

DEFINE_HOOK(0x4CCBB3, FlyLocomotionClass_Freeze, 0x5)
{
	FlyLocomotionClass* pFly = (FlyLocomotionClass*)(R->ESI() - 4);
	TechnoClass* pTechno = pFly->LinkedTo;
	TechnoStatus* status = nullptr;
	if (pTechno && TryGetStatus<TechnoExt>(pTechno, status) && (status->Freezing || status->Teleport->IsFreezing()))
	{
		return 0x4CCBCE;
	}
	return 0;
}

DEFINE_HOOK(0x54AED0, JumpjetLocomotionClass_Freeze, 0x5)
{
	JumpjetLocomotionClass* pJJ = (JumpjetLocomotionClass*)(R->ESI() - 4);
	TechnoClass* pTechno = pJJ->LinkedTo;
	TechnoStatus* status = nullptr;
	if (pTechno && TryGetStatus<TechnoExt>(pTechno, status) && (status->Freezing || status->Teleport->IsFreezing()))
	{
		// 被冻结时，保持朝向不变
		DirStruct dir = pJJ->LocomotionFacing.Current();
		if (status->JJMark)
		{
			status->JJMark = false;
			dir = status->JJFacing;
		}
		pJJ->LocomotionFacing.SetCurrent(dir);
		pTechno->PrimaryFacing.SetCurrent(dir);
		return 0x54B16C;
	}
	return 0;
}

DEFINE_HOOK(0x54D600, JumpjetLocomotionClass_Update_DontTurnInCell, 0x6)
{
	GET(JumpjetLocomotionClass*, pJJ, ESI);
	CoordStruct targetPos = pJJ->DestinationCoords;
	if (!targetPos.IsEmpty())
	{
		TechnoClass* pTechno = pJJ->LinkedTo;
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status) && status->Teleport->IsReadyToMoveWarp())
		{
			CoordStruct sourcePos = pTechno->GetCoords();
			CellStruct s = CellClass::Coord2Cell(sourcePos);
			CellStruct t = CellClass::Coord2Cell(targetPos);
			if (s.X == t.X && s.Y == t.Y)
			{
				// Same cell, don't move
				GET_STACK(CoordStruct, fixPos, 0x44);
				fixPos.X = targetPos.X;
				fixPos.Y = targetPos.Y;
				R->Stack(0x44, fixPos);
			}
		}
	}
	return 0;
}

// 爬升
DEFINE_HOOK(0x54BC44, JumpjetLocomotionClass_Update_State1_DontTurnInCell, 0x8)
{
	GET(JumpjetLocomotionClass*, pJJ, ESI);
	TechnoClass* pTechno = pJJ->LinkedTo;
	CoordStruct sourcePos = pTechno->GetCoords();
	CoordStruct targetPos = pJJ->DestinationCoords;
	if (sourcePos.X == targetPos.X && sourcePos.Y == targetPos.Y)
	{
		// 完全垂直的升降，不要计算转角，保持朝向不变
		DirStruct dir = pJJ->LocomotionFacing.Current();
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status))
		{
			if (status->JJMark)
			{
				status->JJMark = false;
				dir = status->JJFacing;
			}
		}
		R->Stack(0x10, dir);
	}
	return 0;
}

// 悬停
DEFINE_HOOK(0x54BF53, JumpjetLocomotionClass_Update_State2_MarkDir, 0x5)
{
	GET(JumpjetLocomotionClass*, pJJ, ESI);
	CoordStruct targetPos = pJJ->DestinationCoords;
	if (!targetPos.IsEmpty())
	{
		TechnoClass* pTechno = pJJ->LinkedTo;
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status))
		{
			if (!status->JJMark)
			{
				status->JJMark = true;
				GET_STACK(DirStruct, dir, 0x10); // 自身和目的地之间计算出来的转角
				CoordStruct sourcePos = pTechno->GetCoords();
				CellStruct s = CellClass::Coord2Cell(sourcePos);
				CellStruct t = CellClass::Coord2Cell(targetPos);
				if (s.X == t.X && s.Y == t.Y)
				{
					// 如果在同一个格子，不要乱转
					dir = pJJ->LocomotionFacing.Current();
				}
				status->JJFacing = dir;
			}
		}
	}
	return 0;
}

// 下降
DEFINE_HOOK(0x54C0BB, JumpjetLocomotionClass_Update_State3_DontTurnInCell, 0x7)
{
	GET(JumpjetLocomotionClass*, pJJ, ESI);
	TechnoClass* pTechno = pJJ->LinkedTo;
	CoordStruct sourcePos = pTechno->GetCoords();
	CoordStruct targetPos = pJJ->DestinationCoords;
	if (sourcePos.X == targetPos.X && sourcePos.Y == targetPos.Y)
	{
		// 完全垂直的升降，不要计算转角，保持朝向不变
		DirStruct dir = pJJ->LocomotionFacing.Current();
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status))
		{
			if (status->JJMark)
			{
				status->JJMark = false;
				dir = status->JJFacing;
			}
		}
		R->Stack(0x20, dir);
	}
	return 0;
}

DEFINE_HOOK(0x6622E0, RocketLocomotionClass_Update_Freezing, 0x6)
{
	GET(TechnoClass*, pTechno, ECX);
	TechnoStatus* status = nullptr;
	if (pTechno && TryGetStatus<TechnoExt>(pTechno, status) && (status->Freezing || status->Teleport->IsFreezing()))
	{
		return 0x662FEC;
	}
	return 0;
}
