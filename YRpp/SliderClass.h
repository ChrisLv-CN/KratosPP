#pragma once

#include <GaugeClass.h>

class NOVTABLE SliderClass : public GaugeClass
{
public:

	//Destructor
	virtual ~SliderClass() RX;

	//GadgetClass

	//ControlClass

	//GaugeClass

	//SliderClass
	virtual int Bump(bool bMinus) R0; // CurrentValue +=/-= Thumb
	virtual int Step(bool bMinus) R0; // CurrentValue +=/-= 1

	//Non virtual
	void RecalculateThumb() { JMP_THIS(0x6B1EE0); }

	//Statics

	//Constructors
	SliderClass(unsigned int nID, int nX, int nY, int nWidth, int nHeight, bool bBelongToList) noexcept
		: GaugeClass(noinit_t()) { JMP_THIS(0x6B1B20); }

	SliderClass(SliderClass& another) noexcept
		: GaugeClass(noinit_t()) { JMP_THIS(0x5581A0); }

	explicit __forceinline SliderClass(noinit_t)  noexcept // not protected for ListClass Constructor
		: GaugeClass(noinit_t())
	{
	}

	//Properties
public:

	GadgetClass* PlusGadget;
	GadgetClass* MinusGadget;
	bool BelongToList;
	int Thumb;
	int ThumbSize;
	int ThumbStart;
};