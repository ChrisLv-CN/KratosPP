#include "GameObject.h"

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
