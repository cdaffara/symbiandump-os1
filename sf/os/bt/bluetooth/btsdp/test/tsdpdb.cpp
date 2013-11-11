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
// SDP Database library test code
// 
//


#include <e32test.h>
#include <f32file.h>
#include <e32math.h>
#include <btsdp.h>
#include <e32def.h>
#include <e32const.h>
#include <e32std.h>
#include "listener.h"
#include "reqhandler.h"
#include "SDPDatabase.h"
#include "protocolmanl.h"
#include "MAttributeVisitor.h"
#include "ServiceSearchVisitor.h"
#include "DataEncoder.h"
#include "ExtractorVisitor.h"
#include "EncoderVisitor.h"
#include "responsesizevisitor.h"
#include "debug.h"
//#ifdef __EPOC32__
#include <c32comm.h>
//#endif

GLDEF_D RTest test(_L("SDP database DLL tests"));

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over com2
#endif

CSdpDatabase *BuildDbL();
CSdpDatabase *BuildUpf4DbL();
CSdpDatabase* BuildContinuationDbL();
CSdpDatabase* BuildNestedListDbL();
CSdpDatabase* BuildPrequalDbL();
// in Builddb.cpp

void LoadLDD_PDD()
	{
	TInt r;
//#ifdef __EPOC32__
	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));
//#endif
	test.Printf(_L("Loading PDD\n"));
	r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else 
		test.Printf(_L("Loaded PDD\n"));
	test.Printf(_L("Loading LDD\n"));
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Loaded LDD\n"));
	}

// plagiarised from tsdp.cpp
// actually Mel can't cast from Shortest form (TPtrC8) to a TDesC8 without this call
void HexDes(const TDesC8& aDes)
	{
	for (TInt i = 0; i < aDes.Length(); ++i)
		test.Printf(_L("%02x"), aDes[i]);
	}

class CAttrPrintVisitor : public CBase, public MAttributeVisitor
	{
public:
	CAttrPrintVisitor(CConsoleBase& aConsole) : iConsole(aConsole) {iIndent=0;}
	~CAttrPrintVisitor() {/*iConsole.Getch();*/}
    void VisitAttributeL(CSdpAttr &aAttribute)
		{
		Indent();
		test.Printf(_L("\nID:0x%x:"), aAttribute.AttributeID());
		}
	void VisitAttributeValueL(CSdpAttrValue & aValue, TSdpElementType aType)
		{
// FIXME need a new object like the match list that will just store the object in
// FIXME an array, or perhaps just print it ? Is it a visitor ?
//		CSdpSearchPattern* encAttrList = new (ELeave) CSdpSearchPattern;
//		CleanupStack::PushL(encAttrList);

//		CElementParser* parser = CElementParser::NewL(encAttrList);
//		CleanupStack::PushL(parser);

//		TInt rem;

		TBuf16<64> iString;
//		TInt iFound;
		switch (aType)
			{
			case ETypeString:
				iString.Copy(aValue.Des());
				test.Printf(_L("\"%S\""),&iString);
				break;
			case ETypeDES:
				test.Printf(_L(" DES"));
				break;
			case ETypeUint:
				test.Printf(_L(" UInt:0x%x"), aValue.Uint());
				break;
			case ETypeUUID:
				test.Printf(_L(" UUID:0x"));
				HexDes(aValue.UUID().ShortestForm());
				break;
			case ETypeEncoded:
				HexDes(aValue.Des());  // simplest
//parse out the elements in this encoded attribute
//				rem = parser->ParseElementsL(aValue.Des());
//				CleanupStack::PopAndDestroy(/*parser*/);
//				for (iFound=0 ; iFound++ ; (iFound < (encAttrList->Length())))
//				{
//					VisitAttributeValueL(encAttrList->At(iFound).Value, encAttrList->At(iFound).Type);
//				}
//				CleanupStack::PopAndDestroy(/*encAttrList*/);
				break;
			default:
				test.Printf(_L("type %d\n"), aType);
			}
		}
	void StartListL(CSdpAttrValueList &/*aList*/)
		{
		++iIndent;
		test.Printf(_L("{"));
		}
    void EndListL()
		{
		if(iIndent<=0)
			{
			test.Printf(_L("\nERROR! Unmatched EndList!\n"));
			__DEBUGGER();
			}
		test.Printf(_L("}"));
		--iIndent;
		}
private:
	void Indent() {/*test.SetPos(iIndent)*/;}
	CConsoleBase &iConsole;
	TInt iIndent;
	};

void PrintDb(CSdpDatabase& aDb, CConsoleBase& aConsole)
	{
	aConsole.Printf(_L("Printing Database...\n"));

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		aConsole.Printf(_L("\n...Printing Record 0x%x\n"), (*recIter).Handle());
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(aConsole);
			(*attrIter).AcceptVisitorL(*theVisitor);
			delete theVisitor;
			}
		}
	}

class CAttrFlogVisitor : public CBase, public MAttributeVisitor
	{
public:
	CAttrFlogVisitor(){}
    void VisitAttributeL(CSdpAttr &aAttribute)
		{
		Indent();

		//3 lines for one here, but prevents warning.
		TUint id;
		id= aAttribute.AttributeID(); 
		FTRACE(FPrint(_L("Attribute ID: 0x%x\n"), id));

		(void)(id != 0); // keep compiler happy by referencing id as an r-value in urel
		}
	void VisitAttributeValueL(CSdpAttrValue & aValue, TSdpElementType aType)
		{
		TBuf16<64> iString;
		switch (aType)
			{
			case ETypeString:
				iString.Copy(aValue.Des());
				FTRACE(FPrint(_L("\"%S\""),&iString));
				break;
			case ETypeDES:
				FTRACE(FPrint(_L(" DES")));
				break;
			case ETypeUint:
				FTRACE(FPrint(_L(" UInt:0x%x"), aValue.Uint()));
				break;
			case ETypeUUID:
				FTRACE(FPrint(_L(" UUID:0x")));
				HexDes(aValue.UUID().ShortestForm());
				break;
			case ETypeEncoded:
				HexDes(aValue.Des());  // simplest
				break;
			default:
				FTRACE(FPrint(_L("type %d\n"), aType));
			}
		}
    //void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	//	{
	//	Indent();
	//	FTRACE(FPrint(_L("(Val: type %d)\n"), aType));
	//	if(aType==ETypeUUID)
	//		{
	//		TUUID u = aValue.UUID();
	//		FTRACE(FPrint(_L("(Val: uuid 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)\n"), 
	//					  u[0],u[1],u[2],u[3],u[4],u[5],u[6],u[7],u[8],u[9],u[10],u[11],u[12],u[13],u[14],u[15]));
	//		}
	//
	//		}
    void StartListL(CSdpAttrValueList &/*aList*/)
		{
		++iIndent;
		Indent();
		FTRACE(FPrint(_L("{\n")));
		}
    void EndListL()
		{
		if(iIndent<=0)
			{
			FTRACE(FPrint(_L("\nERROR! Unmatched EndList!\n")));
			__DEBUGGER();
			}
		Indent();
		FTRACE(FPrint(_L("}\n")));
		--iIndent;
		}
private:
	void HexDes(const TDesC8& aDes)
		{
		for (TInt i = 0; i < aDes.Length(); ++i)
			{
			FTRACE(FPrint(_L("%02x"), aDes[i]));
			}
		}

	void Indent() 
		{
		for(TInt i=0; i<iIndent;++i)
			{
			FTRACE(FPrint(_L("  ")));
			}
			
		}
	TInt iIndent;
	};


void FlogDb(CSdpDatabase& aDb)
	{
	FTRACE(FPrint(_L("\n\nPrinting Database...\n\n")));

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		FTRACE(FPrint(_L("\n...Printing Record 0x%x\n"), (*recIter).Handle()));
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			CAttrFlogVisitor* theVisitor = new CAttrFlogVisitor();
			(*attrIter).AcceptVisitorL(*theVisitor);
			delete theVisitor;
			}
		}
	FTRACE(FPrint(_L("\n\nEnd Printing Database...\n\n\n\n")));
	}



class CSearchObserver : public CBase, public MServiceSearchHandler
	{
public:
	void RecordFoundL(CSdpServRecord& aMatchingRec)
		{
		++iMatchesFound;
		test.Printf(_L("Match %d found in record 0x%x\n"), iMatchesFound, aMatchingRec.Handle());
		};
	TInt iMatchesFound;
	};

void SearchDbL(CSdpDatabase &aDb, const CSdpSearchPattern &aPattern, TInt aExpectedMatches)
   	{
	CSearchObserver *ob = new(ELeave) CSearchObserver;
	CleanupStack::PushL(ob);
	CServiceSearchVisitor::SearchDbL(aDb, aPattern, *ob);
	test(aExpectedMatches==ob->iMatchesFound);
	CleanupStack::PopAndDestroy();
	}

void RegisterDatabaseInServerL (RSdpDatabase& aDb, CSdpDatabase& aDbStruct)
	{
	for(TServRecordIter recIter(aDbStruct.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		TServAttrIter attrIter((*recIter).AttributeIter());
		if(!attrIter) 
			continue;
		if((*attrIter).AttributeID() == 0)
			attrIter++;	//Skip rec handle
		if(!attrIter) 
			continue;
		if((*attrIter).AttributeID() != 1 || (*attrIter).Value().Type() != ETypeDES)
			continue;
		// Get service class list
		TSdpServRecordHandle handle;
		aDb.CreateServiceRecordL((CSdpAttrValueDES&)(*attrIter).Value(), handle);
		attrIter++;
		for(; attrIter; attrIter++)
			{// Iterate thru attributes in record
			aDb.UpdateAttributeL(handle, (*attrIter).AttributeID(), (*attrIter).Value());
			}
		}
	}
 
TSdpServRecordHandle BuildPagonisL(RSdpDatabase& aDb, TUUID aUUID, TUint8 aCN)
/**
 Record Pagonis should be used when building a record with a protocol ID
 list consisting of L2CAP and RFCOMM. This record is for a service of class
 "serial port" - UUID16 0x1101
 note that attributes 2, 5 and 0x201 should be updated.
 also note only English, pas de Francais, keine Deutsch, non Espanol
**/
	{
	TBuf8<4> value1;
	TBuf8<4> value2;
	TSdpServRecordHandle recHandle;
	CSdpAttrValue* attrVal = 0;
	CSdpAttrValueDES* attrValDES = 0;


	value1.FillZ(4);
	value2.FillZ(4);

	// Set Attr 1 (service class list) to list with UUID = 0x1101 (serial port)
	//													 or 0x1102 (LANP)
	aDb.CreateServiceRecordL(aUUID, recHandle);
	test.Printf(_L("Service Record Created - Handle: 0x%x!\n\n"), recHandle);

	// Set Attr 2 (service record state) to 0
	value1.FillZ(4);
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x02, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set attr 4 (protocol list) to L2CAP, and RFCOMM
	value1.FillZ(4);
	value1[3] = 3;
	value2.FillZ(4);
	value2[3] = aCN;
	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
				->BuildUintL(value1)
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(value2)
			->EndListL()
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x04, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;

	// Set Attr 5 (browse group list) to list with one UUID
	// 0x1101 (serial port class)
	// this should be updated with other service classes when other services are added.
	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildUUIDL(TUUID(TUint32(0x1002)))
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x05, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;


	// Set Attr 0x006 (language base)
	value1.FillZ(4);
	value1[2] = 0x65;
	value1[3] = 0x6e;
	TBuf8<4> val2;
	TBuf8<4> val3;
	val2.FillZ(4);
	val3.FillZ(4);
	val2[3] = 0x6a;
	val3[2] = 0x01;

	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildUintL(value1) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x06, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;

	// Set Attr 0x007 (time to live) to 1200 (0x4B0) seconds (20 minutes)
	value1.FillZ(4);
	value1[2]=4;
	value1[3]=0xb0;
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x07, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x008 (availability) to 0xff - fully available - not in use
	value1.FillZ(4);
	value1[3]=0xff;
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x08, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;


	// Set Attr 0x100 (default Name) to string
	attrVal = CSdpAttrValueString::NewStringL(_L8("Pagonis Service"));
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0100, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x101 (def. description) to string
	attrVal = CSdpAttrValueString::NewStringL(_L8("It's all Greek to me!"));
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0101, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x102 (def. provider) to Symbian
	attrVal = CSdpAttrValueString::NewStringL(_L8("Symbian Ltd."));
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0102, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x201 (service database state) to 0
	value1.FillZ(4);
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0201, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	return recHandle;
}


TSdpServRecordHandle BuildWAPL(RSdpDatabase& aDb, TUint8 aCN)
/**
 Record Wap should be used when building a record with a protocol ID
 list consisting of L2CAP and RFCOMM, similar to Pagonis but containing
 a profile descriptor list with LAN access using PPP.
 This record is for a service of class "WAP bearer" - UUID16 0x1113
**/
	{
	TBuf8<4> value1;
	TBuf8<4> value2;
	TSdpServRecordHandle recHandle;
	CSdpAttrValue* attrVal = 0;
	CSdpAttrValueDES* attrValDES = 0;


	value1.FillZ(4);
	value2.FillZ(4);

	// Set Attr 1 (service class list) to list with UUID = 0x1113 (WAP access device)
	aDb.CreateServiceRecordL(TUUID(TUint16(0x1113)), recHandle);
	test.Printf(_L("WAP Service Record Created - Handle: 0x%x!\n\n"), recHandle);

	// Set Attr 2 (service record state) to 0
	value1.FillZ(4);
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x02, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set attr 4 (protocol list) to L2CAP, and RFCOMM
	value1.FillZ(4);
	value1[3] = 3;
	value2.FillZ(4);
	value2[3] = aCN;
	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0100))) // L2CAP
				->BuildUintL(value1)
			->EndListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x0003))) // RFCOMM
				->BuildUintL(value2)
			->EndListL()
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x04, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;

	// Set Attr 5 (browse group list) to list with one UUID
	// 0x1101 (serial port class)
	// this should be updated with other service classes when other services are added.
	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildUUIDL(TUUID(TUint32(0x1002)))
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x05, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;


	// Set Attr 0x006 (language base)
	value1.FillZ(4);
	value1[2] = 0x65;
	value1[3] = 0x6e;
	TBuf8<4> val2;
	TBuf8<4> val3;
	val2.FillZ(4);
	val3.FillZ(4);
	val2[3] = 0x6a;
	val3[2] = 0x01;

	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildUintL(value1) // english
			->BuildUintL(val2) // UTF-8
			->BuildUintL(val3) // language base
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x06, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;

	// Set Attr 0x007 (time to live) to 1200 (0x4B0) seconds (20 minutes)
	value1.FillZ(4);
	value1[2]=4;
	value1[3]=0xb0;
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x07, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x008 (availability) to 0xff - fully available - not in use
	value1.FillZ(4);
	value1[3]=0xff;
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x08, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x009 (profile descriptor) to confuse the BTCOMM client when looking for LANP
	value1.FillZ(4);
	value1[3] = 1;
	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	attrValDES
		->StartListL()
			->BuildDESL()
			->StartListL()
				->BuildUUIDL(TUUID(TUint16(0x1102))) // LAN access using PPP
				->BuildUintL(value1)
			->EndListL()
		->EndListL();
	aDb.UpdateAttributeL(recHandle, 0x09, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;


	// Set Attr 0x100 (default Name) to string
	attrVal = CSdpAttrValueString::NewStringL(_L8("Pagonis Service"));
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0100, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x101 (def. description) to string
	attrVal = CSdpAttrValueString::NewStringL(_L8("It's all Greek to me!"));
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0101, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x102 (def. provider) to Symbian
	attrVal = CSdpAttrValueString::NewStringL(_L8("Symbian Ltd."));
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0102, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	// Set Attr 0x201 (service database state) to 0
	value1.FillZ(4);
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	aDb.UpdateAttributeL(recHandle, 0x0201, *attrVal);
	CleanupStack::PopAndDestroy();
	attrVal = 0;

	return recHandle;
}



void Test0()
/**
	Client/server test.
**/
	{
	FLOG(_L("Running test 0"));
	//FIXME: TRAPD? 
	test.Start(_L("Test 0 -- Client/Server"));
	TInt ret;
	LoadLDD_PDD();

	RSdp sdp;
	ret = sdp.Connect();
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}
	RSdpDatabase db;
	ret = db.Open(sdp);
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}
	TSdpServRecordHandle recHandle = 0;
	TUUID uuid = TUUID(0x20000); //prevents warning
	db.CreateServiceRecordL(uuid, recHandle);
	test.Printf(_L("Service Record Created - Handle: 0x%x!\n\n"), recHandle);
		CSdpAttrValueDES* list = CSdpAttrValueDES::NewDESL(NULL);
		MSdpElementBuilder* bldr= list;
		CleanupStack::PushL(list);

		bldr
			->StartListL()
				->BuildUUIDL(TUUID(TUint32(0x20002000)))
				->BuildUUIDL(TUUID(TUint32(0x11112222), TUint32(0x33334444), 
									  TUint32(0x55556666), TUint32(0x77778888)))
				->BuildUUIDL(TUUID(TUint32(0x40000)))
			->EndListL();
		//db.CreateServiceRecordL(*list, recHandle);
		CleanupStack::PopAndDestroy(); //list
		//test.Printf(_L("Service Record Created - Handle: 0x%x!\n\n"), recHandle);
	//User::WaitForRequest(status);
	//if(status==KErrNone)
	//	test.Printf(_L("Service Record Created - Handle: 0x%x!\n\n"), recHandlePckgBuf());
	//else
	//	test.Printf(_L("Service Record Creation ERROR %d!\n\n"), status.Int());

	TBuf8<8> buf8(_L8("Mel!"));
	buf8[1] = 'E';
	buf8[2] = 'L';
	TDesC16 buf16(_L("Nicky!"));
	db.UpdateAttributeL(recHandle, 0x3454, buf8);
	db.UpdateAttributeL(recHandle, 0x3455, _L("Nicky!"));
	db.UpdateAttributeL(recHandle, 0x3455, _L("Micky!"));
	db.UpdateAttributeL(recHandle, 0x3456, 0x9999);
	db.DeleteAttributeL(recHandle, 0x3456);
	db.UpdateAttributeL(recHandle, 0x3456, 0x7777);
	db.DeleteAttributeL(recHandle, 0x5555); //does not exist!
	//db.DeleteRecordL(recHandleSpecial);
	db.Close();
	sdp.Close();
	test.End();
	}


void Test1()
/**
	Client/server test for use with John Pagonis.
	Suitable database built on client side, and placed in server.
	Server left alive till user chooses to kill it.
**/
	{
	FLOG(_L("Running test 1"));
	//FIXME: TRAPD? 
	test.Start(_L("Test 1 -- Client/Server for BTCOMM testing...\n"));
	test.Printf(_L("This builds a database with three service records\n"));
	test.Printf(_L("The First one (UUID 1101) containing protocol attribute value: L2CAP, 3; RFCOMM, 1\n"));
	test.Printf(_L("the second (UUID 1102 LANP) containing protocol attribute value: L2CAP, 3; RFCOMM, 2.\n"));
	test.Printf(_L("the third (UUID 1103) containing protocol attribute value: L2CAP, 3; RFCOMM, 3.\n"));
	test.Printf(_L("the third is a WAP server record and should never be selected\n"));
	test.Printf(_L("The server then remains in listening mode\n"));
	test.Printf(_L("until the user of Test 1 types 'x'.\n\n\n\n"));
	
	TChar c;
	RSdp sdp;
	RSdpDatabase db;
//	User::After(5000000);
	TInt ret;
	LoadLDD_PDD();

	ret = sdp.Connect();
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}
	ret = db.Open(sdp);
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}

	TSdpServRecordHandle recHandle1;
	TSdpServRecordHandle recHandle2;
	TSdpServRecordHandle recHandle3;

	test.Printf(_L("SDP server with record 0, type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle1 = BuildPagonisL(db,TUUID(TUint32(0x1101)), 1);
	test.Printf(_L("Pagonis record (0x1101), type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();


	recHandle2 = BuildPagonisL(db,TUUID(TUint32(0x1102)), 2);
	test.Printf(_L("Pagonis record (0x1102), type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle3 = BuildWAPL(db, 3);
	test.Printf(_L("one WAP record, type 'x' to continue.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	test.Printf(_L("The database is now ready to test, type 'x' to exit.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();
/**/
	db.DeleteRecordL(recHandle2);

	test.Printf(_L("The database has now had the second Pagonis record removed.\
					\nWhen you have finished using this database please type 'x'.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();
	db.UpdateAttributeL(recHandle1, 0x3455, _L("Nicky!"));
	test.Printf(_L("\nTo check on updating record STATE attribute we have now\
		            \nupdated one random attribute in the first Pagonis record.\
					\nPlease type 'x' AGAIN.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();
	db.UpdateAttributeL(recHandle1, 0x3455, _L("Nicky!"));
	test.Printf(_L("\nTo check on updating record STATE attribute we have now\
		            \nupdated ONCE AGAIN a random attribute in the first Pagonis\
					\nrecord.\
					\nPlease type 'x' ONCE AGAIN.\n\n"));
	c = test.Getch();
	db.DeleteAttributeL(recHandle1, 0x3455);
	while (c!='x' && c!='X')
		c = test.Getch();
	test.Printf(_L("\nTo check on updating record STATE attribute we have now\
		            \ndeleted an attribute in the first Pagonis\
					\nrecord.\
					\nPlease type 'x' YET AGAIN - THIS TIME TO KILL THE DATABASE.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();
	db.DeleteAttributeL(recHandle1, 0x3455);
/**/
	db.DeleteRecordL(recHandle3);
	// Record with recHandle2 has already been deleted
	db.DeleteRecordL(recHandle1);
	db.Close();
	sdp.Close();

  
	test.End();
	}




void TestD()
/**
	Client/server test for use with John Pagonis.
	Suitable database built on client side, and placed in server.
	Server left alive till user chooses to kill it.
**/
	{
	FLOG(_L("Running test D"));
	//FIXME: TRAPD? 
	test.Start(_L("Test D -- Client/Server for BTCOMM testing...\n"));
	test.Printf(_L("This builds a database with four service records\n"));
	test.Printf(_L("The first one (UUID 1103 WAP) containing protocol attribute value: L2CAP, 3; RFCOMM, 3\n"));
	test.Printf(_L("the second (UUID 1101) containing protocol attribute value: L2CAP, 3; RFCOMM, 1.\n"));
	test.Printf(_L("the third (UUID 1103 WAP) containing protocol attribute value: L2CAP, 3; RFCOMM, 4.\n"));
	test.Printf(_L("the forth (UUID 1102 LANP) containing protocol attribute value: L2CAP, 3; RFCOMM, 2.\n"));
	test.Printf(_L("The server then remains in listening mode\n"));
	test.Printf(_L("until the user of Test D types 'x'.\n\n\n\n"));
	
	TChar c;
	RSdp sdp;
	RSdpDatabase db;
//	User::After(5000000);
	TInt ret;
	LoadLDD_PDD();

	ret = sdp.Connect();
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}
	ret = db.Open(sdp);
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}

	TSdpServRecordHandle recHandle1;
	TSdpServRecordHandle recHandle2;
	TSdpServRecordHandle recHandle3;
	TSdpServRecordHandle recHandle4;

	test.Printf(_L("SDP server with record 0, type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle3 = BuildWAPL(db, 3);
	test.Printf(_L("one WAP record, type 'x' to continue.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle1 = BuildPagonisL(db,TUUID(TUint32(0x1101)), 1);
	test.Printf(_L("Pagonis record (0x1101), type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle4 = BuildWAPL(db, 4);
	test.Printf(_L("one WAP record, type 'x' to continue.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle2 = BuildPagonisL(db,TUUID(TUint32(0x1102)), 2);
	test.Printf(_L("Pagonis record (0x1102), type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();


	test.Printf(_L("The database is now ready to test, type 'x' to exit.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();
	db.DeleteRecordL(recHandle4);
	db.DeleteRecordL(recHandle3);
	db.DeleteRecordL(recHandle2);
	db.DeleteRecordL(recHandle1);
	db.Close();
	sdp.Close();

  
	test.End();
	}


void TestE()
/**
	Client/server test for use with John Pagonis.
	Suitable database built on client side, and placed in server.
	Server left alive till user chooses to kill it.
**/
	{
	FLOG(_L("Running test D"));
	//FIXME: TRAPD? 
	test.Start(_L("Test E -- Client/Server for BTCOMM testing...\n"));
	test.Printf(_L("This builds a database with four service records\n"));
	test.Printf(_L("The first one (UUID 1103 WAP) containing protocol attribute value: L2CAP, 3; RFCOMM, 3\n"));
	test.Printf(_L("the second (UUID 1101) containing protocol attribute value: L2CAP, 3; RFCOMM, 1.\n"));
	test.Printf(_L("the third (UUID 1102 LANP) containing protocol attribute value: L2CAP, 3; RFCOMM, 2.\n"));
	test.Printf(_L("the forth (UUID 1103 WAP) containing protocol attribute value: L2CAP, 3; RFCOMM, 4.\n"));
	test.Printf(_L("The server then remains in listening mode\n"));
	test.Printf(_L("until the user of Test E types 'x'.\n\n\n\n"));
	
	TChar c;
	RSdp sdp;
	RSdpDatabase db;
//	User::After(5000000);
	TInt ret;
	LoadLDD_PDD();

	ret = sdp.Connect();
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}
	ret = db.Open(sdp);
	if(ret!=KErrNone)
		{
		test.End();
		return;
		}

	TSdpServRecordHandle recHandle1;
	TSdpServRecordHandle recHandle2;
	TSdpServRecordHandle recHandle3;
	TSdpServRecordHandle recHandle4;

	test.Printf(_L("SDP server with record 0, type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle3 = BuildWAPL(db, 3);
	test.Printf(_L("one WAP record, type 'x' to continue.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle1 = BuildPagonisL(db,TUUID(TUint32(0x1101)), 1);
	test.Printf(_L("Pagonis record (0x1101), type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle2 = BuildPagonisL(db,TUUID(TUint32(0x1102)), 2);
	test.Printf(_L("Pagonis record (0x1102), type 'x' to continue.\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();

	recHandle4 = BuildWAPL(db, 4);
	test.Printf(_L("one WAP record, type 'x' to continue.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();


	test.Printf(_L("The database is now ready to test, type 'x' to exit.\n\n"));
	c = test.Getch();
	while (c!='x' && c!='X')
		c = test.Getch();
	db.DeleteRecordL(recHandle4);
	db.DeleteRecordL(recHandle3);
	db.DeleteRecordL(recHandle2);
	db.DeleteRecordL(recHandle1);
	db.Close();
	sdp.Close();

  
	test.End();
	}

void WhichTest()
	{
	TChar c;
	test.Printf(_L("\nwhich test? "));
	test.Printf(_L("\npress 0 for UUID 1101, 1102, 1103 "));
	test.Printf(_L("\npress 1 for UUID 1103, 1101, 1103, 1102 "));
	test.Printf(_L("\npress 2 for UUID 1103, 1101, 1102, 1103 "));
	c = test.Getch(); 
	switch (c) 
		{
	case '0':
		Test1();
		break;
	case '1':
		TestD();
		break;
	case '2':
		TestE();
		break;
	default:
		break;
		}
	}






void Test2()
/**
	Builds up a static DB, and prints it,
	using the record and attribute iteraotrs, and a
	simple console-printing attribute value visitor.
**/
	{
	//FIXME: TRAPD?
	test.Start(_L("Test 2 -- Build and print Db"));
//	CSdpDatabase *theDb = BuildDbL();
	CSdpDatabase *theDb = BuildUpf4DbL();
	test(theDb!=NULL);
	PrintDb(*theDb, *test.Console());
	delete theDb;

	test.End();
	}

void Test3()
/**
	Builds up a static DB, and copies it into the RSdpDatabase
**/
	{
	//FIXME: TRAPD?
	test.Start(_L("Test 3 -- Build and print Db"));
//	CSdpDatabase *theDb = BuildDbL();
	CSdpDatabase *theDb = BuildUpf4DbL();
	test(theDb!=NULL);
	CleanupStack::PushL(theDb);

	RSdp sdp;
	RSdpDatabase db;
	TInt ret;
	LoadLDD_PDD();

	test.Next(_L("Opening SDP Database"));
	ret = sdp.Connect();
	test(ret==KErrNone);
	CleanupClosePushL(sdp);
	ret = db.Open(sdp);
	test(ret==KErrNone);
	CleanupClosePushL(db);

	test.Next(_L("Storing database"));
	RegisterDatabaseInServerL(db, *theDb);

	test.Next(_L("Database stored. Now run your tests! Press any key to close SDDB"));
	test.Getch();

	CleanupStack::PopAndDestroy(3); // db, sdp, theDb

	test.End();
	}


void Test4()
/**
	Builds a static DB and searches it for a 
	UUID search pattern (as per SDP service search
	requests)
**/
	{
	test.Start(_L("Test 2 -- Service search over Db"));
	CSdpDatabase *theDb = BuildDbL();

	CSdpSearchPattern* thePattern = CSdpSearchPattern::NewL();
	//Service pattern with one UUID in it...
	test.Next(_L("Doing un-encoded tests"));
	test.Next(_L("Searching for SDP server service UUID (0x1000)"));
	thePattern->AddL(TUUID(TUint16(0x1000)));
	SearchDbL(*theDb, *thePattern, 2); // Expect 2 matches

	test.Next(_L("Searching for SDP server service and bogoid UUID"));
	thePattern->AddL(TUUID(TUint32(0x55667788)));	
	SearchDbL(*theDb, *thePattern, 1); // Expect 1 matches

	test.Next(_L("Searching for SDP server service and two bogoid UUID"));
	thePattern->AddL(TUUID(TUint32(0x11112222)));	
	SearchDbL(*theDb, *thePattern, 0); // Expect 0 matches

	test.Next(_L("Searching for RFCOMM uuid"));
	thePattern->Reset();
	thePattern->AddL(TUUID(TUint16(0x0003)));	
	SearchDbL(*theDb, *thePattern, 1); // Expect 1 matches


	thePattern->Reset();

	theDb->EncodeDbL();

	test.Next(_L("Doing encoded tests"));
	test.Next(_L("Searching for SDP server service UUID (0x1000)"));
	thePattern->AddL(TUUID(TUint16(0x1000)));
	SearchDbL(*theDb, *thePattern, 2); // Expect 2 matches

	test.Next(_L("Searching for SDP server service and bogoid UUID"));
	thePattern->AddL(TUUID(TUint32(0x55667788)));	
	SearchDbL(*theDb, *thePattern, 1); // Expect 1 matches

	test.Next(_L("Searching for SDP server service and two bogoid UUID"));
	thePattern->AddL(TUUID(TUint32(0x11112222)));	
	SearchDbL(*theDb, *thePattern, 0); // Expect 0 matches

	test.Next(_L("Searching for RFCOMM uuid"));
	thePattern->Reset();
	thePattern->AddL(TUUID(TUint16(0x0003)));	
	SearchDbL(*theDb, *thePattern, 1); // Expect 1 matches



	delete thePattern;
	delete theDb;
	test.End();
	}

void TestEncoder(TInt aType, CSdpDatabase *aDb)
	{
	TInt64 seed = aType * 49;

	test.Start(_L("Test 3 -- Encoder & Parser"));
	CSdpDatabase *builtDb = CSdpDatabase::NewL();

	HBufC8* outbuf = HBufC8::New(1024);
	TPtr8 writePtr (outbuf->Des());
	TElementEncoder theEncoder(writePtr);
	CElementParser* theParser = CElementParser::NewL(0);
	CSdpAttrIdMatchList *theAttrList = CSdpAttrIdMatchList::NewL();

	theAttrList->AddL(KAttrRangeAll);

	test.Next(_L("Encode and parse ref DB"));
		{
		for(TServRecordIter recIter(aDb->RecordIter()); recIter; recIter++)
			{// Iterate thru records in Db
			TInt startPos = writePtr.Length();
			CAttrExtractVisitor::EncodeAttributesL(theEncoder, *recIter, *theAttrList);
			// grows writePtr

			CSdpServRecord* builtRec = CSdpServRecord::NewL();
			theParser->Reset(builtRec);
			TInt incr = 1;
			TBool moreExpected = ETrue;
			for (TInt i = startPos; i < writePtr.Length(); i += incr)
				{// parse it back, in inreasing chunks
				switch(aType)
					{
				case 0:
					incr += 1;
					break;
				case 1:
					incr <<= 1;
					break;
				case 2:
				default:
					incr = Math::Rand(seed) % (i+2);
					}
				incr=Min(incr, writePtr.Length() - i);
				moreExpected = theParser->BufferedParseL(writePtr.Mid(i, incr));
				}
			test(!moreExpected);
			builtDb->AddRecord(builtRec);
			}
		}

	HBufC8* outbuf2 = HBufC8::New(outbuf->Size());
	writePtr.Set (outbuf2->Des());

	test.Next(_L("Re-Encode the DB built from parser"));
		{
		for(TServRecordIter recIter(builtDb->RecordIter()); recIter; recIter++)
			{// Iterate thru records in Db
			CAttrExtractVisitor::EncodeAttributesL(theEncoder, *recIter, *theAttrList);
			}
		}

//	test(*outbuf == *outbuf2);

	// The bit below is equivalent to the above line, only easier to debug
	test(outbuf->Length() == outbuf2->Length());
	for (TInt i = 0; i < outbuf->Length(); ++i)
		{
		test((*outbuf)[i] == (*outbuf2)[i]);
		}

	delete outbuf;
	delete outbuf2;
	delete theParser;
	delete theAttrList;
	delete builtDb;
	test.End();
	}


void Test5()
	{
	for (TInt i = 0; i < 8; ++i)
		{
		test.Next(_L("Doing tests with ref Db"));
		CSdpDatabase *theDb = BuildDbL();
		TestEncoder(i, theDb);
		delete theDb;

		test.Next(_L("Doing tests with UPF4 Db"));
		theDb = BuildUpf4DbL();
		TestEncoder(i, theDb);
		delete theDb;
		}
	}

void Test6()
	{
	test.Start(_L("Test 6 -- Database size and encoding stuff"));
	CSdpDatabase *theDb = BuildDbL();

	const TUint KExpectedSizes[] = 
		{
		5,  5,  12, 28,
		14, 5,  5,  22,
		23, 35, 5,  10,
		3,  15, 19,
		};
	test.Next(_L("Testing the size functions"));
	TInt ind = 0;
	for(TServRecordIter recIter(theDb->RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			TUint size = TElementEncoder::EncodedSize((*attrIter).Value().Type(), (*attrIter).Value().DataSize());
			test.Printf(_L("Attribute value Encoded size is %d\n"), size);
			test(size == KExpectedSizes[ind]);
			++ind;
			}
		}

	delete theDb;

	test.Next(_L("Testing attribute match list stuff"));
	CSdpAttrIdMatchList *theAttrList = CSdpAttrIdMatchList::NewL();

	theAttrList->AddL(TAttrRange(10, 15));	// insert at beginning ->	10-15
	theAttrList->AddL(TAttrRange(30, 40));	// insert at end	->	10-15,30-40
	theAttrList->AddL(TAttrRange(18, 19));	// insert between 2	->	10-15, 18-19, 30-40
	theAttrList->AddL(TAttrRange(27, 29));	// append to start of another range	->	10-15, 18-19, 27-40
	theAttrList->AddL(TAttrRange(41, 45));	// append to end of another range	->	10-15, 18-19, 27-45
	theAttrList->AddL(TAttrRange(16, 17));	// join 2 ranges together	->	10-19,  27-45
	theAttrList->AddL(TAttrRange(21, 24));	// insert between 2	->	10-19, 21-24, 27-45
	test(theAttrList->Count() == 3);
		
	TInt posn;
	test(theAttrList->InMatchListRange(10, posn));
	test(theAttrList->InMatchListRange(11, posn));
	test(theAttrList->InMatchListRange(15, posn));
	
	test(theAttrList->InMatchListRange(21, posn));
	test(theAttrList->InMatchListRange(22, posn));
	test(theAttrList->InMatchListRange(23, posn));	
	test(theAttrList->InMatchListRange(24, posn));	
	
	test(theAttrList->InMatchListRange(45, posn));	
	
	theAttrList->RemoveL(TAttrRange(12, 14));	// mid-range ->	10-11, 15-19, 21-24, 27-45
	test(theAttrList->Count() == 4);

	theAttrList->RemoveL(TAttrRange(12, 14));	// 	no change ->	10-11, 15-19, 21-24, 27-45
	theAttrList->RemoveL(TAttrRange(14, 16));	// overlap start	->	10-11, 17-19, 21-24, 27-45
	test(theAttrList->Count() == 4);	
	theAttrList->RemoveL(TAttrRange(17, 20));	// remove one item	->	10-11, 21-24, 27-45
	theAttrList->RemoveL(TAttrRange(40, 45));	// end of range 	->	10-11, 21-24, 27-39
	theAttrList->RemoveL(TAttrRange(28, 28));	// range of one 	->	10-11, 21-24, 27, 29-39
	test(theAttrList->Count() == 4);	
	test(theAttrList->InMatchList(27));	
	theAttrList->RemoveL(TAttrRange(26, 28));	// remove range of one 	->	10-11, 21-24, 29-39
	test(theAttrList->Count() == 3);
			
	theAttrList->AddL(TAttrRange(15, 15));	// insert between 2	->	10-11, 15, 21-24, 29-39
	theAttrList->AddL(TAttrRange(17, 18));	// insert between 2	->	10-11, 15, 17-18, 21-24, 29-39
	test(theAttrList->Count() == 5);
	theAttrList->AddL(TAttrRange(13, 26));	// obliterate 3 with one big insertion ->	10-11, 13-26, 29-39
	test(theAttrList->Count() == 3);			
	theAttrList->RemoveL(TAttrRange(15, 17));	//->	10-11, 13-14, 18-26, 29-39		
	theAttrList->RemoveL(TAttrRange(19, 21));	//->	10-11, 13-14, 18, 22-26, 29-39
	theAttrList->RemoveL(TAttrRange(23, 24));	//->	10-11, 13-14, 18, 22, 25-26, 29-39	
	test(theAttrList->Count() == 6);	
	theAttrList->AddL(TAttrRange(16, 28));	// obliterate 3 with one big deletion ->	10-11, 13-14, 29-39
	test(theAttrList->Count() == 3);	

	theAttrList->RemoveL(TAttrRange(31, 31));	//->		10-11, 13-14, 29-30 32-39
	test(theAttrList->Count() == 4);	
	theAttrList->AddL(TAttrRange(28, 31));	// new range contiguous with 2 current ranges ->	10-11, 13-14, 28-39	
	test(theAttrList->Count() == 3);				
				
	theAttrList->RemoveL(TAttrRange(0, 45)); // remove all entries
					
	theAttrList->AddL(TAttrRange(50, 300));	// 50-300
		
	test(theAttrList->InMatchList(50));
	test(theAttrList->InMatchList(256));	
	test(theAttrList->InMatchList(300));

	test(theAttrList->InMatchList(301) == EFalse);
	test(theAttrList->InMatchList(49) == EFalse);

	theAttrList->AddL(TAttrRange(302, 304));	// 50-300, 302-304
	test(theAttrList->Count() == 2);

	theAttrList->AddL(TAttrRange(50, 50));
	test(theAttrList->Count() == 2);

	theAttrList->AddL(TAttrRange(45, 310));		// 45-310
	test(theAttrList->Count() == 1);
	test(theAttrList->InMatchList(45));
	test(theAttrList->InMatchList(310));
	test(theAttrList->InMatchList(311) == EFalse);
	test(theAttrList->InMatchList(44) == EFalse);

	theAttrList->AddL(TAttrRange(45, 200));		// 45-310
	test(theAttrList->Count() == 1);

	theAttrList->RemoveL(TAttrRange(50, 60));		// 45-49 61-310
	test(theAttrList->Count() == 2);
	test(theAttrList->InMatchList(49));
	test(theAttrList->InMatchList(50) == EFalse);
	test(theAttrList->InMatchList(60) == EFalse);
	test(theAttrList->InMatchList(61));
	test(theAttrList->InMatchList(45));
	test(theAttrList->InMatchList(310));
	test(theAttrList->InMatchList(311) == EFalse);

	theAttrList->RemoveL(TAttrRange(55, 65));		// 45-49 66-310
	test(theAttrList->Count() == 2);
	test(theAttrList->InMatchList(66));
	test(theAttrList->InMatchList(65) == EFalse);

	theAttrList->RemoveL(TAttrRange(55, 55));
	test(theAttrList->Count() == 2);
	theAttrList->RemoveL(TAttrRange(1, 2));
	theAttrList->RemoveL(TAttrRange(400, 500));
	test(theAttrList->Count() == 2);

	theAttrList->RemoveL(TAttrRange(40, 70));		// 71-310
	test(theAttrList->Count() == 1);

	theAttrList->AddL(TAttrRange(5, 39));			// 5-39, 71-310
	test(theAttrList->Count() == 2);

	theAttrList->RemoveL(TAttrRange(38, 38));		// 5-37, 39, 71-310
	test(theAttrList->Count() == 3);
	theAttrList->RemoveL(TAttrRange(36, 36));		// 5-35, 37, 39, 71-310
	test(theAttrList->Count() == 4);

	theAttrList->RemoveL(TAttrRange(30, 70));		// 5-29, 71-310
	test(theAttrList->Count() == 2);

	theAttrList->RemoveL(TAttrRange(1, 309));		// 310
	test(theAttrList->Count() == 1);
	test(theAttrList->InMatchList(310));
	test(theAttrList->InMatchList(309) == EFalse);
	test(theAttrList->InMatchList(311) == EFalse);

	theAttrList->RemoveL(TAttrRange(1, 310));		// none
	test(theAttrList->Count() == 0);


	theAttrList->AddL(TAttrRange(45, 310));			// 45-310
	test(theAttrList->Count() == 1);

	theAttrList->AddL(TAttrRange(2000));			// 45-310, 2000
	test(theAttrList->Count() == 2);
	test(theAttrList->InMatchList(2000));
	test(theAttrList->InMatchList(2001) == EFalse);

	theAttrList->AddL(TAttrRange(2002));			// 45-310, 2000, 2002
	test(theAttrList->Count() == 3);

	theAttrList->AddL(TAttrRange(2004));			// 45-310, 2000, 2002, 2004
	test(theAttrList->Count() == 4);
	test(theAttrList->InMatchList(2004));
	test(theAttrList->InMatchList(2003) == EFalse);

	theAttrList->AddL(TAttrRange(2003));			// 45-310, 2000, 2002-2004
	test(theAttrList->Count() == 3);

	theAttrList->AddL(TAttrRange(301, 2001));		// 45-2004
	test(theAttrList->Count() == 1);
	test(theAttrList->InMatchList(45));
	test(theAttrList->InMatchList(2004));
	test(theAttrList->InMatchList(0) == EFalse);
	test(theAttrList->InMatchList(2010) == EFalse);

	TInt count = theAttrList->Count();
	for (TUint16 i = 3000; i < 4000; i += 2)
		{
		theAttrList->AddL(TAttrRange(i, i));
		++count;
		test(theAttrList->Count() == count);
		test(theAttrList->InMatchList(i));
		test(theAttrList->InMatchList(TUint16(i-1)) == EFalse);
		test(theAttrList->InMatchList(TUint16(i+1)) == EFalse);
		}

	for (TUint16 j = 3001; j < 3501; j += 2)
		{
		theAttrList->AddL(TAttrRange(j, j));
		--count;
		test(theAttrList->Count() == count);
		test(theAttrList->InMatchList(j));
		test(theAttrList->InMatchList(TUint16(j-1)));
		test(theAttrList->InMatchList(TUint16(j+1)));
		}

	for (TUint16 k = 3001; k < 3400; k += 2)
		{
		theAttrList->RemoveL(TAttrRange(k, k));
		++count;
		test(theAttrList->Count() == count);
		test(theAttrList->InMatchList(k) == EFalse);
		test(theAttrList->InMatchList(TUint16(k-1)));
		test(theAttrList->InMatchList(TUint16(k+1)));
		}

	theAttrList->AddL(TAttrRange(0, 5000));
	test(theAttrList->Count() == 1);
	test(theAttrList->InMatchList(0));
	test(theAttrList->InMatchList(5000));
	test(theAttrList->InMatchList(5001) == EFalse);
	test(theAttrList->InMatchList(KMaxTUint16) == EFalse);

	theAttrList->RemoveL(TAttrRange(0, 5000));
	test(theAttrList->Count() == 0);

	delete theAttrList;

	test.End();

	}

/* moved out to the database dll as a utility method
CSdpDatabase * EncodeDb(CSdpDatabase * nonEncodedDb)
{
	CSdpDatabase *encDb = CSdpDatabase::NewL();

	TUint8 outbuf[72];
	TPtr8 writePtr (outbuf, 0, 72);
	CSdpServRecord *theRec = CSdpServRecord::NewL();
	TElementEncoder theEncoder(writePtr);

	test.Next(_L("Test attribute encoder"));
	for(TServRecordIter recIter(nonEncodedDb->RecordIter()); recIter; recIter++)
		{// Iterate through records in Db

		for (TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// iterate through attributes in record
			writePtr.Set(outbuf, 0, 72);  // bug, just declaring this (see above) didn't reset the pointer on each loop

			CAttrEncoderVisitor::EncodeAttributeL(theEncoder, (*attrIter).Value());
			TBuf8<sizeof(TUint)> val(2);
			TElementEncoder::PutUint(&val[0], (*attrIter).AttributeID(), 2);
			test.Printf(_L("AttrID:0x%02x%02x, <"), val[0], val[1]);
			HexDes(writePtr);
			test.Printf(_L(">\n"));
			theRec->BuildUintL(val)->BuildEncodedL(writePtr);
			}
		encDb->AddRecord(theRec);
		theRec = CSdpServRecord::NewL(); // need a new one
		}
	delete theRec;
	return encDb;
}
*/

// build an encoded version of the database then print it.
void Test7()
	{
	test.Start(_L("Test 7 -- Encoded database"));
	CSdpDatabase *theDb = BuildUpf4DbL();
	theDb->EncodeDbL();

	test.Next(_L("Print encoded attributes"));
	PrintDb(*theDb, *test.Console());

	delete theDb;
	test.End();
	}

// test the access routines.
void ShowCollected(	CSizeAccumulator* aCollector)
{
	__UHEAP_MARK;
	CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(*test.Console());
	TInt indRec, indAtt;
	TUint leftOver;
	TInt startSize = 0;
	TInt nextLeft;
	TInt nextSize;
	TBool succ = EFalse;
	TInt orig = aCollector->SizeLeft();
	TInt hcount = aCollector->HandleCount();
	test.Printf(_L("Size:%d, handles %d\n"), orig, hcount);
	for (TInt i = 0; i<hcount; i++)
	{
		TSdpServRecordHandle hhand = aCollector->HandleAt(i);
		TInt atcount = aCollector->AttrCount(i);
		test.Printf(_L("HandleID:%x, attrib count:%d\n"), hhand, atcount);
		for (TInt j = 0; j<atcount; j++)
		{
			CAttrSizeItem* atpointer = aCollector->AttributeOf(i, j);
			test.Printf(_L("Attribute Pointer:%x\n"), atpointer);
			test.Printf(_L("Attr:"));
//			HexDes(atpointer->Attr()->Value().Des());
// just in case we get a non-encoded attribute
			theVisitor->VisitAttributeL(*atpointer->Attr());
			theVisitor->VisitAttributeValueL(atpointer->Attr()->Value(), atpointer->Attr()->Value().Type());
			test.Printf(_L("\n"));
			nextSize = atpointer->Size();
			startSize += nextSize;
			startSize += 3;			// the header size...
			indRec = 0;
			indAtt = 0;
			leftOver = 0;
			succ = aCollector->StartAt(startSize, leftOver, indRec, indAtt);
			if (succ)
			{
				nextLeft = aCollector->SizeLeft();
				test.Printf(_L("updated start point by (%d+3) from %d to %d\n"), nextSize, orig, nextLeft);
				test.Printf(_L("indices are L:%d, R:%d, A:%d\n"),leftOver, indRec, indAtt);
				orig = nextLeft;
			}
			else
				test.Printf(_L("failed to update start point by %d from %d\n"), nextSize, orig);


		}
	}
	delete theVisitor;
	__UHEAP_MARKEND;
	test.Getch();
}

void Test8()
	{
	__UHEAP_MARK;
	test.Start(_L("Test 8 -- Size visitor"));
	CSdpDatabase *theDb = BuildUpf4DbL();
	theDb->EncodeDbL();

//	test.Next(_L("Testing sizes of records"));
	CSdpAttrIdMatchList *theAttrList = CSdpAttrIdMatchList::NewL();

	theAttrList->AddL(TAttrRange(4)); // protocol descriptor list

	theAttrList->AddL(TAttrRange(50, 300)); // mostly text based stuff

	CSdpSearchPattern* thePattern = CSdpSearchPattern::NewL();
//Service pattern with one UUID in it...
	thePattern->AddL(TUUID(TUint16(0x1103)));
	CSizeAccumulator* collector = CSizeAccumulator::NewL();
	test.Next(_L("Testing for UUID 1103\n"));
	CResponseSizeVisitor::SizeRespSSL(*theDb, *thePattern, *collector);
	ShowCollected(collector);
// Iterate thru records in Db for SizeRespARL
	test.Next(_L("Testing for attr 4, 50-300\n"));
	for(TServRecordIter recIter(theDb->RecordIter()); recIter; recIter++)
		{
		test.Printf(_L("\n...Sizing Record 0x%x\n"), (*recIter).Handle());
		delete collector;
		collector = CSizeAccumulator::NewL();
		CResponseSizeVisitor::SizeRespARL((*recIter), *theAttrList, *collector); // what does it do if the collectors been used ?
		ShowCollected(collector);
		}
//
	test.Next(_L("Testing for attr 100\n"));
	delete theAttrList;
	theAttrList = CSdpAttrIdMatchList::NewL();
	theAttrList->AddL(TAttrRange(0x0100)); // names
	delete thePattern;
	thePattern = CSdpSearchPattern::NewL();
//Service pattern with one UUID in it...
	thePattern->AddL(TUUID(TUint16(0x1002)));
	delete collector;
	collector = CSizeAccumulator::NewL();
	CResponseSizeVisitor::SizeRespSAL(*theDb, *thePattern, *theAttrList, *collector);
	ShowCollected(collector);
	delete collector;
	delete theAttrList;
	delete thePattern;
	delete theDb;
	__UHEAP_MARKEND;
	test.End();
	}


void Test9(TInt flag)
	{
	CSdpDatabase *theDb;
	__UHEAP_MARK;
	TChar c = 'c';
	if(!flag)
		{
		c = test.Getch();
		switch (c)
			{
		case 'A': case 'a':
			test.Start(_L("Test 9 -- \"Rhubarb\" Continuation"));
			//FIXME: TRAPD?
			theDb = BuildContinuationDbL();
			break;
		case 'B': case 'b':
			test.Start(_L("Test 9 -- Continuation with Nested Lists"));
			//FIXME: TRAPD?
			theDb = BuildNestedListDbL();
			break;
		default:
		return;
			}
		}
	else
		{
		test.Start(_L("Test 9 -- Prequal Database"));
		//FIXME: TRAPD?
		theDb = BuildPrequalDbL();
		}

	test(theDb!=NULL);
	CleanupStack::PushL(theDb);
	TServRecordIter recIter(theDb->RecordIter());
	TServAttrIter attrIter((*recIter).AttributeIter());
	TServAttrIter attrIter2((*recIter).AttributeIter());
	for(; recIter; recIter++)
		{// Iterate thru records in Db
		attrIter = (*recIter).AttributeIter();
		for(; attrIter; attrIter++)
			{// Iterate thru attributes in record
			if((*attrIter).Value().Type()==ETypeString)
				{
				TPtrC8 ptr((*attrIter).Value().Des());
				if(ptr.Length()<0x100)
					{
					attrIter2 = attrIter;
					}
				}
			}
		}
	if((*attrIter2).Value().Type()==ETypeString)
		{
		TPtrC8 ptr1((*attrIter2).Value().Des());
		TBuf16<0x100> ptr(0);
		for(TInt i = 0;i<ptr1.Length()&&i<(ptr.MaxLength()-3);++i)
			ptr.Append(ptr1[i]);
		if(c=='a'||c=='A')
			{
			test.Printf(_L("Id: %d\n"),(*attrIter2).AttributeID());
			test.Printf(_L("String length: %d\n"),ptr.Length());
			if(ptr.Length()<0x100)
				test.Printf(_L("%s\n"),&ptr[0]);
			}
		}

	RSdp sdp;
	RSdpDatabase db;
	TInt ret;
	LoadLDD_PDD();

	test.Next(_L("Opening SDP Database"));
	ret = sdp.Connect();
	if(ret!=KErrNone)
		{
		if(ret==KErrNotFound)
			test.Printf(_L("Cannot start sdp server, could not find bluetooth.\n"));
		else
			test.Printf(_L("Cannot start sdp server.\n"));
		test.Next(_L("PRESS ANY KEY.\n"));
		test.Getch();

		CleanupStack::PopAndDestroy(); // theDb
		goto end;
		}
	test(ret==KErrNone);
	CleanupClosePushL(sdp);
	ret = db.Open(sdp);
	test(ret==KErrNone);
	CleanupClosePushL(db);

	test.Next(_L("Storing database"));
	RegisterDatabaseInServerL(db, *theDb);

	test.Next(_L("Database stored. Now run your tests!"));
	test.Next(_L("Press any key to close SDDB"));
	test.Getch();

	CleanupStack::PopAndDestroy(3); // db, sdp, theDb
end:
	__UHEAP_MARKEND;
	test.End();
	}

void Test10()
{
	__UHEAP_MARK;
//TUint32
	TInt64 putVal32 = static_cast<TInt64>(0xBA1EFACE);
	TPckgBuf<TUint32> value32Packaged;	
	SdpUtil::PutUint(&value32Packaged[0], putVal32, 4);
	
	TUint32 getVal32= SdpUtil::GetUint(value32Packaged);
	test.Printf(_L("SdpUtil: TPckgBuf<TUint32> created with 0x%08lx would return 0x%x\n"), putVal32, getVal32);


	TBuf8<32/8> testValue32Packaged;
	testValue32Packaged.SetLength(4);
	testValue32Packaged = value32Packaged;
	
	CSdpAttrValue* attrVal = 0;	
	attrVal = CSdpAttrValueUint::NewUintL(testValue32Packaged);
	TUint getter = attrVal->Uint();
	delete attrVal;
	
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%08lx returns 0x%x\n"), putVal32, getter);
	
	
// Now we have confirmed that exercising SdpUtil this way is equivalent to using CSdpAttrValueUint's methods.
// The CSdpAttrValueUint::Uint64 and CSdpAttrValueUint::Uint128 methods are now exported so we can test them, 
// but they use SdpUtil too, so..
	
//TUint64
//	TPckgBuf<TUint64> putVal64Packaged =0x1234FACE5678ABCD;   // Use this to test the full functionality
	TPckgBuf<TUint64> putVal64Packaged =0x1234FACE;  	// This number is acceptable to the gcc_xml compiler

	TUint64 putVal64 = putVal64Packaged();
	
	TPckgBuf<TUint64> val64Packaged;
	SdpUtil::PutUint64(&val64Packaged[0], putVal64);
	
	TUint64 getVal64;
	SdpUtil::GetUint64(val64Packaged, getVal64);
	
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx would return 0x%lx\n"), putVal64, getVal64);

	TBuf8<64/8> testValue64Packaged;
	testValue64Packaged.SetLength(8);
	testValue64Packaged = val64Packaged;

	CSdpAttrValue* attrVal64 = 0;	
	attrVal64 = CSdpAttrValueUint::NewUintL(testValue64Packaged);
	TUint size64 = attrVal64->DataSize();
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx would be %d bytes long\n"), putVal64, size64);

	CSdpAttrValueUint& attrValueUint64 = static_cast<CSdpAttrValueUint&>(*attrVal64);
	attrValueUint64.Uint64(getVal64);
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx would return 0x%lx\n"), putVal64, getVal64);

	delete attrVal64;
	

//two TUint64s	
//	TUint64 putLoVal = 0x1234FACE5678ABCD;  // Use these to test the full functionality
//	TUint64 putHiVal = 0x3456789BA1EFACED; 

	TUint64 putLoVal = 0x1234FACE; 	// These numbers are acceptable to the gcc_xml compiler
	TUint64 putHiVal = 0xBA1EACED; 

	TBuf8<128/8> putValue128Packaged;
	putValue128Packaged.SetLength(16);

	SdpUtil::PutUint128(&putValue128Packaged[0], putLoVal, putHiVal);
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx,0x%lx would be %d bytes long\n"), putHiVal, putLoVal, 16);

	TUint64 getLoVal128;
	TUint64 getHiVal128;
	SdpUtil::GetUint128(putValue128Packaged, getLoVal128, getHiVal128);
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx,0x%lx would return 0x%lx,0x%lx\n"), putHiVal, putLoVal, getHiVal128, getLoVal128);

	TBuf8<128/8> testValue128Packaged;
	testValue128Packaged.SetLength(16);
	testValue128Packaged = putValue128Packaged;

	CSdpAttrValue* attrVal128 = 0;	
	attrVal128 = CSdpAttrValueUint::NewUintL(testValue128Packaged);
	TUint size128 = attrVal128->DataSize();
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx,0x%lx would be %d bytes long\n"), putHiVal, putLoVal, size128);

	CSdpAttrValueUint& attrValueUint128 = static_cast<CSdpAttrValueUint&>(*attrVal128);
	attrValueUint128.Uint128(getLoVal128, getHiVal128);
	test.Printf(_L("SdpUtil: CSdpAttrValueUint created with 0x%lx,0x%lx would return 0x%lx,0x%lx\n"), putHiVal, putLoVal, getHiVal128, getLoVal128);

	delete attrVal128;

	__UHEAP_MARKEND;
	
}

void IntConversionTest()
/**
	Checks compiler has done uint-int cast correctly
	in CSdpAttrValueInt::Int().
**/
	{
	TBuf8<4> tstr;
	tstr.FillZ(4);
	tstr[0] = 0xff;
	tstr[1] = 0xff;
	tstr[2] = 0xff;
	tstr[3] = 0xff;
	TInt ret = 0;
	CSdpAttrValueInt* avi = 0;
	avi = CSdpAttrValueInt::NewIntL(tstr);
	ret = avi->Int();
	delete avi;
	avi = 0;
	__ASSERT_DEBUG(ret==-1, User::Panic(_L("Bad4ByteIntConversion"), KErrGeneral));
	tstr.SetLength(2);
	avi = CSdpAttrValueInt::NewIntL(tstr);
	ret = avi->Int();
	delete avi;
	avi = 0;
	__ASSERT_DEBUG(ret==-1, User::Panic(_L("Bad2ByteIntConversion"), KErrGeneral));
	//tstr[0] = 0x0e;
	tstr.SetLength(1);
	avi = CSdpAttrValueInt::NewIntL(tstr);
	ret = avi->Int();
	delete avi;
	avi = 0;
	__ASSERT_DEBUG(ret==-1, User::Panic(_L("BadByteIntConversion"), KErrGeneral));

	(void)(ret != 0); // keep compiler happy by referencing ret as an r-value in urel

	return;
	}

void RunAppL()
	{
//	TInt ret;
	test.Title();
	test.Start(_L("Running SDP server tests"));
// For some reason, you have to do the following to
// ensure that the file server behaves properly.
	RFs fs;
	(void)fs.Connect();
	fs.Close();
// Run through tests
	IntConversionTest();
	FOREVER
		{
		test.Printf(_L("Choose the test to run...\n"));
		test.Printf(_L("************************\n"));
		test.Printf(_L("0. Run client/server\n"));
		test.Printf(_L("1. Run client/server building the John Pagonis database\n"));
		test.Printf(_L("2. Build static DB & print\n"));
		test.Printf(_L("3. Build static DB & copy it over into the SDP database server\n"));
		test.Printf(_L("4. Service Search (no continuation)\n"));
		test.Printf(_L("5. Attribute Request (no continuation)\n"));
		test.Printf(_L("6. Sizing and encoding stuff\n"));
		test.Printf(_L("7. encoded database\n"));
		test.Printf(_L("8. size visitor\n"));
		test.Printf(_L("9a.(Press keys SLOWLY) Build \"Rhubarb\" continuation database\n"));
		test.Printf(_L("9b.(Press keys SLOWLY) Build nested lists continuation database\n"));
		test.Printf(_L("B. Run UPF back to back tests\n"));
		test.Printf(_L("C. Continuation stress tests\n"));
//		test.Printf(_L("U. Unplugfest tests\n"));
		test.Printf(_L("P. Prequal tests database\n"));
		test.Printf(_L("Q. SdpUtil tests\n"));
		test.Printf(_L("\n"));
		test.Printf(_L("A. Run All tests\n"));
		test.Printf(_L("************************\n"));
		test.Printf(_L("press Escape to quit\n\n"));

		__UHEAP_MARK;
		TChar c = test.Getch();
		switch (c)
			{
		case 'A': case 'a':
			Test1();
			__UHEAP_CHECK(0);
			Test2();
			__UHEAP_CHECK(0);
			Test3();
			__UHEAP_CHECK(0);
			Test4();
			__UHEAP_CHECK(0);
			Test5();
			__UHEAP_CHECK(0);
			Test6();
			__UHEAP_CHECK(0);
			break;
		case '0':
			Test0();
			break;
		case '1':
			WhichTest();
			break;
		case '2':
			Test2();
			break;
		case '3':
			Test3();
			break;
		case '4':
			Test4();
			break;
		case '5':
			Test5();
			break;
		case '6':
			Test6();
			break;
		case '7':
			Test7();
			break;
		case '8':
			Test8();
			break;
		case '9':
			Test9(0);
			break;
		case 'B': case 'b':
			TestBL(test);
			break;
		case 'C': case 'c':
			__UHEAP_MARK;
			TestCL(test, BuildContinuationDbL());
			__UHEAP_MARKEND;
			break;
		case 'D': case 'd':
			TestD();
			break;
		case 'E': case 'e':
			TestE();
			break;
		case 'P': case 'p':
			Test9(1);
			break;
		case 'Q': case 'q':
			Test10();
			break;

		case EKeyEscape:
			goto done;
		default:
			test.Printf(_L("Unknown command\n"));
			};
		__UHEAP_MARKEND;
		}

done:

	test.Printf(_L("Finished! Press any key...\r\n"));
	test.Getch();
	test.End();
	}


TInt E32Main()
	{

	test.SetLogged(ETrue);
	test.Title();
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(error,RunAppL()); // Run the App
	__ASSERT_ALWAYS(error==KErrNone,User::Panic(_L("EPOC32EX"),error));
	delete cleanup;
	__UHEAP_MARKEND;
	test.Close();
	return KErrNone;
	}
