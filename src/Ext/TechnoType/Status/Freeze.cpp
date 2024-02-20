#include "../TechnoStatus.h"

#include <Ext/Helper/Status.h>

void TechnoStatus::OnUpdate_Freeze()
{
	Freezing = Freeze->IsActive();
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
