#include "DestroySelfState.h"


bool DestroySelfState::AmIDead()
{
	return IsActive() && (GoDie || Timeup());
}

void DestroySelfState::DestroyNow(bool peaceful)
{
	// 强制开启状态机
	DestroySelfData data;
	data.Enable = true;
	data.Delay = 0;
	data.Peaceful = peaceful;
	Start(&data);
	// 狗带
	GoDie = true;
}

void DestroySelfState::ExtendLife()
{
	GoDie = false;
	_delay = Data.Delay;
	_delayTimer.Stop();
	if (_delay > 0)
	{
		_delayTimer.Start(_delay);
	}
}

bool DestroySelfState::Timeup()
{
	GoDie = _delay <= 0 || _delayTimer.Expired();
	return GoDie;
}

void DestroySelfState::OnStart()
{
	ExtendLife();
}
