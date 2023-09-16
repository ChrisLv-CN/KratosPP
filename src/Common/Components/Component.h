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

class IComponent
{
public:
	IComponent() {};
	virtual ~IComponent() {};

	/// <summary>
	/// Awake is called when an enabled instance is being created.
	/// TechnoExt::ExtData() call
	/// </summary>
	virtual void Awake() { };

	/// <summary>
	/// OnStart called on the frame
	/// </summary>
	virtual void Start() { };

	virtual void OnUpdate() {};
	virtual void OnUpdateEnd() {};
	virtual void OnWarpUpdate() {};

	/// <summary>
	/// Destroy is called when enabled instance is delete.
	/// </summary>
	virtual void Destroy() { };

	virtual void InvalidatePointer(void* ptr) {};

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

	virtual ~Component() override
	{
#ifdef DEBUG_COMPONENT
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		std::string thisId2 = { t_this };
		Debug::Log("Component [%s]%s - %s is release.\n\n", thisName.c_str(), thisId.c_str(), thisId2.c_str());
#endif // DEBUG
	}

	virtual void OnUpdate() override;

	void EnsureAwaked();
	void EnsureStarted();
	void EnsureDestroy();

	bool AlreadyAwake();
	bool AlreadyStart();
	bool IsActive();

	void AddComponent(Component& component);
	// <summary>
	// 由Component自身调用GameObject的函数，将自己从GameObject中移除
	// </summary>
	void RemoveComponent(Component* component);

	/// <summary>
	/// Load存档后，初始化所有的component列表，需要
	/// 从_children中清理已经记录为失效的component
	/// </summary>
	void ClearDisableComponent();

	void AttachToComponent(Component* component);
	void DetachFromParent();

#pragma region Foreach
	/// <summary>
	/// execute action for each components in root (include itself)
	/// </summary>
	/// <param name="action"></param>
	void Foreach(std::function<void(Component*)> action);

	void ForeachChild(std::function<void(Component*)> action);

	/// <summary>
	/// execute action for each components in root (include root)
	/// </summary>
	/// <param name="root">the root component</param>
	/// <param name="action">the action to executed</param>
	void ForeachComponents(Component* root, std::function<void(Component*)> action);

	void ForeachComponents(std::list<Component*>& components, std::function<void(Component*)> action);
#pragma endregion

#pragma region GetComponent
	template <typename TComponent>
	TComponent* GetComponent()
	{
		return GetComponentInChildren<TComponent>();
	}

	template <typename TComponent>
	TComponent* GetComponentInParent()
	{
		Component* c = nullptr;
		// find first level
		for (Component& children : _children)
		{
			if (typeid(children) == TComponent)
			{
				c = &children;
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
			if (typeid(children) == typeid(TComponent))
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
#pragma endregion

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream, bool isLoad)
	{
		if (isLoad)
		{
			// 从存档读取需要被移除的Component的名单
			stream.Process(this->_disableComponents);
#ifdef DEBUG
			Debug::Log("Component [%s]%s is loading, has %d disable components, children has %d\n", this->thisName.c_str(), this->thisId.c_str(), _disableComponents.size(), _children.size());
#endif //DEBUG
			// 读入存档后，清理失效的Component
			ClearDisableComponent();
#ifdef DEBUG
			Debug::Log("Component [%s]%s is loading, clear disable done, has %d disable components, children has %d\n\n", this->thisName.c_str(), this->thisId.c_str(), _disableComponents.size(), _children.size());
#endif //DEBUG
		}
		else
		{
#ifdef DEBUG
			Debug::Log("Component [%s]%s is saveing, has %d disable components, children has %d\n\n", this->thisName.c_str(), this->thisId.c_str(), _disableComponents.size(), _children.size());
#endif //DEBUG
			// 需要被移除的Component的名单先写入存档
			stream.Process(this->_disableComponents);
		}
		return stream
			.Process(this->Name)
			// 每次读档之后，所有的Component实例都是重新创建的，不从存档中读取，只获取事件控制
			.Process(this->_awaked)
			.Process(this->_started)
			.Process(this->_children)
			.Success();
	}
	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		bool loaded = this->Serialize(stream, true);
		this->ForeachChild([&stream, &registerForChange](Component* c) { c->Load(stream, registerForChange); });
		return loaded;
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component* pThis = const_cast<Component*>(this);
		bool saved = pThis->Serialize(stream, false);
		pThis->ForeachChild([&stream](Component* c) {
			c->Save(stream);
		});
		return saved;
	}
#pragma endregion

private:
	bool _awaked = false;
	bool _started = false;
	bool _disable = false;

	// 读取存档时，所有的Component都是重新构建的，运行时失效的Component需要被记录
	std::vector<std::string> _disableComponents{};

	Component* _parent = nullptr;
	std::list<Component*> _children{};
};
