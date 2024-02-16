#pragma once

#include <list>
#include <vector>

#include "Component.h"

#include <Common/MyDelegate.h>

using namespace Delegate;

class GameObject : public Component
{
public:
	bool ExtChanged = false;

	GameObject() : Component()
	{
		this->Name = ComponentName(GameObject);
#ifdef DEBUG
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		this->thisId = { t_this };
#endif // DEBUG
	}

#ifdef DEBUG_COMPONENT
	virtual ~GameObject() override
	{
		Debug::Log("GameObject [%s]%s is release.\n", this->thisName.c_str(), this->thisId.c_str());
	}
#endif // DEBUG

	GameObject* GetAwaked()
	{
		EnsureAwaked();
		return this;
	}

	template <typename TScript>
	TScript* FindOrAttach()
	{
		return static_cast<TScript*>(FindOrAllocate(TScript::ScriptName));
	}

	virtual void OnForeachEnd() override
	{
		if (ExtChanged)
		{
			ExtChanged = false;
			for (Component* c : _children)
			{
				c->Foreach([](Component* cc) { cc->ExtChanged(); });
			}
		}
	}

};
