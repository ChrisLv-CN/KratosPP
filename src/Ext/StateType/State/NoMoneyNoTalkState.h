#pragma once

#include "../StateScript.h"
#include "NoMoneyNoTalkData.h"

class NoMoneyNoTalkState : public StateScript<NoMoneyNoTalkData>
{
public:
	STATE_SCRIPT(NoMoneyNoTalk);

	virtual void Clean() override
	{
		StateScript<NoMoneyNoTalkData>::Clean();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<NoMoneyNoTalkData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<NoMoneyNoTalkData>::Save(stream);
		return const_cast<NoMoneyNoTalkState*>(this)->Serialize(stream);
	}
#pragma endregion
};
