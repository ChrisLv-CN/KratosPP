#include "Component.h"

void Component::SetExtData(IExtData* extData)
{
	_extData = extData;
	for (Component* c : _children)
	{
		c->SetExtData(extData);
	}
}

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
		// 销毁失效的子模块
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

void Component::Disable()
{
	_disable = true;
}

bool Component::IsEnable()
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

void Component::AddComponent(Component* component, int index)
{
	// 将要加入的组件的子组件的extData全部更换
	component->SetExtData(_extData);
	component->_parent = this;
	if (index < 0 || index >= (int)_children.size())
	{
		// vector::push_back 和 vector::emplace_back 会调用析构
		// list::emplace_back 不会
		_children.emplace_back(component);
	}
	else
	{
		// 插入指定位置
		auto it = _children.begin();
		if (index > 0)
		{
			std::advance(it, index);
		}
		_children.insert(it, component);
	}
#ifdef DEBUG_COMPONENT
	std::string thisId = component->thisId;
	std::string thisName = component->thisName;
	Debug::Log("Add Component [%s]%s to %s [%s]%s.\n", thisName.c_str(), thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
}

Component* Component::AddComponent(const std::string& name, int index)
{
	Component* c = CreateComponent(name);
	if (c)
	{
		AddComponent(c, index);
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

void Component::RemoveComponent(Component* component, bool disable)
{
	auto it = std::find(_children.begin(), _children.end(), component);
	if (it != _children.end())
	{
#ifdef DEBUG_COMPONENT
		std::string thisId = component->thisId;
		std::string thisName = component->thisName;
		Debug::Log("Remove Component [%s]%s from %s [%s]%s.\n", thisName.c_str(), thisId.c_str(), extName.c_str(), this->thisName.c_str(), this->thisId.c_str());
#endif // DEBUG
		Component* c = *it;
		c->_parent = nullptr;
		if (disable)
		{
			c->Disable();
		}
		// 从_children清单中删除
		_children.erase(it);
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

Component* Component::GetParent()
{
	return _parent;
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

void Component::DetachFromParent(bool disable)
{
	if (_parent)
	{
		_parent->RemoveComponent(this, disable);
		this->_parent = nullptr;
	}
}

void Component::PrintNames(std::vector<std::string>& names, int& level)
{
	// 自己
	std::string name = "";
	if (level > 1)
	{
		for (int i = 0; i < level - 1; i++)
		{
			name.append("  ");
		}
	}
	if (level > 0)
	{
		name.append("--");
	}
	name.append(this->Name);
	if (!this->Tag.empty())
	{
		name.append("#").append(this->Tag);
	}
	names.push_back(name);
	ForeachChild([&names, &level](Component* c) {
		int l = level + 1;
		c->PrintNames(names, l);
		});
}

#pragma region Foreach

void Component::Foreach(std::function<void(Component*)> action)
{
	// 执行全部
	int level = 0;
	int maxLevel = -1;
	ForeachLevel(action, level, maxLevel);
}

void Component::ForeachLevel(std::function<void(Component*)> action, int& level, int& maxLevel)
{
	// 执行自身
	if (IsEnable() && IsActive())
	{
		action(this);
		int nextLevel = level + 1;
		if (maxLevel < 0 || nextLevel < maxLevel)
		{
			// 执行子模块
			for (Component* c : _children)
			{
				c->ForeachLevel(action, nextLevel, maxLevel);
				if (c->IsBreak())
				{
					break;
				}
			}
		}
	}
	// 清理失效的子模块
	ClearDisableComponent();
}

void Component::ForeachChild(std::function<void(Component*)> action)
{
	for (Component* c : _children)
	{
		action(c);
		if (c->IsBreak())
		{
			break;
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
