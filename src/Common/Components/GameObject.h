#pragma once

#include <list>
#include <vector>

#include <Common/Components/Component.h>

class GameObject : public Component
{
public:
	GameObject() : Component()
	{

	}
	GameObject(std::string name) : Component()
	{
		this->Name = name;
	}
private:
	//std::vector<Component*> _unstartedComponents;
};

