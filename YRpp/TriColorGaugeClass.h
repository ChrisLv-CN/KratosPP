#pragma once

#include <GaugeClass.h>

class NOVTABLE TriColorGaugeClass : public GaugeClass
{
public:

	//Destructor
	virtual ~TriColorGaugeClass() RX;

	//GadgetClass

	//ControlClass

	//GaugeClass
	
	//TriColorGaugeClass
	virtual bool SetRedLimit(int value) R0;
	virtual bool SetYellowLimit(int value) R0;

	//Non virtual

	//Statics

	//Constructors
	TriColorGaugeClass(unsigned int nID, int nX, int nY, int nWidth, int nHeight) noexcept
		: GaugeClass(noinit_t()) { JMP_THIS(0x4E2A50); }

protected:
	explicit __forceinline TriColorGaugeClass(noinit_t)  noexcept
		: GaugeClass(noinit_t())
	{
	}

	//Properties
public:

	int RedLimit;
	int YellowLimit;
};