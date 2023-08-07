#pragma once

#include <ControlClass.h>

class NOVTABLE GaugeClass : public ControlClass
{
public:

	//Destructor
	virtual ~GaugeClass() RX;

	//GadgetClass

	//ControlClass

	//GaugeClass
	virtual bool SetMaximum(int value) R0;
	virtual bool SetValue(int value) R0;
	virtual int GetValue() R0;
	virtual void SetThumb(bool value) RX; // Set HasThumb
	virtual int GetThumbPixel() R0; // return 4 if not overloaded
	virtual void DrawThumb() RX;
	virtual int PixelToValue(int pixel) R0;
	virtual int ValueToPixel(int value) R0;

	//Non virtual

	//Statics

	//Constructors
	GaugeClass(unsigned int nID, int nX, int nY, int nWidth, int nHeight) noexcept
		: ControlClass(noinit_t()) { JMP_THIS(0x4E2500); }

protected:
	explicit __forceinline GaugeClass(noinit_t)  noexcept
		: ControlClass(noinit_t())
	{
	}

	//Properties
public:

	bool IsColorized;
	bool HasThumb;
	bool IsHorizontal;
	int MaxValue;
	int CurrentValue;
	int ClickDiff;
};