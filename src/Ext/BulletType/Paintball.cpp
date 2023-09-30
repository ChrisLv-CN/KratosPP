#include "../BulletStatus.h"
#include <Ext/CommonStatus.h>

void BulletStatus::InitState_Paintball()
{
	PaintballData* data = INI::GetConfig<PaintballData>(INI::Rules, pBullet->Type->ID)->Data;
	if (data->Enable)
	{
		PaintballState.Enable(*data);
	}
}

void BulletStatus::DrawVXL_Paintball(REGISTERS* R)
{
	bool changeColor = false;
	bool changeBright = false;
	// pBullet.Ref.ForceShielded 在护盾启用一次后，一直是1，所以不能用来判断是否正处于护盾状态，只能通过判断pBullet.Ref.Base.IsIronCurtained()来判断处于无敌状态
	if (PaintballState.NeedPaint(changeColor, changeBright))
	{
		if (changeColor)
		{
			R->Stack(0x0, PaintballState.Data.Color2);
		}
		if (changeBright)
		{
			GET_STACK(unsigned int, bright, 0x118);
			R->Stack(0x118, GetBright(bright, PaintballState.Data.BrightMultiplier));
		}
	}
}

