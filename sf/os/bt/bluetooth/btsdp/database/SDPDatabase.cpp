// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_sock.h>
#include <btsdp.h>
#include <e32des8.h>
#include <bluetooth/logger.h>

#include "SDPDatabase.h"
#include "DataEncoder.h"
#include "EncoderVisitor.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDPDATABASE);
#endif

void Panic(TSdpDatabasePanics aCode)
	{
	User::Panic(KSdpDatabasePanicName, aCode);
	}

// Class CSdpDatabase 

EXPORT_C CSdpDatabase* CSdpDatabase::NewLC()
/**
Creates a CSdpDatabase object, but leaves on the cleanup stack.

@return Pointer to the created CSdpDatabase object
*/
	{
	LOG_STATIC_FUNC
	CSdpDatabase* self = new(ELeave) CSdpDatabase;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSdpDatabase* CSdpDatabase::NewL()
/**
Creates a CSdpDatabase object.(Does NOT leave it on the cleanup stack.)

@return Pointer to the created CSdpDatabase object
*/
	{
	LOG_STATIC_FUNC
	CSdpDatabase* self = CSdpDatabase::NewLC();
	CleanupStack::Pop();
	return self;
	}

CSdpDatabase::CSdpDatabase()
: iCanAddRecord(ETrue), 
  iRecords(CSdpServRecord::LinkOffset()), 
  iNextFreeHandle(KSdpMinServRecordHandle)
	{
	CONNECT_LOGGER
	}

void CSdpDatabase::ConstructL()
	{
	LOG_FUNC
	}

EXPORT_C CSdpDatabase::~CSdpDatabase()
/** Destructor
*/
	{
	LOG_FUNC
	// Delete all records on the record list
	iRecords.ResetAndDestroy();
	CLOSE_LOGGER
	}

/** Adds a service record to this database.
@param aRecord The record being added
*/
EXPORT_C void CSdpDatabase::AddRecord(CSdpServRecord* aRecord)
	{
	LOG_FUNC
	iRecords.AddInOrder(*aRecord); //Panics if aRecord's handle is already bagged.
	//Must do this AFTER adding latest record...hence need for 'iCanAddRecord'
	iCanAddRecord = ReviseNextFreeHandle();
	}

/** Creates a new record on this database
@leave KErrSdpDatabaseFull implies no more spare handles
*/
EXPORT_C CSdpServRecord* CSdpDatabase::NewRecordL()
	{
	LOG_FUNC
	// Add a new record only if we have spare handles...
	// ...there are 2^32 - 0x10000 to choose from!
	if(!iCanAddRecord)
		{
		User::Leave(KErrSdpDatabaseFull);
		}

	CSdpServRecord* record = CSdpServRecord::NewL();
	CleanupStack::PushL(record);

	record
		->BuildUintL(TSdpIntBuf<TUint16>(0))
		->BuildUintL(TSdpIntBuf<TSdpServRecordHandle>(NextFreeHandle()));

	AddRecord(record);
	CleanupStack::Pop(); //record
	return record;
	}

/** Encodes complete database. */
EXPORT_C void CSdpDatabase::EncodeDbL()
	{
	LOG_FUNC
	for(TServRecordIter recIter(RecordIter()); recIter; recIter++)
		{// Iterate through records in Db
		for (TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// iterate through attributes in record
			CSdpAttr* theAttr = &(*attrIter);
			CSdpAttrValue* theAttrValue = &(theAttr->Value());
			TUint size = TElementEncoder::EncodedSize(theAttrValue->Type(), 
													  theAttrValue->DataSize());

			HBufC8* outbuf = HBufC8::NewLC(size);
			TPtr8 writePtr = outbuf->Des();
			TElementEncoder theEncoder(writePtr);
			CAttrEncoderVisitor::EncodeAttributeL(theEncoder, (*attrIter).Value());
			theAttr->BuildEncodedL(writePtr);
			CleanupStack::PopAndDestroy(); //outbuf
			}
		}
	}

/** Dead function. Space filler for .def file (left in for BC)
*/
EXPORT_C void CSdpDatabase::ParseDbL_Obscelete_DoesNothing()
	{
	LOG_FUNC
	return;
	}

/** Returns the 'TSdpServRecordHandle' iNextFreeHandle.
@return the next free handle 
*/
EXPORT_C TSdpServRecordHandle CSdpDatabase::NextFreeHandle()
	{
	LOG_FUNC
	return iNextFreeHandle;
	}

/**	Finds a handle not used by any record in database.
	Starts at current next free handle value, and discovers if
	it is used by a record in the database. If so cycles it on one in
	in the allowed range, and checks again. This algorithm continues
	until an unused handle is found (=>return ETrue) or all handles
	have been checked and are found to be in use. (=>return EFalse)
	NB. We start by looking for the next available handle going upwards
	in value. Only if none are found do we return to the lower limit and
	search for "holes" below the last claimed handle. This should
	reduce processing time. We could have gone further and incremented
	iNextFreeHandle value before doing anything else, because it is likely
	that the current iNextFreeHandle will have just been used when this
	function is called
*/
TBool CSdpDatabase::ReviseNextFreeHandle()
	{
	LOG_FUNC
	if(iNextFreeHandle>KSdpMaxServRecordHandle||
		iNextFreeHandle<KSdpMinServRecordHandle)
		{
		iNextFreeHandle = KSdpMinServRecordHandle;
		}

	TSdpServRecordHandle origNextFreeHandle(iNextFreeHandle);
	for(TServRecordIter recIter(RecordIter()); recIter; recIter++)
		{// Iterate thru records in iDatabase
		if((*recIter).Handle()==iNextFreeHandle)
			{
			if(++iNextFreeHandle>KSdpMaxServRecordHandle)
				iNextFreeHandle = KSdpMinServRecordHandle;
			//We've run out of handles
			if(iNextFreeHandle==origNextFreeHandle)
				return EFalse;
			//Run through all records again to check for new incremented handle.
			recIter.SetToFirst();
			}
		}
	return ETrue;
	}


EXPORT_C CAttrUuidVisitor* CAttrUuidVisitor::NewL()
	{
	LOG_STATIC_FUNC
	CAttrUuidVisitor* self = new(ELeave)CAttrUuidVisitor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
EXPORT_C CAttrUuidVisitor::~CAttrUuidVisitor()
	{
	LOG_FUNC
	iUuidContainer.Close();
	}
	
EXPORT_C RUUIDContainer& CAttrUuidVisitor::GetUuids()
	{
	LOG_FUNC
	return iUuidContainer;
	}

void CAttrUuidVisitor::EncodeAttributeL(CSdpAttrValue& /*aSrcAttr*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(ETrue, User::Panic(_L("EncodeAttributeL() erroneously called"), KErrArgument));
	}
	
CAttrUuidVisitor::CAttrUuidVisitor()
	{
	LOG_FUNC
	iUuidContainer.Reset();
	}
	
void CAttrUuidVisitor::ConstructL()
	{
	LOG_FUNC
	}

// Implementation of MAttributeVisitor interface
void CAttrUuidVisitor::VisitAttributeL(CSdpAttr& /*aAttribute*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(ETrue, Panic(ESdpDatabaseVisitAttributeLErroneouslyCalled));
	}
	
void CAttrUuidVisitor::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	LOG_FUNC
	switch (aType)
		{
		case ETypeUUID:
			iUuidContainer.Add(aValue.UUID());
			break;
		case ETypeEncoded:
			{
			TPtrC8 ptr = aValue.Des();
			if(ptr.Length() == KSdpElemHdrTypeShift + KSizeOf16BitUUID)
				{
				// 16-bit UUID
				TUUID uuid = TUUID(BigEndian::Get16(&ptr[KSdpElemHdrTypeShift]));
				iUuidContainer.Add(uuid);
				}
			else if(ptr.Length() == KSdpElemHdrTypeShift + KSizeOf32BitUUID)
				{
				// 32-bit UUID
				TUUID uuid = TUUID(BigEndian::Get32(&ptr[KSdpElemHdrTypeShift]));
				iUuidContainer.Add(uuid);
				}
			else if(ptr.Length() == KSdpElemHdrTypeShift + KSizeOf128BitUUID)
				{
				// 128-bit UUID
				TUUID uuid = TUUID(BigEndian::Get32(&ptr[KSdpElemHdrTypeShift]),
								   BigEndian::Get32(&ptr[KSdpElemHdrTypeShift+sizeof(TUint32)]),
								   BigEndian::Get32(&ptr[KSdpElemHdrTypeShift+2*sizeof(TUint32)]),
								   BigEndian::Get32(&ptr[KSdpElemHdrTypeShift+3*sizeof(TUint32)]));
				iUuidContainer.Add(uuid);
				}
			else
				{
				// We don't support other UUIDs
				}
			}
			break;
		default:
			__ASSERT_DEBUG(ETrue, Panic(ESdpDatabaseUnexpectedAttributeType));
			break;
		}
	}

void CAttrUuidVisitor::StartListL(CSdpAttrValueList& /*aList*/)
	{
	LOG_FUNC
	}

void CAttrUuidVisitor::EndListL()
	{
	LOG_FUNC
	}

