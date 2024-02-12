#include "EffectScript.h"

#include <Ext/Helper/Status.h>

OBJECT_SCRIPT_CPP(EffectScript);


AttachEffectScript* EffectScript::GetAE()
{
	if (!_ae)
	{
		_ae = dynamic_cast<AttachEffectScript*>(_parent);
	}
	return _ae;
}
