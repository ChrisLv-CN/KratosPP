#pragma once

#include <CellClass.h>
#include <GeneralStructures.h>
#include <MapClass.h>

static bool InRect(Point2D point, RectangleStruct bound)
{
	return point.X >= bound.X && point.X <= bound.X + bound.Width && point.Y >= bound.Y && point.Y <= bound.Y + bound.Height;
}

static bool InFog(CoordStruct location)
{
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		if (pCell->Flags & CellFlags::Revealed)
		{
			return false;
		}
	}
	return true;
}

