#include "ImmuneEffect.h"

void ImmuneEffect::ImmuneLogic()
{
	if (pTechno && Data->EMP)
	{
		pTechno->EMPLockRemaining = 0;
	}
}

void ImmuneEffect::OnUpdate()
{
	ImmuneLogic();
}

void ImmuneEffect::OnReceiveDamage(args_ReceiveDamage* args)
{
	if (*args->Damage > 0)
	{
		WarheadTypeClass* pWH = args->WH;
		if ((pWH->Radiation && Data->Radiation)
			|| (pWH->PsychicDamage && Data->PsionicWeapons)
			|| (pWH->Poison && Data->Poison)
			|| Data->CeaseFire(pWH->ID))
		{
			*args->Damage = 0;
		}
	}
	ImmuneLogic();
}
