#include "../BulletStatus.h"

void BulletStatus::DrawVXL_Paintball(REGISTERS* R)
{
	bool changeColor = false;
	bool changeBright = false;
	// pBullet.Ref.ForceShielded 在护盾启用一次后，一直是1，所以不能用来判断是否正处于护盾状态，只能通过判断pBullet.Ref.Base.IsIronCurtained()来判断处于无敌状态
	if (Paintball->NeedPaint(changeColor, changeBright))
	{
		if (changeColor)
		{
			R->Stack(0x0, Paintball->Data.Color2);
		}
		if (changeBright)
		{
			GET_STACK(unsigned int, bright, 0x118);
			R->Stack(0x118, GetBright(bright, Paintball->Data.BrightMultiplier));
		}
	}
}

