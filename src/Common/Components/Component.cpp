#include "Component.h"

void Component::EnsureAwaked()
{
	if (!_awaked)
	{
		_awaked = true;
		Awake();
		ForeachChild([](Component* c)
			{ c->EnsureAwaked(); });
	}
}

void Component::EnsureStarted()
{
	if (!_started)
	{
		_started = true;
		Start();
		ForeachChild([](Component* c)
			{ c->EnsureStarted(); });
	}
}

void Component::EnsureDestroy()
{
	Destroy();
	for (Component* c : _children)
	{
		c->EnsureDestroy();
	}

	_children.clear();
	DetachFromParent();

	// 删除实例
	// delete this;
}

void Component::AddComponent(Component* component)
{
	auto it = std::find(_disableComponents.begin(), _disableComponents.end(), component->Name);
	if (it == _disableComponents.end())
	{
		component->_parent = this;
		_children.push_back(component);
	}
}

void Component::RemoveComponent(Component* component)
{
	for (auto it = _children.begin(); it != _children.end(); it++)
	{
		if (*it == component)
		{
			_children.erase(it);

			std::string disableName = component->Name;
			_disableComponents.push_back(disableName);

			break;
		}
	}
}

void Component::ClearDisableComponent()
{
	for (std::string disableName : _disableComponents)
	{
		for (auto it = _children.begin(); it != _children.end(); it++)
		{
			if ((*it)->Name == disableName)
			{
				_children.erase(it);
			}
		}
	}
}

void Component::AttachToComponent(Component* component)
{
	if (_parent == component)
	{
		return;
	}
	DetachFromParent();

	component->AddComponent(this);
}

void Component::DetachFromParent()
{
	if (_parent)
	{
		_parent->RemoveComponent(this);
		this->_parent = nullptr;
	}
}

