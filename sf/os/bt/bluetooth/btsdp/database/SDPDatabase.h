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

/**
 @file
 @internalTechnology
*/

#ifndef SDPDATABASE_H
#define SDPDATABASE_H

#include <btsdp.h>
#include "SDPServiceRecord.h"
#include "sdpconsts.h"
#include "sdputil.h"
#include "MAttributeVisitor.h"

// Size of 16/128 bit UUIDs in bytes
const TInt KSizeOf16BitUUID			= 2;
const TInt KSizeOf32BitUUID			= 4;
const TInt KSizeOf128BitUUID		= 16;

// Class representing the SDP database
NONSHARABLE_CLASS(CSdpDatabase) : public CBase
	{
public:
	IMPORT_C static CSdpDatabase* NewLC();
	IMPORT_C static CSdpDatabase* NewL();
	IMPORT_C ~CSdpDatabase();
	IMPORT_C void AddRecord(CSdpServRecord *aRecord);
	IMPORT_C CSdpServRecord*  NewRecordL();
	IMPORT_C void EncodeDbL();
	IMPORT_C void ParseDbL_Obscelete_DoesNothing();
	IMPORT_C TSdpServRecordHandle NextFreeHandle();
	TBool ReviseNextFreeHandle();
	inline TServRecordIter RecordIter();

private:
	CSdpDatabase();
	void ConstructL();
	
private:
	TBool iCanAddRecord;
	TServRecordList iRecords;
	TSdpServRecordHandle iNextFreeHandle;
	};

inline TServRecordIter CSdpDatabase::RecordIter()
	{
	return TServRecordIter(iRecords);
	}


//**********************************
// CAttrUUidVisitor
//**********************************
/**
Provides vistor functionalities to retrieve UUIDs from CSdpAttr
**/
NONSHARABLE_CLASS(CAttrUuidVisitor) : public CBase, public MAttributeVisitor
	{
public:
	IMPORT_C static CAttrUuidVisitor *NewL();
	IMPORT_C ~CAttrUuidVisitor();
	IMPORT_C RUUIDContainer& GetUuids();
	
private:
	// Implementation of MAttributeVisitor interface
	void EncodeAttributeL(CSdpAttrValue &aSrcAttr);
	void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
	void StartListL(CSdpAttrValueList &aList);
	void EndListL();

private:
	CAttrUuidVisitor();
	void ConstructL();

	// Implementation of MAttributeVisitor interface
	void VisitAttributeL(CSdpAttr &aAttribute);

private:
	RUUIDContainer		iUuidContainer;
	};

enum TSdpDatabasePanics
	{
	ESdpDatabaseVisitAttributeLErroneouslyCalled = 0,
	ESdpDatabaseUnexpectedAttributeType = 1,
	};
_LIT(KSdpDatabasePanicName, "SdpDatabase");

#endif	// SDPDATABASE_H
