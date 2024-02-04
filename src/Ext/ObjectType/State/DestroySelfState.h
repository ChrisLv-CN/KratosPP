#pragma once

#include "State.h"
#include "DestroySelfData.h"

class DestroySelfState : public State<DestroySelfData>
{
public:

	virtual void OnEnable() override
	{
		ExtendLife();
	}

	bool AmIDead()
	{
		return IsActive() && (GoDie || Timeup());
	}

	void DestroyNow(bool peaceful)
	{
		// 强制开启状态机
		DestroySelfData data;
		data.Enable = true;
		data.Delay = 0;
		data.Peaceful = peaceful;
		Enable(data);
		// 狗带
		GoDie = true;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->GoDie)
			.Process(this->_timeLeft)
			.Process(this->_lifeTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		State<DestroySelfData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		State<DestroySelfData>::Save(stream);
		return const_cast<DestroySelfState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void ExtendLife()
	{
		GoDie = false;
		_timeLeft = Data.Delay;
		_lifeTimer.Stop();
		if (_timeLeft > 0)
		{
			_lifeTimer.Start(_timeLeft);
		}
	}

	bool Timeup()
	{
		GoDie = _timeLeft <= 0 || _lifeTimer.Expired();
		return GoDie;
	}

	bool GoDie = false;

	int _timeLeft = 0;
	CDTimerClass _lifeTimer{};
};
