#include "Finder.h"
#include "MathEx.h"
#include "Status.h"

#include <string>
#include <vector>

#include <Utilities/Debug.h>

TechnoClass* FindRandomTechno(HouseClass* pHouse)
{
	// 遍历所有单位，组成这个所属的单位清单
	DynamicVectorClass<TechnoClass*>* pArray = TechnoClass::Array.get();
	std::vector<TechnoClass*> pTechnoArray{};
	for (auto it = pArray->begin(); it != pArray->end(); it++)
	{
		TechnoClass* pTechno = *it;
		if (pTechno && pTechno->Owner && pTechno->Owner == pHouse)
		{
			pTechnoArray.push_back(pTechno);
		}
	}
	// 抽取一个幸运儿
	int size = pTechnoArray.size();
	if (size > 0)
	{
		int targetIdx = GetRandom().RandomRanged(0, size - 1); // 同一帧的随机值永远都是同一个，容易造成死循环
		bool forward = true;
		for (int i = 0; i < size; i++)
		{
			TechnoClass* pTarget = pTechnoArray[targetIdx];
			if (!IsDeadOrInvisible(pTarget))
			{
#ifdef DEBUG
				Debug::Log("Pickup a Luckey Techno [%s]%d\n", pTarget->GetType()->ID, pTarget);
#endif // DEBUG
				return pTarget;
			}
			if (targetIdx >= size - 1)
			{
				targetIdx = size - 1;
				forward = false;
			}
			else if (targetIdx <= 0)
			{
				targetIdx = 0;
				forward = true;
			}
			if (forward)
			{
				targetIdx++;
			}
			else
			{
				targetIdx--;
			}
		}
	}
	return nullptr;
}
