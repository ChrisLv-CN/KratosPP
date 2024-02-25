#include <Extension.h>

#include <Extension/AnimExt.h>
#include <Extension/AnimTypeExt.h>
#include <Extension/BulletExt.h>
#include <Extension/BulletTypeExt.h>
#include <Extension/SuperWeaponExt.h>
#include <Extension/SuperWeaponTypeExt.h>
#include <Extension/TechnoExt.h>
#include <Extension/TechnoTypeExt.h>
#include <Extension/TerrainTypeExt.h>
#include <Extension/VoxelAnimExt.h>
#include <Extension/VoxelAnimTypeExt.h>
#include <Extension/WarheadTypeExt.h>
#include <Extension/WeaponTypeExt.h>

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
	requires (void* ptr) { T::PointerGotInvalid(ptr); };

template <typename T>
concept DetachSubscribable =
	requires (void* ptr, bool all) { T::ObjectWantDetach(ptr, all); };

template <typename T>
concept GlobalSaveLoadable = requires
{
	T::LoadGlobals(std::declval<ExStreamReader&>());
	T::SaveGlobals(std::declval<ExStreamWriter&>());
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
	static bool Process(void* ptr)
	{
		if constexpr (PointerInvalidationSubscribable<T>)
			T::PointerGotInvalid(ptr);
		else if constexpr (HasExtMap<T>)
			T::ExtMap.PointerGotInvalid(ptr);

		return true;
	}
};

// calls:
// T::Detach(void*, bool)
// T::ExtMap.Detach(void*, bool)
struct DetachAllAction
{
	template <typename T>
	static bool Process(void* ptr, bool all)
	{
		if constexpr (DetachSubscribable<T>)
			T::ObjectWantDetach(ptr, all);
		else if constexpr (HasExtMap<T>)
			T::ExtMap.ObjectWantDetach(ptr, all);

		return true;
	}
};

// calls:
// T::LoadGlobals(ExStreamReader&)
struct LoadGlobalsAction
{
	template <typename T>
	static bool Process(IStream* pStm)
	{
		if constexpr (GlobalSaveLoadable<T>)
		{
			ExByteStream stm(0);
			stm.ReadBlockFromStream(pStm);
			ExStreamReader reader(stm);

			return T::LoadGlobals(reader) && reader.ExpectEndOfBlock();
		}
		else
		{
			return true;
		}
	}
};

// calls:
// T::SaveGlobals(ExStreamWriter&)
struct SaveGlobalsAction
{
	template <typename T>
	static bool Process(IStream* pStm)
	{
		if constexpr (GlobalSaveLoadable<T>)
		{
			ExByteStream stm;
			ExStreamWriter writer(stm);

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

	__forceinline static void InvalidatePointer(void* ptr)
	{
		dispatch_mass_action<InvalidatePointerAction>(ptr);
	}

	__forceinline static void DetachAll(void* ptr, bool all)
	{
		dispatch_mass_action<DetachAllAction>(ptr, all);
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

	// Ext classes
	AnimTypeExt,
	AnimExt,
	BulletExt,
	BulletTypeExt,
	TechnoExt,
	TechnoTypeExt,
	TerrainTypeExt,
	VoxelAnimExt,
	VoxelAnimTypeExt,
	WarheadTypeExt,
	WeaponTypeExt
	// New classes
>;

void ExtTypeRegistryClear(EventSystem* sender, Event e, void* args)
{
	ExtTypeRegistry::Clear();
}
void InvalidatePointer(EventSystem* sender, Event e, void* args)
{
	ExtTypeRegistry::InvalidatePointer(args);
}

void DetachAll(EventSystem* sender, Event e, void* args)
{
	auto const& argsArray = reinterpret_cast<void**>(args);
	AbstractClass* pItem = (AbstractClass*)argsArray[0];
	bool all = argsArray[1];
	ExtTypeRegistry::DetachAll(pItem, all);
}

// Ares saves its things at the end of the save
// Phobos will save the things at the beginning of the save
// Considering how DTA gets the scenario name, I decided to save it after Rules - secsome

DEFINE_HOOK(0x67D32C, SaveGame_Ext, 0x5)
{
	GET(IStream*, pStm, ESI);
	//UNREFERENCED_PARAMETER(pStm);
	ExtTypeRegistry::SaveGlobals(pStm);
	return 0;
}

DEFINE_HOOK(0x67E826, LoadGame_Ext, 0x6)
{
	GET(IStream*, pStm, ESI);
	//UNREFERENCED_PARAMETER(pStm);
	ExtTypeRegistry::LoadGlobals(pStm);
	return 0;
}

// DEFINE_HOOK(0x67D04E, Game_Save_SavegameInformation, 0x7)
// {
// 	REF_STACK(SavegameInformation, Info, STACK_OFFSET(0x4A4, -0x3F4));
// 	Info.Version = Info.Version + SAVEGAME_ID;
// 	return 0;
// }

// DEFINE_HOOK(0x559F29, LoadOptionsClass_GetFileInfo, 0x8)
// {
// 	if (!R->BL()) return 0x55A03D; // vanilla overridden check

// 	REF_STACK(SavegameInformation, Info, STACK_OFFSET(0x400, -0x3F4));
// 	Info.Version = Info.Version - SAVEGAME_ID;
// 	return 0x559F29 + 0x8;
// }
