#include "../TechnoStatus.h"

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
