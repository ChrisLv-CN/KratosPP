#include "Scripts.h"

#include <Extension/BulletExt.h>
#include <Extension/TechnoExt.h>

#include <Ext/StateType/State/BlackHoleState.h>


BlackHoleState* GetBlackHoleState(ObjectClass* pObject)
{
	BlackHoleState* state = nullptr;
	TryGetBlackHoleState(pObject, state);
	return state;
}

bool TryGetBlackHoleState(ObjectClass* pObject, BlackHoleState*& state)
{
	switch (pObject->WhatAmI())
	{
	case AbstractType::Building:
	case AbstractType::Infantry:
	case AbstractType::Unit:
	case AbstractType::Aircraft:
	{
		state = GetScript<TechnoExt, BlackHoleState>(dynamic_cast<TechnoClass*>(pObject));
		break;
	}
	case AbstractType::Bullet:
	{
		state = GetScript<BulletExt, BlackHoleState>(dynamic_cast<BulletClass*>(pObject));
		break;
	}
	default:
		state = nullptr;
		break;
	}
	return state != nullptr;
}
