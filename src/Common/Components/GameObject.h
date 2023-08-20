#pragma once

#include <list>
#include <vector>

#include "Component.h"

class GameObject : public Component
{
public:
	GameObject();
	GameObject(std::string name);

	GameObject* GetAwaked();
private:
	//std::vector<Component*> _unstartedComponents;
};

