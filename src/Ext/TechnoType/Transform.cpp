#include "../TechnoStatus.h"

void TechnoStatus::InitState_Transform()
{
	pSourceType = pTechno->GetTechnoType();
	pTargetType = pSourceType;
}

void TechnoStatus::OnUpdate_Transform()
{
	if (!_transformLocked)
	{
		// 执行变形逻辑
		if (TransformState.IsActive())
		{
			if (!_hasBeenChanged || _changeToType != TransformState.Data.TransformToType)
			{
				_changeToType = TransformState.Data.TransformToType;
				TechnoTypeClass* pTargetType = nullptr;
				if (IsNotNone(_changeToType) && (pTargetType = TechnoTypeClass::Find(_changeToType.c_str())) != nullptr)
				{
					_hasBeenChanged = true;
					ChangeTechnoTypeTo(pTargetType);
				}
				else
				{
					TransformState.Disable();
				}
			}
		}
		else if (_hasBeenChanged)
		{
			// 还原
			_hasBeenChanged = false;
			ChangeTechnoTypeTo(pSourceType);
		}
		// 单位类型发生了改变，发出通知
		if (pTechno->GetTechnoType() != pTargetType)
		{
			pTargetType = pTechno->GetTechnoType();
			OnTransform();
		}
	}
}

void TechnoStatus::OnReceiveDamageDestroy_Transform()
{
	if (_hasBeenChanged)
	{
		// 死亡时强制还原
		// Logger.Log("强制还原类型 {0}, ConverTypeStatus = {1}", OwnerObject, ConvertTypeStatus);
		ChangeTechnoTypeTo(pSourceType);
		_hasBeenChanged = false;
		_transformLocked = true;
	}
}

void TechnoStatus::ChangeTechnoTypeTo(TechnoTypeClass* pNewType)
{
	switch (GetAbsType())
	{
	case AbstractType::Infantry:
		dynamic_cast<InfantryClass*>(pTechno)->Type = dynamic_cast<InfantryTypeClass*>(pNewType);
		break;
	case AbstractType::Unit:
		dynamic_cast<UnitClass*>(pTechno)->Type = dynamic_cast<UnitTypeClass*>(pNewType);
		break;
	case AbstractType::Aircraft:
		dynamic_cast<AircraftClass*>(pTechno)->Type = dynamic_cast<AircraftTypeClass*>(pNewType);
		break;
	}
}
