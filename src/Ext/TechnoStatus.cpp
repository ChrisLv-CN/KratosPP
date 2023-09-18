#include "TechnoStatus.h"

void TechnoStatus::OnUpdate()
{
	if (!IsDead(pTechno))
	{
		Mission mission = pTechno->CurrentMission;
		switch (pTechno->CurrentMission)
		{
		case Mission::Move:
		case Mission::AttackMove:
			// 上一次任务不是这两个说明是起步
			if (Mission::Move != _lastMission && Mission::AttackMove != _lastMission)
			{
				drivingState = DrivingState::Start;
			}
			else
			{
				drivingState = DrivingState::Moving;
			}
			break;
		default:
			// 上一次任务如果是Move或者AttackMove说明是刹车
			if (Mission::Move == _lastMission || Mission::AttackMove == _lastMission)
			{
				drivingState = DrivingState::Stop;
			}
			else
			{
				drivingState = DrivingState::Stand;
			}
			break;
		}
	}
}

void TechnoStatus::OnUpdateEnd()
{
	if (!IsDeadOrInvisible(pTechno))
	{
		this->_lastMission = pTechno->CurrentMission;
	}
}
