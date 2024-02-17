#pragma once

#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

template <typename TData>
class StateScript : public ObjectScript
{
public:
	OBJECT_SCRIPT(StateScript);

	std::string Token{ "" };
	TData Data{};

	void Start(TData& data, int duration = -1, std::string token = "");
	void End(std::string token = "");
	void Replace(TData& data, int duration = -1, std::string token = "");

	virtual void OnStart() {};
	virtual void OnEnd() {};

	virtual bool IsAlive();

	void ResetDuration(int duration, std::string token = "");

	bool IfReset();

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->Token)
			.Process(this->Data)

			.Process(this->_duration)
			.Process(this->_immortal)
			.Process(this->_lifeTimer)
			.Process(this->_reset)
			.Process(this->_frame)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<StateScript*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	int _duration = -1; // 寿命
	bool _immortal = true; // 永生
	CDTimerClass _lifeTimer{};
	bool _reset = false; // Start被调用过的标记
	int _frame = 0; // 当前帧数
};
