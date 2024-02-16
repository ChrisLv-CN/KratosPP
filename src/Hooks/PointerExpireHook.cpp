#include <exception>
#include <Windows.h>

#include <GeneralDefinitions.h>
#include <ObjectClass.h>
#include <FootClass.h>

#include <Extension.h>
#include <Helpers/Macro.h>

#include <Common/EventSystems/EventSystem.h>
/*
class PointerExpireHook
{
public:
	PointerExpireHook()
	{
		// EventSystems::General.AddHandler(Events::DetachAll, DetachAll);
		// EventSystems::General.AddHandler(Events::PointerExpireEvent, InvalidatePointer);
	}
};

static PointerExpireHook _pointerExpireHook;

DEFINE_HOOK(0x4101F0, AbstractClass_Delete, 0x6)
{
	GET(ObjectClass*, pObject, ECX);
	EventSystems::General.Broadcast(Events::PointerExpireEvent, &pObject);
	return 0;
}

// this function is a Object want Detach_All when Limbo or Delete
DEFINE_HOOK(0x7258D0, DetachThisFromAll, 0x6)
{
	GET(AbstractClass* const, pInvalid, ECX);
	GET(bool const, removed, EDX);
	void* args[] = { pInvalid, (void*)removed };

	// 该广播会执行三次
	// (01) DetachAll
	// (01) Limbo
	// (01) DetachAll
	// (02) DetachAll
	// (02) Delete
	EventSystems::General.Broadcast(Events::DetachAll, &args);

	return 0;
}
*/
