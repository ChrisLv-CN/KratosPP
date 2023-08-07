#pragma once

#include <ControlClass.h>
#include <ArrayClasses.h>
#include <ShapeButtonClass.h>
#include <SliderClass.h>

class NOVTABLE ListClass : public ControlClass
{
public:

	//Destructor
	virtual ~ListClass() RX;

	//GadgetClass

	//ControlClass

	//ListClass
	virtual int AddItem(const char* lpStr) R0;
	virtual bool EnableScrollBar() R0;
	virtual bool Bump(bool bMinus) R0;
	virtual const int GetCount() R0;
	virtual const int GetCurrentIndex() R0;
	virtual const char* GetCurrentItem() R0;
	virtual const char* GetItem(int index) R0;
	virtual int StepSelectedIndex(int step) R0; // huh?
	virtual void RemoveItem(const char* lpStr) RX;
	virtual void RemoveItemAt(int index) RX;
	virtual bool DisableScrollBar() R0;
	virtual void SetSelectedIndex(int index) RX;
	virtual void SetSelectedItem(const char* lpStr) RX;
	virtual void SetTabs(void* pTabs) RX;
	virtual bool SetViewIndex(int index) R0;
	virtual bool Step(bool bMinus) R0;
	virtual void DrawEntry(int index, int nX, int nY, int nWidth, bool bUnk) RX; // bUnk : Maybe fill background?

	//Non virtual

	//Statics

	//Constructors
	ListClass(unsigned int nID, int nX, int nY, int nWidth, int nHeight, TextPrintType eFlag, SHPStruct* UpSHP, SHPStruct* DownSHP) noexcept
		: ListClass(noinit_t()) { JMP_THIS(0x557230); }

	explicit __forceinline ListClass(noinit_t)  noexcept
		: ControlClass(noinit_t())
		, Scroller(noinit_t())
	{
	}

	//Properties
public:

	TextPrintType TextFlags;
	void* Tabs; // Not sure what it is
	DynamicVectorClass<const char*> List;
	int LineHeight;
	int LineCount;
	bool IsScrollActive;
	ShapeButtonClass UpButton;
	ShapeButtonClass DownButton;
	SliderClass Scroller;
	int SelectedIndex;
	int CurrentTopIndex;
};