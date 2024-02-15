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
