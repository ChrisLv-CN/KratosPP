#include "../TechnoStatus.h"

#include <Ext/Common/CommonStatus.h>

unsigned int TechnoStatus::GetLaserTargetColor2()
{
	if (_laserTargetColor2 == 0)
	{
		ColorStruct color = RulesClass::Instance->ColorAdd[RulesClass::Instance->LaserTargetColor];
		_laserTargetColor2 = Add2RGB565(color);
	}
	return _laserTargetColor2;
}

unsigned int TechnoStatus::GetBerserkColor2()
{
	if (_berserkColor2 == 0)
	{
		ColorStruct color = RulesClass::Instance->ColorAdd[RulesClass::Instance->BerserkColor];
		_berserkColor2 = Add2RGB565(color);
	}
	return _berserkColor2;
}

AirstrikeData* TechnoStatus::GetAirstrikeData(TechnoClass* pOwner)
{
	return INI::GetConfig<AirstrikeData>(INI::Rules, pOwner->GetTechnoType()->ID)->Data;;
}

void TechnoStatus::SetExtraSparkleAnim(AnimClass* pAnim)
{
	if (pExtraSparkleAnim)
	{
		pExtraSparkleAnim->RemainingIterations = 0;
		pExtraSparkleAnim = nullptr;
	}
	pExtraSparkleAnim = pAnim;
}

void TechnoStatus::DrawSHP_Paintball(REGISTERS* R)
{
	bool changeColor = false;
	bool changeBright = false;
	if (Paintball->NeedPaint(changeColor, changeBright) && !pTechno->Berzerk && !pTechno->IsIronCurtained())
	{
		if (changeColor)
		{
			R->EAX(Paintball->Data.Color2);
		}
		if (changeBright)
		{
			GET(unsigned int, bright, EBP);
			R->EBP(GetBright(bright, Paintball->Data.BrightMultiplier));
		}
	}
}

/**
 *@brief 建筑动画调用建筑的染色设置染色建筑动画自身
 *
 * @param R
 */
void TechnoStatus::DrawSHP_Paintball_BuildingAnim(REGISTERS* R)
{
	GET_STACK(unsigned int, bright, 0x38);
	if (Airstrike && Airstrike->Target == pTechno)
	{
		if (GetAirstrikeData(Airstrike->Owner)->AirstrikeDisableColor)
		{
			R->EBP(0);
		}
		else
		{
			R->EBP(GetLaserTargetColor2());
		}
	}
	if (pTechno->Berzerk)
	{
		R->EBP(GetBerserkColor2());
	}
	if (pTechno->IsUnderEMP())
	{
		R->Stack(0x38, GetBright(bright, _deactivateDimEMP));
	}

	bool changeColor = false;
	bool changeBright = false;
	// pTechno.Ref.ForceShielded 在护盾启用一次后，一直是1，所以不能用来判断是否正处于护盾状态，只能通过判断pTechno.Ref.Base.IsIronCurtained()来判断处于无敌状态
	if (Paintball->NeedPaint(changeColor, changeBright) && !pTechno->Berzerk && !pTechno->IsIronCurtained())
	{
		if (changeColor)
		{
			R->EBP(Paintball->Data.Color2);
		}
		if (changeBright)
		{
			R->Stack(0x38, GetBright(bright, Paintball->Data.BrightMultiplier));
		}
	}
}

/**
 *@brief SHP载具、建筑染狂暴色，EMP变黑
 *
 * @param R
 */
void TechnoStatus::DrawSHP_Colour(REGISTERS* R)
{
	if (!IsDead(pTechno) && !pTechno->IsVoxel())
	{
		//== Colour ==
		// ForceShield
		// LaserTarget
		if (Airstrike && Airstrike->Target == pTechno)
		{
			if (GetAirstrikeData(Airstrike->Owner)->AirstrikeDisableColor)
			{
				R->EAX(0);
			}
			else
			{
				R->EAX(GetLaserTargetColor2());
			}
		}
		// Berzerk, 建筑不支持狂暴染色
		if (pTechno->Berzerk) // paint color to building
		{
			R->EAX(GetBerserkColor2());
		}

		//== Darker ==
		// IronCurtain, SHP载具支持铁幕染色
		// EMP, 建筑不支持变黑
		// NoPower
		if (pTechno->IsUnderEMP())
		{
			GET(unsigned int, bright, EBP);
			R->EBP(GetBright(bright, _deactivateDimEMP));
		}
		else if (IsUnit() && pTechno->IsActive() && pTechno->GetTechnoType()->PoweredUnit)
		{
			GET(unsigned int, bright, EBP);
			R->EBP(GetBright(bright, _deactivateDimPowered));
		}
	}
}

void TechnoStatus::DrawVXL_Paintball(REGISTERS* R, bool isBuilding)
{
	if (isBuilding)
	{
		// Vxl turret
		GET_STACK(unsigned int, bright, 0x20);
		if (Airstrike && Airstrike->Target == pTechno)
		{
			if (GetAirstrikeData(Airstrike->Owner)->AirstrikeDisableColor)
			{
				R->Stack(0x24, 0);
			}
			else
			{
				R->Stack(0x24, GetLaserTargetColor2());
			}
		}
		if (pTechno->Berzerk) // paint color to building's anim
		{
			R->Stack(0x24, GetBerserkColor2());
		}
		if (pTechno->IsUnderEMP())
		{
			R->Stack(0x20, GetBright(bright, _deactivateDimEMP));
		}
	}
	else
	{
		if (Airstrike && Airstrike->Target == pTechno)
		{
			if (GetAirstrikeData(Airstrike->Owner)->AirstrikeDisableColor)
			{
				R->ESI(0);
			}
			else
			{
				R->ESI(GetLaserTargetColor2());
			}
		}
	}


	bool changeColor = false;
	bool changeBright = false;
	// pTechno.Ref.ForceShielded 在护盾启用一次后，一直是1，所以不能用来判断是否正处于护盾状态，只能通过判断pTechno.Ref.Base.IsIronCurtained()来判断处于无敌状态
	if (Paintball->NeedPaint(changeColor, changeBright) && !pTechno->Berzerk && !pTechno->IsIronCurtained())
	{
		if (changeColor)
		{
			if (isBuilding)
			{
				// Vxl turret
				R->Stack(0x24, Paintball->Data.Color2);
			}
			else
			{
				R->ESI(Paintball->Data.Color2);
			}
		}
		if (changeBright)
		{
			if (isBuilding)
			{
				// Vxl turret
				GET_STACK(unsigned int, bright, 0x20);
				R->Stack(0x20, GetBright(bright, Paintball->Data.BrightMultiplier));
			}
			else
			{
				GET_STACK(unsigned int, bright, 0x1E0);
				R->Stack(0x1E0, GetBright(bright, Paintball->Data.BrightMultiplier));
			}
		}
	}
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

		if (Paintball->IsAlive())
		{
			if (!_buildingWasColor || Paintball->IfReset())
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
		}
	}

	// 移除额外的EMP动画
	if (pExtraSparkleAnim && !pTechno->IsUnderEMP())
	{
		SetExtraSparkleAnim(nullptr);
	}
}
