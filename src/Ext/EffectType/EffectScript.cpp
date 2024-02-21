#include "EffectScript.h"

#include <Ext/Helper/Status.h>

AttachEffectScript* EffectScript::GetAE()
{
	if (!_ae)
	{
		_ae = dynamic_cast<AttachEffectScript*>(_parent);
	}
	return _ae;
}

void EffectScript::Start()
{
	_started = true;
	OnStart();
}

void EffectScript::Pause()
{
	_pause = true;
	OnPause();
}

void EffectScript::Recover()
{
	if (!_started)
	{
		Start();
	}
	else
	{
		OnRecover();
		_pause = false;
	}
}
