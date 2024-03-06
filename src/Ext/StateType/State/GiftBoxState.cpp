#include "GiftBoxState.h"

#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Gift.h>

bool GiftBoxState::CanOpen()
{
	return IsAlive() && !IsOpen && Timeup() && GetGiftData().Enable;
}


void GiftBoxState::ResetGiftBox()
{
	GiftBoxEntity data = GetGiftData();
	IsOpen = false;
	_delay = GetRandomValue(data.RandomDelay, data.Delay);
	if (_delay > 0)
	{
		_delayTimer.Start(_delay);
	}
}

void GiftBoxState::OnStart()
{
	ResetGiftBox();
}

void GiftBoxState::OnUpdate()
{
#ifdef DEBUG
	StateScript<GiftBoxData>::OnUpdate();
#endif // DEBUG
	bool isElite = false;
	if (pTechno)
	{
		isElite = pTechno->Veterancy.IsElite();
	}
	else if (pBullet && pBullet->Owner)
	{
		isElite = pBullet->Owner->Veterancy.IsElite();
	}
	if (_isElite != isElite && IsAlive() && _delayTimer.Expired())
	{
		ResetGiftBox();
	}
	_isElite = isElite;
}

