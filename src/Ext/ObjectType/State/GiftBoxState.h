#pragma once

#include "State.h"
#include "GiftBoxData.h"

#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/StringEx.h>

class GiftBoxState : public State<GiftBoxData>
{
public:
	bool IsOpen = false;

	// 记录盒子的状态
	bool IsSelected = false;
	DirStruct BodyDir{};
	DirStruct TurretDir{};

	int Group = -1;

	virtual void OnEnable() override
	{
		ResetGiftBox();
	}

	void Update(bool isElite)
	{
		if (_isElite != isElite && IsActive() && _delayTimer.Expired())
		{
			ResetGiftBox();
		}
		_isElite = isElite;
	}

	bool CanOpen()
	{
		return IsActive() && !IsOpen && Timeup() && GetGiftData().Enable;
	}

	void ResetGiftBox()
	{
		GiftBoxEntity data = GetGiftData();
		IsOpen = false;
		_delay = GetRandomValue(data.RandomDelay, data.Delay);
		if (_delay > 0)
		{
			_delayTimer.Start(_delay);
		}
	}

	std::vector<std::string> GetGiftList()
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
#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->IsOpen)
			.Process(this->IsSelected)
			.Process(this->BodyDir)
			.Process(this->TurretDir)
			.Process(this->Group)

			.Process(this->_isElite)
			.Process(this->_delay)
			.Process(this->_delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		State<GiftBoxData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		State<GiftBoxData>::Save(stream);
		return const_cast<GiftBoxState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	GiftBoxEntity GetGiftData()
	{
		if (_isElite && Data.EliteData.Enable)
		{
			return Data.EliteData;
		}
		return Data.Data;
	}

	bool Timeup()
	{
		return _delay <= 0 || _delayTimer.Expired();
	}


	bool _isElite = false;
	int _delay = 0;
	CDTimerClass _delayTimer{};
};
