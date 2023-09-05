#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Utilities/Macro.h>
#include <Extension/TechnoExt.h>
#include <Common/Components/Component.h>
#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <SpecificStructures.h>

// ----------------
// Extension
// ----------------

extern bool IsLoadGame;

DEFINE_HOOK(0x6F3260, TechnoClass_CTOR, 0x5)
{
	// skip this Allocate just left TechnoClass_Load_Suffix => LoadKey to Allocate
	// when is loading a save game.
	if (!IsLoadGame)
	{
		GET(TechnoClass *, pItem, ESI);
#ifdef DEBUG
		const char *typeId = "Unknow";
		if (pItem->GetTechnoType())
		{
			typeId = pItem->GetTechnoType()->ID;
		}
		Debug::Log("Techno [%s]%d calling CTOR\n", typeId, pItem);
#endif
		TechnoExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x6F4500, TechnoClass_DTOR, 0x5)
{
	GET(TechnoClass *, pItem, ECX);

	TechnoExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x70C250, TechnoClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x70BF50, TechnoClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(TechnoClass *, pItem, 0x4);
	GET_STACK(IStream *, pStm, 0x8);
#ifdef DEBUG
	const char *typeId = "Unknow";
	if (pItem->GetTechnoType())
	{
		typeId = pItem->GetTechnoType()->ID;
	}
	Debug::Log("Techno [%s]%d calling SaveLoad\n", typeId, pItem);
#endif
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
	GET(TechnoClass *, pThis, ESI);
#ifdef DEBUG
	const char *typeId = "Unknow";
	if (pThis->GetType())
	{
		typeId = pThis->GetType()->ID;
	}
	Debug::Log("Techno [%s]%d calling OnInit()\n", typeId, pThis);
#endif

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   {if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnInit(); } });

	return 0;
}

DEFINE_HOOK(0x6F6CA0, TechnoClass_Put, 0x7)
{
	GET(TechnoClass *, pThis, ECX);
	GET_STACK(CoordStruct *, pCoord, 0x4);
	GET_STACK(DirType, faceDir, 0x8);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pCoord, faceDir](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnPut(pCoord, faceDir); } });

	return 0;
}

// avoid hook conflict with phobos feature -- shield
//[Hook(HookType.AresHook, Address = 0x6F6AC0, Size = 5)]
DEFINE_HOOK(0x6F6AC4, TechnoClass_Remove, 0x5)
{
	GET(TechnoClass *, pThis, ECX);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnRemove(); } });

	return 0;
}

DEFINE_HOOK(0x6F9E50, TechnoClass_Update, 0x5)
{
	GET(TechnoClass *, pThis, ECX);

	// Do not search this up again in any functions called here because it is costly for performance - Starkku
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (c)c->OnUpdate(); });

	return 0;
}

DEFINE_HOOK_AGAIN(0x6FAFFD, TechnoClass_UpdateEnd, 0x7)
DEFINE_HOOK(0x6FAF7A, TechnoClass_UpdateEnd, 0x7)
{
	GET(TechnoClass *, pThis, ESI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (c)c->OnUpdateEnd(); });

	return 0;
}

// If pObject.Is_Being_Warped() is ture, will skip Foot::AI and Techno::AI
DEFINE_HOOK_AGAIN(0x44055D, TechnoClass_WarpUpdate, 0x6) // Building
DEFINE_HOOK_AGAIN(0x51BBDF, TechnoClass_WarpUpdate, 0x6) // Infantry
DEFINE_HOOK_AGAIN(0x736321, TechnoClass_WarpUpdate, 0x6) // Unit
DEFINE_HOOK(0x414CF2, TechnoClass_WarpUpdate, 0x6)		 // Aircraft
{
	GET(TechnoClass *, pThis, ESI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (c)c->OnWarpUpdate(); });

	return 0;
}

DEFINE_HOOK(0x71A88D, TemporalClass_Update, 0x0)
{
	GET(TemporalClass *, pTemporal, ESI);

	TechnoClass *pThis = pTemporal->Target;
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pTemporal](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnTemporalUpdate(pTemporal); } });

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
	GET(TemporalClass *, pTemporal, ESI);

	TechnoClass *pThis = pTemporal->Target;
	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([pTemporal](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnTemporalEliminate(pTemporal); } });

	return 0;
}

DEFINE_HOOK(0x701900, TechnoClass_ReceiveDamage, 0x6)
{
	GET(TechnoClass *, pThis, ECX);
	LEA_STACK(args_ReceiveDamage *, args, 0x4);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([args](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnReceiveDamage(args); } });

	// TODO Toy warhead

	return 0;
}

bool DamageByToyWH = false;

DEFINE_HOOK(0x701F9A, TechnoClass_ReceiveDamage_SkipAllReaction, 0x6)
{
	// Toy warhead
	if (DamageByToyWH)
	{
		return 0x702D11;
	}
	return 0x70202E;
}

DEFINE_HOOK(0x701DFF, TechnoClass_ReceiveDamageEnd, 0x7)
{
	GET(TechnoClass *, pThis, ESI);
	GET(int *, pRealDamage, EBX);
	GET(WarheadTypeClass *, pWH, EBP);
	GET(DamageState, damageState, EDI);
	GET_STACK(ObjectClass *, pAttacker, 0xD4);
	GET_STACK(HouseClass *, pAttackingHouse, 0xE0);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnReceiveDamageEnd(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse); } });

	return 0;
}

DEFINE_HOOK(0x702050, TechnoClass_ReceiveDamage_Destroy, 0x6)
{
	GET(TechnoClass *, pThis, ESI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnReceiveDamageDestroy(); } });

	return 0;
}

DEFINE_HOOK(0x702E9D, TechnoClass_RegisterDestruction, 0x6)
{
	GET(TechnoClass *, pThis, ESI);
	GET(TechnoClass *, pKiller, EDI);
	GET(int, cost, EBP);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	bool skip = false;
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnRegisterDestruction(pKiller, cost, skip); } });

	// skip the entire veterancy
	if (skip)
	{
		return 0x702FF5;
	}
	return 0;
}

DEFINE_HOOK(0x6FC339, TechnoClass_CanFire, 0x6)
{
	GET(TechnoClass *, pThis, ESI);
	GET(WeaponTypeClass *, pWeapon, EDI);
	GET_STACK(AbstractClass *, pTarget, 0x20 - (-0x4));

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	bool ceaseFire = false;
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->CanFire(pTarget, pWeapon, ceaseFire); } });

	// return FireError::ILLEGAL
	if (ceaseFire)
	{
		return 0x6FCB7E;
	}
	return 0;
}

DEFINE_HOOK(0x6FDD50, TechnoClass_Fire, 0x6)
{
	GET(TechnoClass *, pThis, ECX);
	GET_STACK(AbstractClass *, pTarget, 0x4);
	GET_STACK(int, weaponIdx, 0x8);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnFire(pTarget, weaponIdx); } });

	return 0;
}

DEFINE_HOOK(0x6F65D1, TechnoClass_DrawHealthBar_Building, 0x6)
{
	GET(TechnoClass *, pThis, ESI);
	GET(int, barLength, EBX);
	GET_STACK(Point2D *, pPos, 0x4C - (-0x4));
	GET_STACK(RectangleStruct *, pBound, 0x4C - (-0x8));

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->DrawHealthBar(barLength, pPos, pBound, true); } });

	return 0;
}

DEFINE_HOOK(0x6F683C, TechnoClass_DrawHealthBar_Other, 0x7)
{
	GET(TechnoClass *, pThis, ESI);
	int barLength = pThis->What_Am_I() == AbstractType::Infantry ? 8 : 17;
	GET_STACK(Point2D *, pPos, 0x4C - (-0x4));
	GET_STACK(RectangleStruct *, pBound, 0x4C - (-0x8));

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->DrawHealthBar(barLength, pPos, pBound, false); } });

	return 0;
}

DEFINE_HOOK(0x5F45A0, TechnoClass_Select, 0x5)
{
	GET(TechnoClass *, pThis, EDI);

	auto pExt = TechnoExt::ExtMap.Find(pThis);
	bool selectable = true;
	pExt->_GameObject->Foreach([&](Component *c)
							   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnSelect(selectable); } });

	if (!selectable)
	{
		return 0x5F45A9;
	}
	return 0;
}

DEFINE_HOOK_AGAIN(0x730DEB, ObjectClass_GuardCommand, 0x6) // Building
DEFINE_HOOK(0x730E56, ObjectClass_GuardCommand, 0x6)
{
	GET(ObjectClass *, pThis, ESI);

	if (pThis->AbstractFlags == AbstractFlags::Techno)
	{
		auto pExt = TechnoExt::ExtMap.Find((TechnoClass *)pThis);
		pExt->_GameObject->Foreach([](Component *c)
								   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnGuardCommand(); } });
	}
	return 0;
}

DEFINE_HOOK(0x730EEB, ObjectClass_StopCommand, 0x6)
{
	GET(ObjectClass *, pThis, ESI);

	if (pThis->AbstractFlags == AbstractFlags::Techno)
	{
		auto pExt = TechnoExt::ExtMap.Find((TechnoClass *)pThis);
		pExt->_GameObject->Foreach([](Component *c)
								   { if (auto cc = dynamic_cast<TechnoScript*>(c)) {cc->OnStopCommand(); } });
	}
	return 0;
}
