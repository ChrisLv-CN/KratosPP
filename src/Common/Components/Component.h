#pragma once

#include <typeinfo>
#include <string>
#include <algorithm>
#include <vector>

class Component;

typedef void (*ComponentAction)(Component*);

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

	template<typename T>
	void Serialize(T& stream) { };

	template<typename T>
	void SaveToStream(T& stream) { };

	template<typename T>
	void LoadFromStream(T& stream) { };
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
	void Foreach(ComponentAction action);
	void ForeachChild(ComponentAction action);
	
	static void ForeachComponents(std::vector<Component*> components, ComponentAction action);
	/// <summary>
	/// execute action for each components in root (include root)
	/// </summary>
	/// <param name="root">the root component</param>
	/// <param name="action">the action to executed</param>
	static void ForeachComponents(Component* root, ComponentAction action);

	void EnsureAwaked();
	void EnsureStarted();
	void Destroy();
//protected:
	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
private:
	Component* _parent;
	std::vector<Component*> _children;
	bool _awaked;
	bool _started;
};

