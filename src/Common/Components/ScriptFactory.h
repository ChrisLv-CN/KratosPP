#pragma once

#include <string>
#include <typeinfo>
#include <functional>
#include <map>

#include "Component.h"

class IExtData;

class ScriptFactory
{
public:
	static ScriptFactory& GetInstance()
	{
		static ScriptFactory instance;
		return instance;
	}

	using ScriptCreator = std::function<Component* (IExtData*)>;

	int Register(const std::string& name, ScriptCreator creator)
	{
		_creatorMap.insert(make_pair(name, creator));
		Debug::Log("Registration script \"%s\".\n", name.c_str());
		return 0;
	}

	Component* Create(const std::string& name, IExtData* extData)
	{
		auto it = _creatorMap.find(name);
		if (it != _creatorMap.end())
		{
			Component* c = it->second(extData);
			c->Name = name;
#ifdef DEBUG
			char c_this[1024];
			sprintf_s(c_this, "%p", c);
			std::string thisId = { c_this };
			Debug::Log("Create Script [%s]%s. \n", name.c_str(), thisId.c_str());
#endif // DEBUG
			return c;
		}
		return nullptr;
	}

	static Component* CreateScript(const std::string name, IExtData* extData)
	{
		return ScriptFactory::GetInstance().Create(name, extData);
	}

	template<typename T>
	static T* CreateScript(IExtData* extData)
	{
		return dynamic_cast<T*>(ScriptFactory::GetInstance().Create(T::ScriptName, extData));
	}

private:
	ScriptFactory() {};
	~ScriptFactory() {};

	ScriptFactory(const ScriptFactory&) = delete;

	std::map<std::string, ScriptCreator> _creatorMap{};
};
