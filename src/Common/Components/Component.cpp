#include "Component.h"

Component* Component::GetParent()
{
	return _parent;
}

Component* Component::GetRoot()
{
	if (_parent == NULL)
	{
		return this;
	}
	return _parent->GetRoot();
}

GameObject* Component::GetGameObject()
{
	return (GameObject*)Root;
}

void Component::AttachToComponent(Component component)
{
	if (_parent == &component)
	{
		return;
	}
	DetachFromParent();

	component.AddComponent(this);
}

void Component::DetachFromParent()
{
	if (_parent)
	{
		_parent->RemoveComponent(this);
	}
}

template <typename TComponent> TComponent* Component::GetComponent()
{
	return GetComponentInChildren<TComponent>();
}


template <typename TComponent> TComponent* Component::GetComponentInParent()
{
	Component* c = nullptr;
	// find first level
	for (Component* children : _children)
	{
		if (typeid(children) == TComponent)
		{
			c = children;
			break;
		}
	}
	if (!c && _parent)
	{
		c = _parent->GetComponentInParent<TComponent>();
	}
	return c;
}

template <typename TComponent> TComponent* Component::GetComponentInChildren()
{
	TComponent* c = nullptr;
	// find first level
	for (Component* children : _children)
	{
		if (typeid(children) == TComponent)
		{
			c = (TComponent*)children;
			break;
		}
	}
	if (!c)
	{
		for (Component* children : _children)
		{
			TComponent* r = children->GetComponentInChildren<TComponent>();
			if (r)
			{
				c = r;
				break;
			}
		}
	}
	return c;
}


std::vector<Component*> Component::GetComponentsInChildren()
{
	return _children;
}

void Component::Foreach(ComponentAction action)
{
	ForeachComponents(this, action);
}

void Component::ForeachChild(ComponentAction action)
{
	ForeachComponents(GetComponentsInChildren(), action);
}

void Component::ForeachComponents(std::vector<Component*> components, ComponentAction action)
{
	for (Component* compoent : components)
	{
		action(compoent);
	}
}

void Component::ForeachComponents(Component* root, ComponentAction action)
{
		action(root);
		root->ForeachChild(action);
}

void Component::EnsureAwaked()
{
	if (!_awaked)
	{
		_awaked = true;
		Awake();
		ForeachChild([](Component* c) {c->EnsureAwaked(); });
	}
}

void Component::EnsureStarted()
{
	if (!_started)
	{
		_started = true;
		Start();
		ForeachChild([](Component* c) {c->EnsureStarted(); });
	}
}

void Component::Destroy()
{
	for (Component* c : _children)
	{
		c->Destroy();
	}
	_children.clear();

	DetachFromParent();
}

void Component::AddComponent(Component* component)
{
	component->_parent = this;
	_children.push_back(component);
}

void Component::RemoveComponent(Component* component)
{
	for (auto it = _children.begin(); it != _children.end(); it++)
	{
		if (*it == component)
		{
			_children.erase(it);
			break;
		}
	}
	component->_parent = nullptr;
}

