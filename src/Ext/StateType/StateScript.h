#pragma once

#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/INI/INI.h>

#include <Ext/ObjectType/AttachEffect.h>

#define STATE_SCRIPT(CLASS_NAME, STATE_DATA) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, StateScript<STATE_DATA>) \

#define GET_STATE(CLASS_NAME) \
	CLASS_NAME* _ ## CLASS_NAME = nullptr; \
	CLASS_NAME* Get ## CLASS_NAME() \
	{ \
		if (!_ ## CLASS_NAME) \
		{ \
			_ ## CLASS_NAME = GetComponent<CLASS_NAME>(); \
		} \
		return _ ## CLASS_NAME; \
	} \
	__declspec(property(get = Get ## CLASS_NAME)) CLASS_NAME* CLASS_NAME ## State; \

template <typename TData>
class StateScript : public ObjectScript
{
public:
	void Start(TData& data, int duration = -1, std::string token = "")
	{

		Data = data;
		Token = token;

		if (duration != 0)
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
		ResetDuration(duration);
		_frame = Unsorted::CurrentFrame;
		_reset = true;
		OnStart();
	}

	void End(std::string token = "")
	{
		if (token.empty() || token == Token)
		{
			Deactivate();
			_immortal = false;
			_lifeTimer.Stop();

			// 如果有，关闭AE
			if (!Token.empty())
			{
				if (AttachEffect* aem = _parent->GetComponent<AttachEffect>())
				{
					aem->DetachByToken(Token);
				}
			}
			OnEnd();
		}
	}

	void Replace(TData& data, int duration = -1, std::string token = "")
	{
		End();
		Start(data, duration, token);
	}

	virtual void OnStart() {};
	virtual void OnEnd() {};

	virtual bool IsAlive()
	{
		if (IsActive())
		{
			// 每一帧只检查第一次，结果持续一整帧
			int currentFrame = Unsorted::CurrentFrame;
			if (_frame != currentFrame)
			{
				_frame = currentFrame;
				if (!_immortal && _lifeTimer.Expired())
				{
					Deactivate();
				}
			}
		}
		return IsActive();
	}

	void ResetDuration(int duration, std::string token = "")
	{
		if (token.empty() || token == Token)
		{
			if (duration < 0)
			{
				_immortal = true;
				_lifeTimer.Stop();
			}
			else
			{
				_immortal = false;
				_lifeTimer.Start(duration);
			}
		}
	}

	void Reset()
	{
		_reset = true;
	}

	bool IfReset()
	{
		bool reset = _reset;
		if (_reset)
		{
			_reset = false;
		}
		return reset;
	}

	virtual void Awake() override
	{
		Deactivate();
		OnInitState(false);
	}

	virtual void ExtChanged() override
	{
		Deactivate();
		OnInitState(true);
	}

#ifdef DEBUG
	virtual void OnUpdate() override
	{
		int left = _immortal ? -1 : _lifeTimer.GetTimeLeft();
		Tag = std::to_string(left);
		if (IsNotNone(Token))
		{
			Tag += "#" + Token;
		}
	}
#endif // DEBUG

	virtual void OnInitState(bool replace)
	{
		TData* data = GetInitData();
		if (data->Enable)
		{
			if (replace)
			{
				Replace(*data);
			}
			else
			{
				Start(*data);
			}
		}
	}

	virtual TData* GetInitData()
	{
		return INI::GetConfig<TData>(INI::Rules, pObject->GetType()->ID)->Data;
	};

	StateScript<TData>& operator=(const StateScript<TData>& other)
	{
		if (this != &other)
		{
			Component::operator=(other);
			Token = other.Token;
			Data = other.Data;
		}
		return *this;
	}

	std::string Token{ "" };
	TData Data{};

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
		return const_cast<StateScript<TData>*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	int _duration = -1; // 寿命
	bool _immortal = true; // 永生
	CDTimerClass _lifeTimer{};
	bool _reset = false; // Start被调用过的标记
	int _frame = 0; // 当前帧数
};
