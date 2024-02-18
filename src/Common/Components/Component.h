#pragma once

#include <functional>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <vector>
#include <list>

#include <Common/EventSystems/EventSystem.h>

#include <Utilities/Stream.h>
#include <Utilities/Debug.h>

#define ComponentName(CLASS_NAME) #CLASS_NAME

class IExtData
{
public:
	virtual void AttachComponents() = 0;
};

class IComponent
{
public:
	IComponent() {};
	virtual ~IComponent() {};

	/// <summary>
	/// ExtChanged is called when the ExtData has chaned like Techno's Type has changed.
	/// GameObject call
	/// </summary>
	virtual void ExtChanged() {};

	/// <summary>
	/// Awake is called when an enabled instance is being created.
	/// TechnoExt::ExtData() call
	/// </summary>
	virtual void Awake() {};

	virtual void OnUpdate() {};
	virtual void OnUpdateEnd() {};
	virtual void OnWarpUpdate() {};

	/// <summary>
	/// Destroy is called when enabled instance is delete.
	/// </summary>
	virtual void Destroy() {};

	/// <summary>
	/// OwnerIsRelease is called when TBase pointer is delete.
	/// </summary>
	virtual void OwnerIsRelease(void* ptr) {};

	/// <summary>
	/// ForeachEnd is called when Component::Foreach is end.
	/// </summary>
	virtual void OnForeachEnd() {};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) = 0;
	virtual bool Save(ExStreamWriter& stream) const = 0;
};

class Component : public IComponent
{
public:
	std::string Name;
	std::string Tag;

#ifdef DEBUG
	std::string thisId{};
	std::string GetThisName() { return Name; }
	__declspec(property(get = GetThisName)) std::string thisName;

	std::string extId{};
	std::string extName{};

	std::string baseId{};
	std::string baseName{};
#endif // DEBUG

#ifdef DEBUG_COMPONENT
	virtual ~Component() override
	{
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		std::string thisId2 = { t_this };
		Debug::Log("Component [%s]%s - %s is release.\n\n", thisName.c_str(), thisId.c_str(), thisId2.c_str());
	}
#endif // DEBUG

	void SetExtData(IExtData* extData);

	virtual void OnUpdate() override;

	void EnsureAwaked();
	void EnsureDestroy();

	bool AlreadyAwake();

	/// <summary>
	/// 关闭组件，标记为失效，组件会在执行完Foreach后被移除
	/// </summary>
	void Disable();
	bool IsEnable();


	/// <summary>
	/// 激活组件，使其可以执行Foreach逻辑
	/// </summary>
	void Activate();
	/// <summary>
	/// 失活组件，使其跳过执行Foreach逻辑
	/// </summary>
	void Deactivate();
	bool IsActive();

	/// <summary>
	/// 将Component加入子列表，同时赋予自身储存的IExtData
	/// </summary>
	void AddComponent(Component* component, int index = -1);

	Component* AddComponent(const std::string& name, int index = -1);

	Component* FindOrAllocate(const std::string& name);

	template <typename TScript>
	TScript* FindOrAttach()
	{
		return static_cast<TScript*>(FindOrAllocate(TScript::ScriptName));
	}

	// <summary>
	// 将Component加入子列表中移除
	// </summary>
	void RemoveComponent(Component* component, bool disable = true);

	/// <summary>
	/// 在结束循环后需要从_children中清理已经标记为disable的component
	/// </summary>
	void ClearDisableComponent();

	/// <summary>
	/// 从存档中恢复子组件列表
	/// </summary>
	void RestoreComponent();

	void AttachToComponent(Component* component);
	void DetachFromParent(bool disable = true);

#ifdef DEBUG
	struct ComponentState
	{
	public:
		std::string Name;
		bool Active;
	};
	void GetComponentStates(std::vector<ComponentState>& states, int& level);
#endif

#pragma region Foreach
	/// <summary>
	/// execute action for each components in root (include itself)
	/// </summary>
	/// <param name="action"></param>
	void Foreach(std::function<void(Component*)> action);

	void ForeachLevel(std::function<void(Component*)> action, int& level, int& maxLevel);

	/// <summary>
	/// execute action for each child (exclude child's child)
	/// </summary>
	/// <param name="action"></param>
	void ForeachChild(std::function<void(Component*)> action);

	void Break();
	bool IsBreak();
#pragma endregion

#pragma region GetComponent
	Component* GetComponentInParentByName(const std::string& name);

	Component* GetComponentInChildrenByName(const std::string& name);

	Component* GetComponentByName(const std::string& name);

	template <typename TComponent>
	TComponent* GetComponentInParent()
	{
		TComponent* c = nullptr;
		// find first level
		for (Component* children : _children)
		{
			if (typeid(*children) == typeid(TComponent))
			{
				c = (TComponent*)children;
				break;
			}
		}
		if (!c && _parent)
		{
			c = _parent->GetComponentInParent<TComponent>();
		}
		return c;
	}

	template <typename TComponent>
	TComponent* GetComponentInChildren()
	{
		TComponent* c = nullptr;
		// find first level
		for (Component* children : _children)
		{
			if (typeid(*children) == typeid(TComponent))
			{
				c = (TComponent*)children;
				break;
			}
		}
		if (!c)
		{
			for (Component* children : _children)
			{
				TComponent* r = children->GetComponentInChildren<TComponent>();
				if (r)
				{
					c = r;
					break;
				}
			}
		}
		return c;
	}

	template <typename TComponent>
	TComponent* GetComponent()
	{
		return GetComponentInChildren<TComponent>();
	}

	Component* GetParent();
#pragma endregion

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream, bool isLoad)
	{
		// 储存Component的控制参数
		stream
			// 存取子组件清单
			.Process(this->_childrenNames)

			// 存取Component自身的属性
			.Process(this->Name)
			.Process(this->Tag)
			// 每次读档之后，所有的Component实例都是重新创建的，不从存档中读取，只获取事件控制
			.Process(this->_awaked)
			.Process(this->_disable)
			.Process(this->_active)

			.Process(this->_break);
		return stream.Success();
	}
	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		bool loaded = this->Serialize(stream, true);
		// 根据子组件清单恢复
		RestoreComponent();
		// 读取每个子组件的内容
		this->ForeachChild([&stream, &registerForChange](Component* c) { c->Load(stream, registerForChange); });
		return loaded;
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component* pThis = const_cast<Component*>(this);
		// 生成子组件清单
		pThis->_childrenNames.clear();
		for (Component* c : pThis->_children)
		{
			pThis->_childrenNames.push_back(c->Name);
		}
		bool saved = pThis->Serialize(stream, false);
		// 存入每个子组件的内容
		pThis->ForeachChild([&stream](Component* c) {
			c->Save(stream);
			});
		return saved;
	}
#pragma endregion

protected:
	// Ext由ScriptFactory传入
	IExtData* _extData = nullptr;

	bool _awaked = false; // 已经完成初始化
	bool _disable = false; // 已经失效，等待移除
	bool _active = true; // 可以执行循环

	bool _break = false; // 中断上层循环

	// 添加的Component名单，在存档时生成
	std::vector<std::string> _childrenNames{};

	Component* _parent = nullptr;
	std::list<Component*> _children{};
};

class ComponentFactory
{
public:
	static ComponentFactory& GetInstance()
	{
		static ComponentFactory instance;
		return instance;
	}

	using ComponentCreator = std::function<Component* (void)>;

	int Register(const std::string& name, ComponentCreator creator)
	{
		_creatorMap.insert(make_pair(name, creator));
		Debug::Log("Registration Component \"%s\".\n", name.c_str());
		return 0;
	}

	Component* Create(const std::string& name)
	{
		auto it = _creatorMap.find(name);
		if (it != _creatorMap.end())
		{
			Component* c = it->second();
			c->Name = name;
#ifdef DEBUG_COMPONENT
			char c_this[1024];
			sprintf_s(c_this, "%p", c);
			std::string thisId = { c_this };
			Debug::Log("Create Component [%s]%s. \n", name.c_str(), thisId.c_str());
#endif // DEBUG
			return c;
		}
		return nullptr;
	}


private:
	ComponentFactory() {};
	~ComponentFactory() {};

	ComponentFactory(const ComponentFactory&) = delete;

	std::map<std::string, ComponentCreator> _creatorMap{};
};

static Component* CreateComponent(const std::string name)
{
	return ComponentFactory::GetInstance().Create(name);
}

