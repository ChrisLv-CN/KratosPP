#include "GiftBoxState.h"

#include <Ext/Helper/MathEx.h>

bool GiftBoxState::CanOpen()
{
	return IsActive() && !IsOpen && Timeup() && GetGiftData().Enable;
}


void GiftBoxState::ResetGiftBox()
{
	GiftBoxEntity data = GetGiftData();
	IsOpen = false;
	_delay = GetRandomValue(data.RandomDelay, data.Delay);
	if (_delay > 0)
	{
		_delayTimer.Start(_delay);
	}
}

std::vector<std::string> GiftBoxState::GetGiftList()
{
	GiftBoxEntity data = GetGiftData();
	std::vector<std::string> gifts{};
	if (data.Enable)
	{
		int giftCount = data.Gifts.size();
		int numsCount = data.Nums.size();
		if (data.RandomType)
		{
			// 随机类型，将所有的数量限制加起来，总和为礼物数量
			int times = 1;
			if (numsCount > 0)
			{
				times = 0;
				for (int num : data.Nums)
				{
					times += num;
				}
			}
			// 计算权重
			int maxValue = 0;
			std::map<Point2D, int> targetPad = MakeTargetPad(data.RandomWeights, giftCount, maxValue);
			// 算出随机值，确认位置，取得序号，选出单位
			for (int i = 0; i < times; i++)
			{
				int index = Hit(targetPad, maxValue);
				// 计算概率
				if (Bingo(data.Chances, index))
				{
					std::string id = data.Gifts[index];
					if (IsNotNone(id))
					{
						gifts.emplace_back(id);
					}
				}
			}
		}
		else
		{
			// 指定类型，遍历礼物类型，取指定数量个
			for (int index = 0; index < giftCount; index++)
			{
				std::string id = data.Gifts[index];
				if (IsNotNone(id))
				{
					int times = 1;
					if (numsCount > 0 && index < numsCount)
					{
						times = data.Nums[index];
					}
					// 取指定数量个
					for (int i = 0; i < times; i++)
					{
						// 计算概率
						if (Bingo(data.Chances, index))
						{
							gifts.push_back(id); // 存copy
						}
					}
				}
			}
		}
	}
	return gifts;
}

void GiftBoxState::OnStart()
{
	ResetGiftBox();
}

void GiftBoxState::OnUpdate()
{
#ifdef DEBUG
	StateScript<GiftBoxData>::OnUpdate();
#endif // DEBUG
	bool isElite = false;
	if (pTechno)
	{
		isElite = pTechno->Veterancy.IsElite();
	}
	else if (pBullet && pBullet->Owner)
	{
		isElite = pBullet->Owner->Veterancy.IsElite();
	}
	if (_isElite != isElite && IsActive() && _delayTimer.Expired())
	{
		ResetGiftBox();
	}
	_isElite = isElite;
}

