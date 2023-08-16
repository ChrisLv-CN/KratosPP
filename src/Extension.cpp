#include <Extension.h>

/*
#include <Ext/Aircraft/Body.h>
#include <Ext/AnimType/Body.h>
#include <Ext/Anim/Body.h>
#include <Ext/Building/Body.h>
#include <Ext/BuildingType/Body.h>
#include <Ext/Bullet/Body.h>
#include <Ext/BulletType/Body.h>
#include <Ext/House/Body.h>
#include <Ext/RadSite/Body.h>
#include <Ext/Rules/Body.h>
#include <Ext/Scenario/Body.h>
#include <Ext/Script/Body.h>
#include <Ext/Side/Body.h>
#include <Ext/SWType/Body.h>
#include <Ext/TAction/Body.h>
#include <Ext/Team/Body.h>
#include <Ext/Techno/Body.h>
#include <Ext/TechnoType/Body.h>
#include <Ext/TerrainType/Body.h>
#include <Ext/Tiberium/Body.h>
#include <Ext/VoxelAnim/Body.h>
#include <Ext/VoxelAnimType/Body.h>
#include <Ext/WarheadType/Body.h>
#include <Ext/WeaponType/Body.h>

#include <New/Type/RadTypeClass.h>
#include <New/Type/LaserTrailTypeClass.h>
#include <New/Type/DigitalDisplayTypeClass.h>
*/


#pragma region Implementation details

#pragma region Concepts

// a hack to check if some type can be used as a specialization of a template
template <template <class...> class Template, class... Args>
void DerivedFromSpecialization(const Template<Args...>&);

template <class T, template <class...> class Template>
concept DerivedFromSpecializationOf =
	requires(const T & t) { DerivedFromSpecialization<Template>(t); };

template<typename TExt>
concept HasExtMap = requires { { TExt::ExtMap } -> DerivedFromSpecializationOf<Container>; };

template <typename T>
concept Clearable = requires { T::Clear(); };

template <typename T>
concept PointerInvalidationSubscribable =
	requires (void* ptr, bool removed) { T::PointerGotInvalid(ptr, removed); };

template <typename T>
concept GlobalSaveLoadable = requires
{
	T::LoadGlobals(std::declval<PhobosStreamReader&>());
	T::SaveGlobals(std::declval<PhobosStreamWriter&>());
};

template <typename TAction, typename TProcessed, typename... ArgTypes>
concept DispatchesAction =
	requires (ArgTypes... args) { TAction::template Process<TProcessed>(args...); };

#pragma endregion

// calls:
// T::Clear()
// T::ExtMap.Clear()
struct ClearAction
{
	template <typename T>
	static bool Process()
	{
		if constexpr (Clearable<T>)
			T::Clear();
		else if constexpr (HasExtMap<T>)
			T::ExtMap.Clear();

		return true;
	}
};

// calls:
// T::PointerGotInvalid(void*, bool)
// T::ExtMap.PointerGotInvalid(void*, bool)
struct InvalidatePointerAction
{
	template <typename T>
	static bool Process(void* ptr, bool removed)
	{
		if constexpr (PointerInvalidationSubscribable<T>)
			T::PointerGotInvalid(ptr, removed);
		else if constexpr (HasExtMap<T>)
			T::ExtMap.PointerGotInvalid(ptr, removed);

		return true;
	}
};

// calls:
// T::LoadGlobals(PhobosStreamReader&)
struct LoadGlobalsAction
{
	template <typename T>
	static bool Process(IStream* pStm)
	{
		if constexpr (GlobalSaveLoadable<T>)
		{
			PhobosByteStream stm(0);
			stm.ReadBlockFromStream(pStm);
			PhobosStreamReader reader(stm);

			return T::LoadGlobals(reader) && reader.ExpectEndOfBlock();
		}
		else
		{
			return true;
		}
	}
};

// calls:
// T::SaveGlobals(PhobosStreamWriter&)
struct SaveGlobalsAction
{
	template <typename T>
	static bool Process(IStream* pStm)
	{
		if constexpr (GlobalSaveLoadable<T>)
		{
			PhobosByteStream stm;
			PhobosStreamWriter writer(stm);

			return T::SaveGlobals(writer) && stm.WriteBlockToStream(pStm);
		}
		else
		{
			return true;
		}
	}
};

// this is a complicated thing that calls methods on classes. add types to the
// instantiation of this type, and the most appropriate method for each type
// will be called with no overhead of virtual functions.
template <typename... RegisteredTypes>
struct TypeRegistry
{
	__forceinline static void Clear()
	{
		dispatch_mass_action<ClearAction>();
	}

	__forceinline static void InvalidatePointer(void* ptr, bool removed)
	{
		dispatch_mass_action<InvalidatePointerAction>(ptr, removed);
	}

	__forceinline static bool LoadGlobals(IStream* pStm)
	{
		return dispatch_mass_action<LoadGlobalsAction>(pStm);
	}

	__forceinline static bool SaveGlobals(IStream* pStm)
	{
		return dispatch_mass_action<SaveGlobalsAction>(pStm);
	}

private:
	// TAction: the method dispatcher class to call with each type
	// ArgTypes: the argument types to call the method dispatcher's Process() method
	template <typename TAction, typename... ArgTypes>
	requires (DispatchesAction<TAction, RegisteredTypes, ArgTypes...> && ...)
	__forceinline static bool dispatch_mass_action(ArgTypes... args)
	{
		// (pack expression op ...) is a fold expression which
		// unfolds the parameter pack into a full expression
		return (TAction::template Process<RegisteredTypes>(args...) && ...);
	}
};

#pragma endregion

// Add more class names as you like
using ExtTypeRegistry = TypeRegistry<
	/*
	// Ext classes
	AircraftExt,
	AnimTypeExt,
	AnimExt,
	BuildingExt,
	BuildingTypeExt,
	BulletExt,
	BulletTypeExt,
	HouseExt,
	RadSiteExt,
	RulesExt,
	ScenarioExt,
	ScriptExt,
	SideExt,
	SWTypeExt,
	TActionExt,
	TeamExt,
	TechnoExt,
	TechnoTypeExt,
	TerrainTypeExt,
	TiberiumExt,
	VoxelAnimExt,
	VoxelAnimTypeExt,
	WarheadTypeExt,
	WeaponTypeExt,
	// New classes
	ShieldTypeClass,
	LaserTrailTypeClass,
	RadTypeClass,
	ShieldClass,
	DigitalDisplayTypeClass
	// other classes
	*/
>;

void ExtTypeRegistryClear(EventSystem* sender, Event e, void* args)
{
	ExtTypeRegistry::Clear();
}

void InvalidatePointer(EventSystem* sender, Event e, void* args)
{
	auto const& argsArray = reinterpret_cast<void**>(args);
	AbstractClass* pInvalid = (AbstractClass*)argsArray[0];
	bool removed = argsArray[1];
	ExtTypeRegistry::InvalidatePointer(pInvalid, removed);
}

// Ares saves its things at the end of the save
// Phobos will save the things at the beginning of the save
// Considering how DTA gets the scenario name, I decided to save it after Rules - secsome

DEFINE_HOOK(0x67D32C, SaveGame_Phobos, 0x5)
{
	GET(IStream*, pStm, ESI);
	//UNREFERENCED_PARAMETER(pStm);
	ExtTypeRegistry::SaveGlobals(pStm);
	return 0;
}

DEFINE_HOOK(0x67E826, LoadGame_Phobos, 0x6)
{
	GET(IStream*, pStm, ESI);
	//UNREFERENCED_PARAMETER(pStm);
	ExtTypeRegistry::LoadGlobals(pStm);
	return 0;
}

DEFINE_HOOK(0x67D04E, Game_Save_SavegameInformation, 0x7)
{
	REF_STACK(SavegameInformation, Info, STACK_OFFSET(0x4A4, -0x3F4));
	Info.Version = Info.Version + SAVEGAME_ID;
	return 0;
}

DEFINE_HOOK(0x559F29, LoadOptionsClass_GetFileInfo, 0x8)
{
	if (!R->BL()) return 0x55A03D; // vanilla overridden check

	REF_STACK(SavegameInformation, Info, STACK_OFFSET(0x400, -0x3F4));
	Info.Version = Info.Version - SAVEGAME_ID;
	return 0x559F29 + 0x8;
}
