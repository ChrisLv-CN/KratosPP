#pragma once

#include "State.h"
#include "AntiBulletData.h"

class AntiBulletState : public State<AntiBulletData>
{
public:
	bool CanSearchBullet()
	{
		bool can = _delayTimer.Expired();
		if (can)
		{
			_delayTimer.Start(Data.Rate);
		}
		return can;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		State<AntiBulletData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		State<AntiBulletData>::Save(stream);
		return const_cast<AntiBulletState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	CDTimerClass _delayTimer;
};
