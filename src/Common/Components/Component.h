#pragma once

#include <typeinfo>
#include <string>
#include <algorithm>
#include <vector>

class Component;
class GameObject;

typedef void (*ComponentAction)(Component*);

class ComponentBase
{
public:
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
	virtual void OnRender() { };
	virtual void OnRenderEnd() { };
	virtual void OnDestroy() { };

	template<typename T>
	void Serialize(T& stream) { };

	template<typename T>
	void SaveToStream(T& stream) { };

	template<typename T>
	void LoadFromStream(T& stream) { };
};

class Component : public ComponentBase
{
public:
	std::string Name;
	std::string Tag;

	__declspec(property(get = GetParent)) Component* Parent;
	__declspec(property(get = GetRoot)) Component* Root;
	__declspec(property(get = GetGameObject)) GameObject* _GameObject;

	Component* GetParent();
	Component* GetRoot();
	GameObject* GetGameObject();

	void AttachToComponent(Component component);
	void DetachFromParent();

	template <typename TComponent> TComponent* GetComponent();
	template <typename TComponent> TComponent* GetComponentInParent();
	template <typename TComponent> TComponent* GetComponentInChildren();

	std::vector<Component*> GetComponentsInChildren();

	void Foreach(ComponentAction action);
	void ForeachChild(ComponentAction action);
	
	static void ForeachComponents(std::vector<Component*> components, ComponentAction action);
	static void ForeachComponents(Component* root, ComponentAction action);
//protected:
	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
private:
	Component* _parent;
	std::vector<Component*> _children;
	bool _awaked;
	bool _started;
};

