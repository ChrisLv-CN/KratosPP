#pragma once

#include <typeinfo>
#include <string>
#include <algorithm>
#include <vector>

#include <Utilities/Stream.h>

class Component;

class IComponent
{
public:
	IComponent() { };
	virtual ~IComponent() { };

	/// <summary>
	/// Awake is called when an enabled instance is being created.
	/// TechnoExt::ExtData() call
	/// </summary>
	virtual void Awake() { };

	/// <summary>
	/// OnStart called on the frame
	/// </summary>
	virtual void Start() { };

	virtual void OnUpdate() { };
	virtual void OnLateUpdate() { };
	virtual void OnWarpUpdate() { };

	/// <summary>
	/// OnDestroy is called when enabled instance is delete.
	/// </summary>
	virtual void OnDestroy() { };

	virtual void Serialize(StreamWorkerBase& stream) { };

	virtual void LoadFromStream(ExStreamReader& stream)
	{
		this->Serialize(stream);
	};

	virtual void SaveToStream(ExStreamWriter& stream)
	{
		this->Serialize(stream);
	};
};

class Component : public IComponent
{
public:
	std::string Name;
	std::string Tag;

	__declspec(property(get = GetParent)) Component* Parent;
	__declspec(property(get = GetRoot)) Component* Root;

	Component* GetParent();
	Component* GetRoot();

	void AttachToComponent(Component component);
	void DetachFromParent();

	template <typename TComponent> TComponent* GetComponent();
	template <typename TComponent> TComponent* GetComponentInParent();
	template <typename TComponent> TComponent* GetComponentInChildren();

	std::vector<Component*> GetComponentsInChildren();

	/// <summary>
	/// execute action for each components in root (include itself)
	/// </summary>
	/// <param name="action"></param>
	template<typename T>
	void Foreach(T action)
	{
		ForeachComponents(this, action);
	}

	template<typename T>
	void ForeachChild(T action)
	{
		ForeachComponents(GetComponentsInChildren(), action);
	}
	
	template<typename T>
	static void ForeachComponents(std::vector<Component*> components, T action)
	{
		for (Component* compoent : components)
		{
			action(compoent);
		}
	}

	/// <summary>
	/// execute action for each components in root (include root)
	/// </summary>
	/// <param name="root">the root component</param>
	/// <param name="action">the action to executed</param>
	template<typename T>
	static void ForeachComponents(Component* root, T action)
	{
		action(root);
		root->ForeachChild(action);
	}

	void EnsureAwaked();
	void EnsureStarted();
	void Destroy();

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
private:
	Component* _parent;
	std::vector<Component*> _children{};
	bool _awaked = false;
	bool _started = false;
};

