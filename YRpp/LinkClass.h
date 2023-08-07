#pragma once

#include <GeneralDefinitions.h>
#include <YRPPCore.h>

class LinkClass
{
public:
	//Destructor
	virtual ~LinkClass() RX;

	//LinkClass
	virtual LinkClass* GetNext() R0;
	virtual LinkClass* GetPrev() R0;
	virtual LinkClass* Add(LinkClass& another) R0;
	virtual LinkClass* AddTail(LinkClass& another) R0;
	virtual LinkClass* AddHead(LinkClass& another) R0;
	virtual LinkClass* HeadOfList(LinkClass& another) R0;
	virtual LinkClass* TailOfList(LinkClass& another) R0;
	virtual void Zap() RX;
	virtual LinkClass* Remove() R0;

	//Non virtual
	LinkClass& operator=(LinkClass& another) { JMP_THIS(0x556600); }

	//Constructors
	LinkClass(LinkClass& another) { JMP_THIS(0x556550); }

protected:
	explicit __forceinline LinkClass(noinit_t)  noexcept
	{
	}

	//Properties
public:

	LinkClass* Next;
	LinkClass* Previous;
};
