#include "../BulletStatus.h"

void BulletStatus::DrawVXL_Paintball(REGISTERS* R)
{
	// pBullet.Ref.ForceShielded 在护盾启用一次后，一直是1，所以不能用来判断是否正处于护盾状态，只能通过判断pBullet.Ref.Base.IsIronCurtained()来判断处于无敌状态
	if (MyPaintData.ChangeColor)
	{
		R->Stack(0x0, MyPaintData.Data.Color2);
	}
	if (MyPaintData.ChangeBright)
	{
		GET_STACK(unsigned int, bright, 0x118);
		R->Stack(0x118, GetBright(bright, MyPaintData.Data.BrightMultiplier));
	}
}


void BulletStatus::OnUpdate_Paintball()
{
	if (Paintball->IsAlive())
	{
		PaintballData data = Paintball->Data;
		MyPaintData.ChangeColor = data.ChangeColor;
		MyPaintData.ChangeBright = data.ChangeBright;
		MyPaintData.Data = data;
	}
	else
	{
		MyPaintData.Reset();
	}
}
