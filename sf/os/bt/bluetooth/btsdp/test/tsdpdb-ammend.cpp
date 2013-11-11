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
	test.Start(_L("Test 1 ...\n"));
	//__BREAKPOINT();
	
	TChar c;
	RSdp sdp;
	RSdpDatabase db;
	TInt ret;
	LoadLDD_PDD();

	test.Printf(_L("About to play with the SDP database\n - make sure the SDP database EXISTS, and then press any key.\n"));
	test.Getch();
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

		test.End();
		return;
		}
	test(ret==KErrNone);
	ret = db.Open(sdp);
	if(ret!=KErrNone)
		{
		test.Printf(_L("Starting SDP Server SESSION FAILED.\n"));
		test.End();
		return;
		}

	TSdpServRecordHandle recHandle1 = 0x0010000;
	TUint16 attrId = 0x01;


	CSdpAttrValueDES* attrVal = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(attrVal);
	MSdpElementBuilder* bldr = attrVal;
	bldr->StartListL()->BuildUUIDL(TUUID(TUint16(0x1000)))->EndListL();
	TRAPD(err, db.UpdateAttributeL(recHandle1, attrId, *attrVal));
	CleanupStack::PopAndDestroy();

	test.Printf(_L("Update Attribute on Attr %d in Service Record %d in SDP Server's Database returned %d.\n"), attrId, recHandle1, err);
	test.Printf(_L("Press any key to continue.\n"));
	test.Getch();

	attrId = 0x3455;
	TRAP(err, db.UpdateAttributeL(recHandle1, attrId, _L("Nicky!")));
	test.Printf(_L("Update Attribute on Attr %d in Service Record %d in SDP Server's Database returned %d.\n"), attrId, recHandle1, err);
	test.Printf(_L("Press any key to continue.\n"));
	test.Getch();

	attrId = 0x01;
	TRAP(err, db.DeleteAttributeL(recHandle1, attrId));
	test.Printf(_L("Delete Attribute on Attr %d in Service Record %d in SDP Server's Database returned %d.\n"), attrId, recHandle1, err);
	test.Printf(_L("Press any key to continue.\n"));
	test.Getch();

	TRAP(err, db.DeleteRecordL(recHandle1));
	test.Printf(_L("Delete Service Record %d in SDP Server's Database returned %d.\n"), attrId, recHandle1, err);
	test.Printf(_L("Press any key to continue.\n"));
	test.Getch();
	db.Close();
	sdp.Close();

  
	test.End();
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
//	TUid uid;
//	uid.iUid = 0x00ce;
//	User::SetIdentity(uid);
// Run through tests
	IntConversionTest();
	FOREVER
		{
		Test1();
//		uid = User::Identity();
		test.Printf(_L("Press 'T' to retry.\r\n"));
		TInt c = test.Getch();
		if(c!='t'&&c!='T')break;
		}


	test.Printf(_L("Finished! Press any key...\r\n"));
	test.Getch();
	test.End();
	}


TInt E32Main()
	{
	//__BREAKPOINT();
	test.SetLogged(ETrue);
	test.Title();
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(error,RunAppL()); // Run the App
	test.Printf(_L("RunAppL left with result: %d"), error);
	delete cleanup;
	__UHEAP_MARKEND;
	test.Close();
	return KErrNone;
	}
