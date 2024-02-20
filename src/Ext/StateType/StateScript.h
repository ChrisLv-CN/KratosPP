#pragma once

#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>
#include <HouseClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/INI/INI.h>

#include <Ext/ObjectType/AttachEffect.h>

class IStateScript
{
public:
	virtual void Start(EffectData* data, int duration = -1, std::string token = "", bool receiverOwn = true, HouseClass* pTargetHouse = nullptr) {};
	virtual void End(std::string token = "") {};
	virtual void Replace(EffectData* data, int duration = -1, std::string token = "", bool receiverOwn = true, HouseClass* pTargetHouse = nullptr) {};
	virtual void ResetDuration(int duration, std::string token = "") {};
};

#define STATE_SCRIPT(STATE_NAME) \
	DECLARE_DYNAMIC_SCRIPT(STATE_NAME ## State, StateScript<STATE_NAME ## Data>) \

#define STATE_VAR_DEFINE(STATE_NAME) \
	STATE_NAME ## State * _ ## STATE_NAME = nullptr; \
	STATE_NAME ## State * Get ## STATE_NAME() \
	{ \
		if (!_ ## STATE_NAME) \
		{ \
			_ ## STATE_NAME = GetComponent<STATE_NAME ## State>(); \
		} \
		return _ ## STATE_NAME; \
	} \
	__declspec(property(get = Get ## STATE_NAME)) STATE_NAME ## State* STATE_NAME; \

#define STATE_VAR_INIT(STATE_NAME) \
	FindOrAttach<STATE_NAME ## State>(); \

#define STATE_VAR_TRYGET(STATE_NAME) \
	else if (dynamic_cast<T*>(STATE_NAME)) \
		state = STATE_NAME; \

template <typename TData>
class StateScript : public ObjectScript, public IStateScript
{
public:
	virtual void Start(EffectData* data, int duration = -1, std::string token = "", bool receiverOwn = true, HouseClass* pTargetHouse = nullptr) override
	{
		if (TData* pData = dynamic_cast<TData*>(data))
		{
			Data = *pData;
			Token = token;
			ReceiverOwn = receiverOwn;
			if (receiverOwn)
			{
				pAEHouse = nullptr;
			}
			else
			{
				pAEHouse = pTargetHouse;
			}

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
	}

	virtual void End(std::string token = "") override
	{
		if (token.empty() || token == Token)
		{
			Deactivate();
			_immortal = false;
			_lifeTimer.Stop();
			// 如果有，关闭AE
			if (!Token.empty())
			{
				if (AttachEffect* aem = _gameObject->GetComponent<AttachEffect>())
				{
					aem->DetachByToken(Token);
				}
			}
			OnEnd();
		}
	}

	virtual void Replace(EffectData* data, int duration = -1, std::string token = "", bool receiverOwn = true, HouseClass* pTargetHouse = nullptr) override
	{
		End();
		Start(data, duration, token, receiverOwn, pTargetHouse);
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

	virtual void ResetDuration(int duration, std::string token = "") override
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
				Replace(data);
			}
			else
			{
				Start(data);
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
	bool ReceiverOwn = true;
	HouseClass* pAEHouse = nullptr;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->Token)
			.Process(this->Data)
			.Process(this->ReceiverOwn)
			.Process(this->pAEHouse)

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
