#include "StateScript.h"

#include <Ext/Helper/Status.h>

#include <Ext/ObjectType/AttachEffect.h>

template <typename TData>
void StateScript<TData>::Start(TData& data, int duration, std::string token)
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

template <typename TData>
void StateScript<TData>::End(std::string token)
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

template <typename TData>
void StateScript<TData>::Replace(TData& data, int duration, std::string token)
{
	End();
	Start(data, duration, token);
}

template <typename TData>
void StateScript<TData>::ResetDuration(int duration, std::string token)
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

template <typename TData>
bool StateScript<TData>::IsAlive()
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

template <typename TData>
bool StateScript<TData>::IfReset()
{
	if (_reset)
	{
		_reset = false;
	}
	return _reset;
}

