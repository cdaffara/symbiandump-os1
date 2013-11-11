/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
 @publishedPartner
 @released
*/

#if (!defined METADATA_H)
#define METADATA_H

#include <e32base.h>
#include <e32std.h>
#include <elements/cftransportmacro.h>
#include <elements/h_tdeallocator.h>

/**
Macro used to declare virtual functions from MMetaData interface and the virtual data table
*/
#define TYPEID_TABLE \
 /** @internalComponent */ \
 static inline const Meta::STypeId TypeId() { const Meta::SVDataTableEntry* entry = GetVDataTableStatic(); return Meta::STypeId::CreateSTypeId(entry->iOffset,reinterpret_cast<TInt32>(entry->iMetaNewL)); } \
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry* GetVDataTableStatic(); \
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry iVDataTable[];

#define EXPORT_TYPEID_TABLE \
 /** @internalComponent */ \
 static inline const Meta::STypeId TypeId() { const Meta::SVDataTableEntry* entry = GetVDataTableStatic(); return Meta::STypeId::CreateSTypeId(entry->iOffset,reinterpret_cast<TInt32>(entry->iMetaNewL)); } \
 /** @internalComponent */ \
 IMPORT_C static const Meta::SVDataTableEntry* GetVDataTableStatic(); \
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry iVDataTable[];

#define DATA_VTABLE \
 /** @internalComponent */ \
 static inline const Meta::STypeId TypeId() { const Meta::SVDataTableEntry* entry = GetVDataTableStatic(); return Meta::STypeId::CreateSTypeId(entry->iOffset,reinterpret_cast<TInt32>(entry->iMetaNewL)); } \
 /** @internalComponent */ \
 virtual Meta::SVDataTableEntry const* GetVDataTable() const;	\
 /** @internalComponent */ \
 virtual TUint8* GetAttribPtr(const TInt aOffset) const;	\
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry* GetVDataTableStatic(); \
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry iVDataTable[];

#define EXPORT_DATA_VTABLE \
 /** @internalComponent */ \
 static inline const Meta::STypeId TypeId() { const Meta::SVDataTableEntry* entry = GetVDataTableStatic(); return Meta::STypeId::CreateSTypeId(entry->iOffset,reinterpret_cast<TInt32>(entry->iMetaNewL)); } \
 /** @internalComponent */ \
 virtual Meta::SVDataTableEntry const* GetVDataTable() const;	\
 /** @internalComponent */ \
 virtual TUint8* GetAttribPtr(const TInt aOffset) const;	\
 /** @internalComponent */ \
 IMPORT_C static const Meta::SVDataTableEntry* GetVDataTableStatic(); \
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry iVDataTable[];

#define EXPORT_DATA_VTABLE_AND_FN \
 /** @internalComponent */ \
 static inline const Meta::STypeId TypeId() { const Meta::SVDataTableEntry* entry = GetVDataTableStatic(); return Meta::STypeId::CreateSTypeId(entry->iOffset,reinterpret_cast<TInt32>(entry->iMetaNewL)); } \
 /** @internalComponent */ \
 IMPORT_C virtual Meta::SVDataTableEntry const* GetVDataTable() const;	\
 /** @internalComponent */ \
 IMPORT_C virtual TUint8* GetAttribPtr(const TInt aOffset) const;	\
 /** @internalComponent */ \
 IMPORT_C static const Meta::SVDataTableEntry* GetVDataTableStatic(); \
 /** @internalComponent */ \
 static const Meta::SVDataTableEntry iVDataTable[];

/**
Macros used to implement virtual functions from MMetaData interface and construct the v data table
*/
#define START_ATTRIBUTE_TABLE( thisMetaClass, uid, typeId ) \
 /** @internalComponent */ \
 Meta::SVDataTableEntry const* thisMetaClass::GetVDataTable() const { return &iVDataTable[0]; };	\
 /** @internalComponent */ \
 TUint8* thisMetaClass::GetAttribPtr(const TInt aOffset) const { return (TUint8*)this + aOffset; };	\
 /** @internalComponent */ \
 const Meta::SVDataTableEntry* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) },

#define EXPORT_START_ATTRIBUTE_TABLE( thisMetaClass, uid, typeId ) \
 /** @internalComponent */ \
 Meta::SVDataTableEntry const* thisMetaClass::GetVDataTable() const { return &iVDataTable[0]; };	\
 /** @internalComponent */ \
 TUint8* thisMetaClass::GetAttribPtr(const TInt aOffset) const { return (TUint8*)this + aOffset; };	\
 /** @internalComponent */ \
 EXPORT_C const Meta::SVDataTableEntry* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) },

#define EXPORT_START_ATTRIBUTE_TABLE_AND_FN( thisMetaClass, uid, typeId ) \
 /** @internalComponent */ \
 EXPORT_C Meta::SVDataTableEntry const* thisMetaClass::GetVDataTable() const { return &iVDataTable[0]; };	\
 /** @internalComponent */ \
 EXPORT_C TUint8* thisMetaClass::GetAttribPtr(const TInt aOffset) const { return (TUint8*)this + aOffset; };	\
 /** @internalComponent */ \
 EXPORT_C const Meta::SVDataTableEntry* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) },

#define REGISTER_ATTRIBUTE( thisMetaClass, var, metaType ) \
 { _FOFF( thisMetaClass, var ), Meta::metaType::NewL },

#define END_ATTRIBUTE_TABLE() \
 { 0, NULL }};

#define END_ATTRIBUTE_TABLE_BASE( baseMetaClass, baseId ) \
 { (TInt)((TAny*)(baseMetaClass::GetVDataTableStatic)), NULL }};

#define REGISTER_TYPEID( thisMetaClass, uid, typeId ) \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) }, \
 { 0, NULL }};

#define EXPORT_REGISTER_TYPEID( thisMetaClass, uid, typeId ) \
 /** @internalComponent */ \
 EXPORT_C const Meta::SVDataTableEntry* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) }, \
 { 0, NULL }};

#define EXPORT_START_ATTRIBUTE_TABLE_NO_FN( thisMetaClass, uid, typeId ) \
 /** @internalComponent */ \
 EXPORT_C const Meta::SVDataTableEntry* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 /** @internalComponent */ \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) },


namespace Meta
{
//Panics
_LIT (MetaPanic,"MetaDataPanic");
const TInt EBadCast = 1;

struct SMetaData;
typedef Elements::TDeAllocator<const SMetaData> TMetaDataDeAllocator;

class MMetaType;
typedef MMetaType* (*TMetaTypeNewL)(const TAny*, const TAny*);

struct SVDataTableEntry
/**

Single entry of a virtual data table

@publishedPartner
@released since v9.0 */
    {
    TInt iOffset;
    TMetaTypeNewL iMetaNewL;
    };

const TInt32 KNetMetaTypeAny = 0;

struct STypeId
/**

Used to uniquely identify the type of a (meta).object.

*/
	{
	TUid iUid;
	TInt32 iType;
	inline TBool operator==(const STypeId& obj) const { return iUid==obj.iUid && iType==obj.iType; };
	inline TBool operator!=(const STypeId& obj) const { return iUid!=obj.iUid || iType!=obj.iType; };
	static inline STypeId CreateSTypeId();
	static inline STypeId CreateSTypeId(TUint32 aUid, TUint32 aTypeId);
	IMPORT_C static STypeId CreateSTypeId(TPtrC8& aStore);
	IMPORT_C TInt Check( TPtrC8& aDes ) const;
	};

class MMetaData
/**

MetaData interface

@publishedPartner
@released since v9.0 */
	{
public:
	virtual SVDataTableEntry const* GetVDataTable() const = 0;
	virtual TUint8* GetAttribPtr(const TInt aOffset) const = 0;
	};

struct SMetaData : public MMetaData
/**

Meta object's base type

Partially implements the MMetaData interface's pure virtual functions

*/
   {
public:
	IMPORT_C virtual ~SMetaData();

	//Checks the exact type for match and advances the pointer if matched
	IMPORT_C TInt Check( TPtrC8& aDes ) const;
	//Checks the exact type and base types for match and advances the pointer if matched
	IMPORT_C TInt CheckTypeOf( TPtrC8& aDes ) const;
	IMPORT_C STypeId GetTypeId() const;
	IMPORT_C TInt IsTypeOf( const STypeId& aTypeId ) const;

	IMPORT_C void Copy(const SMetaData& aSource);
	IMPORT_C TInt Store(TDes8& aDes) const;
	IMPORT_C TInt Load(TPtrC8& aDes);

	IMPORT_C TInt Length() const;

protected:
	IMPORT_C explicit SMetaData();
   };

struct SMetaDataECom : public SMetaData
/**
Meta object based on ECom factory base type
Implements static LoadL(TPtrC8& aDes) required by templated
*/
	{
public:
	IMPORT_C static SMetaDataECom* LoadL(TPtrC8& aDes);
    IMPORT_C static SMetaDataECom* NewInstanceL(const STypeId& aTypeId);
	IMPORT_C virtual ~SMetaDataECom();

protected:
    IMPORT_C explicit SMetaDataECom();

private:
	TUid iDestroyUid;
	};

} //namespace Meta


template <class SMETADATATYPE>
SMETADATATYPE& smetadata_cast(Meta::SMetaData& aObject)
    {
    __ASSERT_DEBUG(aObject.IsTypeOf(Meta::STypeId::CreateSTypeId(SMETADATATYPE::EUid, SMETADATATYPE::ETypeId)),User::Panic(Meta::MetaPanic, Meta::EBadCast));
    return static_cast<SMETADATATYPE&>(aObject);
    }

template <class SMETADATATYPE>
const SMETADATATYPE& smetadata_cast(const Meta::SMetaData& aObject)
    {
    __ASSERT_DEBUG(aObject.IsTypeOf(Meta::STypeId::CreateSTypeId(SMETADATATYPE::EUid, SMETADATATYPE::ETypeId)),User::Panic(Meta::MetaPanic, Meta::EBadCast));
    return static_cast<const SMETADATATYPE&>(aObject);
    }

template <class SMETADATATYPE>
SMETADATATYPE* smetadata_cast(Meta::SMetaData* aObject)
    {
    __ASSERT_DEBUG(aObject,User::Panic(Meta::MetaPanic, Meta::EBadCast));
    return aObject->IsTypeOf(Meta::STypeId::CreateSTypeId(SMETADATATYPE::EUid, SMETADATATYPE::ETypeId)) ? static_cast<SMETADATATYPE*>(aObject) : NULL;
    }

template <class SMETADATATYPE>
const SMETADATATYPE* smetadata_cast(const Meta::SMetaData* aObject)
    {
    __ASSERT_DEBUG(aObject,User::Panic(Meta::MetaPanic, Meta::EBadCast));
    return aObject->IsTypeOf(Meta::STypeId::CreateSTypeId(SMETADATATYPE::EUid, SMETADATATYPE::ETypeId)) ? static_cast<const SMETADATATYPE*>(aObject) : NULL;
    }

#include <elements/metadata.inl>

#endif //METADATA_H


