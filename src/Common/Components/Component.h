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

#define DECLARE_COMPONENT(CLASS_NAME, ...) \
	CLASS_NAME() : __VA_ARGS__() \
	{ \
		this->Name = ScriptName; \
	} \
	\
	inline static std::string ScriptName = #CLASS_NAME; \
	static Component* Create() \
	{ \
		Component* c = nullptr; \
		if (!Pool.empty()) \
		{ \
			auto it = Pool.begin(); \
			c = *it; \
			Pool.erase(it); \
		} \
		if (!c) \
		{ \
			c = static_cast<Component*>(new CLASS_NAME()); \
		} \
		return c; \
	} \
	\
	inline static int g_temp_##CLASS_NAME = \
	ComponentFactory::GetInstance().Register(#CLASS_NAME, CLASS_NAME::Create); \
	\
	inline static std::vector<CLASS_NAME*> Pool{}; \
	\
	virtual void FreeComponent() override \
	{ \
		Clean(); \
		Pool.push_back(this); \
	} \

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
	/// Clean is called when component instance back to object-pool or initialization.
	/// </summary>
	virtual void Clean() = 0;

	/// <summary>
	/// Awake is called when an enabled instance is being created.
	/// TechnoExt::ExtData() call
	/// </summary>
	virtual void Awake() {};

	/// <summary>
	/// Destroy is called when enabled instance is delete.
	/// </summary>
	virtual void Destroy() {};

	virtual void OnUpdate() {};
	virtual void OnUpdateEnd() {};
	virtual void OnWarpUpdate() {};

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

	virtual void Clean() override
	{
		// Ext由ScriptFactory传入
		_extData = nullptr;

		_awaked = false; // 已经完成初始化
		_disable = false; // 已经失效，等待移除
		_active = true; // 可以执行循环

		_break = false; // 中断上层循环

		// 添加的Component名单，在存档时生成
		_childrenNames.clear();

		_parent = nullptr;
		_children.clear();
	}

	virtual void OnUpdate() override;

	void EnsureAwaked();
	void EnsureDestroy();

	/// <summary>
	/// 重置组件，返回对象池
	/// </summary>
	virtual void FreeComponent() = 0;

	bool AlreadyAwake();

	/// <summary>
	/// 关闭组件，标记为失效，组件会在执行完Foreach后被移除
	/// </summary>
	void Disable();
	bool IsEnable();


	/// <summary>
	/// 激活组件，使其可以执行Foreach逻辑
	/// </summary>
	virtual void Activate();
	/// <summary>
	/// 失活组件，使其跳过执行Foreach逻辑
	/// </summary>
	virtual void Deactivate();
	virtual bool IsActive();

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
	template<typename F>
	void Foreach(F action)
	{
		// 执行全部
		int level = 0;
		int maxLevel = -1;
		ForeachLevel(action, level, maxLevel);

		OnForeachEnd();
	}

	template<typename F>
	void ForeachLevel(F action, int& level, int& maxLevel)
	{
		// 执行自身
		if (IsEnable() && IsActive())
		{
			Component* _this = this;
			action(_this);
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

	/// <summary>
	/// execute action for each child (exclude child's child)
	/// </summary>
	/// <param name="action"></param>
	template<typename F>
	void ForeachChild(F action, bool force = false)
	{
		for (Component* c : _children)
		{
			action(c);
			if (!force)
			{
				if (c->IsBreak())
				{
					break;
				}
			}
		}
	}

	void Break()
	{
		_break = true;
	}

	bool IsBreak()
	{
		if (_break)
		{
			_break = false;
			return true;
		}
		return _break;
	}
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

	Component& operator=(const Component& other)
	{
		if (this != &other)
		{
			Name = other.Name;
			Tag = other.Tag;
			// 只赋值控制位，不赋值上下层关系
			_awaked = other._awaked;
			_disable = other._disable;
			_active = other._active;
			_break = other._break;
		}
		return *this;
	}

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

	// using ComponentCreator = std::function<Component* (void)>;
	using ComponentCreator = Component * (*) (void);

	int Register(const std::string& name, ComponentCreator creator)
	{
		_creatorMap.insert(make_pair(name, creator));
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

	void PrintCreaterInfo()
	{
		if (!_creatorMap.empty())
		{
			Debug::Log("Component List:\n");
			for (auto it : _creatorMap)
			{
				std::string scriptName = it.first;
				Debug::Log(" -- %s\n", scriptName.c_str());
			}
		}
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

