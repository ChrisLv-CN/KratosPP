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
#ifdef DEBUG
		Debug::Log("Add Component [%s]%s to %s [%s]%s.\n", component->thisName.c_str(), component->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
	}
#ifdef DEBUG
	else
	{
		Debug::Log("Add Component [%s]%s to %s [%s]%s, but is already exist.\n", component->thisName.c_str(), component->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
	}
#endif // DEBUG
}

void Component::RemoveComponent(Component* component)
{
	for (auto it = _children.begin(); it != _children.end(); it++)
	{
		if (*it == component)
		{
			_children.erase(it);
#ifdef DEBUG
			Debug::Log("Remove Component [%s]%s from %s [%s]%s.\n", component->Name.c_str(), component->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
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
		for (auto it = _children.begin(); it != _children.end();)
		{
			if ((*it)->Name == disableName)
			{
				(*it)->_parent = nullptr;
				it = _children.erase(it);
#ifdef DEBUG
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

