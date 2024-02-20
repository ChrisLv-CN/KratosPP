﻿#include "../TechnoStatus.h"

#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/Scripts.h>

void TechnoStatus::OnUpdate_Deselect()
{
	if (_disableSelectable = Deselect->IsActive())
	{
		Disappear = Deselect->Data.Disappear;
	}
	else
	{
		Disappear = false;
	}
	if (pTechno->IsSelected && _disableSelectable)
	{
		pTechno->Deselect();
	}
}

bool TechnoStatus::OnSelect_Deselect()
{
	return !_disableSelectable;
}
