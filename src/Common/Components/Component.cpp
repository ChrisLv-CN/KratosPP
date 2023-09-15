#include "Component.h"


void Component::EnsureAwaked()
{
	if (!_awaked)
	{
		_awaked = true;
#ifdef DEBUG_COMPONENT
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		std::string thisId2 = { t_this };
		Debug::Log("Component [%s]%s - %s calling awake, has %d disable components, children has %d\n", this->thisName.c_str(), this->thisId.c_str(), thisId2.c_str(), _disableComponents.size(), _children.size());
#endif // DEBUG
		Awake();
		ForeachChild([](Component* c)
			{
#ifdef DEBUG_COMPONENT
				char c_this[1024];
				sprintf_s(c_this, "%p", c);
				std::string thisId3 = { c_this };
				Debug::Log("Child Component [%s]%s - %s calling EnsureAwaked.\n", c->thisName.c_str(), c->thisId.c_str(), thisId3.c_str());
#endif // DEBUG
				c->EnsureAwaked();
			});
	}
}

void Component::EnsureStarted()
{
	if (!_started)
	{
		_started = true;
#ifdef DEBUG_COMPONENT
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		std::string thisId2 = { t_this };
		Debug::Log("Component [%s]%s - %s calling start, has %d disable components, children has %d\n", this->thisName.c_str(), this->thisId.c_str(), thisId2.c_str(), _disableComponents.size(), _children.size());
#endif // DEBUG
		Start();
		ForeachChild([](Component* c)
			{
#ifdef DEBUG_COMPONENT
				char c_this[1024];
				sprintf_s(c_this, "%p", c);
				std::string thisId3 = { c_this };
				Debug::Log("Child Component [%s]%s - %s calling EnsureStarted.\n", c->thisName.c_str(), c->thisId.c_str(), thisId3.c_str());
#endif // DEBUG
				c->EnsureStarted();
			});
	}
}

void Component::EnsureDestroy()
{
#ifdef DEBUG_COMPONENT
	char t_this[1024];
	sprintf_s(t_this, "%p", this);
	std::string thisId2 = { t_this };
	Debug::Log("Component [%s]%s - %s calling destroy, has %d disable components, children has %d\n", this->thisName.c_str(), this->thisId.c_str(), thisId2.c_str(), _disableComponents.size(), _children.size());
#endif // DEBUG
	Destroy();
	for (auto it = _children.begin(); it != _children.end();)
	{
#ifdef DEBUG_COMPONENT
		Component* c = (*it);
		char c_this[1024];
		sprintf_s(c_this, "%p", c);
		std::string thisId3 = { c_this };
		Debug::Log("Child Component [%s]%s - %s calling EnsureDestroy.\n", c->thisName.c_str(), c->thisId.c_str(), thisId3.c_str());
#endif // DEBUG
		(*it)->EnsureDestroy();
		it = _children.erase(it);
	}
	_parent = nullptr;
	_destroy = true;

	// Remove调用该函数之后，将其移除出_children列表
	// 需要删除实例释放内存，但不可以在本函数内执行，利用EventSystem异步操作
	EventSystems::Logic.AddHandler(Events::LogicUpdateEvent, this, &Component::DestroySelf);

	// GameDelete(this);
	// delete this;
}

void Component::DestroySelf(EventSystem* sender, Event e, void* args)
{
	sender->RemoveHandler(e, this, &Component::DestroySelf);
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

void Component::AddComponent(Component& component)
{
	auto it = std::find(_disableComponents.begin(), _disableComponents.end(), component.Name);
	if (it == _disableComponents.end())
	{
		component._parent = this;
		// vector::push_back 和 vector::emplace_back 会调用析构
		// list::emplace_back 不会
		_children.emplace_back(&component);
#ifdef DEBUG_COMPONENT
		Debug::Log("Add Component [%s]%s to %s [%s]%s.\n", component.thisName.c_str(), component.thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
	}
#ifdef DEBUG_COMPONENT
	else
	{
		Debug::Log("Add Component [%s]%s to %s [%s]%s, but is already exist.\n", component.thisName.c_str(), component.thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
	}
#endif // DEBUG
}

void Component::RemoveComponent(Component* component, bool destroy)
{
	auto it = std::find(_children.begin(), _children.end(), component);
	if (it != _children.end())
	{
		(*it)->_parent = nullptr;
		it = _children.erase(it);
#ifdef DEBUG_COMPONENT
		std::string thisId = component->thisId;
		std::string thisName = component->thisName;
		Debug::Log("Remove Component [%s]%s from %s [%s]%s.\n", thisName.c_str(), thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
		std::string disableName = component->Name;
		_disableComponents.push_back(disableName);
		if (destroy)
		{
			component->EnsureDestroy();
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

	component->AddComponent(*this);
}

void Component::DetachFromParent()
{
	if (_parent)
	{
		_parent->RemoveComponent(this);
		this->_parent = nullptr;
	}
}

#pragma region Foreach
/// <summary>
/// execute action for each components in root (include itself)
/// </summary>
/// <param name="action"></param>
void Component::Foreach(std::function<void(Component*)> action)
{
	ForeachComponents(this, action);
}

void Component::ForeachChild(std::function<void(Component*)> action)
{
	ForeachComponents(_children, action);
}

/// <summary>
/// execute action for each components in root (include root)
/// </summary>
/// <param name="root">the root component</param>
/// <param name="action">the action to executed</param>
void Component::ForeachComponents(Component* root, std::function<void(Component*)> action)
{
	action(root);
	root->ForeachChild(action);
}

void Component::ForeachComponents(std::list<Component*>& components, std::function<void(Component*)> action)
{
	auto it = components.begin();
	while (it != components.end())
	{
		action(*it);
		if (it == components.end())
		{
			break;
		}
		it++;
	}
}
#pragma endregion
