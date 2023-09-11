#include "GameObject.h"

void GameObject::Awake()
{
	this->Component::Awake();
	_OnAwake();
}

void GameObject::OnUpdate()
{
	this->Component::OnUpdate();
	for (Component *c : _unstartedComponents)
	{
		c->EnsureStarted();
	}
	_unstartedComponents.clear();
}

GameObject *GameObject::GetAwaked()
{
	EnsureAwaked();
	return this;
}

void GameObject::AddComponentNotAwake(Component *component)
{
	this->Component::AddComponent(component);
	_unstartedComponents.push_back(component);
}

void GameObject::ClearUnstartComponents()
{
	for (auto it = _unstartedComponents.begin(); it != _unstartedComponents.end();)
	{
		if ((*it)->AlreadyStart())
		{
			it = _unstartedComponents.erase(it);
		}
		else
		{
			it++;
		}
	}
}
