#pragma once

#include <LinkClass.h>

enum class KeyModifier : int
{
	None = 0,
	Shift = 1,
	Ctrl = 2,
	Alt = 4
};

enum class GadgetFlag : int
{
	LeftPress = 0x1,
	LeftHeld = 0x2,
	LeftRelease = 0x4,
	LeftUp = 0x8,
	RightPress = 0x10,
	RightHeld = 0x20,
	RightRelease = 0x40,
	RightUp = 0x80,
	Keyboard = 0x100
};

class NOVTABLE GadgetClass : public LinkClass
{
public:

	//Destructor
	virtual ~GadgetClass() RX;

	//LinkClass
	virtual GadgetClass* GetNext() R0;
	virtual GadgetClass* GetPrev() R0;
	virtual void Zap() RX;
	virtual GadgetClass* Remove() R0;

	//GadgetClass
	virtual DWORD Input() R0;
	virtual void DrawAll(bool bForced) RX;
	virtual void DeleteList() RX;
	virtual GadgetClass* ExtractGadget(unsigned int nID) R0;
	virtual void MarkListToRedraw() RX;
	virtual void Disable() RX;
	virtual void Enable() RX;
	virtual unsigned int const GetID() R0;
	virtual void MarkRedraw() RX;
	virtual void PeerToPeer(unsigned int Flags, DWORD* pKey, GadgetClass* pSendTo) RX;
	virtual void SetFocus() RX;
	virtual void KillFocus() RX;
	virtual bool IsFocused() R0;
	virtual bool IsListToRedraw() R0;
	virtual bool IsToRedraw() R0;
	virtual void SetPosition(int X, int Y) RX;
	virtual void SetDimension(int Width, int Height) RX;
	virtual bool Draw(bool bForced) R0;
	virtual void OnMouseEnter() RX;
	virtual void OnMouseLeave() RX;
	virtual void StickyProcess(GadgetFlag Flags) RX;
	virtual bool Action(GadgetFlag Flags, DWORD* pKey, KeyModifier Modifier) R0;
	virtual bool Clicked(DWORD* pKey, GadgetFlag Flags, int X, int Y, KeyModifier Modifier) R0; // Clicked On

	//Non virtual
	GadgetClass& operator=(GadgetClass& const another) { JMP_THIS(0x4B5780); }
	GadgetClass* ExtractGadgetAt(int X, int Y) { JMP_THIS(0x4E15A0); }
	
	//Statics
	static int __fastcall GetColorScheme() { JMP_STD(0x4E12D0); }

	//Constructors
	GadgetClass(int nX,int nY,int nWidth,int nHeight,GadgetFlag eFlag, bool bSticky) noexcept
		: GadgetClass(noinit_t()) { JMP_THIS(0x4E12F0); }

	GadgetClass(GadgetClass& another) noexcept 
		: GadgetClass(noinit_t()) { JMP_THIS(0x4E1340); }

protected:
	explicit __forceinline GadgetClass(noinit_t)  noexcept
		: LinkClass(noinit_t())
	{
	}

	//Properties
public:

	int X;
	int Y;
	int Width;
	int Height;
	bool NeedsRedraw;
	bool IsSticky;
	bool Disabled;
	GadgetFlag Flags;
};