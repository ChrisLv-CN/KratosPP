#include "ScriptComponent.h"

#include <Extension/TechnoExt.h>


GameObject* TechnoScript::GetGameObject()
{
	return ((TechnoExt::ExtData*)ExtData)->_GameObject;
}
