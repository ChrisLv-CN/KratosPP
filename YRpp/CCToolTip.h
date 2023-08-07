#pragma once

#include <ToolTipManager.h>

#include <Helpers/CompileTime.h>

class NOVTABLE CCToolTip : public ToolTipManager
{
public:
	// It's also used in MoneyFormat at 6A934A, not sure what side effect it might leads
	static constexpr reference<bool, 0x884B8C> HideName {};
	static constexpr reference<CCToolTip*, 0x887368> Instance {};

	//Properties
public:
	bool FullRedraw;
	int Delay;
};