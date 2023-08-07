/*
	Cells
*/

#pragma once

#include <AbstractClass.h>

//forward declarations
class ObjectClass;
class TechnoClass;
class BuildingClass;
class BuildingTypeClass;
class UnitClass;
class InfantryClass;
class AircraftClass;
class TerrainClass;
class LightConvertClass;
class RadSiteClass;
class FootClass;
class TubeClass;
class FoggedObjectClass;
class TagClass;
class TiberiumClass;
class PixelFXClass;

class NOVTABLE CellClass : public AbstractClass
{
public:
	static const AbstractType AbsID = AbstractType::Cell;

	// the height of a bridge in leptons
	static const int BridgeLevels = 4;
	static const int BridgeHeight = BridgeLevels * Unsorted::LevelHeight;

	//IPersist
	virtual HRESULT __stdcall GetClassID(CLSID* pClassID) R0;

	//IPersistStream
	virtual HRESULT __stdcall Load(IStream* pStm) R0;
	virtual HRESULT __stdcall Save(IStream* pStm, BOOL fClearDirty) R0;

	//Destructor
	virtual ~CellClass() RX;

	//AbstractClass
	virtual AbstractType WhatAmI() const RT(AbstractType);
	virtual int Size() const R0;

	// non-virtual

	// get content objects
	TechnoClass* FindTechnoNearestTo(Point2D const& offsetPixel, bool alt, TechnoClass const* pExcludeThis = nullptr) const
		{ JMP_THIS(0x47C3D0); }

	ObjectClass* FindObjectOfType(AbstractType abs, bool alt) const
		{ JMP_THIS(0x47C4D0); }

	BuildingClass* GetBuilding() const
		{ JMP_THIS(0x47C520); }

	UnitClass* GetUnit(bool alt) const
		{ JMP_THIS(0x47EBA0); }

	InfantryClass* GetInfantry(bool alt) const
		{ JMP_THIS(0x47EC40); }

	AircraftClass* GetAircraft(bool alt) const
		{ JMP_THIS(0x47EBF0); }

	TerrainClass* GetTerrain(bool alt) const
		{ JMP_THIS(0x47C550); }

	/* craziest thing... first iterates Content looking to Aircraft,
	 * failing that, calls FindTechnoNearestTo,
	 * if that fails too, reiterates Content looking for Terrain
	 */
	ObjectClass* GetSomeObject(const CoordStruct& coords, bool alt) const
		{ JMP_THIS(0x47C5A0); }

	// misc
	void SetWallOwner()
		{ JMP_THIS(0x47D210); }

	bool IsShrouded() const
		{ JMP_THIS(0x487950); }

	void Unshroud()
		{ JMP_THIS(0x4876F0); }

	RectangleStruct* ShapeRect(RectangleStruct* pRet)
		{ JMP_THIS(0x47FDE0); }

	bool IsFogged() // Check Fog maybe?
		{ JMP_THIS(0x4879B0); }

	void CleanFog()
		{ JMP_THIS(0x486BF0); }

	void ClearFoggedObjects()
		{ JMP_THIS(0x486C50); }

	// adjusts LAT
	void SetupLAT()
		{ JMP_THIS(0x47CA80); }

	// checks for nearby cliff impassability, calls SetupLAT(), sets up TubeClass if tunnel, cell anim if attached, etc
	void Setup(DWORD dwUnk)
		{ JMP_THIS(0x47D2B0); }

	void BlowUpBridge()
		{ JMP_THIS(0x47DD70); }

	bool CanThisExistHere(SpeedType SpeedType, BuildingTypeClass* pObject, HouseClass* pOwner) const
		{ JMP_THIS(0x47C620); }

	// those unks are passed to TechnoClass::Scatter in that same order
	void ScatterContent(const CoordStruct &crd, bool ignoreMission, bool ignoreDestination, bool alt)
		{ JMP_THIS(0x481670); }

	CellClass* GetNeighbourCell(unsigned int direction) const
		{ JMP_THIS(0x481810); }

	// called whenever anything moves, first to remove threat from source cell, second time to add threat to dest cell
	void UpdateThreat(unsigned int SourceHouse, int ThreatLevel)
		{ JMP_THIS(0x481870); }

	void CollectCrate(FootClass* pCollector)
		{ JMP_THIS(0x481A00); }

	void ProcessColourComponents(int* arg0, int* pIntensity, int* pAmbient, int* a5, int* a6, int* tintR, int* tintG, int* tintB)
		{ JMP_THIS(0x484180); }

	TubeClass* GetTunnel()
		{ JMP_THIS(0x484F20); }

	RectangleStruct* GetContainingRect(RectangleStruct* dest) const
		{ JMP_THIS(0x47FB90); }

	// don't laugh, it returns the uiname of contained tiberium... which nobody ever sets
	const wchar_t* GetUIName() const
		{ JMP_THIS(0x484FF0); }

	// returns whether a cell behaves as if it contained overlay (for gates and wall towers)
	bool ConnectsToOverlay(int idxOverlay = -1, int direction = -1) const
		{ JMP_THIS(0x480510); }

	// returns the tiberium's index in OverlayTypes
	int GetContainedTiberiumIndex() const
		{ JMP_THIS(0x485010); }

	int GetContainedTiberiumValue() const
		{ JMP_THIS(0x485020); }

	bool SpreadTiberium(bool forced)
		{ JMP_THIS(0x483780); }

	// add or create tiberium of the specified type
	bool IncreaseTiberium(int idxTiberium, int amount)
		{ JMP_THIS(0x487190); }

	// decreases thze tiberium in the cell
	void ReduceTiberium(int amount)
		{ JMP_THIS(0x480A80); }

	bool CanTiberiumGerminate(TiberiumClass* tib)
		{ JMP_THIS(0x4838E0); }

	void SetMapCoords(const CoordStruct& coords)
		{ JMP_THIS(0x485240); }

	int GetFloorHeight(Point2D const& subcoords) const
		{ JMP_THIS(0x47B3A0); }

	// used by ambient waves and stuff
	CoordStruct* GetCenterCoords(CoordStruct* pOutBuffer) const
		{ JMP_THIS(0x480A30); }

	CoordStruct GetCenterCoords() const
	{
		CoordStruct buffer;
		GetCenterCoords(&buffer);
		return buffer;
	}

	void ActivateVeins()
		{ JMP_THIS(0x486920); }

	// cloak generators
	bool CloakGen_InclHouse(unsigned int idx) const
		{ return ((1 << idx) & this->CloakedByHouses) != 0; }

	void CloakGen_AddHouse(unsigned int idx)
		{ this->CloakedByHouses |= 1 << idx; }

	void CloakGen_RemHouse(unsigned int idx)
		{ this->CloakedByHouses &= ~(1 << idx); }

	// unused, returns 0 if that house doesn't have cloakgens covering this cell or Player has sensors over this cell
	bool DrawObjectsCloaked(int OwnerHouseIdx) const
		{ JMP_THIS(0x486800); }

	// sensors
	bool Sensors_InclHouse(unsigned int idx) const
		{ return this->SensorsOfHouses[idx] > 0; }

	void Sensors_AddOfHouse(unsigned int idx)
		{ ++this->SensorsOfHouses[idx]; }

	void Sensors_RemOfHouse(unsigned int idx)
		{ --this->SensorsOfHouses[idx]; }

	// disguise sensors
	bool DisguiseSensors_InclHouse(unsigned int idx) const
		{ return this->DisguiseSensorsOfHouses[idx] > 0; }

	void DisguiseSensors_AddOfHouse(unsigned int idx)
		{ ++this->DisguiseSensorsOfHouses[idx]; }

	void DisguiseSensors_RemOfHouse(unsigned int idx)
		{ --this->DisguiseSensorsOfHouses[idx]; }

	// Rad Sites
	void SetRadSite(RadSiteClass* pRad)
		{ this->RadSite = pRad; }

	RadSiteClass* GetRadSite() const
		{ return this->RadSite; }

	bool IsRadiated() const
		{ JMP_THIS(0x487C90); }

	int GetRadLevel() const
		{ JMP_THIS(0x487CB0); }

	void RadLevel_Increase(double amount)
		{ JMP_THIS(0x487CE0); }

	void RadLevel_Decrease(double amount)
		{ JMP_THIS(0x487D00); }

	// helper
	bool ContainsBridge() const
		{ return (this->Flags & cf_Bridge) != 0; }

	// helper mimicking game's behaviour
	ObjectClass* GetContent() const
		{ return this->ContainsBridge() ? this->AltObject : this->FirstObject; }

	int GetLevel() const
		{ return this->Level + (this->ContainsBridge() ? BridgeLevels : 0); }

	// tilesets
#define ISTILE(tileset, addr) \
	bool Tile_Is_ ## tileset() const \
		{ JMP_THIS(addr); }

	ISTILE(Tunnel, 0x484AB0);
	ISTILE(Water, 0x485060);
	ISTILE(Blank, 0x486380);
	ISTILE(Ramp, 0x4863A0);
	ISTILE(Cliff, 0x4863D0);
	ISTILE(Shore, 0x4865B0);
	ISTILE(Wet, 0x4865D0);
	ISTILE(MiscPave, 0x486650);
	ISTILE(Pave, 0x486670);
	ISTILE(DirtRoad, 0x486690);
	ISTILE(PavedRoad, 0x4866D0);
	ISTILE(PavedRoadEnd, 0x4866F0);
	ISTILE(PavedRoadSlope, 0x486710);
	ISTILE(Median, 0x486730);
	ISTILE(Bridge, 0x486750);
	ISTILE(WoodBridge, 0x486770);
	ISTILE(ClearToSandLAT, 0x486790);
	ISTILE(Green, 0x4867B0);
	ISTILE(NotWater, 0x4867E0);
	ISTILE(DestroyableCliff, 0x486900);

	static CoordStruct Cell2Coord(const CellStruct &cell, int z = 0)
	{
		CoordStruct ret;
		ret.X = cell.X * 256 + 128;
		ret.Y = cell.Y * 256 + 128;
		ret.Z = z;
		return ret;
	}

	static CellStruct Coord2Cell(const CoordStruct &crd)
	{
		CellStruct ret;
		ret.X = static_cast<short>(crd.X / 256);
		ret.Y = static_cast<short>(crd.Y / 256);
		return ret;
	}

	CoordStruct FixHeight(CoordStruct crd) const
	{
		if(this->ContainsBridge()) {
			crd.Z += BridgeHeight;
		}
		return crd;
	}

	// helper - gets coords and fixes height for bridge
	CoordStruct GetCoordsWithBridge() const
	{
		CoordStruct buffer =  this->GetCoords();
		return FixHeight(buffer);
	}

	void MarkForRedraw()
		{ JMP_THIS(0x486E70); }

	void ChainReaction() {
		CellStruct* cell = &this->MapCoords;
		SET_REG32(ecx, cell);
		CALL(0x489270);
	}

	CoordStruct* FindInfantrySubposition(CoordStruct* pOutBuffer, const CoordStruct& coords, bool ignoreContents, bool alt, bool useCellCoords)
		{ JMP_THIS(0x481180); }

	CoordStruct FindInfantrySubposition(const CoordStruct& coords, bool ignoreContents, bool alt, bool useCellCoords) {
		CoordStruct outBuffer;
		this->FindInfantrySubposition(&outBuffer, coords, ignoreContents, alt, useCellCoords);
		return outBuffer;
	}

	bool TryAssignJumpjet(FootClass* pObject)
		{ JMP_THIS(0x487D70); }

	void  AddContent(ObjectClass* Content, bool onBridge)
		{ JMP_THIS(0x47E8A0); }

	void  RemoveContent(ObjectClass* pContent, bool onBridge)
		{ JMP_THIS(0x47EA90); }

	void ReplaceTag(TagClass* pTag)
		{ JMP_THIS(0x485250) }

	void InitLightConvert(int Red2, int nIntensity, int nAmbient, int Red1, int Green1, int Blue1)
		{ JMP_THIS(0x483E30); }

	void DrawOverlay(Point2D& Location, RectangleStruct& Bound)
		{ JMP_THIS(0x47F6A0); }

	void DrawOverlayShadow(Point2D& Location, RectangleStruct& Bound)
		{ JMP_THIS(0x47F510); }

protected:
	//Constructor
	CellClass() noexcept
		: CellClass(noinit_t())
	{ JMP_THIS(0x47BBF0); }

	explicit __forceinline CellClass(noinit_t) noexcept
		: AbstractClass(noinit_t())
	{ }

	//===========================================================================
	//===== Properties ==========================================================
	//===========================================================================

public:

	CellStruct MapCoords;	//Where on the map does this Cell lie?
	DynamicVectorClass<FoggedObjectClass*>* FoggedObjects;
	CellClass*         BridgeOwnerCell;
	DWORD              unknown_30;
	LightConvertClass* LightConvert;
	int                IsoTileTypeIndex;	//What tile is this Cell?
	TagClass*          AttachedTag;			// The cell tag
	BuildingTypeClass* Rubble;				// The building type that provides the rubble image
	int                OverlayTypeIndex;	//What Overlay lies on this Cell?
	int                SmudgeTypeIndex;	//What Smudge lies on this Cell?

	DWORD              Passability;
	int                WallOwnerIndex; // Which House owns the wall placed in this Cell?
	                                              // Determined by finding the nearest BuildingType and taking its owner
	int                InfantryOwnerIndex;
	int                AltInfantryOwnerIndex;
	DWORD              unknown_5C;
	DWORD              unknown_60;
	DWORD              RedrawFrame;
	RectangleStruct    InViewportRect;
	DWORD              CloakedByHouses;	//Is this cell in a cloak generator's radius? One bit per House.

	// Is this cell in range of some SensorsSight= equipment? One Word(!) per House, ++ and -- per unit.
protected:
	unsigned short               SensorsOfHouses[0x18]; // ! 24 houses instead of 32 like cloakgen
	// use Sensors_ funcs above

	// Is this cell in range of some DetectDisguise= equipment? One Word(!) per House, ++ and -- per unit.
protected:
	unsigned short               DisguiseSensorsOfHouses[0x18]; // ! 24 houses instead of 32 like cloakgen
	// use DisguiseSensors_ funcs above

public:

	DWORD              BaseSpacerOfHouses; // & (1 << HouseX->ArrayIndex) == base spacing dummy for HouseX
	FootClass*         Jumpjet; // a jumpjet occupying this cell atm

	ObjectClass*       FirstObject;	//The first Object on this Cell. NextObject functions as a linked list.
	ObjectClass*       AltObject;

	LandType           LandType;	//What type of floor is this Cell?
	double             RadLevel;	//The level of radiation on this Cell.
	RadSiteClass*      RadSite;	//A pointer to the responsible RadSite.

	PixelFXClass*      PixelFX;
	int                OccupyHeightsCoveringMe;
	DWORD              Intensity;
	WORD               Ambient;
	//ColorStruct      Color1; //10A-10E
	WORD               Color1_Red;
	WORD               Color1_Green;
	WORD               Color1_Blue;
	//ColorStruct      Color2; //110-114
	WORD               Color2_Red;
	WORD               Color2_Green;
	WORD               Color2_Blue;
	signed short       TubeIndex; // !@#% Westwood braindamage, can't use > 127! (movsx eax, al)

	char               unknown_118;
	char               IsIceGrowthAllowed;
	char               Height;
	char               Level;

	BYTE               SlopeIndex;  // this + 2 == cell's slope shape as reflected by PLACE.SHP
	BYTE               unknown_11D;

	unsigned char      Powerup;	//The crate type on this cell. Also indicates some other weird properties

	BYTE               SmudgeData;
	char               Visibility; // trust me, you don't wanna know... if you do, see 0x7F4194 and cry
	char               Foggedness; // same value as above: -2: Occluded completely, -1: Visible, 0...48: frame in fog.shp or shroud.shp
	BYTE               BlockedNeighbours; // number of somehow occupied cells next to this
	PROTECTED_PROPERTY(BYTE, align_123);
	DWORD              OccupationFlags; // 0x1F: infantry subpositions: center, TL, TR, BL, BR
	DWORD              AltOccupationFlags; // 0x20: Units, 0x40: Objects, Aircraft, Overlay, 0x80: Building

	eCellFlags_12C     CopyFlags;	// related to Flags below
	int                ShroudCounter;
	DWORD              GapsCoveringThisCell; // actual count of gapgens in this cell, no idea why they need a second layer
	bool               VisibilityChanged;
	PROTECTED_PROPERTY(BYTE,     align_139[0x3]);
	DWORD              unknown_13C;

	eCellFlags         Flags;	//Various settings.
	PROTECTED_PROPERTY(BYTE,     padding_144[4]);
};
