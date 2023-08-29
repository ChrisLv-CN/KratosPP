#pragma once

#include <list>
#include <vector>

#include "Component.h"

#include <Common/MyDelegate.h>


using namespace Delegate;

class GameObject : public Component
{
public:
	GameObject();
	GameObject(std::string name);

	virtual void Awake() override;

	virtual void OnUpdate() override;

	GameObject* GetAwaked();

	void AddComponentNotAwake(Component* component);

	CMultiDelegate<void> _OnAwake;
private:
	std::vector<Component*> _unstartedComponents{};
};

