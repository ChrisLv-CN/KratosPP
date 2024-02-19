#include "AntiBulletState.h"

bool AntiBulletState::CanSearchBullet()
{
	bool can = _delayTimer.Expired();
	if (can)
	{
		_delayTimer.Start(Data.Rate);
	}
	return can;
}
