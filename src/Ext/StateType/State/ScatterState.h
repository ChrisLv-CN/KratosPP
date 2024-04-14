#pragma once

#include "../StateScript.h"
#include "ScatterData.h"

class ScatterState : public StateScript<ScatterData>
{
public:
	STATE_SCRIPT(Scatter);

	virtual void Clean() override
	{
		StateScript<ScatterData>::Clean();

		_forceMoving = false;
		_panic = false;
	}

	virtual void OnUpdate() override;

	ScatterState& operator=(const ScatterState& other)
	{
		if (this != &other)
		{
			StateScript<ScatterData>::operator=(other);
			_forceMoving = other._forceMoving;
			_panic = other._panic;
		}
		return *this;
	}
#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_forceMoving)
			.Process(this->_panic)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<ScatterData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<ScatterData>::Save(stream);
		return const_cast<ScatterState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool _forceMoving = false;
	bool _panic = false;
};
