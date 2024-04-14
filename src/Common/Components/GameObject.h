#pragma once

#include <list>
#include <vector>

#include "Component.h"

#include <Common/MyDelegate.h>

using namespace Delegate;

class GameObject : public Component
{
public:
	DECLARE_COMPONENT(GameObject, Component);

	bool ExtChanged = false;

#ifdef DEBUG_COMPONENT
	virtual ~GameObject() override
	{
		Debug::Log("GameObject [%s]%s is release.\n", this->thisName.c_str(), this->thisId.c_str());
	}
#endif // DEBUG

	virtual void Clean() override
	{
		Component::Clean();
	};

	GameObject* GetAwaked()
	{
		EnsureAwaked();
		return this;
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
