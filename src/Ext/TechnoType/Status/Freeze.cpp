#include "../TechnoStatus.h"

#include <Ext/Helper/Status.h>

DestroyAnimData* TechnoStatus::GetDestroyAnimData()
{
	if (!_destroyAnimData)
	{
		_destroyAnimData = INI::GetConfig<DestroyAnimData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _destroyAnimData;
}

void TechnoStatus::OnUpdate_Freeze()
{
	Freezing = Freeze->IsAlive() || GetDestroyAnimData()->Wreck; // 我是残骸
	if (Freezing)
	{
		if (!_cantMoveFlag)
		{
			// 清除所有目标
			ClearAllTarget(pTechno);
			// 马上停止活动
			if (FootClass* pFoot = dynamic_cast<FootClass*>(pTechno))
			{
				ForceStopMoving(pFoot);
			}
		}
	}
	else
	{
		_cantMoveFlag = false;
	}
}
