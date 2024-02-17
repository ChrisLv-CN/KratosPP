#include "../TechnoStatus.h"

bool TechnoStatus::AmIStand()
{
	// 抛射体上的替身可能会因为抛射体的攻击者已经死亡而MyMaster为空
	return pMyMaster || MyMasterIsAnim || StandData.Enable;
}

bool TechnoStatus::OnSelect_VirtualUnit()
{
	if (pMyMaster && StandData.Enable && StandData.SelectToMaster)
	{
		pMyMaster->Select();
	}
	return !VirtualUnit;
}

bool TechnoStatus::OnSelect_Deselect()
{
	return true;
}
