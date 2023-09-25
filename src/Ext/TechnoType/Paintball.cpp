#include "../TechnoStatus.h"

void TechnoStatus::SetExtraSparkleAnim(AnimClass* pAnim)
{
	if (pExtraSparkleAnim)
	{
		pExtraSparkleAnim->RemainingIterations = 0;
		pExtraSparkleAnim = nullptr;
	}
	pExtraSparkleAnim = pAnim;
}

void TechnoStatus::OnUpdate_Paintball()
{
	// 修改建筑动画的染色状态
	if (IsBuilding() && !AmIStand())
	{
		// 检查状态有所变化，则重新渲染
		if (pTechno->Berzerk)
		{
			if (!_buildingWasBerzerk)
			{
				_buildingWasBerzerk = true;
				pTechno->NeedsRedraw = true;
			}
		}
		else
		{
			if (_buildingWasBerzerk)
			{
				_buildingWasBerzerk = false;
				pTechno->NeedsRedraw = true;
			}
		}

		if (pTechno->IsUnderEMP())
		{
			if (!_buildingWasEMP)
			{
				_buildingWasEMP = true;
				pTechno->NeedsRedraw = true;
			}
		}
		else
		{
			if (_buildingWasEMP)
			{
				_buildingWasEMP = false;
				pTechno->NeedsRedraw = true;
			}
		}
		/*
		if (PaintballState.IsActive())
		{
			if (!_buildingWasColor || PaintballState.IsReset())
			{
				_buildingWasColor = true;
				pTechno->NeedsRedraw = true;
			}
		}
		else
		{
			if (_buildingWasColor)
			{
				_buildingWasColor = false;
				pTechno->NeedsRedraw = true;
			}
		}*/
	}

	// 移除额外的EMP动画
	if (pExtraSparkleAnim && !pTechno->IsUnderEMP())
	{
		SetExtraSparkleAnim(nullptr);
	}
}
