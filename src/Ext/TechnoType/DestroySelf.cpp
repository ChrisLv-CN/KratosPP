﻿#include "../TechnoStatus.h"

#include <Ext/Helper.h>

void TechnoStatus::InitState_DestroySelf()
{
	DestroySelfData* data = INI::GetConfig<DestroySelfData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	if (data->Enable)
	{
		DestroySelfState.Enable(*data);
	}
}

void TechnoStatus::OnUpdate_DestroySelf()
{
	if (DestroySelfState.AmIDead() && !IsDead(pTechno))
	{
		// 啊我死了
		if (DestroySelfState.Data.Peaceful)
		{
			pTechno->Limbo();
			pTechno->Health = 0;
			pTechno->UnInit();
		}
		else
		{
			_skipDamageText = true;
			pTechno->TakeDamage(pTechno->Health + 1, pTechno->GetTechnoType()->Crewed);
		}
	}
}


void TechnoStatus::OnWarpUpdate_DestroySelf_Stand()
{
	if (AmIStand())
	{
		OnUpdate_DestroySelf();
	}
}
