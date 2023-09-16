#pragma once

#include <list>
#include <vector>

#include "Component.h"

#include <Common/MyDelegate.h>

using namespace Delegate;

class GameObject : public Component
{
public:
	GameObject() : Component()
	{
		this->Name = "GameObject";
#ifdef DEBUG
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		this->thisId = { t_this };
#endif // DEBUG
	}

#ifdef DEBUG_COMPONENT
	virtual ~GameObject() override
	{
		Debug::Log("GameObject [%s]%s is release.\n", this->thisName.c_str(), this->thisId.c_str());
	}
#endif // DEBUG

	virtual void Awake() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Success();
	}

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		// 清理_unstartedComponents
		return this->Serialize(stream);
	}

	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<GameObject*>(this)->Serialize(stream);
	}
#pragma endregion

	GameObject* GetAwaked();

	CMultiDelegate<void> _OnAwake;

private:
};
