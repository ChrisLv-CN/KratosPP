#include "ExpandAnimsManager.h"

#include <Ext/AnimType/AnimStatus.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Status.h>
#include <Ext/Helper/StringEx.h>

void ExpandAnimsManager::PlayExpandAnims(ExpandAnimsData data, CoordStruct location, HouseClass* pHouse)
{
	if (!data.Anims.empty())
	{
		std::vector<std::string> anims{};
		int animCount = data.Anims.size();
		int numsCount = data.Nums.size();
		if (data.RandomType)
		{
			int times = 1;
			if (numsCount > 0)
			{
				times = 0;
				for (int num : data.Nums)
				{
					times += num;
				}
			}
			// 获取权重标靶
			int maxValue = 0;
			std::map<Point2D, int> targetPad = MakeTargetPad(data.RandomWeights, animCount, maxValue);
			// 算出随机值，确认位置，取得序号，选出单位
			for (int i = 0; i < times; i++)
			{
				// 选出类型的序号
				int index = Hit(targetPad, maxValue);
				// 计算概率
				if (Bingo(data.Chances, index))
				{
					anims.push_back(data.Anims[index]);
				}
			}
		}
		else
		{
			// 不随机
			for (int index = 0; index < animCount; index++)
			{
				std::string id = data.Anims[index];
				int times = 1;
				if (numsCount > 0 && index < numsCount)
				{
					times = data.Nums[index];
				}
				for (int i = 0; i < times; i++)
				{
					// 计算概率
					if (Bingo(data.Chances, index))
					{
						anims.push_back(id);
					}
				}
			}
		}
		// 开始召唤
		for (std::string animType : anims)
		{
			if (IsNotNone(animType))
			{
				AnimTypeClass* pAnimType = AnimTypeClass::Find(animType.c_str());
				if (pAnimType)
				{
					// 位置偏移
					CoordStruct offset = data.GetOffset();
					AnimClass* pNewAnim = GameCreate<AnimClass>(pAnimType, location + offset);
					pNewAnim->Owner = pHouse;
				}
			}
		}
	}
}

struct VoxelAnimTypeCache
{
public:
	VoxelAnimTypeClass* pType;
	int count;
};

void ExpandAnimsManager::PlayExpandDebirs(DynamicVectorClass<VoxelAnimTypeClass*> types, DynamicVectorClass<int> nums, int times, CoordStruct location, HouseClass* pHouse, TechnoClass* pCreater)
{
	int numsCount = nums.Count;
	int max = 0;
	std::vector<VoxelAnimTypeCache> debrisTypeCaches{};
	for (int i = 0; i < types.Count; i++)
	{
		if (i < numsCount)
		{
			int num = nums[i];
			max += num;
			VoxelAnimTypeCache cache{ types[i], num };
			debrisTypeCaches.push_back(cache);
		}
		else
		{
			break;
		}
	}
	// 刷碎片
	if (max <= times)
	{
		// 刷出所有的vxl碎片
		for (auto cache : debrisTypeCaches)
		{
			VoxelAnimTypeClass* pAnimType = cache.pType;
			for (int i = 0; i < cache.count; i++)
			{
				GameCreate<VoxelAnimClass>(pAnimType, &location, pHouse);
			}
		}
		// 剩余的从shp碎片中随机
		int lastTimes = times - max;
		if (lastTimes > 0)
		{
			DynamicVectorClass<AnimTypeClass*> debirs = RulesClass::Instance->MetallicDebris;
			int count = debirs.Count;
			if (count > 0)
			{
				for (int i = 0; i < lastTimes; i++)
				{
					int index = Random::RandomRanged(0, count - 1);
					AnimTypeClass* pAnimType = debirs[index];
					AnimClass* pNewAnim = GameCreate<AnimClass>(pAnimType, location);
					pNewAnim->Owner = pHouse;
					SetAnimCreater(pNewAnim, pCreater);
				}
			}
		}
	}
	else
	{
		// 按照权重随机
		std::map<int, int> marks{};
		std::vector<int> tempWeight{};
		for (auto cache : debrisTypeCaches)
		{
			tempWeight.push_back(cache.count);
		}
		// 权重标靶
		int maxValue = 0;
		std::map<Point2D, int> targetPad = MakeTargetPad(tempWeight, debrisTypeCaches.size(), maxValue);
		for (int i = 0; i < times; i++)
		{
			bool spawn = false;
			VoxelAnimTypeClass* pAnimType = nullptr;
			int index = Hit(targetPad, maxValue);
			auto it = marks.find(index);
			if (it == marks.end())
			{
				spawn = true;
				marks[index] = 1;
				pAnimType = debrisTypeCaches[index].pType;
			}
			else
			{
				int count = marks[index];
				auto cache = debrisTypeCaches[index];
				if (count < cache.count)
				{
					spawn = true;
					marks[index]++;
					pAnimType = cache.pType;
				}
				else
				{
					i--;
				}
			}
			if (spawn)
			{
				GameCreate<VoxelAnimClass>(pAnimType, &location, pHouse);
			}
		}
	}
}
