#pragma once

#include <GeneralDefinitions.h>

#include <Helpers/CompileTime.h>

struct LineTrailNode
{
	CoordStruct Position;
	DWORD unknown;
};

class LineTrail
{
public:
	static constexpr constant_ptr<DynamicVectorClass<LineTrail*>, 0xABCB78u> const Array{};

	//Constructor, Destructor
	LineTrail()
		{ JMP_THIS(0x556A20); }

	~LineTrail()
		{ JMP_THIS(0x556B30); }

	void SetDecrement(int val)
		{ JMP_THIS(0x556B50); }

	static void DeleteAll()
		{ JMP_STD(0x556DF0); }


	//===========================================================================
	//===== Properties ==========================================================
	//===========================================================================

public:

	ColorStruct Color;
	ObjectClass* Owner;
	DWORD Decrement;
	DWORD ActiveSlot;
	LineTrailNode Trails [32];
};
