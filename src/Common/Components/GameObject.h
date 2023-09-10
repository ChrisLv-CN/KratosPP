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

#pragma region save/load
	template <typename T>
	void Serialize(T &stream)
	{ }

	virtual void LoadFromStream(ExStreamReader &stream) override
	{
		Component::LoadFromStream(stream);
		this->Serialize(stream);
	}
	virtual void SaveToStream(ExStreamWriter &stream) override
	{
		Component::SaveToStream(stream);
		this->Serialize(stream);
	}
#pragma endregion

	GameObject *GetAwaked();

	void AddComponentNotAwake(Component* component);

	CMultiDelegate<void> _OnAwake;

private:
	std::vector<Component*> _unstartedComponents{};
};
