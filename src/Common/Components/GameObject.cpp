#include "GameObject.h"

GameObject::GameObject() : Component() { }
GameObject::GameObject(std::string name) : Component()
{
	this->Name = name;
}

void GameObject::Awake()
{
	this->Component::Awake();
	_OnAwake();
}

void GameObject::OnUpdate()
{
	this->Component::OnUpdate();
	for (Component* c : _unstartedComponents)
	{
		c->EnsureStarted();
	}
	_unstartedComponents.clear();
}

GameObject* GameObject::GetAwaked()
{
	EnsureAwaked();
	return this;
}

void GameObject::AddComponentNotAwake(Component* component)
{
	this->Component::AddComponent(component);
	_unstartedComponents.push_back(component);
}

