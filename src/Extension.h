#pragma once

#include <utility>

#include <Common.h>

#include <LoadOptionsClass.h>
#include <Helpers/Macro.h>

#include <Common/EventSystems/EventSystem.h>
#include <Utilities/Container.h>


void ExtTypeRegistryClear(EventSystem* sender, Event e, void* args);
void InvalidatePointer(EventSystem* sender, Event e, void* args);
