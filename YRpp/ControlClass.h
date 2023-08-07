#pragma once

#include <GadgetClass.h>

class NOVTABLE ControlClass : public GadgetClass
{
public:

	//Destructor
	virtual ~ControlClass() RX;

	//GadgetClass
	virtual ControlClass* ExtractGadget(unsigned int nID) R0;
	virtual void PeerToPeer(unsigned int Flags, DWORD* pKey, ControlClass* pSendTo) RX;

	//ControlClass
	virtual void MakePeer(ControlClass const& target) RX;

	//Non virtual

	//Statics

	//Constructors
	ControlClass(unsigned int nID, int nX, int nY, int nWidth, int nHeight, GadgetFlag eFlag, bool bSticky) noexcept
		: ControlClass(noinit_t()) { JMP_THIS(0x48E520); }

	ControlClass(ControlClass& another) noexcept
		: ControlClass(noinit_t()) { JMP_THIS(0x48E570); }

protected:
	explicit __forceinline ControlClass(noinit_t)  noexcept
		: GadgetClass(noinit_t())
	{
	}

	//Properties
public:

	int ID;
	GadgetClass* SendTo; // Peer
};