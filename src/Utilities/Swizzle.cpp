#include "Swizzle.h"

#include <Common.h>

#include <SwizzleManagerClass.h>

ExSwizzle ExSwizzle::Instance;

HRESULT ExSwizzle::RegisterForChange(void** p)
{
	return SwizzleManagerClass::Instance().Swizzle(p);
}

HRESULT ExSwizzle::RegisterChange(void* was, void* is)
{
	return SwizzleManagerClass::Instance().Here_I_Am((long)was, is);
}
