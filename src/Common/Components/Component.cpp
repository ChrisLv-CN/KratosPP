#include "Component.h"

void Component::OnUpdate()
{
	IComponent::OnUpdate();
}

void Component::EnsureAwaked()
{
	if (!_awaked)
	{
#ifdef DEBUG_COMPONENT
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		std::string thisId2 = { t_this };
		Debug::Log("Component [%s]%s - %s calling awake, has %d disable components, children has %d\n", this->thisName.c_str(), this->thisId.c_str(), thisId2.c_str(), _disableComponents.size(), _children.size());
#endif // DEBUG
		Awake();
		// 在Awake中可能出现移除自身的操作，_awaked标记也用于控制Remove时是否延迟删除
		_awaked = true;
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
		// 销毁失效的Component
		ClearDisableComponent();
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
	_disable = true;
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

	// 释放资源
	// GameDelete(this);
	delete this;
}

bool Component::AlreadyAwake()
{
	return _awaked;
}

bool Component::IsAlive()
{
	return !_disable;
}

void Component::Activate()
{
	_active = true;
}

void Component::Deactivate()
{
	_active = false;
}

bool Component::IsActive()
{
	return _active;
}

void Component::AddComponent(Component* component)
{
	component->extData = extData;
	component->_parent = this;
	// vector::push_back 和 vector::emplace_back 会调用析构
	// list::emplace_back 不会
	_children.emplace_back(component);
#ifdef DEBUG_COMPONENT
	std::string thisId = component->thisId;
	std::string thisName = component->thisName;
	Debug::Log("Add Component [%s]%s to %s [%s]%s.\n", thisName.c_str(), thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
}

Component* Component::AddComponent(const std::string& name)
{
	Component* c = CreateComponent(name);
	if (c)
	{
		AddComponent(c);
	}
	return c;
}

Component* Component::FindOrAllocate(const std::string& name)
{
	Component* c = GetComponentByName(name);
	if (!c)
	{
		// 添加新的Component
		c = AddComponent(name);
		// 激活新的Component
		c->EnsureAwaked();
	}
	return c;
}

void Component::RemoveComponent(Component* component)
{
	auto it = std::find(_children.begin(), _children.end(), component);
	if (it != _children.end())
	{
#ifdef DEBUG_COMPONENT
		std::string thisId = component->thisId;
		std::string thisName = component->thisName;
		Debug::Log("Remove Component [%s]%s from %s [%s]%s.\n", thisName.c_str(), thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
		// 在Awake和Start中删除Component，在循环内，需要延迟删除，
		// 将Component失活，并记录，在结束_children的循环后，再清除
		Component* c = *it;
		c->_parent = nullptr;
		c->_disable = true;

		// 在其他位置的删除为直接删除
		if (c->AlreadyAwake())
		{
			it = _children.erase(it);
		}
	}
}

void Component::ClearDisableComponent()
{
	for (auto it = _children.begin(); it != _children.end();)
	{
		Component* c = *it;
		if (c->_disable)
		{
#ifdef DEBUG_COMPONENT
			Debug::Log("Remove disable [%s] Component [%s]%s from %s [%s]%s.\n", (*it)->Name.c_str(), (*it)->thisName.c_str(), (*it)->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif //DEBUG
			c->_parent = nullptr;
			it = _children.erase(it);
			c->EnsureDestroy();
		}
		else
		{
			it++;
		}
	}
}

void Component::RestoreComponent()
{
	// 去除名单中不存在的组件
	for (auto it = _children.begin(); it != _children.end();)
	{
		Component* c = *it;
		if (std::find(_childrenNames.begin(), _childrenNames.end(), c->Name) == _childrenNames.end())
		{
#ifdef DEBUG_COMPONENT
			Debug::Log("Remove disable [%s] Component [%s]%s from %s [%s]%s.\n", (*it)->Name.c_str(), (*it)->thisName.c_str(), (*it)->thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif //DEBUG
			c->_parent = nullptr;
			it = _children.erase(it);
			c->EnsureDestroy();
		}
		else
		{
			it++;
		}
	}
	// 添加列表中不存在的组件
	std::vector<std::string> currentNames{};
	for (Component* c : _children)
	{
		currentNames.push_back(c->Name);
	}

	// 取差集
	std::vector<std::string> v;
	v.resize(_childrenNames.size());
	std::vector<std::string>::iterator end = set_difference(_childrenNames.begin(), _childrenNames.end(), currentNames.begin(), currentNames.end(), v.begin());

#ifdef DEBUG_COMPONENT
	std::string s1 = "_childrenNames: ";
	for_each(_childrenNames.begin(), _childrenNames.end(), [&](std::string& s) {
		s1.append(s).append(", ");
		});
	s1.append("\n");
	Debug::Log(s1.c_str());

	std::string s2 = "currentNames: ";
	for_each(currentNames.begin(), currentNames.end(), [&](std::string& s) {
		s2.append(s).append(", ");
		});
	s2.append("\n");
	Debug::Log(s2.c_str());

	std::string s3;
	for_each(v.begin(), end, [&](std::string& s) {
		s3.append(s).append(", ");
		});
	s3.append("\n");
	Debug::Log(s3.c_str());
#endif

	for (auto ite = v.begin(); ite != end; ite++)
	{
#ifdef DEBUG_COMPONENT
		Debug::Log("Nedd to add new Component [%s] to Component [%s]%s \n", (*ite).c_str(), thisName.c_str(), thisId.c_str());
#endif
		Component* c = CreateComponent((*ite));
		AddComponent(c);
	}
}

Component* Component::GetComponentInParentByName(const std::string& name)
{
	Component* c = nullptr;
	// find first level
	for (Component* children : _children)
	{
		if (children->Name == name)
		{
			c = children;
			break;
		}
	}
	if (!c && _parent)
	{
		c = _parent->GetComponentInParentByName(name);
	}
	return c;
}

Component* Component::GetComponentInChildrenByName(const std::string& name)
{
	Component* c = nullptr;
	// find first level
	for (Component* children : _children)
	{
		if (children->Name == name)
		{
			c = children;
			break;
		}
	}
	if (!c)
	{
		for (Component* children : _children)
		{
			Component* r = children->GetComponentInChildrenByName(name);
			if (r)
			{
				c = r;
				break;
			}
		}
	}
	return c;
}

Component* Component::GetComponentByName(const std::string& name)
{
	return GetComponentInChildrenByName(name);
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
	if (IsAlive() && IsActive())
	{
		action(root);
		root->ForeachChild(action);
	}
}

void Component::ForeachComponents(std::list<Component*>& components, std::function<void(Component*)> action)
{
	for (Component* c : components)
	{
		if (c && c->IsAlive() && c->IsActive())
		{
			action(c);
			if (c->IsBreak())
			{
				break;
			}
		}
	}
}

void Component::Break()
{
	_break = true;
}

bool Component::IsBreak()
{
	if (_break)
	{
		_break = false;
		return true;
	}
	return _break;
}
#pragma endregion
