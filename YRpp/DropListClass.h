#pragma once

#include <EditClass.h>
#include <ListClass.h>

class NOVTABLE DropListClass : public EditClass
{
public:

	//Destructor
	virtual ~DropListClass() RX;

	//GadgetClass

	//ControlClass

	//EditClass

	//DropListClass
	virtual int AddItem(const wchar_t* lpStr) R0;
	virtual wchar_t* CurrentItem() R0;
	virtual int CurrentIndex() R0;
	virtual void SetSelectedItem(const wchar_t* lpStr) RX;
	virtual void SetSelectedIndex(int index) RX;
	virtual int GetCount() R0;
	virtual wchar_t* GetItem(int index) R0;

	//Non virtual

	//Statics

	//Constructors
	DropListClass(unsigned int nID, wchar_t* pText, int nMaxLength, TextPrintType eTextFlag, int nX, int nY, 
		int nWidth, int nHeight, EditFlag eEditFlag,int nSomeHeight, SHPStruct* UpSHP, SHPStruct* DownSHP) noexcept
		: DropListClass(noinit_t()) { JMP_THIS(0x4B4E10); }

protected:
	explicit __forceinline DropListClass(noinit_t)  noexcept
		: EditClass(noinit_t())
		, List(noinit_t())
	{
	}

	//Properties
public:

	bool bool_48;
	int SomeHeight;
	ShapeButtonClass SomeButton;
	ListClass List;
};