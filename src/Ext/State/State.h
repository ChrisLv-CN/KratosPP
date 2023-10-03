#pragma once

#include <string>
#include <any>

#include <GeneralStructures.h>

#include <Utilities/Stream.h>

template <typename TData>
class State
{
public:
	TData Data{};
	std::string Token{};

	/**
	 *@brief 由外部开启，通常是AE给替身开启.
	 *
	 * @param data 配置
	 * @param duration 持续时间
	 * @param token 令牌
	 */
	void EnableAndReplace(TData& data, int duration, std::string token)
	{
		// 强制关闭原有的
		Disable();
		// 附加新的
		Enable(data, duration, token);
	}

	/**
	 *@brief 不论来源直接开启，同时清除AE关联.
	 *
	 * @param data 配置
	 */
	void Enable(TData& data)
	{
		Enable(data, -1);
	}

	/**
	 *@brief 开启状态机
	 *
	 * @param data 配置
	 * @param duration 持续时间
	 * @param token 令牌
	 */
	void Enable(TData& data, int duration, std::string token = "")
	{
		Data = data;
		Token = token;

		_active = duration != 0;
		ResetDuration(duration);
		_frame = Unsorted::CurrentFrame;
		_resetFlag = true;
		OnEnable();
	}

	virtual void OnEnable() {};

	virtual void StartTimer(int duration)
	{
		_timer.Start(duration);
	}

	void ResetDuration(int duration, std::string token = "")
	{
		if (token.empty() || token == Token)
		{
			if (duration < 0)
			{
				_infinite = true;
				_timer.Stop();
			}
			else
			{
				_infinite = false;
				StartTimer(duration);
			}
		}
	}

	void Disable(std::string token = "")
	{
		if (token.empty() || token == Token)
		{
			_active = false;
			_infinite = false;
			_timer.Stop();
			// TODO 关闭AE
			OnDisable();
		}
	}

	virtual void OnDisable() {};

	virtual bool IsActive()
	{
		// 当前帧内持续有效，下一帧检查计时器
		if (_active)
		{
			int currentFrame = Unsorted::CurrentFrame;
			if (_frame != currentFrame)
			{
				_frame = currentFrame;
				_active = _infinite || _timer.InProgress();
			}
		}
		return _active;
	}

	bool IsReset()
	{
		bool reset = _resetFlag;
		if (_resetFlag)
		{
			_resetFlag = false;
		}
		return reset;
	}

	void Reset()
	{
		_resetFlag = true;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)
			.Process(this->Token)

			.Process(this->_active)
			.Process(this->_infinite)
			.Process(this->_timer)
			.Process(this->_resetFlag)
			.Process(this->_frame)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<State<TData>*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool _active = false;
	bool _infinite = false; // 无限时长
	CDTimerClass _timer; // 持续时间
	bool _resetFlag = false;
	int _frame = 0; // 当前帧数
};
