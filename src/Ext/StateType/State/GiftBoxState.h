#pragma once

#include <GeneralDefinitions.h>
#include <HouseClass.h>

#include "../StateScript.h"
#include "GiftBoxData.h"

#include <Ext/TechnoType/DamageText.h>

class GiftBoxState : public StateScript<GiftBoxData>
{
public:
	STATE_SCRIPT(GiftBox);

	bool CanOpen();

	/**
	 *@brief 每次开盒后重制状态，记录次数和开启冷却时间
	 *
	 */
	void ResetGiftBox();

	/**
	 *@brief 获取礼物清单
	 *
	 * @return std::vector<std::string>
	 */
	std::vector<std::string> GetGiftList();

	virtual void OnStart() override;

	virtual void OnUpdate() override;

	bool IsOpen = false;

	// 记录盒子的状态
	bool IsSelected = false;
	DirStruct BodyDir{};
	DirStruct TurretDir{};

	int Group = -1;

	GiftBoxState& operator=(const GiftBoxState& other)
	{
		if (this != &other)
		{
			StateScript<GiftBoxData>::operator=(other);
			IsOpen = other.IsOpen;
			IsSelected = other.IsSelected;
			BodyDir = other.BodyDir;
			TurretDir = other.TurretDir;
			Group = other.Group;
			_isElite = other._isElite;
			_delay = other._delay;
			_delayTimer = other._delayTimer;
		}
		return *this;
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
		StateScript<GiftBoxData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<GiftBoxData>::Save(stream);
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
