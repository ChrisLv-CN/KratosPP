#include "Mark.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

EFFECT_SCRIPT_CPP(Mark);

void Mark::Start()
{
	if (Data.Enable)
	{
		std::string marks;
		std::vector<std::string> m = Data.Names;
		for (auto it = m.begin(); it != m.end();)
		{
			std::string mark = *it;
			marks.append(mark);
			if (++it != m.end())
			{
				marks.append(",");
			}
		}
		this->Tag = marks;
	}
}
