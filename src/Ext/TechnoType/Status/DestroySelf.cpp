#include "../TechnoStatus.h"

#include <Common/INI/INI.h>

#include <Ext/TechnoType/DamageText.h>

void TechnoStatus::OnUpdate_DestroySelf()
{
	if (DestroySelf->AmIDead() && !IsDead(pTechno) && (DestroySelf->Data.InTransport || !pTechno->Transporter))
	{
		// 通知AE管理器
		if (AEManager())
		{
			AEManager()->OnDestroySelf();
		}
		// 啊我死了
		if (DestroySelf->Data.Peaceful)
		{
			pTechno->Limbo();
			pTechno->Health = 0;
			pTechno->UnInit();
		}
		else
		{
			if (DamageText* damageText = GetComponentInParent<DamageText>())
			{
				damageText->SkipDamageText = true;
			}
			pTechno->TakeDamage(pTechno->Health + 1, pTechno->GetTechnoType()->Crewed);
		}
		_isDead = true;
		// 重要，击杀自己后中断所有后续循环
		Break();
	}
}


void TechnoStatus::OnWarpUpdate_DestroySelf_Stand()
{
	if (AmIStand())
	{
		OnUpdate_DestroySelf();
	}
}
