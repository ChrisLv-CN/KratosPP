#include <exception>
#include <Windows.h>

#include <AnimTypeClass.h>
#include <GeneralDefinitions.h>
#include <SpecificStructures.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/AnimExt.h>
#include <Extension/TechnoExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/AnimType/ExpireAnimData.h>
#include <Ext/AnimType/AnimStatus.h>
#include <Ext/Common/CommonStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>

// ----------------
// Extension
// ----------------


DEFINE_HOOK_AGAIN(0x422126, AnimClass_CTOR, 0x5)
DEFINE_HOOK_AGAIN(0x422707, AnimClass_CTOR, 0x5)
DEFINE_HOOK(0x4228D2, AnimClass_CTOR, 0x5)
{
	if (!Common::IsLoadGame)
	{
		GET(AnimClass*, pItem, ESI);

		AnimExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x422967, AnimClass_DTOR, 0x6)
{
	GET(AnimClass*, pItem, ESI);

	AnimExt::ExtMap.Remove(pItem);

	return 0;
}

/*Crash when Anim called with GameDelete()
DEFINE_HOOK(0x426598, AnimClass_SDDTOR, 0x7)
{
	GET(AnimClass*, pItem, ESI);

	if(AnimExt::ExtMap.Find(pItem))
	{
		AnimExt::ExtMap.Remove(pItem);
	}

	return 0;
}
*/

DEFINE_HOOK_AGAIN(0x425280, AnimClass_SaveLoad_Prefix, 0x5)
DEFINE_HOOK(0x4253B0, AnimClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(AnimClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	AnimExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK_AGAIN(0x425391, AnimClass_Load_Suffix, 0x7)
DEFINE_HOOK_AGAIN(0x4253A2, AnimClass_Load_Suffix, 0x7)
DEFINE_HOOK(0x425358, AnimClass_Load_Suffix, 0x7)
{
	AnimExt::ExtMap.LoadStatic();
	return 0;
}

DEFINE_HOOK(0x4253FF, AnimClass_Save_Suffix, 0x5)
{
	AnimExt::ExtMap.SaveStatic();
	return 0;
}

// ----------------
// Component
// ----------------

DEFINE_HOOK(0x423AC0, AnimClass_Update, 0x6)
{
	GET(AnimClass*, pThis, ECX);

	if (auto pExt = AnimExt::ExtMap.Find(pThis))
	{
		pExt->_GameObject->Foreach([](Component* c)
			{ c->OnUpdate(); });
	}

	return 0;
}

DEFINE_HOOK_AGAIN(0x42429E, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x42437E, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x4243A6, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x424567, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x4246DC, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x424B42, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x4247EB, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x42492A, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK_AGAIN(0x424B29, AnimClass_UpdateEnd, 0x6)
DEFINE_HOOK(0x424B1B, AnimClass_UpdateEnd, 0x6)
{
	GET(AnimClass*, pThis, ESI);

	if (auto pExt = AnimExt::ExtMap.Find(pThis))
		pExt->_GameObject->Foreach([](Component* c)
			{ c->OnUpdateEnd(); });

	return 0;
}

DEFINE_HOOK(0x424785, AnimClass_Loop, 0x6)
{
	GET(AnimClass*, pThis, ESI);

	if (auto pExt = AnimExt::ExtMap.Find(pThis))
	{
		pExt->_GameObject->Foreach([](Component* c)
			{if (auto cc = dynamic_cast<IAnimScript*>(c)) { cc->OnLoop(); } });
	}

	return 0;
}

DEFINE_HOOK_AGAIN(0x4247F3, AnimClass_Done, 0x6)
DEFINE_HOOK(0x424298, AnimClass_Done, 0x6)
{
	GET(AnimClass*, pThis, ESI);

	if (auto pExt = AnimExt::ExtMap.Find(pThis))
	{
		pExt->_GameObject->Foreach([](Component* c)
			{ if (auto cc = dynamic_cast<IAnimScript*>(c)) { cc->OnDone(); } });
	}

	return 0;
}

DEFINE_HOOK(0x424807, AnimClass_Next, 0x6)
{
	GET(AnimClass*, pThis, ESI);
	GET(AnimTypeClass*, pNextAnimType, ECX);

	if (auto pExt = AnimExt::ExtMap.Find(pThis))
	{
		pExt->_GameObject->Foreach([pNextAnimType](Component* c)
			{ if (auto cc = dynamic_cast<IAnimScript*>(c)) { cc->OnNext(pNextAnimType); } });
	}

	return 0;
}

// ----------------
// Feature
// ----------------

#pragma region remap

DEFINE_HOOK(0x42312A, AnimClass_Draw_Remap, 0x6)
{
	GET(AnimClass*, pThis, ESI);
	if (pThis && pThis->Type->AltPalette && pThis->Owner)
	{
		return 0x423130;
	}
	return 0x4231F3;
}

DEFINE_HOOK(0x423136, AnimClass_Draw_Remap2, 0x6)
{
	GET(AnimClass*, pThis, ESI);
	if (pThis && pThis->Type->AltPalette && pThis->Owner)
	{
		R->ECX(pThis->Owner);
	}
	return 0;
}

DEFINE_HOOK(0x423E75, AnimClass_Extras_Remap, 0x6)
{
	GET(AnimClass*, pThis, ESI);
	GET(AnimClass*, pNewAnim, EDI);

	pNewAnim->Owner = pThis->Owner;

	return 0;
}

// Take over to Create Bounce Anim
DEFINE_HOOK(0x423991, AnimClass_Bounce_Remap, 0x5)
{
	GET(AnimClass*, pThis, EBP);
	if (pThis->Type && pThis->Type->BounceAnim)
	{
		AnimClass* pNewAnim = GameCreate<AnimClass>(pThis->Type->BounceAnim, pThis->GetCoords());
		pNewAnim->Owner = pThis->Owner;
		return 0x4239D3;
	}

	return 0;
}

// Take over to Create Spawn Anim
DEFINE_HOOK(0x423F8C, AnimClass_Spawn_Remap, 0x5)
{
	GET(AnimClass*, pThis, ESI);
	if (pThis->Type && pThis->Type->Spawns)
	{
		AnimClass* pNewAnim = GameCreate<AnimClass>(pThis->Type->Spawns, pThis->GetCoords());
		pNewAnim->Owner = pThis->Owner;
		return 0x423FC3;
	}

	return 0;
}

// Take over to Create Trailer Anim
DEFINE_HOOK(0x4242E1, AnimClass_Trailer_Remap, 0x5)
{
	GET(AnimClass*, pThis, ESI);
	if (pThis->Type && pThis->Type->TrailerAnim)
	{
		AnimClass* pNewAnim = GameCreate<AnimClass>(pThis->Type->TrailerAnim, pThis->GetCoords());
		pNewAnim->Owner = pThis->Owner;
		return 0x424322;
	}

	return 0;
}

DEFINE_HOOK(0x45197B, BuildingClass_UpdateAnim_SetOwner, 0x6)
{
	GET(AnimClass*, pThis, EBP);
	if (pThis)
	{
		GET(TechnoClass*, pBuilding, ESI);
		SetAnimOwner(pThis, pBuilding);
		// Building Anim is not attach to the building
		AnimStatus* status = nullptr;
		if (TryGetStatus<AnimExt>(pThis, status))
		{
			GET(CoordStruct*, pOffset, EBX);
			status->Offset = *pOffset;
			status->pAttachOwner = pBuilding;
			status->pCreater = pBuilding; // Building's anim bind to building
		}
	}
	return 0;
}

DEFINE_HOOK(0x423630, AnimClass_Draw_Colour, 0x6)
{
	GET(AnimClass*, pAnim, ESI);

	if (pAnim)
	{
		AnimStatus* animStatus = nullptr;
		if (pAnim->IsBuildingAnim)
		{
			TechnoClass* pCreater = nullptr;
			TechnoStatus* technoStatus = nullptr;
			if (TryGetStatus<AnimExt, AnimStatus>(pAnim, animStatus)
				&& animStatus->TryGetCreater(pCreater)
				&& TryGetStatus<TechnoExt, TechnoStatus>(pCreater, technoStatus))
			{
				technoStatus->DrawSHP_Paintball_BuildingAnim(R);
			}

		}
		else if (TryGetStatus<AnimExt, AnimStatus>(pAnim, animStatus))
		{
			animStatus->DrawSHP_Paintball(R);
		}
	}

	return 0;
}

#pragma endregion

#pragma region AnimType Damage

// Takes over all damage from animations, including Ares
DEFINE_HOOK(0x424513, AnimClass_Update_Explosion, 0x6)
{
	GET(AnimClass*, pThis, ESI);
	AnimStatus* status = nullptr;
	if (CombatDamage::Data()->AllowAnimDamageTakeOverByKratos && TryGetStatus<AnimExt>(pThis, status))
	{
		status->Explosion_Damage();
		return 0x42464C;
	}

	return 0;
}

// 碎片、流星敲地板触发，砸水中不触发
DEFINE_HOOK(0x423E7B, AnimClass_Extras_Explosion, 0xA)
{
	GET(AnimClass*, pThis, ESI);

	AnimStatus* status = nullptr;
	if (CombatDamage::Data()->AllowAnimDamageTakeOverByKratos && TryGetStatus<AnimExt>(pThis, status))
	{
		status->Explosion_Damage(true, true);
		return 0x423EFD;
	}

	return 0;
}

// Take over to create Extras Anim when Meteor/Debris hit the water
// Phobos hook on 0x423CC7 and skip all game code, so it won't work with Phobos
DEFINE_HOOK(0x423CD5, AnimClass_Extras_HitWater, 0x6)
{
	GET(AnimClass*, pThis, ESI);

	AnimStatus* status = nullptr;
	if (TryGetStatus<AnimExt>(pThis, status))
	{
		if (CombatDamage::Data()->AllowAnimDamageTakeOverByKratos && CombatDamage::Data()->AllowDamageIfDebrisHitWater)
		{
			status->Explosion_Damage(true);
		}
	}
	// 接管砸在水中的动画
	ExpireAnimData* data = INI::GetConfig<ExpireAnimData>(INI::Art, pThis->Type->ID)->Data;
	CoordStruct location = pThis->GetCoords();
	// 涟漪
	AnimTypeClass* pWake = RulesClass::Instance->Wake;
	if (IsNotNone(data->WakeAnimOnWater))
	{
		pWake = AnimTypeClass::Find(data->WakeAnimOnWater.c_str());
		if (!pWake)
		{
			Debug::Log("Warning: Anim %s try to create a unknow wake anim %s.\n", pThis->Type->ID, data->WakeAnimOnWater.c_str());
		}
	}
	if (pWake)
	{
		AnimClass* pNewAnim = GameCreate<AnimClass>(pWake, location);
		pNewAnim->Owner = pThis->Owner;
	}
	// 水花
	AnimTypeClass* pSplash = nullptr;
	if (IsNotNone(data->ExpireAnimOnWater))
	{
		pSplash = AnimTypeClass::Find(data->ExpireAnimOnWater.c_str());
		if (!pSplash)
		{
			Debug::Log("Warning: Anim %s try to create a unknow splash anim %s.\n", pThis->Type->ID, data->ExpireAnimOnWater.c_str());
		}
	}
	else
	{
		// 流星是大水花，碎片是小水花
		pSplash = pThis->Type->IsMeteor ? *RulesClass::Instance->SplashList.back() : *RulesClass::Instance->SplashList.front();
	}
	if (pSplash)
	{
		location.Z += 3;
		AnimClass* pNewAnim = GameCreate<AnimClass>(pSplash, location);
		pNewAnim->Owner = pThis->Owner;
	}
	return 0x423EFD;
}

#pragma endregion
