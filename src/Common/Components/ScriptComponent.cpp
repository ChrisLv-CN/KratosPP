#include "ScriptComponent.h"

#include <Extension/TechnoExt.h>
#include <Extension/BulletExt.h>


GameObject* TechnoScript::GetGameObject()
{
	return ((TechnoExt::ExtData *)ExtData)->_GameObject;
}

GameObject* BulletScript::GetGameObject()
{
	return ((BulletExt::ExtData *)ExtData)->_GameObject;
}
