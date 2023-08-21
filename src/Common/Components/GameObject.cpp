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

GameObject* GameObject::GetAwaked()
{
	EnsureAwaked();
	return this;
}

