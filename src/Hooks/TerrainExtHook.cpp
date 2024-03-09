#include <exception>
#include <Windows.h>

#include <TerrainClass.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/TerrainTypeExt.h>

#include <Ext/TerrainType/TerrainDestroyAnim.h>

// DEFINE_HOOK(0x71C94C, TerrainClass_Remove_PlayDestroyAnim, 0xA)
DEFINE_HOOK(0x71BB2C, TerrainClass_TakeDamage_NowDead_Add, 0x6) // from Phobos
{
	GET(TerrainClass*, pTerrain, ESI);
	TerrainDestroyAnim::PlayDestroyAnim(pTerrain);
	return 0;
}
