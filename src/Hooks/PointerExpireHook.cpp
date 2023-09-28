#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Helpers/Macro.h>
#include <Common/EventSystems/EventSystem.h>

class PointerExpireHook
{
public:
	PointerExpireHook()
	{
		EventSystems::General.AddHandler(Events::PointerExpireEvent, InvalidatePointer);
	}
};

static PointerExpireHook _pointerExpireHook;

DEFINE_HOOK(0x7258D0, AnnounceInvalidPointer, 0x6)
{
	GET(AbstractClass* const, pInvalid, ECX);
	GET(bool const, removed, EDX);
	void* args[] = { pInvalid, (void*)removed };
	EventSystems::General.Broadcast(Events::PointerExpireEvent, &args);

	return 0;
}
