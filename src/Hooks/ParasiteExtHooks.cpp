#include <exception>
#include <Windows.h>

#include <AnimClass.h>
#include <AnimTypeClass.h>
#include <ParasiteClass.h>

#include <Extension.h>
#include <Helpers/Macro.h>

#include <Ext/Helper/Status.h>

// Take over create Anim by ParasiteEat
DEFINE_HOOK(0x62A13F, ParasiteClass_Update_Anim_Remap, 0x5)
{
	GET(ParasiteClass*, pParasite, ESI);
	GET(AnimTypeClass*, pAnimType, EBP);
	GET_STACK(CoordStruct, location, 0x34);

	AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, location);
	SetAnimOwner(pAnim, dynamic_cast<TechnoClass*>(pParasite->Victim));
	pAnim->Owner = pParasite->Owner->Owner;

	return 0;
}

// Eat by Ginormous Squid
/*
DEFINE_HOOK(0x6297F0, ParasiteClass_GrappleUpdate_Anim_Remap, 0x6)
{
	GET(ParasiteClass*, pParasite, ESI);

	return 0;
}*/
