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
#ifdef DEBUG_COMPONENT
	Debug::Log("Component [%s]%s is destroy, has %d disable components, children has %d\n", this->thisName.c_str(), this->thisId.c_str(), _disableComponents.size(), _children.size());
#endif // DEBUG
	Destroy();
	for (Component* c : _children)
	{
		c->EnsureDestroy();
	}

	_children.clear();
	DetachFromParent();

	// 删除实例
	// 在Ext中创建component实例时使用new才可以在移除component后将其delete
	// 如果使用GameCreate创建实例，在移除后不可以GameDelete，游戏会报错，原因不明
	// GameDelete(this);
	delete this;
}

bool Component::AlreadyAwake()
{
	return _awaked;
}

bool Component::AlreadyStart()
{
	return _started;
}

void Component::AddComponent(Component* component)
{
	auto it = std::find(_disableComponents.begin(), _disableComponents.end(), component->Name);
	if (it == _disableComponents.end())
	{
		component->_parent = this;
		_children.push_back(component);
#ifdef DEBUG_COMPONENT
		Debug::Log("Add Component [%s]%s to %s [%s]%s.\n", component->thisName.c_str(), component->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
	}
#ifdef DEBUG_COMPONENT
	else
	{
		Debug::Log("Add Component [%s]%s to %s [%s]%s, but is already exist.\n", component->thisName.c_str(), component->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
	}
#endif // DEBUG
}

void Component::RemoveComponent(Component* component, bool destroy)
{
	for (auto it = _children.begin(); it != _children.end(); it++)
	{
		if (*it == component)
		{
			_children.erase(it);
#ifdef DEBUG_COMPONENT
			Debug::Log("Remove Component [%s]%s from %s [%s]%s.\n", component->Name.c_str(), component->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
			std::string disableName = component->Name;
			_disableComponents.push_back(disableName);
			if (destroy)
			{
				component->EnsureDestroy();
			}
			break;
		}
	}
}

void Component::ClearDisableComponent()
{
	for (std::string disableName : _disableComponents)
	{
		for (auto it = _children.begin(); it != _children.end();)
		{
			if ((*it)->Name == disableName)
			{
				(*it)->_parent = nullptr;
				it = _children.erase(it);
#ifdef DEBUG_COMPONENT
				Debug::Log("Remove disable [%s] Component [%s]%s from %s [%s]%s.\n", disableName.c_str(), (*it)->thisName.c_str(), (*it)->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif //DEBUG
			}
			else
			{
				it++;
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

