#include "GameObject.h"

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
#ifdef DEBUG_COMPONENT
		char c_this[1024];
		sprintf_s(c_this, "%p", c);
		std::string thisId3 = { c_this };
		Debug::Log("Unstarted Component [%s]%s - %s on %s [%s]%s calling EnsureStarted  .\n", c->thisName.c_str(), c->thisId.c_str(), thisId3.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
		c->EnsureStarted();
	}
	_unstartedComponents.clear();
}

GameObject* GameObject::GetAwaked()
{
	EnsureAwaked();
	return this;
}

void GameObject::AddComponentNotAwake(Component& component)
{
#ifdef DEBUG_COMPONENT
	Debug::Log("Add Component but not Awake [%s]%s to %s [%s]%s.\n", component.thisName.c_str(), component.thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
	this->Component::AddComponent(component);
	Component* c = &component;
	_unstartedComponents.push_back(c);
#ifdef DEBUG_COMPONENT
	char c_this[1024];
	sprintf_s(c_this, "%p", c);
	std::string thisId3 = { c_this };
	Debug::Log("Add Component to Unstarted [%s]%s - %s to %s [%s]%s.\n", component.thisName.c_str(), component.thisId.c_str(), thisId3.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
}
