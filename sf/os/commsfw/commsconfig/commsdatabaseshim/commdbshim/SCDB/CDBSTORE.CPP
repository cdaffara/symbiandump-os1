// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CStoreableOverrideSettings class definitions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <cdbstore.h>
#include "OVERRIDE.H"

namespace commdb_dll
	{
/** 
Maximum size of the store 
@internalComponent
*/
const TInt KStoreExpandSize=100;
	}
using namespace commdb_dll;


EXPORT_C CStoreableOverrideSettings* CStoreableOverrideSettings::NewL(TParamList aParamList)
/** Allocates and constructs a storable override settings object.

The construction process automatically opens the communications database and 
connects to the DBMS. The database is closed and the connection to the DBMS 
severed when this override object is destroyed

This member function was withdrawn in release 6.0, but re-introduced in 7.0

@param aParamList An enumerator which indicates whether the override settings 
are partial or full. 
@return A pointer to the override settings object. */
	{
	CStoreableOverrideSettings* r=new(ELeave) CStoreableOverrideSettings(aParamList);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

EXPORT_C CStoreableOverrideSettings* CStoreableOverrideSettings::NewL(TParamList aParamList, TCommDbDatabaseType /*aDbType*/)
/** Allocates and constructs a storable override settings object.

The construction process automatically opens the communications database and 
connects to the DBMS. The database is closed and the connection to the DBMS 
severed when this override object is destroyed
 
Re-instated override variant of NewL function in order to maintain BC with 6.1.
Disgards a TCommDbDatabaseType value and calls NewL(TParamList aParamList).

@param aParamList
@param aDbType Whether Database is IAP or ISP version. All Db's are IAP type now.
@return CCommDbOverrideSettings* the calling function takes ownership of the returned object/
*/
	{
	return CStoreableOverrideSettings::NewL(aParamList);
	}

CStoreableOverrideSettings::CStoreableOverrideSettings(TParamList aParamList)
	: CCommDbOverrideSettings(aParamList)
/** Constructor
@internalAll  */
	{}

void CStoreableOverrideSettings::ConstructL()
	{
	iStore=CBufStore::NewL(KStoreExpandSize);
	CCommDbOverrideSettings::ConstructL();
	}

EXPORT_C CStoreableOverrideSettings::~CStoreableOverrideSettings()
/** Frees all resources owned by this object, prior to its destruction.

Specifically, it deletes the memory store. */
	{
	delete iStore;
	}

EXPORT_C void CStoreableOverrideSettings::ExternalizeL(RWriteStream& aStream) const
/** Externalises override settings to a stream.

@param aStream The stream to write to. */
	{
	TInt i;
	TInt count;
	
	count = iOverrides.Count();
	aStream.WriteUint32L(count);
	for (i=0; i<count; i++)
		{
		iOverrides[i]->ExternalizeL(aStream);
		}
	count = iIapOverrides.Count();
	aStream.WriteUint32L(count);
	for (i=0; i<count; i++)
		{
		iIapOverrides[i].ExternalizeL(aStream);
		}
	}

EXPORT_C void CStoreableOverrideSettings::InternalizeL(RReadStream& aStream)
/** Internalises the override settings from a stream.

@param aStream The stream to read from. */
	{
	TInt count;
	TInt i;

	count=aStream.ReadUint32L();
	for (i=0; i < count; i++)
		{
		CCommDbOverride pattern(EUnknownType);
		pattern.InternalizeL(aStream);
		CCommDbOverride* override = 0;
		switch (pattern.iType)
			{
			case EIntValue:
				override = new (ELeave) CCommDbIntOverride(pattern.iTableName,
														   pattern.iColumnName);
				break;
			case EBoolValue:
				override = new (ELeave) CCommDbBoolOverride(pattern.iTableName,
															pattern.iColumnName);
				break;
			case EDes8Value:
				override = new (ELeave) CCommDbDes8Override(pattern.iTableName,
															pattern.iColumnName);
				break;
			case EDes16Value:
				override = new (ELeave) CCommDbDes16Override(pattern.iTableName,
															 pattern.iColumnName);
				break;
			case ELongDesValue:
				override = CCommDbLongDesOverride::NewL(pattern.iTableName,
														pattern.iColumnName);
				break;
			default:
				User::Leave(KErrArgument);
				break;
			}
		CleanupStack::PushL(override);
		override->InternalizeL(aStream);
		User::LeaveIfError(iOverrides.Append(override));
		CleanupStack::Pop();  // override
		}
	count=aStream.ReadUint32L();
	for (i=0; i < count; i++)
		{
		TCommDbIapConnectionPrefOverride override;
		override.InternalizeL(aStream);
		User::LeaveIfError(iIapOverrides.Append(override));
		}
	}

EXPORT_C CBufFlat* CStoreableOverrideSettings::StoreLC()
/** Constructs a flat buffer and stores the override settings in it.

The function returns a pointer to the buffer; the pointer to this object is 
placed on the cleanup stack.

@return A pointer to a flat buffer in which this object is stored. */
	{
	CBufFlat* buf=CBufFlat::NewL(256); // expand size = 256
	CleanupStack::PushL(buf);
	RBufWriteStream strm(*buf);
	strm << *this;
	strm.CommitL();
	return buf;
	}

EXPORT_C void CStoreableOverrideSettings::RestoreL(HBufC8* aBuf)
/** Restores the override settings from the buffer memory store hosted by the specified 
8-bit descriptor.

@param aBuf A pointer to an 8 bit memory descriptor containing the data to 
be restored. */
	{
	RDesReadStream strm(*aBuf);			// turn it into a stream
	strm >> *this;						// re-construct arrays
	}
