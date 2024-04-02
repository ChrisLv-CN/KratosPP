#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <InfantryClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TechnoExt.h>
#include <Extension/WarheadTypeExt.h>

#include <Ext/TechnoType/TechnoStatus.h>

DEFINE_HOOK(0x5194EF, InfantryClass_DrawIt_InAir_Shadow_Skip, 0x5)
{
	GET(InfantryClass*, pInf, EBP);
	if (pInf->Type->NoShadow)
	{
		return 0x51958A;
	}
	return 0;
}

#pragma region Infantry death anims
// NotHutman and not set DeathAnims
DEFINE_HOOK(0x518505, Infantry_ReceiveDamage_NotHuman_DeathAnim_Remap, 0x6)
{
	GET(InfantryClass*, pInf, ESI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pInf))
	{
		if (status->PlayDestroyAnims())
		{
			pInf->UnInit();
			return 0x5185E5;
		}
	}
	return 0;
}

// IsHutman and not set DeathAnims
DEFINE_HOOK(0x5185C8, Infantry_ReceiveDamage_DeathAnim_Remap, 0x6)
{
	GET(InfantryClass*, pInf, ESI);
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pInf))
	{
		if (status->PlayDestroyAnims())
		{
			return 0x5185F1;
		}
	}
	return 0;
}

#pragma endregion

// DEFINE_HOOK(0x51F5C0, Infantry_Mission_Hunt_Deploy, 0x6)
// {
// 	GET(InfantryClass*, pInf, ESI);
// 	Sequence doing = pInf->SequenceAnim;
// 	AbstractClass* pTarget = pInf->Target;
// 	if (!pInf->Owner->IsControlledByHuman()
// 		&& pTarget && pInf->IsCloseEnough(pTarget, pInf->SelectWeapon(pTarget))
// 		&& doing != Sequence::Deploy && doing != Sequence::Deployed && doing != Sequence::DeployedFire && doing != Sequence::DeployedIdle)
// 	{
// 		int result = pInf->UpdateDeplory();
// 		if (result != -1)
// 		{
// 			R->EAX(result);
// 			return 0x51F5BE;
// 		}
// 	}
// 	return 0;
// }
