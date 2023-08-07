#pragma once

#include <ControlClass.h>
#include <GeneralStructures.h>

// Accept Alpha, Digit, and other characters, ToUpper will convert alpha into upper-case
enum class EditFlag : int
{
	Alpha = 0x1,
	Digit = 0x2,
	Other = 0x4,
	ToUpper = 0x8
};

class NOVTABLE EditClass : public ControlClass
{
public:

	//Destructor
	virtual ~EditClass() RX;

	//GadgetClass

	//ControlClass

	//EditClass
	virtual void SetText(wchar_t* lpStr, int nMaxLength) RX;
	virtual wchar_t* GetText() R0;
	virtual void DrawBackground() RX;
	virtual void DrawText(wchar_t* lpStr) RX;
	virtual bool HandleKeyInput(int nASCII) R0;

	//Non virtual

	//Statics

	//Constructors
	EditClass(int ID, wchar_t* Text, int nMaxLength, TextPrintType eTextFlag, int X, int Y, int Width, int Height, EditFlag eEditFlag) noexcept
		: ControlClass(noinit_t()) { JMP_THIS(0x4C2FC0); }

protected:
	explicit __forceinline EditClass(noinit_t)  noexcept
		: ControlClass(noinit_t())
	{
	}

	//Properties
public:

	TextPrintType TextFlags;
	EditFlag EditFlags; // 1 for alpha only; 2 for digit only; 4 for not num nor alpha accepted; 8 to convert alpha into upper case
	wchar_t* Text;
	int MaxLength;
	int Length;
	int Color;
	bool IsReadOnly;
};