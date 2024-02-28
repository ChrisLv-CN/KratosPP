#include "VampireEffect.h"

void VampireEffect::Trigger()
{
	// 检查触发次数
	if (Data->TriggeredTimes > 0 && ++_count >= Data->TriggeredTimes)
	{
		Deactivate();
		AE->TimeToDie();
	}
}


