#include "GameObject.h"

GameObject::GameObject() : Component() { }
GameObject::GameObject(std::string name) : Component()
{
	this->Name = name;
}

GameObject* GameObject::GetAwaked()
{
	EnsureAwaked();
	return this;
}

