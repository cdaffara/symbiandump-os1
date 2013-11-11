// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file 
 @internalTechnology
*/

#include <ecom/ecom.h>
#include <elements/metadata.h>
#include <elements/metatype.h>
#include <elements/metaiterator.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNetMetaMtDt, "ElemNetMetaMtDt.");
#endif

// Maximum size of the MMetaType based object in terms of sizeof(TAny*).
// KMMetaTypeMaxSize * sizeof(TAny*) is the size of the memory reserved
// on the stack for the in-place instantiation.
const TInt KMMetaTypeMaxSize = 10;


using namespace Meta;

#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
EXPORT_C STypeId STypeId::CreateSTypeId(TPtrC8& aDes)
	{
	// check no padding, so no alignment worries in copying single block
	STypeId id;
	__ASSERT_COMPILE(sizeof(STypeId) == sizeof(id.iUid) + sizeof(id.iType));

	Mem::Copy(&id, aDes.Ptr(), sizeof(STypeId));
	
	// trim typeid from front of store
	aDes.Set(aDes.Ptr() + sizeof(STypeId), aDes.Length() - sizeof(STypeId));
	return id;
	}
#endif

EXPORT_C TInt STypeId::Check( TPtrC8& aDes ) const
	{
	if (aDes.Length() < (TInt)sizeof(STypeId))
		{
		return KErrArgument;
		}
	STypeId typeId;
	Mem::Copy(&typeId, aDes.Ptr(), sizeof(STypeId));
	TInt error = KErrArgument;
	if ( *this == typeId )
		{//update pointer (type) only if the type matches
		aDes.Set(aDes.Ptr()+sizeof(STypeId),aDes.Length()-sizeof(STypeId));
		error = KErrNone;
		}
	return  error;
	}

EXPORT_C SMetaData::SMetaData()
/**
 * Protected and explicit constructor
 */
    {
    }

EXPORT_C SMetaData::~SMetaData()
/**
 * Virtual destructor
 */
	{
	}

EXPORT_C TInt SMetaData::CheckTypeOf( TPtrC8& aDes ) const
	{
	if (aDes.Length() < (TInt)sizeof(STypeId))
		{
		return KErrArgument;
		}
	STypeId typeId;
	Mem::Copy(&typeId, aDes.Ptr(), sizeof(STypeId));
	TInt error = KErrArgument;
	if ( IsTypeOf(typeId) )
		{//update pointer (type) only if the type matches
		aDes.Set(aDes.Ptr()+sizeof(STypeId),aDes.Length()-sizeof(STypeId));
		error = KErrNone;
		}
	return  error;
	}

EXPORT_C STypeId SMetaData::GetTypeId() const
	{
	const SVDataTableEntry* entry = GetVDataTable();
	__ASSERT_DEBUG( entry , User::Panic(KSpecAssert_ElemNetMetaMtDt, 1));
	return STypeId::CreateSTypeId( entry->iOffset, reinterpret_cast<TInt32>(entry->iMetaNewL) );
	}

EXPORT_C TInt SMetaData::IsTypeOf( const STypeId& aTypeId ) const
	{
	const SVDataTableEntry* entry = GetVDataTable();
	__ASSERT_DEBUG( entry , User::Panic(KSpecAssert_ElemNetMetaMtDt, 2));
	for(;;)
		{
		STypeId typeId = { entry->iOffset, reinterpret_cast<TInt32>(entry->iMetaNewL) };
		if ( typeId == aTypeId )
			{
			return 1;
			}
		//move through the table
		while ( (++entry)->iMetaNewL ) {;}
		if ( !entry->iOffset )
			{
			break;
			}
		typedef SVDataTableEntry* (*TMetaBaseTableNewL)();
		entry = reinterpret_cast<TMetaBaseTableNewL>(entry->iOffset)();
		}
	return 0;
	}

EXPORT_C void SMetaData::Copy(const SMetaData& aSource)
/**
 * Copies values of a SMetaData object's properties into properties of *this.
 * Both meta objects are of the same type.
 */
    {
    __ASSERT_DEBUG(GetTypeId()==aSource.GetTypeId(),User::Panic(_L("SMetaData"),KErrArgument));

    TAny* mem[KMMetaTypeMaxSize];
    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry;
    while ((entry = attribIter++) != NULL)
        {
        MMetaType* metaType = entry->iMetaNewL(mem,GetAttribPtr(entry->iOffset));
        metaType->Copy(aSource.GetAttribPtr(entry->iOffset));
        }
    }

EXPORT_C TInt SMetaData::Store(TDes8& aDes) const
/**
 * Stores the meta object into a descriptor
 */
    {
	const STypeId& typeId = GetTypeId();
	TInt len = Length();

	if ((aDes.MaxLength()-aDes.Length()) < len)
		{
		return KErrOverflow;
		}

	aDes.Append((TUint8*)&typeId,sizeof(STypeId)); //store TID
	aDes.Append((TUint8*)&len,sizeof(TInt32)); //store length

    TAny* mem[KMMetaTypeMaxSize];
    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry;
    while ((entry = attribIter++) != NULL)
        {
        MMetaType* metaType = entry->iMetaNewL(mem,GetAttribPtr(entry->iOffset));
		TInt ret = metaType->Store(aDes);
		if (ret != KErrNone)
			{
			return ret;
			}
        }
	return KErrNone;
    }

EXPORT_C TInt SMetaData::Check(TPtrC8& aDes) const
	{
	return GetTypeId().Check(aDes);
	}

EXPORT_C TInt SMetaData::Load(TPtrC8& aDes)
/**
 * Loads the meta object from a descriptor
 */
    {
	if (aDes.Length() < (TInt)sizeof(TInt32))
		{
		return KErrArgument;
		}

	TUint32 length;
	Mem::Copy(&length, aDes.Ptr(), sizeof(TUint32));
	aDes.Set(aDes.Ptr()+sizeof(TUint32),aDes.Length()-sizeof(TUint32)); //update pointer (type)

	if (length > aDes.Length() + sizeof(TUint32) + sizeof(STypeId))
		{
		return KErrArgument;
		}

    TAny* mem[KMMetaTypeMaxSize];
    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry;
    while ((entry = attribIter++) != NULL)
        {
		//xxx This isn't a leaving function! It should be New not NewL
        MMetaType* metaType = entry->iMetaNewL(mem,GetAttribPtr(entry->iOffset));
        TInt ret = metaType->Load(aDes);
		if (ret != KErrNone)
			{
			return ret;
			}
        }
	return KErrNone;
    }

EXPORT_C TInt SMetaData::Length() const
/**
 * Returns length of the object excluding TID and stored length fields
 */
	{
	TInt length = sizeof(STypeId) + sizeof(TInt32);
    TAny* mem[KMMetaTypeMaxSize];
    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry;
    while ((entry = attribIter++) != NULL)
        {
        MMetaType* metaType = entry->iMetaNewL(mem,GetAttribPtr(entry->iOffset));
        length += metaType->Length();
        }
	return length;
	}

using Meta::SMetaDataECom;

EXPORT_C SMetaDataECom::SMetaDataECom()
/**
 * Protected and explicit constructor
 */
	{
	iDestroyUid.iUid = 0;
	}

EXPORT_C SMetaDataECom::~SMetaDataECom()
/**
 * Destructor
 */
	{
	if ( iDestroyUid.iUid )
      {
      REComSession::DestroyedImplementation(iDestroyUid);
      }
   }

EXPORT_C SMetaDataECom* SMetaDataECom::LoadL(TPtrC8& aDes)
/**
 * Static, encapsulates instantiation and loading of a content of a meta object
 * from data contained in a descriptor.
 * This function must be provided by all meta objects that will be used as a parameter
 * to TMetaPtr<> template.
 */
	{
	if (aDes.Length() < (3 * (TInt) sizeof(TInt32)))
		{
		// Fatal error, descriptor is corrupt
		User::Leave(KErrArgument);
		}

	// Extract UID and type
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
	STypeId typeId = STypeId::CreateSTypeId(aDes);
#else
	STypeId typeId;
	Mem::Copy(&typeId.iUid, aDes.Ptr(), sizeof(TUid));
	aDes.Set(aDes.Ptr()+sizeof(TUint32),aDes.Length()-sizeof(TUint32)); //update pointer (uid)
	Mem::Copy(&typeId.iType, aDes.Ptr(), sizeof(TInt32));
	aDes.Set(aDes.Ptr()+sizeof(TUint32),aDes.Length()-sizeof(TUint32)); //update pointer (type)
#endif

	// Attempt to load the object
	SMetaDataECom* mtd = NULL;
	TRAPD(ret, mtd = NewInstanceL(typeId));

	if (ret != KErrNone)
		{
		if (ret == KErrNotFound)
			{
			// Attempt to move the pointer on
			TUint32 len;
			Mem::Copy(&len, aDes.Ptr(), sizeof(TUint32));
			if (len <= (TUint32) aDes.Length())
				{
				aDes.Set(aDes.Ptr()+len,aDes.Length()-len); //update pointer (type)
				}
			else
				{
				aDes.Set(aDes.Right(0));
				}
			}
		User::Leave(ret);
		}

	CleanupDeletePushL(mtd);

	// Ask the object to de-marshall its data
	ret = mtd->Load(aDes);
	if (ret != KErrNone)
		{
		User::Leave(ret);
		}
	CleanupStack::Pop(mtd);
    return mtd;
	}

EXPORT_C SMetaDataECom* SMetaDataECom::NewInstanceL(const STypeId& aTypeId)
/**
 * Static, instantiates a meta object based on its Type ID.
 */
    {
	TUid destroyUid;
	SMetaDataECom* obj = reinterpret_cast<SMetaDataECom*>(REComSession::CreateImplementationL(aTypeId.iUid, destroyUid, (TAny*)aTypeId.iType));
	obj->iDestroyUid = destroyUid;
	return obj;
	}


