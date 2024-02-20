#include "StackEffect.h"

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

bool StackEffect::CanActive(int stacks)
{
	int level = Data->Level;
	if (level >= 0)
	{
		switch (Data->Condition)
		{
		case Condition::EQ:
			return stacks == level;
		case Condition::NE:
			return stacks != level;
		case Condition::GT:
			return stacks > level;
		case Condition::LT:
			return stacks < level;
		case Condition::GE:
			return stacks >= level;
		case Condition::LE:
			return stacks <= level;
		}
	}
	return true;
}

void StackEffect::Watch()
{
	std::string watch = Data->Watch;
	int stacks = -1;
	std::map<std::string, int> aeStacks = AE->AEManager->AEStacks;
	auto it = aeStacks.find(watch);
	if (it != aeStacks.end())
	{
		stacks = it->second;
		if (stacks > 0 && CanActive(stacks))
		{
			_count++;
			// 添加AE
			if (Data->Attach)
			{
				AE->AEManager->Attach(Data->AttachEffects, Data->AttachChances, false, AE->pSource, AE->pSourceHouse);
			}
			// 移除AE
			if (Data->Remove)
			{
				AE->AEManager->DetachByName(Data->RemoveEffects);
			}
			// 移除被监视者
			if (Data->RemoveAll)
			{
				AE->AEManager->DetachByName({ watch });
			}
		}
	}
	// 检查触发次数
	if (Data->TriggeredTimes > 0 && _count >= Data->TriggeredTimes)
	{
		End(CoordStruct::Empty);
	}
}

void StackEffect::OnUpdate()
{
	if (!AE->OwnerIsDead())
	{
		Watch();
	}
}

void StackEffect::OnWarpUpdate()
{
	if (!AE->OwnerIsDead())
	{
		Watch();
	}
}
