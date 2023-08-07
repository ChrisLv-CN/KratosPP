#pragma once

#include <ObjectClass.h>

//forward declarations
class IsometricTileTypeClass;

class NOVTABLE IsometricTileClass : public ObjectClass
{
public:
	static const AbstractType AbsID = AbstractType::Isotile;

	//Array
	static constexpr constant_ptr<DynamicVectorClass<IsometricTileClass*>, 0x87F750u> Array{};

	//IPersist
	virtual HRESULT __stdcall GetClassID(CLSID* pClassID) override R0;

	virtual HRESULT __stdcall Load(IStream* pStm) override R0;
	virtual HRESULT __stdcall Save(IStream* pStm, BOOL fClearDirty) override R0;

	//AbstractClass
	virtual AbstractType WhatAmI() const override RT(AbstractType);
	virtual int Size() const override R0;

	//ObjectClass
	virtual ObjectTypeClass* GetType() const override R0;
	virtual bool Limbo() override R0;
	virtual bool Unlimbo(const CoordStruct& Crd, Direction::Value dFaceDir) override R0;
	virtual void Draw(Point2D* pLocation, RectangleStruct* pBounds) const override RX;

	//Destructor
	virtual ~IsometricTileClass() RX;

	//Constructor
	IsometricTileClass(int idxType, CellStruct const& location) noexcept
		: IsometricTileClass(noinit_t())
	{ JMP_THIS(0x543780); }

protected:
	explicit __forceinline IsometricTileClass(noinit_t) noexcept
		: ObjectClass(noinit_t())
	{ }

	//===========================================================================
	//===== Properties ==========================================================
	//===========================================================================

public:
	IsometricTileTypeClass* Type;
};
