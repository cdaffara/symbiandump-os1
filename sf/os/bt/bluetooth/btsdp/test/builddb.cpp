// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Build a static SDP Database
// 
//

#include "SDPDatabase.h"


// Offsets and constants
/*const TInt KRecordHandleOffset = 9;
const TInt KTotalCountOffset = 5;
const TInt KCurrentCountOffset = 7;
const TInt KErrorCodeOffset = 5;
const TInt KAttributeByteCountOffset = 5;
const TInt KAttributeListOffset = 7;*/


void BuildRecordZeroL(CSdpDatabase* aDb)
/**
 Record 0 should be used when a server instance is created.
 note that attributes 2, 5 and 0x201 should be updated.
 also note only English, pas de Francais, keine Deutsch, non Espanol
**/
	{
	TBuf8<2> attrId;
	TBuf8<4> val;
	CSdpServRecord* theRec = CSdpServRecord::NewL();
	CleanupStack::PushL(theRec);


	// Set Attr 0 (Record handle) to 0
	attrId.FillZ(2);
	val.FillZ(4);
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with UUID = 0x1000
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1000)))
		->EndListL();

	// Set Attr 2 (service record state) to 0
	attrId[0] = 0x00;
	attrId[1] = 0x02;
	val.FillZ(4);
	theRec->BuildUintL(attrId)->BuildUintL(val);


	// Set attr 4 (protocol list) to L2CAP, no RFCOMM, no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 1;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
		->EndListL();

	// Set Attr 5 (browse group list) to list with one UUID
	// 0x1000 (SDP server class)
// this should be updated with other service classes when other services are added.
	attrId[0] = 0x00;
	attrId[1] = 5;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint32(0x1002)))
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	TUint16 lang = 0x656e;
	TUint16 coding = 0x006a;
	TUint16 base = 0x0100;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(TSdpIntBuf<TUint16>(lang)) // english
			->BuildUintL(TSdpIntBuf<TUint16>(coding)) // UTF-8
			->BuildUintL(TSdpIntBuf<TUint16>(base)) // language base
		->EndListL();

	// Set Attr 0x007 (time to live) to 1200 (0x4B0) seconds (20 minutes)
	attrId[0] = 0x00;
	attrId[1] = 0x07;
	val.FillZ(4);
	val[2]=4;
	val[3]=0xb0;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 0x008 (availability) to 0xff - fully available - not in use
	attrId[0] = 0x00;
	attrId[1] = 0x08;
	TBuf8<1> val4;
	val4.FillZ(1);
	val4[0]=0xff;
	theRec->BuildUintL(attrId)->BuildUintL(val4);


	// Set Attr 0x100 (default Name) to string
	attrId[0] = 0x01;
	attrId[1] = 0;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("SDP Server"));

	// Set Attr 0x101 (def. description) to string
	attrId[0] = 0x01;
	attrId[1] = 1;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("EPOC SDP server UPF-4"));

	// Set Attr 0x102 (def. provider) to Symbian
	attrId[0] = 0x01;
	attrId[1] = 2;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Symbian Ltd."));

	// Set Attr 0x201 (service database state) to 0
	attrId[0] = 0x02;
	attrId[1] = 0x01;
	val.FillZ(4);
	theRec->BuildUintL(attrId)->BuildUintL(val);

	CleanupStack::Pop();
	// Add the record into the database
	aDb->AddRecord(theRec);
}



// this is a real dummy database which doesn't conform to the service class attribute numbering
CSdpDatabase *BuildDbL()
	{
	CSdpDatabase *theDb = CSdpDatabase::NewL();
	
	// First Record
	CSdpServRecord *theRec = CSdpServRecord::NewL();

	// Set Attr 0 (Record handle) to 0
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0))->BuildUintL(TSdpIntBuf<TUint32>(0));
	// Set Attr 0x100 (default Name) to string
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x100))->BuildStringL(_L8("SDP Server"));
	// Set Attr 0x102 (def. provider) to Symbian
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x102))->BuildStringL(_L8("Symbian Ltd."));
	// Set Attr 0x101 (def. description) to string
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x101))->BuildStringL(_L8("EPOC SDP server test suite"));
	// Set Attr 1 (service class list) to list with one UUID = 0x1000 (SDP server service)
	theRec->BuildUintL(TSdpIntBuf<TUint16>(1))->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint32(0x1000)))
		->EndListL();
	// Add the record into the database
	theDb->AddRecord(theRec);

	// Second Record
	theRec = CSdpServRecord::NewL();

	// Set Attr 0 (Record handle) to 0x11223344
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0))->BuildUintL(TSdpIntBuf<TUint32>(0x11223344));
	// Set Attr 0x100 (default Name) to string
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x100))->BuildStringL(_L8("Dummy Service"));
	// Set Attr 0x101 (def. description) to string
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x101))->BuildStringL(_L8("A Test SDP record"));
	// Set Attr 1 (service class list) to list with Two UUID = 0x1000, 0x55667788
	theRec->BuildUintL(TSdpIntBuf<TUint16>(1))->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint32(0x1000)))
			->BuildUUIDL(TUUID(TUint32(0x55667788)))
		->EndListL();
	// Set Attr 3 (service ID) to a bogoid 0x9999
	theRec->BuildUintL(TSdpIntBuf<TUint16>(3))->BuildUUIDL(TUUID(TUint16(0x9999)));
	// Add the record into the database
	theDb->AddRecord(theRec);

	// Third Record
	theRec = CSdpServRecord::NewL();

	// Set Attr 0 (Record handle) to 0x9899
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0))->BuildUintL(TSdpIntBuf<TUint32>(0x9899));
	// Set Attr 0x100 (default Name) to string
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x100))->BuildStringL(_L8("Another dummy service"));
	// Set Attr 0x101 (def. description) to string
	theRec->BuildUintL(TSdpIntBuf<TUint16>(0x101))->BuildStringL(_L8("This is the second service on SDP"));
	// Set Attr 1 (service class list) to list with one UUID = 0x1105 (OBEX Push)
	theRec->BuildUintL(TSdpIntBuf<TUint16>(1))->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint32(0x1105)))
		->EndListL();
	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=5)

	theRec->BuildUintL(TSdpIntBuf<TUint16>(4))->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(TSdpIntBuf<TUint32>(5))	// DLCI = 5
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0008))) // OBEX
			->EndListL()
		->EndListL();

	// Add the record into the database
	theDb->AddRecord(theRec);

	return theDb;
	}


// this is the nearest thing to a real database. 
// originally for UPF4, but record 0 removed above.
	
CSdpDatabase *BuildUpf4DbL()
	{
	CSdpDatabase *theDb = CSdpDatabase::NewL();
	
	TBuf8<2> attrId;
	TBuf8<4> val;
	CSdpServRecord *theRec = CSdpServRecord::NewL();
// all attribute IDs are BIG ENDian 

// Record 0 should be used when a server instance is created.
// note that attributes 2, 5 and 0x201 should be updated.
// also note only English, pas de Francais, keine Deutsch, non Espanol
	BuildRecordZeroL(theDb);
	
// Second Record DUN service

	attrId.FillZ(2);
	val.FillZ(4);
	// Set Attr 0 (Record handle) to 0x00010000
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x00;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with Two UUID = 0x1103, 0x1201
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1103))) // dial up networking
			->BuildUUIDL(TUUID(TUint16(0x1201))) // generic networking
		->EndListL();

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=1), no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 1;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(TSdpIntBuf<TUint8>(1)) // DLCI = 1
//				->BuildUintL(val) was four bytes	// DLCI = 1 
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
/*	val.FillZ(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	TBuf8<2> val2;
	TBuf8<2> val3;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;
*/
	TUint16 lang = 0x656e;
	TUint16 coding = 0x006a;
	TUint16 base = 0x0100;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(TSdpIntBuf<TUint16>(lang)) // english
			->BuildUintL(TSdpIntBuf<TUint16>(coding)) // UTF-8
			->BuildUintL(TSdpIntBuf<TUint16>(base)) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Dun version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;
	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KDialUpNetworkingUUID))) // Dialup Networking
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Dial-up networking"));
	
	// Add the record into the database
	theDb->AddRecord(theRec);

	

	// Third Record Fax
	theRec = CSdpServRecord::NewL();

	attrId.FillZ(2);
	val.FillZ(4);
	// Set Attr 0 (Record handle) to 0x00010001
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x01;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with Two UUID = 0x1111, 0x1204
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1111))) // Fax
			->BuildUUIDL(TUUID(TUint16(0x1204))) // generic telephony
		->EndListL();

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=2), no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 2;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val)	// DLCI = 2
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	lang = 0x656e;
	coding = 0x006a;
	base = 0x0100;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(TSdpIntBuf<TUint16>(lang)) // english
			->BuildUintL(TSdpIntBuf<TUint16>(coding)) // UTF-8
			->BuildUintL(TSdpIntBuf<TUint16>(base)) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Dun version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;
	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KFaxUUID))) // Fax
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Fax"));
	
	// Add the record into the database
	theDb->AddRecord(theRec);

	// Fourth Record Obex
	theRec = CSdpServRecord::NewL();

	attrId.FillZ(2);
	val.FillZ(4);
	// Set Attr 0 (Record handle) to 0x00010001
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x02;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with One UUID = 0x1105
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1105))) // Obex
		->EndListL();

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=3), OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 3;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val)	// DLCI = 3
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0008))) // Obex
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	lang = 0x656e;
	coding = 0x006a;
	base = 0x0100;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(TSdpIntBuf<TUint16>(lang)) // english
			->BuildUintL(TSdpIntBuf<TUint16>(coding)) // UTF-8
			->BuildUintL(TSdpIntBuf<TUint16>(base)) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Obex version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;

	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KObexUUID))) // Obex
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("OBEX Objext Push"));
	
	// Add the record into the database
	theDb->AddRecord(theRec);

	return theDb;
	}

void BuildContinuationString(CSdpServRecord* aRec, TSdpAttributeID aID, TUint16 aLen)
	{
	HBufC8* buf = HBufC8::NewMaxL(aLen+1); //believe it or not printing requires null termination of string
	TPtr8 bptr = buf->Des();
	bptr.SetLength(0);
	TPtrC8 base(_L8("Rhubarb"));
	TChar ch;
	for(TUint i = 0; i<aLen; ++i)
		{
		ch = base.Ptr()[i%(base.Length())];
		//bptr[i] = ch; //bptr's length already set to aLen
		bptr.Append(ch);
		}
	aRec->BuildUintL(TSdpIntBuf<TUint16>(aID))->BuildStringL(bptr);
	delete buf;
	}

void BuildContinuationRecord(CSdpDatabase* aDb, TSdpAttributeID aID, TUint16 aStringLen)
	{
	CSdpServRecord* theRec = aDb->NewRecordL(); //this will set-up the handle attribute

	// Set Attr 1 (service class list) to list with one UUID = KSdpContTestUUID 
	// (Single UUID common to all records in a continuation test database)
	// Add the record into the database
	theRec->BuildUintL(TSdpIntBuf<TUint16>(1))->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(KSdpContTestUUID))
		->EndListL();
	// Set Attr 0x100 (default Name) to string
	BuildContinuationString(theRec, aID, aStringLen);
	}

CSdpDatabase* BuildContinuationDbL()
	{
	CSdpDatabase *theDb = CSdpDatabase::NewL();
	
	// Record 0 should be used when a server instance is created.
	// note that attributes 2, 5 and 0x201 should be updated.
	// also note only English, pas de Francais, keine Deutsch, non Espanol
	BuildRecordZeroL(theDb);
	
	// Build records containing increasingly long strings
	for(TUint16 i = 1; i<200; ++i)
		{
		//5 lines for 1: prevent warnings.
		TUint16 attrID(i);
		attrID+=2;
		TUint16 len(i);
		len*=5;
		BuildContinuationRecord(theDb, attrID, len);
		}

	return theDb;
	}

CSdpDatabase* BuildNestedListDbL()
	{
	CSdpDatabase *theDb = CSdpDatabase::NewL();
	
	// Record 0 should be used when a server instance is created.
	// note that attributes 2, 5 and 0x201 should be updated.
	// also note only English, pas de Francais, keine Deutsch, non Espanol
	BuildRecordZeroL(theDb);
	
	// Build records containing increasingly long strings
	for(TUint16 i = 1; i<200; ++i)
		{
		//5 lines for 1: prevent warnings.
		TUint16 attrID(i);
		attrID+=2;
		TUint16 len(i);
		len*=5;
		BuildContinuationRecord(theDb, attrID, len);
		}

	return theDb;
	}



CSdpDatabase* BuildTBTDbL()
	{
	CSdpDatabase* theDb = CSdpDatabase::NewL();
	
	TBuf8<2> attrId;
	TBuf8<4> val;
	TBuf8<2> val1;
	CSdpServRecord* theRec = CSdpServRecord::NewL();
// all attribute IDs are BIG ENDian 

// First Record DUN service. This record will have a value for each attribute so it
// can be used for the attribute queries.

	attrId.FillZ(2);
	val.FillZ(4);
	val1.FillZ(2);
	// Set Attr 0 (Record handle) to 0x00010000
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x00;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with Three UUIDs = 0x1103, 0x1201 and 0x1000.
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(KDialUpNetworkingUUID))
			->BuildUUIDL(TUUID(KGenericNetworkingUUID)) 
			->BuildUUIDL(TUUID(KServiceDiscoveryServerServiceClassUUID))
		->EndListL();

	// Set Attr 2 (service record state) to 0.
	val.FillZ(4);
	attrId[0] = 0x00;
	attrId[1] = 0x02;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set attr 3 (service id) to 0x0001
	attrId[0] = 0x00;
	attrId[1] = 0x03;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(0x0001))
		->EndListL();

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=1), no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val1.FillZ(2);
	val1[1] = 1;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val1)	// DLCI = 1
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	val.FillZ(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	TBuf8<2> val2;
	TBuf8<2> val3;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();


	// Set Attr 0x007 (service info time to live) to 0x0000 (not used).
	attrId[0] = 0x00;
	attrId[1] = 0x07;
	val.FillZ(4);

	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 0x008 (service availability) to 0xff (not currently in use).
	attrId[0] = 0x00;
	attrId[1] = 0x08;
	TBuf8<1> ff;
	ff.FillZ(1);
	ff[0] = 0xff;	

	theRec->BuildUintL(attrId)->BuildUintL(ff);

	// Set Attr 0x009 (profile descriptor list) to Dun version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;

	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KDialUpNetworkingUUID))) 
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x00a (Documentation URL) to "http://my.fake/public/documentation/*".
	attrId[0] = 0x00;
	attrId[1] = 0x0a;

	theRec->BuildUintL(attrId)->BuildURLL(_L8("http://my.fake/public/documentation.doc"));

	// Set Attr 0x00b (Client Executable URL) to "http://my.fake/public/*/client.exe".
	attrId[0] = 0x00;
	attrId[1] = 0x0b;

	theRec->BuildUintL(attrId)->BuildURLL(_L8("http://my.fake/public/*/client.exe"));

	// Set Attr 0x00c (ICON URL) to "http://my.fake/public/icons/*".
	attrId[0] = 0x00;
	attrId[1] = 0x0c;

	theRec->BuildUintL(attrId)->BuildURLL(_L8("http://my.fake/public/icons/*"));
		
	// Set Attr 0x100 (service Name) to string
	attrId[0] = 0x01;
	attrId[1] = 0x00;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Dial-up networking"));

	// Set Attr 0x101 (service description) to string
	attrId[0] = 0x01;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Dummy description for dial-up-networking"));

	// Set Attr 0x102 (provider name) to string
	attrId[0] = 0x01;
	attrId[1] = 0x02;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("PAN team"));

	// Set attr 0x200 (version number list) to version 1.1
	attrId[0]=0x02;
	attrId[1]=0x00;
	val1.FillZ(2);
	val1[0]=0x01;
	val1[1]=0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val1)
		->EndListL();


	//Set Attr 0x201 (Service database state) to 0
	attrId[0]=0x02;
	attrId[1]=0x01;
	val.FillZ(4);
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Add the record into the database
	theDb->AddRecord(theRec);

// Second Record Fax
	theRec = CSdpServRecord::NewL();

	attrId.FillZ(2);
	val.FillZ(4);
	// Set Attr 0 (Record handle) to 0x00010001
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x01;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with Two UUID = 0x1111, 0x1204
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(KFaxUUID)) 
			->BuildUUIDL(TUUID(KGenericTelephonyUUID)) 
		->EndListL();


	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=2), no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 2;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val)	// DLCI = 2
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	val.FillZ(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Dun version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;

	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KFaxUUID))) // Fax
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x00a (Documentation URL) to a very long value, for use with continuation
	attrId[0] = 0x00;
	attrId[1] = 0x0a;

	TBuf8<654> urlBuf;
	urlBuf.Append(_L8("http://"));
	for(TInt r = 0; r < 160; r ++)
	{
		urlBuf.Append(_L8("url/"));
	}	
	urlBuf.Append(_L8("url.doc"));
	theRec->BuildUintL(attrId)->BuildURLL(urlBuf);


	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Fax"));
	
	// Add the record into the database
	theDb->AddRecord(theRec);

// Third Record Obex
	theRec = CSdpServRecord::NewL();

	attrId.FillZ(2);
	val.FillZ(4);
	// Set Attr 0 (Record handle) to 0x00010001
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x02;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to list with One UUID = 0x1105
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(KObexUUID))) // Obex
		->EndListL();

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=3), OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 3;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val)	// DLCI = 3
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0008))) // Obex
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	val.FillZ(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Obex version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;

	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KObexUUID))) // Obex
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("OBEX Objext Push"));
	
	// Add the record into the database
	theDb->AddRecord(theRec);

	// Now add a load more records to force continuation for the service search
	//
	for(TUint8 i = 0; i < 200; i ++)
	{
		theRec = CSdpServRecord::NewL();

		attrId.FillZ(2);
		val.FillZ(4);
		// Set Attr 0 (Record handle) to 0x00010003
		val[0] = 0x00;
		val[1] = 0x01;
		val[2] = 0x00;
		val[3] = TUint8(i+3);
		theRec->BuildUintL(attrId)->BuildUintL(val);
		// Set Attr 1 (service class list) to list with UUID = 0x1108
		attrId[0] = 0x00;
		attrId[1] = 0x01;
		theRec->BuildUintL(attrId)->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(0x1108))
			->EndListL();
		// Add the record into the database
		theDb->AddRecord(theRec);
	}
	return theDb;
	}



	
	
void AddBrowseGroupRecord(CSdpDatabase& aDb, TUUID aBrowseUUID)
	{
	CSdpServRecord* theRec = aDb.NewRecordL();

	TBuf8<2> attrId;
	TBuf8<4> val;
	attrId.FillZ(2);
	val.FillZ(4);

	// Set Attr 1 (service class list) to list with One UUID = 0x1001
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(KBrowseGroupDescriptorServiceClassUUID))) // BrowseGroup(DescriptorList)
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x200 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x02;

	theRec->BuildUintL(attrId)->BuildUUIDL(aBrowseUUID);
	}


void AddBrowsableRecord(CSdpDatabase& aDb, TUUID aBrowseUUID)
	{
	CSdpServRecord* theRec = aDb.NewRecordL();

	TBuf8<2> attrId;
	TBuf8<4> val;
	attrId.FillZ(2);
	val.FillZ(4);

	// Set Attr 1 (service class list) to list with UUID = 0x1108
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(0x1108))
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(aBrowseUUID) // publicBrowseRoot
		->EndListL();
	}





CSdpDatabase* BuildPrequalDbL()
	{
	CSdpDatabase* theDb = CSdpDatabase::NewL();
	
	TInt i;
	TBuf8<2> attrId;
	TBuf8<4> val;
	TBuf8<2> val1;
	CSdpServRecord* theRec = theDb->NewRecordL();
// all attribute IDs are BIG ENDian 

// Prequal Dummy Service Record. Designed to respond to SA tests - 
// where, it seems, the initial service search is done using UUIDs 1101 and 0011.
// This record will have a value for each attribute so it
// can be used for the attribute queries.
	attrId.FillZ(2);
	val.FillZ(4);
	val1.FillZ(2);

	// Set Attr 1 (service class list) to list with UUID = 0x1108
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	MSdpElementBuilder* bdr;
	bdr = theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(0x0011))
			->BuildUUIDL(TUUID(KDialUpNetworkingUUID));
	//TUint16 x = 0x1201;
	//while(x<0x120a)
	//	bdr->BuildUUIDL(TUUID(x++));
	bdr->EndListL();

	// Set Attr 2 (service record state) to 0.
	val.FillZ(4);
	attrId[0] = 0x00;
	attrId[1] = 0x02;
	val[0] = 0xdd;
	val[1] = 0xdd;
	val[2] = 0xdd;
	val[3] = 0xcd;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set attr 3 (service id) to 0x0001
	attrId[0] = 0x00;
	attrId[1] = 0x03;
	theRec->BuildUintL(attrId)
			->BuildUUIDL(TUUID(0xeeee));

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=1), no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val1.FillZ(2);
	val1[1] = 1;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0001)))
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(TSdpIntBuf<TUint8>(1)) // DLCI = 1
			->EndListL()
		->EndListL();
	
	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	val.FillZ(2);
	//val.SetLength(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	TBuf8<2> val2;
	TBuf8<2> val3;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();


	// Set Attr 0x007 (service info time to live) to 0x0000 (not used).
	attrId[0] = 0x00;
	attrId[1] = 0x07;
	//val.SetLength(4);
	val.FillZ(4);
	val[0] = 0xcc;
	val[1] = 0xcc;
	val[2] = 0xcc;
	val[3] = 0xcc;

	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 0x008 (service availability) to 0xff (not currently in use).
	attrId[0] = 0x00;
	attrId[1] = 0x08;
	TBuf8<1> ff;
	ff.FillZ(1);
	ff[0] = 0xbb;	

	theRec->BuildUintL(attrId)->BuildUintL(ff);

	// Set Attr 0x009 (profile descriptor list) to Dun version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;
	val.FillZ(2);
	val[0] = 0x01;
	val[1] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KSerialPortUUID))) 
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x00a (Documentation URL) to "http://www.documentation.url/*".
	attrId[0] = 0x00;
	attrId[1] = 0x0a;

	theRec->BuildUintL(attrId)->BuildURLL(_L8("http://www.documentation.url"));

	// Set Attr 0x00b (Client Executable URL) to "http://url.i86-microsoft-win2000/".
	attrId[0] = 0x00;
	attrId[1] = 0x0b;

	theRec->BuildUintL(attrId)->BuildURLL(_L8("http://url.i86-microsoft-win2000/"));

	// Set Attr 0x00c (ICON URL) to "http://url.32x32x8.png/icon".
	attrId[0] = 0x00;
	attrId[1] = 0x0c;

	theRec->BuildUintL(attrId)->BuildURLL(_L8("http://url.32x32x8.png/icon"));
	//theRec->BuildUintL(attrId)->BuildStringL(_L8("http://url.32x32x8.png/icon/"));
		
	// Set Attr 0x100 (service Name) to string
	//BuildContinuationString(theRec, 0x100, 0x200);
	attrId[0] = 0x01;
	attrId[1] = 0x00;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Indigo_Chat_Security_Mode_1"));

	// Set Attr 0x101 (service description) to string
	attrId[0] = 0x01;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("ServiceDescriptionAttributeValue"));
		// Set Attr 0x102 (provider name) to string
	attrId[0] = 0x01;
	attrId[1] = 0x02;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("ServiceProviderAttributeValue"));

	// Set attr 0x200 (version number list) to version 1.1
	attrId[0]=0x02;
	attrId[1]=0x00;
	val1.FillZ(2);
	val1[0]=0x01;
	val1[1]=0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val1)
		->EndListL();


	//Set Attr 0x201 (Service database state) to 0
	attrId[0]=0x02;
	attrId[1]=0x01;
	val.FillZ(4);
	val[3] = 0x0e;
	theRec->BuildUintL(attrId)->BuildUintL(val);

// Second Record Fax
	theRec = theDb->NewRecordL();

	attrId.FillZ(2);
	val.FillZ(4);

	// Set Attr 1 (service class list) to list with Two UUID = 0x1111, 0x1204
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(KFaxUUID)) 
			->BuildUUIDL(TUUID(KGenericTelephonyUUID)) 
		->EndListL();


	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=2), no OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 2;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val)	// DLCI = 2
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	val.FillZ(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Dun version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;
	val.FillZ(2);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KFaxUUID))) // Fax
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x00a (Documentation URL) to a very long value, for use with continuation
	attrId[0] = 0x00;
	attrId[1] = 0x0a;

	TBuf8<654> urlBuf;
	urlBuf.Append(_L8("http://"));
	for(TInt r = 0; r < 160; r ++)
	{
		urlBuf.Append(_L8("url/"));
	}
	urlBuf.Append(_L8("url.doc"));
	theRec->BuildUintL(attrId)->BuildURLL(urlBuf);


	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("Fax"));
	
// Third Record Obex
	theRec = theDb->NewRecordL();

	attrId.FillZ(2);
	val.FillZ(4);

	// Set Attr 1 (service class list) to list with One UUID = 0x1105
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(KObexUUID))) // Obex
		->EndListL();

	// Set attr 4 (protocol list) to L2CAP, RFCOMM (DLCI=3), OBEX
	attrId[0] = 0x00;
	attrId[1] = 0x04;
	val.FillZ(4);
	val[3] = 3;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(val)	// DLCI = 3
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0008))) // Obex
			->EndListL()
		->EndListL();

	// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x006 (language base)
	attrId[0] = 0x00;
	attrId[1] = 0x06;
	val.FillZ(2);
	val[0] = 0x65;
	val[1] = 0x6e;
	val2.FillZ(2);
	val3.FillZ(2);
	val2[1] = 0x6a;
	val3[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUintL(val) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();

	// Set Attr 0x009 (profile descriptor list) to Obex version 1.00
	attrId[0] = 0x00;
	attrId[1] = 0x09;
	val.FillZ(4);
	val[0] = 0x01;

	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(KObexUUID))) // Obex
				->BuildUintL(val)	// version 0x100
			->EndListL()
		->EndListL();

	// Set Attr 0x100 (default Name) to string
	attrId[1] = 0x00;
	attrId[0] = 0x01;
	theRec->BuildUintL(attrId)->BuildStringL(_L8("OBEX Objext Push"));


	// Now add browse test stuff
	//
	AddBrowseGroupRecord(*theDb, TUUID(0xbbcb));
	AddBrowseGroupRecord(*theDb, TUUID(0xbbdb));
	AddBrowseGroupRecord(*theDb, TUUID(0xbbeb));
	AddBrowseGroupRecord(*theDb, TUUID(0xbbfb));
	AddBrowseGroupRecord(*theDb, TUUID(0xccec));
	AddBrowseGroupRecord(*theDb, TUUID(0xccfc));
	AddBrowseGroupRecord(*theDb, TUUID(0xaaba));
	AddBrowseGroupRecord(*theDb, TUUID(0xaaca));

	AddBrowsableRecord(*theDb, TUUID(0xbbcb));
	AddBrowsableRecord(*theDb, TUUID(0xbbdb));
	AddBrowsableRecord(*theDb, TUUID(0xbbeb));
	AddBrowsableRecord(*theDb, TUUID(0xbbfb));
	AddBrowsableRecord(*theDb, TUUID(0xccec));
	AddBrowsableRecord(*theDb, TUUID(0xccfc));
	AddBrowsableRecord(*theDb, TUUID(0xaaba));
	AddBrowsableRecord(*theDb, TUUID(0xaaca));



	// Now add a load more records to force continuation for the service search
	//
	for(i = 0; i < 165; i ++)
	{
		theRec = theDb->NewRecordL();

		attrId.FillZ(2);
		val.FillZ(4);

		// Set Attr 1 (service class list) to list with UUID = 0x1108
		attrId[0] = 0x00;
		attrId[1] = 0x01;
		theRec->BuildUintL(attrId)->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(0x1101))
			->EndListL();
	}
	return theDb;
	}
