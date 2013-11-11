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
// Bluetooth SDP verification tests
// 
//

#include <e32test.h>
#include <bluetooth/logger.h>
#include "TSDPSAS.h"
#include "MAttributeVisitor.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_TSDPSAS);
#endif

#ifdef _DEBUG
PANICCATEGORY("tsdpsas");
#endif

#undef STOPPING_MODE
#ifndef STOPPING_MODE
	//#define STOPPING_MODE
#endif

#undef SEVEN_LAYERS_BUG
#ifndef SEVEN_LAYERS_BUG
	//#define SEVEN_LAYERS_BUG
#endif

#define ONLY_CONTINUATION_AND_BROWSE

//If you want to see what actual attributes are coming back from an SAS query.
//#define PRINT_BUILDER
// constants
const TUint KUselessRecordHandle=0xffffffff;
const TUint KNonExistantUUID=0xffff;
const TUint KAttributeTestsServiceUUID=0x0011;
//const TInt KNumSdpVerificationTests=55;
const TInt KServiceRequestTest = 0;	// Number of the first test
const TInt KServiceAttributeRequestTest = 6;	// Number of the first test
const TInt KServiceSearchAttributeRequestTest = 29;	// Number of the first test
const TInt KSdpBrowseTest = 53;
const TInt KSdpLastTest = KSdpBrowseTest + 1;
const TInt KServiceRequestBITest = KServiceAttributeRequestTest - 2;
const TInt KServiceAttributeRequestBITest = KServiceSearchAttributeRequestTest - 3;
const TInt KServiceSearchAttributeRequestBITest = KSdpBrowseTest - 2;
const TInt KMinResponseSize = 8; // This is the 5 byte header + 2 byte size (zero) + cont bytes(zero)
const TInt KAttributeParamsSize = 11;
const TInt KServiceParamsSize16 = 7;
const TInt KServiceParamsSize32 = 9;
const TInt KServiceParamsSize128 = 21;
//const TInt KMaxCountSize = 2;
//const TInt KUuidListHeaderSize = 2;
const TInt KUuid16ParamSize = 3;
const TInt KUuid32ParamSize = 5;
const TInt KUuid128ParamSize = 17;
const TInt KServiceAttributeParamsSize16 = 12;
const TInt KServiceAttributeParamsSize32 = 14;
const TInt KServiceAttributeParamsSize128 = 26;
//const TInt KSdpSyncPointBase = 0x1000;

const TInt KSdpAttributeBigRange = 0x00000202;
// UUIDS (add to these as necessary) - see builddb.cpp
// UUIDS (add to these as necessary)
//const TUint32	KSdpContTestUUID = 0x100;


// Offsets and constants
const TInt KRecordHandleOffset = 9;
const TInt KTotalCountOffset = 5;
const TInt KCurrentCountOffset = 7;
const TInt KErrorCodeOffset = 5;
const TInt KAttributeByteCountOffset = 5;
const TInt KAttributeListOffset = 7;


GLREF_D TBTDevAddr devAddr;












//
// CSdpTestBase
//
CSdpTestBase::CSdpTestBase(CConsoleBase& aConsole)
:iConsole(aConsole),iRawTransactionId(0x0000)
	{
	}
	
CSdpTestBase::~CSdpTestBase()
	{
		iRawSock.Close();
	}

void CSdpTestBase::StartSDPServerL()
/**
	Builds up a static DB, and copies it into the RSdpDatabase
**/
	{

	}

void CSdpTestBase::RegisterDatabaseInServerL (CSdpDatabase&)
	{
	}
 

void CSdpTestBase::SendCloseL()
/**
Send info to remote device to indicate the tests have finished
**/
	{
	}

void CSdpTestBase::WaitForCloseL()
/**
Wait for input from the other side to show the tests have finished
(We only need to read one byte).
**/
	{
	}

void CSdpTestBase::SdpConnectL()
/**
Connect to remote SDP server
**/
	{
	TRequestStatus stat;
	TL2CAPSockAddr addr;
	addr.SetBTAddr(devAddr);
	addr.SetPort(1);
	iRawSock.Connect(addr, stat);
	User::WaitForRequest(stat);
	if(stat.Int())
		{
		iConsole.Printf(_L("Connection failed with error %d. Press any key.\r\n"), stat.Int());
		iConsole.Getch();
		}

	User::LeaveIfError(stat.Int());
	}


//
// TSDPHeader class
//
TSDPHeader::TSDPHeader()
:iPduId(0), iTransactionId(0), iParamLen(0)
	{
	}

TSDPHeader::TSDPHeader(TUint8 aPduId, TUint16 aTranId, TUint16 aLen)
:iPduId(aPduId), iTransactionId(aTranId), iParamLen(aLen)
	{
	}

void TSDPHeader::AppendHeader(TDes8& aBuf)
	{
	// Need to make sure endina-ness is OK (is this right)
	//
	TUint16 bigTranId = BigEndian::Get16((TUint8*)&iTransactionId);
	TUint16 bigLen = BigEndian::Get16((TUint8*)&iParamLen);
	aBuf.Append(&iPduId, 1);
	aBuf.Append((TUint8*)&bigTranId, 2);
	aBuf.Append((TUint8*)&bigLen, 2);
	}


//
// TServiceRequest class
//

TServiceRequest::TServiceRequest()
:iUuid(0), iSP(0), iMaxRecordCount(0), iUuidType(CSdpTestBase::EUuid16), iAutoDes(ETrue)
	{
	SetupDES();
	}

TServiceRequest::TServiceRequest(TUint8* aDes, 
								 TUUID aUuid, 
								 CSdpTestBase::TUuidType aUuidType,
								 TUint16 aMaxRecordCount)
:iUuid(0), iSP(0), iMaxRecordCount(0), iUuidType(CSdpTestBase::EUuid16), iAutoDes(EFalse)
	{
	Mem::Copy(iDes, aDes, 3);
	iUuid = aUuid;
	iUuidType = aUuidType;
	iMaxRecordCount = aMaxRecordCount;
	}

TServiceRequest::TServiceRequest(TUint8* aDes, 
								 CSdpSearchPattern& aSP, 
								 CSdpTestBase::TUuidType aUuidType,
								 TUint16 aMaxRecordCount)
:iUuid(0), iSP(0), iMaxRecordCount(0), iUuidType(CSdpTestBase::EUuid16), iAutoDes(EFalse)
	{
	Mem::Copy(iDes, aDes, 3);
	iSP = &aSP;
	iUuidType = aUuidType;
	iMaxRecordCount = aMaxRecordCount;
	}

TServiceRequest::TServiceRequest(TUUID aUuid, 
								 CSdpTestBase::TUuidType aUuidType,
								 TUint16 aMaxRecordCount)
:iUuid(aUuid), iSP(0), iMaxRecordCount(aMaxRecordCount), iUuidType(aUuidType), iAutoDes(ETrue)  
	{
	SetupDES();
	}

TServiceRequest::TServiceRequest(CSdpSearchPattern& aSP, 
								 CSdpTestBase::TUuidType aUuidType,
								 TUint16 aMaxRecordCount)
:iUuid(0), iSP(0), iMaxRecordCount(aMaxRecordCount), iUuidType(aUuidType), iAutoDes(ETrue)  
	{
	iSP = &aSP;
	SetupDES();
	}

void TServiceRequest::SetupDES()
	{
	if(iSP)
		//just set up first two bytes
		{
		iDes[0] = 0x35;
		switch(iUuidType)
			{
			case CSdpTestBase::EUuid128:
				iDes[1] = (TUint8)(KUuid128ParamSize*iSP->Count()); //take the length
				break;
			case CSdpTestBase::EUuid32:
				iDes[1] = (TUint8)(KUuid32ParamSize*iSP->Count()); //take the length
				break;
			default:
				iDes[1] = (TUint8)(KUuid16ParamSize*iSP->Count()); //take the length
			}
		}
	else
		{
		TUint8 validDes16[] = {0x35, 0x03, 0x19};
		TUint8 validDes32[] = {0x35, 0x05, 0x1a};
		TUint8 validDes128[] = {0x35, 0x11, 0x1c};
		switch(iUuidType)
			{
		case CSdpTestBase::EUuid16:
			Mem::Copy(iDes, validDes16, 3);
			break;
		case CSdpTestBase::EUuid32:
			Mem::Copy(iDes, validDes32, 3);
			break;
		default:
			Mem::Copy(iDes, validDes128, 3);
			}
		}

	}

void TServiceRequest::SetUuidType(CSdpTestBase::TUuidType aUuidType)
	{
	iUuidType = aUuidType;
	if(iAutoDes)
		{
		SetupDES();
		}
	}

void TServiceRequest::AppendRequest(TDes8& aBuf)
	{
	if(iSP)
		{
		aBuf.Append(iDes, 2); //just take the list header bytes ..then...
		for(TInt i = 0; i<iSP->Count(); i++)
			{
			TUUID uuid = iSP->At(i);
			TPtrC8 ptr16 = uuid.ShortestForm();
			TPtrC8 ptr32 = uuid.LongForm().Mid(0,4);
			TPtrC8 ptr128 = uuid.LongForm();
			switch(iUuidType)
				{
				case CSdpTestBase::EUuid128:
					aBuf.Append(0x1c);
					aBuf.Append(ptr128);
					break;
				case CSdpTestBase::EUuid32:
					aBuf.Append(0x1a);
					aBuf.Append(ptr32);
					break;
				default:
					aBuf.Append(0x19);
					aBuf.Append(ptr16);
				}
			}
		TUint16 bigCount = BigEndian::Get16((TUint8*)&iMaxRecordCount);
		aBuf.Append((TUint8*)&bigCount, 2);
		}
	else
		{
		TPtrC8 ptr16 = iUuid.ShortestForm();
		TPtrC8 ptr32 = iUuid.LongForm().Mid(0,4);
		TPtrC8 ptr128 = iUuid.LongForm();
		TUint16 bigCount = BigEndian::Get16((TUint8*)&iMaxRecordCount);
		aBuf.Append(iDes, 3); //....CHANGED
		switch(iUuidType)
			{
			case CSdpTestBase::EUuid128:
				aBuf.Append(ptr128);
				break;
			case CSdpTestBase::EUuid32:
				aBuf.Append(ptr32);
				break;
			default:
				aBuf.Append(ptr16);
			}

		aBuf.Append((TUint8*)&bigCount, 2);
		}
	}

//
// TAttributeRequest class
//
TAttributeRequest::TAttributeRequest()
:iRecordHandle(0), iMaxByteCount(0), iAttributeId(0), iAttributeIdRange(0), iAutoDes(ETrue)
	{
	Mem::FillZ(iDes, 0); 
	}
TAttributeRequest::TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint8* aDes, TUint16 aAttributeId)
:iRecordHandle(aRecordHandle), iMaxByteCount(aMaxByteCount), iAttributeId(aAttributeId), iAttributeIdRange(0), iAutoDes(EFalse)
	{
	Mem::Copy(iDes, aDes, 3);
	}

TAttributeRequest::TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint8* aDes, TUint32 aAttributeIdRange)
:iRecordHandle(aRecordHandle), iMaxByteCount(aMaxByteCount), iAttributeId(0), iAttributeIdRange(aAttributeIdRange), iAutoDes(EFalse)
	{
	Mem::Copy(iDes, aDes, 3);
	}

TAttributeRequest::TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint16 aAttributeId)
:iRecordHandle(aRecordHandle), iMaxByteCount(aMaxByteCount), iAttributeId(aAttributeId), iAttributeIdRange(0), iAutoDes(ETrue)
	{
	SetupDES();
	}

TAttributeRequest::TAttributeRequest(TUint32 aRecordHandle, TUint16 aMaxByteCount, TUint32 aAttributeIdRange)
:iRecordHandle(aRecordHandle), iMaxByteCount(aMaxByteCount), iAttributeId(0), iAttributeIdRange(aAttributeIdRange), iAutoDes(ETrue)
	{
	SetupDES();
	}

void TAttributeRequest::SetupDES()
	{
	TUint8 validDesId[] = {0x35, 0x03, 0x09};
	TUint8 validDesIdRange[] = {0x35, 0x05, 0x0a};
	if(iAttributeIdRange)
		{
		Mem::Copy(iDes, validDesIdRange, 3);
		}
	else
		{
		Mem::Copy(iDes, validDesId, 3);
		}
	}

void TAttributeRequest ::AppendRequest(TDes8& aBuf)
	{
	TUint32 bigHandle = BigEndian::Get32((TUint8*)&iRecordHandle);
	TUint16 bigCount = BigEndian::Get16((TUint8*)&iMaxByteCount);
	TUint16 bigId = BigEndian::Get16((TUint8*)&iAttributeId);;
	TUint32 bigId32 = BigEndian::Get32((TUint8*)&iAttributeIdRange);;
		
	
	aBuf.Append((TUint8*)&bigHandle, 4);
	aBuf.Append((TUint8*)&bigCount, 2);
	aBuf.Append(iDes, 3);
	if(iAttributeIdRange)
		aBuf.Append((TUint8*)&bigId32, 4);
	else
		aBuf.Append((TUint8*)&bigId, 2);
	}

//
// TServiceAttributeRequest class
//
TServiceAttributeRequest::TServiceAttributeRequest()
	{
	}


TServiceAttributeRequest::TServiceAttributeRequest(TUint8* aServiceDes, TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint16 aAttributeId)
	{
	iServiceRequest=TServiceRequest(aServiceDes, aServiceUuid, aUuidType, 0);
	iAttributeRequest=TAttributeRequest(0, aMaxByteCount, aAttribDes, aAttributeId);
	}

TServiceAttributeRequest::TServiceAttributeRequest(TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint16 aAttributeId)
	{
	iServiceRequest=TServiceRequest(aServiceUuid, aUuidType, 0);
	iAttributeRequest=TAttributeRequest(0, aMaxByteCount, aAttributeId);
	}

TServiceAttributeRequest::TServiceAttributeRequest(TUint8* aServiceDes, TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint8* aAttribDes, TUint32 aAttributeIdRange)
	{
	iServiceRequest=TServiceRequest(aServiceDes, aServiceUuid, aUuidType, 0);
	iAttributeRequest=TAttributeRequest(0, aMaxByteCount, aAttribDes, aAttributeIdRange);
	}

TServiceAttributeRequest::TServiceAttributeRequest(TUUID aServiceUuid, CSdpTestBase::TUuidType aUuidType, TUint16 aMaxByteCount, TUint32 aAttributeIdRange)
	{
	iServiceRequest=TServiceRequest(aServiceUuid, aUuidType, 0);
	iAttributeRequest=TAttributeRequest(0, aMaxByteCount, aAttributeIdRange);
	}




void TServiceAttributeRequest::SetUuidType(CSdpTestBase::TUuidType aUuidType)
	{
	iServiceRequest.SetUuidType(aUuidType);
	}

void TServiceAttributeRequest::AppendRequest(TDes8& aBuf)
	{
	TUint16 byteCount = iAttributeRequest.MaxByteCount();
	TUint16 bigCount = BigEndian::Get16((TUint8*)&byteCount);
	TUint attribId = 0;
	TUint16 bigAttributeId = 0;
	TUint32 bigAttributeIdRange = 0;

	if((attribId = iAttributeRequest.AttributeIdRange())!=0)
		{
		bigAttributeIdRange = BigEndian::Get32((TUint8*)&attribId);
		}
	else
		{
		attribId = iAttributeRequest.AttributeId();
		bigAttributeId = BigEndian::Get16((TUint8*)&attribId);
		}

	if(iServiceRequest.SP())
		//WARNING-> THIS 'IF' HAS NOT BEEN TESTED!
		{
		aBuf.Append(iServiceRequest.Des(), 2); //just take the list header bytes..then...
		for(TInt i = 0; i<iServiceRequest.SP()->Count(); i++)
			{
			TUUID uuid = iServiceRequest.SP()->At(i);
			TPtrC8 ptr16 = uuid.ShortestForm();
			TPtrC8 ptr32 = uuid.LongForm().Mid(0,4);
			TPtrC8 ptr128 = uuid.LongForm();
			switch(iServiceRequest.UuidType())
				{
				case CSdpTestBase::EUuid128:
					aBuf.Append(0x1c);
					aBuf.Append(ptr128);
					break;
				case CSdpTestBase::EUuid32:
					aBuf.Append(0x1a);
					aBuf.Append(ptr32);
					break;
				default:
					aBuf.Append(0x19);
					aBuf.Append(ptr16);
				}
			}
		}
	else
		{
		TUUID uuid = iServiceRequest.Uuid();
		TPtrC8 ptr16 = uuid.ShortestForm();
		TPtrC8 ptr32 = uuid.LongForm().Mid(0,4);
		TPtrC8 ptr128 = uuid.LongForm();
		aBuf.Append(iServiceRequest.Des(), 3);
		switch(iServiceRequest.UuidType())
			{
			case CSdpTestBase::EUuid128:
				aBuf.Append(ptr128);
				break;
			case CSdpTestBase::EUuid32:
				aBuf.Append(ptr32);
				break;
			default:
				aBuf.Append(ptr16);
			}
		}
	
	aBuf.Append((TUint8*)&bigCount, 2);
	aBuf.Append(iAttributeRequest.Des(), 3);

	if(bigAttributeIdRange)
		{
		aBuf.Append((TUint8*)&bigAttributeIdRange, 4);
		}
	else
		{
		aBuf.Append((TUint8*)&bigAttributeId, 2);
		}
	
	}




class CAttrPrintVisitor : public CBase, public MAttributeVisitor
	{
public:
	CAttrPrintVisitor(CConsoleBase& aConsole) : iConsole(aConsole) {iIndent=0;}
	~CAttrPrintVisitor() {/*iConsole.Getch();*/}
    void VisitAttributeL(CSdpAttr &aAttribute)
		{
		Indent();
		iConsole.Printf(_L("\nID:0x%x:"), aAttribute.AttributeID());
		}
	void HexDes(const TDesC8& aDes)
	{
	for (TInt i = 0; i < aDes.Length(); ++i)
		iConsole.Printf(_L("%02x"), aDes[i]);
		//iConsole.Printf(_L("\n"));
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

		TBuf16<0x64> iString;
//		TInt iFound;
		switch (aType)
			{
			case ETypeString:
				{
				TInt len = aValue.Des().Length();
				if(len<0x60)
					//cannot cope with strings longer than 100 bytes!
					{
					iString.Copy(aValue.Des());
					iConsole.Printf(_L("\"%S\""),&iString);
					}
				else
					{
					iConsole.Printf(_L("String attribute too long for CONSOLE: Length %d\n"),len);
					}
				break;
				}
			case ETypeDES:
				iConsole.Printf(_L(" DES"));
				break;
			case ETypeUint:
				iConsole.Printf(_L(" UInt:0x%x"), aValue.Uint());
				break;
			case ETypeUUID:
				iConsole.Printf(_L(" UUID:0x"));
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
				iConsole.Printf(_L("type %d"), aType);
			}
		}
	void StartListL(CSdpAttrValueList &/*aList*/)
		{
		++iIndent;
		iConsole.Printf(_L("{"));
		}
    void EndListL()
		{
		if(iIndent<=0)
			{
			iConsole.Printf(_L("\nERROR! Unmatched EndList!\n"));
			__DEBUGGER();
			}
		iConsole.Printf(_L("}"));
		--iIndent;
		}
private:
	void Indent() {/*test.SetPos(iIndent)*/;}
	CConsoleBase &iConsole;
	TInt iIndent;
	};

//
// CSdpVerificationTests
//

void CSdpVerificationTests::RunTestL(CConsoleBase& aConsole)
	{
	CSdpVerificationTests* test=new(ELeave) CSdpVerificationTests(aConsole);
	TRAPD(err,test->DoTestsL());
	aConsole.Printf(_L("SAS tests completed with error code: %d\r\nPRESS ANY KEY.\r\n"), err);
	aConsole.Getch();
	delete test;
	}

CSdpVerificationTests::CSdpVerificationTests(CConsoleBase& aConsole)
:CSdpTestBase(aConsole), iIndent(0), iUuidType(CSdpTestBase::EUuid16)
	{
	iSP = CSdpSearchPattern::NewL(); //naughty but time is short!

	
	SetUpServiceTestParams();
	SetUpAttributeTestParams();
	SetUpServiceAttributeTestParams();

	Mem::FillZ( iContinuationBuffer,KSdpContinuationStateMaxLength);
	
	iType = EServiceRequest;
	iQueryIndex = 0;
	iQueryState = EServiceRequest;
	iTestNum = 0;
	iContinuationStore.SetLength(0);
	iNeedAttributeId = ETrue;
	iBuildingList = 0;
	iBrowseIter = 0;
	iRequestMaxCount = 0;
	iRequestMaxCount1 = 0;
	iRequestMaxCount2 = 0;
	iRequestTransId = 0;
	iRequestTransId1 = 0;
	iRequestTransId2 = 0;
	iNeedMoreRecordHandles = EFalse;
	iBrowseTestNumRecordHandlesStored = 0;
	for (TUint i = 0; i < KMaxNumRecordHandles; i ++)
		{
		iBrowseTestServiceRecordHandle[i] = KUselessRecordHandle; //we want test database NOT to need this handle!
		}
	}

CSdpVerificationTests::~CSdpVerificationTests()
	{
	delete iSP;
	}


void CSdpVerificationTests::RunMasterTestL()
	{
	// Set the type
	//
	if(iTestNum<KServiceAttributeRequestTest)
	{
		iType=EServiceRequest;
	}
	if(iTestNum>=KServiceAttributeRequestTest)
	{
		iType=EServiceAttributeRequest;
	}
	if(iTestNum>=KServiceSearchAttributeRequestTest)
	{
		iType=EServiceSearchAttributeRequest;
		iQueryState=EServiceSearchAttributeRequest;
	}
	SDPQueryL();
	}

void CSdpVerificationTests::DoTestsL()
	{

	// Initialisation stuff
	//
    RSocketServ ss;
	TInt ret=ss.Connect();
    if(ret!=KErrNone)
		{
		iConsole.Printf(_L("l2cap: no connection"));
		return;
		}

	TUint numProtocols;
	if(ss.NumProtocols(numProtocols)!=KErrNone)
		{
		iConsole.Printf(_L("l2cap: no protocols"));
		return;
		}
 	TProtocolDesc pInfo;

	ret=ss.FindProtocol(_L("L2CAP"),pInfo);
	if (ret!=KErrNone)										    
		{
		iConsole.Printf(_L("l2cap datagram protocol not loaded"));
		return;
		}


	User::LeaveIfError(iRawSock.Open(ss,pInfo.iAddrFamily,pInfo.iSockType,pInfo.iProtocol));

	WaitForStartSignalL();
	SdpConnectL();

	for(iTestNum=KServiceRequestTest;iTestNum<=KSdpLastTest;iTestNum++)
		{
		// Do any synching required here
		//
		// Reset the continuation buffer
		iContinuationStore.SetLength(0);
		if(AnotherQuery())
		//These two query tests not reomoved from current spec
			{
			continue;
			}
		/*
		//MASK UNWANTED TEST or SPECIFY WANTED TESTS HERE!
		if(iTestNum!=1&&
		   iTestNum!=8&&
		   iTestNum!=34&&
		   iTestNum!=53&&
		   iTestNum!=54)
			{
			continue;
			}
		*/
		if(iTestNum!=34) continue;

		iConsole.Printf(_L("\r\n\r\n"));
		FLOG(_L(""));
		FLOG(_L(""));
		if(/*iTestNum<KServiceRequestTest*/EFalse)
			; //do nothing
		else if(/*iTestNum>=KServiceRequestTest&&*/iTestNum<KServiceAttributeRequestTest)
			{
			iType = EServiceRequest; //allows skip of first SS test
			iQueryState=EServiceRequest;
			for(TUint j = 0;j < 3;j++)
				{
				switch(j)
					{
					case 0:
					iUuidType = CSdpTestBase::EUuid16;
					break;
					case 1:
					iUuidType = CSdpTestBase::EUuid32;
					break;
					default:
					iUuidType = CSdpTestBase::EUuid128;
					break;
					}
				UpdateTestString();
				FTRACE(FPrint(_L("Starting %S..."), &iTestName));
				FLOG(_L(""));
				iConsole.Printf(_L("Starting %S...\r\n"), &iTestName);

				TRAPD(err, RunMasterTestL());
				FTRACE(FPrint(_L("%S returned with error %d. (0 => success!)"), &iTestName, err));
				FTRACE(FPrint(_L("")));
				FTRACE(FPrint(_L("")));
				FTRACE(FPrint(_L("")));
				iConsole.Printf(_L("%S returned with error %d. (0 => success!)\r\n"), &iTestName, err);
				PromptedGetch();
				}
			}
		else if(iTestNum>=KServiceAttributeRequestTest&&iTestNum<KServiceSearchAttributeRequestTest)
			{
			iType = EServiceAttributeRequest; //allows skip of first SA test
			iQueryState=EServiceAttributeRequest;
			iUuidType = CSdpTestBase::EUuid16;
			UpdateTestString();
			FTRACE(FPrint(_L("Starting %S..."), &iTestName));
			FLOG(_L(""));
			iConsole.Printf(_L("Starting %S...\r\n"), &iTestName);

			TRAPD(err, RunMasterTestL());
			FTRACE(FPrint(_L("%S returned with error %d. (0 => success!)"), &iTestName, err));
			FTRACE(FPrint(_L("")));
			FTRACE(FPrint(_L("")));
			FTRACE(FPrint(_L("")));
			iConsole.Printf(_L("%S returned with error %d. (0 => success!)\r\n"), &iTestName, err);
			PromptedGetch();
			}
		else if(iTestNum>=KServiceSearchAttributeRequestTest && iTestNum<KSdpBrowseTest)
			{
			iType = EServiceSearchAttributeRequest; //allows skip of first SSA test
			iQueryState=EServiceSearchAttributeRequest;
#ifdef SEVEN_LAYERS_BUG
			for(TUint j = 0;j < 1;j++)
#else
			for(TUint j = 0;j < 3;j++)
#endif
				{
				switch(j)
					{
					case 0:
						iUuidType = CSdpTestBase::EUuid16;
						break;
					case 1:
						iUuidType = CSdpTestBase::EUuid32;
						break;
					default:
						iUuidType = CSdpTestBase::EUuid128;
						break;
					}
				UpdateTestString();
				FTRACE(FPrint(_L("Starting %S..."), &iTestName));
				FLOG(_L(""));
				iConsole.Printf(_L("Starting %S...\r\n"), &iTestName);

				TRAPD(err, RunMasterTestL());
				FTRACE(FPrint(_L("%S returned with error %d. (0 => success!)"), &iTestName, err));
				FTRACE(FPrint(_L("")));
				FTRACE(FPrint(_L("")));
				FTRACE(FPrint(_L("")));
				iConsole.Printf(_L("%S returned with error %d. (0 => success!)\r\n"), &iTestName, err);
				PromptedGetch();
				}
			}
		else
			{
			TInt browseRet = KErrNone;
			for(TUint j = 0;j < 3;j++)
				{
				switch(j)
					{
					case 0:
						iUuidType = CSdpTestBase::EUuid16;
						break;
					case 1:
						iUuidType = CSdpTestBase::EUuid32;
						break;
					default:
						iUuidType = CSdpTestBase::EUuid128;
						break;
					}

				UpdateTestString();
				FTRACE(FPrint(_L("Starting %S..."), &iTestName));
				FLOG(_L(""));
				iConsole.Printf(_L("Starting %S...\r\n"), &iTestName);
				while(iBrowseIter<3)
					{
					TRAPD(err, SDPBrowseTestL(iUuidType));
					if(err)
						{
						iBrowseIter++; //for messages/logging - this would have been jumped!
						}

					FTRACE(FPrint(_L("%S: Stage %d - returned with error %d (0 => success!)"), &iTestName, iBrowseIter, err));
					FTRACE(FPrint(_L("")));
					FTRACE(FPrint(_L("")));
					FTRACE(FPrint(_L("")));
					iConsole.Printf(_L("%S: Stage %d: returned with error %d (0 => success!)\r\n"), &iTestName, iBrowseIter, err);
					browseRet = err;
					if(browseRet!=KErrNone)
						{
						break;
						}
					}
				iBrowseIter = 0;
				PromptedGetch();
				}
			}
		
		}//end of for loop
	SendCloseL();
	}

void CSdpVerificationTests::SDPQueryL()
	{
	TRequestStatus stat1;
	TRequestStatus stat2;
	
	// Buffers for sending/receiving requests
	// (use 700 so it 3ill definitely be big enough).
	TBuf8<700> queryResult1;
	TBuf8<700> queryResult2;
	
	iQueryIndex = 0;
	
	// Continue round this loop until finished
	// 
	TBool bContinue = ETrue;
	while(bContinue)
	{
		// Run the first query 
		//
		RunSDPSearchL();
		// Do we have to do another query as well??
		//
		TBool anotherQuery = AnotherQuery();
		if(anotherQuery)
		{
			iQueryIndex++;
			RunSDPSearchL();
		}
		// Wait for reply(s)
		//
		iRawSock.Read(queryResult1, stat1);
		User::WaitForRequest(stat1);
		if (stat1!=KErrNone)
		{
			iConsole.Printf(_L("First query failed\n"));
			User::Leave(stat1.Int());
		}
		FlogReadString(queryResult1);

		if(anotherQuery)
		{
			iRawSock.Read(queryResult2, stat2);
			User::WaitForRequest(stat2);
			if (stat2!=KErrNone)
			{
				iConsole.Printf(_L("Second query failed\n"));
				User::Leave(stat1.Int());
			}
			FlogReadString(queryResult2);
		}
		
		// Now process the results
		//
		User::LeaveIfError(ProcessSearchResults(queryResult1, queryResult2));
		// Reset stuff
		iQueryIndex = 0;

		bContinue=Continue();
	}
}


void CSdpVerificationTests::RunSDPSearchL()
	{
	// Run the appropiate query
	//
	if(iQueryState==EServiceRequest)
	{
		SDPServiceSearchL();
	}
	else if(iQueryState==EServiceAttributeRequest)
	{
		SDPAttributeSearchL();
	}
	else 
	{
		SDPServiceAttributeSearch();
	}

	}


 void CSdpVerificationTests::SDPServiceSearchL()
	{
	
	// If aLoop == 1, the first and second tests need to be run concurrently
	//
	TInt paramsIndex = iTestNum;
	if((iTestNum==1)&&(iQueryIndex==0))
	{
		paramsIndex = 0;
	}	
	// Set up the raw buffer.
	// First create the SDP header
	//
	TUint16 paramsSize = TUint16(KServiceParamsSize16);
	switch(iUuidType)
		{
	case CSdpTestBase::EUuid16:
		paramsSize = KServiceParamsSize16;
		break;
	case CSdpTestBase::EUuid32:
		paramsSize = KServiceParamsSize32;
		break;
	case CSdpTestBase::EUuid128:
		paramsSize = KServiceParamsSize128;
		break;
	default:
		break;
		}
	if(iTestNum==4)
	{
		// This is the invalid PDU length test
		//
		paramsSize+=1;
	}
	TSDPHeader header(0x02, iRawTransactionId++, TUint16(paramsSize + iContLength + 1));
	 
	//Now add these to the query
	//
	TRequestStatus writeStat;
	TBuf8<100> request;
	request.Zero();
	header.AppendHeader(request);
	iServiceTests[paramsIndex].SetUuidType(iUuidType); //update with current uuid type
	iServiceTests[paramsIndex].AppendRequest(request);
	request.Append(&iContLength, 1);
	request.Append(iContinuationBuffer, iContLength);
	FlogWriteString(request);
	iRawSock.Write(request, writeStat);
	User::WaitForRequest(writeStat);
	}

void CSdpVerificationTests::SDPAttributeSearchL()
	{
	TRequestStatus writeStat;
	TBuf8<100> request;
	TRequestStatus readStat;
	TBuf8<700> queryResult1;
	TBuf8<700> queryResult2;
	request.Zero();
	TUint8 loop = 1;
	iNeedMoreRecordHandles = EFalse;
	iAttributeTestServiceRecordHandle = KUselessRecordHandle;
	TBool doSS = ETrue;
	if(iTestNum==26||(iTestNum==7&&iQueryIndex!=0))
		{
		doSS = EFalse;
		}
	if(doSS)
		{
		while(loop)
			{
			TServiceRequest serviceRequest;
			TSDPHeader header = TSDPHeader(0x02, iRawTransactionId++, TUint16(KServiceParamsSize16 + iContLength + 1));//cont length byte);
			serviceRequest = TServiceRequest(KAttributeTestsServiceUUID, EUuid16, 1);
			request.Zero();
			header.AppendHeader(request);
			serviceRequest.AppendRequest(request);
			request.Append(&iContLength, 1);
			request.Append(iContinuationBuffer, iContLength);
			FlogWriteString(request);
			iRawSock.Write(request, writeStat);
			User::WaitForRequest(writeStat);
			// Wait for reply
			//
			iRawSock.Read(queryResult1, readStat);
			User::WaitForRequest(readStat);
			User::LeaveIfError(readStat.Int());
			// Process results (fills up browse record handle array)
			//
			FlogReadString(queryResult1);
			User::LeaveIfError(ProcessServiceSearchResults(queryResult1, queryResult2));
			loop = iContLength;
			}
		}
	
	
	// If aLoop ==7 the first and second tests need to be run concurrently
	//
	iNeedAttributeId = ETrue;
	TInt paramsIndex = iTestNum;
	if((iTestNum==7)&&(iQueryIndex==0))
	{
		paramsIndex = 6;
	}	
	// Now take off the index of the service tests
	//
	paramsIndex -= KNumSDPServiceTests;

	// Set up the raw buffer.
	// First create the SDP header
	//
	TUint16 paramLen = TUint16(KAttributeParamsSize + iContLength + 1);//cont length byte
	if(iTestNum==28)
	{
		// This is the invalid PDU length test
		//
		paramLen+=1;
	}
	TSDPHeader header(0x04, iRawTransactionId++, paramLen);
	 
	//Now add these to the query
	//
	request.Zero();
	header.AppendHeader(request);
	if(doSS)
		{
		if(iAttributeTestServiceRecordHandle==KUselessRecordHandle)
			User::Leave(KErrGeneral);
		//***********//
		//overwrite requested record handle with result of 
		//initial service search (required by spec)
		iAttributeTests[paramsIndex].SetRecordHandle((TUint16)iAttributeTestServiceRecordHandle); 
		}
	//***********//
	iAttributeTests[paramsIndex].AppendRequest(request);
	request.Append(&iContLength, 1);
	request.Append(iContinuationBuffer, iContLength);
	FlogWriteString(request);
	iRawSock.Write(request, writeStat);
	User::WaitForRequest(writeStat);
}

void CSdpVerificationTests::SDPServiceAttributeSearch()
	{
	iNeedAttributeId = ETrue;
	// If aLoop == 33 the first and second tests need to be run concurrently
	//
	TInt paramsIndex = iTestNum;
	if((iTestNum==33)&&(iQueryIndex==0))
	{
		paramsIndex = 32;
	}	
	// Now take off the index of the Service and Attribute tests
	//
	paramsIndex -= (KNumSDPServiceTests + KNumSDPAttributeTests);

	// Set up the raw buffer.
	// First create the SDP header
	//
	TUint16 paramsSize = TUint16(KServiceAttributeParamsSize16);

	switch(iUuidType)
		{
	case CSdpTestBase::EUuid16:
		paramsSize = KServiceAttributeParamsSize16;
		break;
	case CSdpTestBase::EUuid32:
		paramsSize = KServiceAttributeParamsSize32;
		break;
	case CSdpTestBase::EUuid128:
		paramsSize = KServiceAttributeParamsSize128;
		break;
	default:
		break;
		}
#ifdef SEVEN_LAYERS_BUG
#else
	if(iTestNum==34)
		{
		paramsSize += 2;	// This is the one test that uses an attr range parameter
		}
#endif

	if(iTestNum==52)
		{
		paramsSize += 1;	// This is the invalid PDU length test
		}

	TSDPHeader header(0x06, iRawTransactionId++, TUint16(paramsSize + iContLength + 1));
	 
	//Now add these to the query
	//
	TRequestStatus writeStat;
	TBuf8<100> request;
	request.Zero();
	header.AppendHeader(request);
	iServiceAttributeTests[paramsIndex].SetUuidType(iUuidType); //update with current uuid type
	iServiceAttributeTests[paramsIndex].AppendRequest(request);
	request.Append(&iContLength, 1);
	request.Append(iContinuationBuffer, iContLength);
	FlogWriteString(request);
	iRawSock.Write(request, writeStat);
	User::WaitForRequest(writeStat);
	}
/*
header = TSDPHeader(0x06, iRawTransactionId++, TUint16(ssaParamsSize + iContLength + 1));//cont length byte);
servAttrRequest = TServiceAttributeRequest(iSP->At(j), aUuidType, 200, attrUUID);
request.Zero();
header.AppendHeader(request);
servAttrRequest.AppendRequest(request);
request.Append(&iContLength, 1);
request.Append(iContinuationBuffer, iContLength);
*/

void CSdpVerificationTests::SDPBrowseTestL(CSdpTestBase::TUuidType aUuidType)
	{
	TSDPHeader header;
	TUint16 serviceUUID = 0;
	TUint16 attrUUID = 0;
	TInt ssParamsSize = KServiceParamsSize16;
	TInt ssaParamsSize = KServiceAttributeParamsSize16;

	switch(aUuidType)
		{
	case CSdpTestBase::EUuid16:
		ssParamsSize = KServiceParamsSize16;
		ssaParamsSize = KServiceAttributeParamsSize16;
		break;
	case CSdpTestBase::EUuid32:
		ssParamsSize = KServiceParamsSize32;
		ssaParamsSize = KServiceAttributeParamsSize32;
		break;
	case CSdpTestBase::EUuid128:
		ssParamsSize = KServiceParamsSize128;
		ssaParamsSize = KServiceAttributeParamsSize128;
		break;
	default:
		break;
		}

	
	//Reset for next level browse query
	iContLength = 0;
	iContinuationStore.SetLength(0);
	iBrowseTestNumRecordHandlesStored = 0;
	for (TUint i = 0; i < KMaxNumRecordHandles; i ++)
		{
		iBrowseTestServiceRecordHandle[i] = KUselessRecordHandle; //we want test database NOT to need this handle!
		}


	TRequestStatus writeStat;
	TBuf8<100> request;
	TRequestStatus readStat;
	TBuf8<700> queryResult1;
	TBuf8<700> queryResult2;
	if(iTestNum == KSdpBrowseTest)
		{
		// This pattern repeats the search/atrribute request pattern
		//
		switch(iBrowseIter)
			{
		case 0:
			serviceUUID = 0x1002;	// Public browse group
			FLOG(_L("***********FIRST PHASE SERVICE SEARCH TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single UUID 0x%04x***********"), serviceUUID));
			break;
		case 1:	
			iSP->Reset();
			serviceUUID = 0x1001;	// Browse group descriptor class ID
			FLOG(_L("***********SECOND PHASE SERVICE SEARCH TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single UUID 0x%04x***********"), serviceUUID));
			break;
		case 2:
			{
			if(iSP->Count()<=0)
				{
				User::Leave(KErrGeneral);
				}
			TPtrC8 uuid = iBrowseUUID[0].ShortestForm();
			serviceUUID = BigEndian::Get16(uuid.Ptr());
			FLOG(_L("***********THIRD PHASE SERVICE SEARCH TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single UUID 0x%04x***********"), serviceUUID));
			}
			break;
		default:
			User::Leave(KErrGeneral);
			break;
			}
		TUint8 loop = 1;
		iNeedMoreRecordHandles = EFalse;
		while(loop)
			{
			TServiceRequest serviceRequest;
			if(iBrowseIter==2)
				{
				iNeedMoreRecordHandles = ETrue;
				TInt j = 0;
				while(j<iSP->Count())
					{
					header = TSDPHeader(0x02, iRawTransactionId++, TUint16(ssParamsSize + iContLength + 1));//cont length byte);
					serviceRequest = TServiceRequest(iSP->At(j), aUuidType, KMaxNumRecordHandles);
					request.Zero();
					header.AppendHeader(request);
					serviceRequest.AppendRequest(request);
					request.Append(&iContLength, 1);
					request.Append(iContinuationBuffer, iContLength);
					FlogWriteString(request);
					iRawSock.Write(request, writeStat);
					User::WaitForRequest(writeStat);
					// Wait for reply
					//
					iRawSock.Read(queryResult1, readStat);
					User::WaitForRequest(readStat);
					User::LeaveIfError(readStat.Int());
					// Process results (fills up browse record handle array)
					//
					FlogReadString(queryResult1);
					User::LeaveIfError(ProcessServiceSearchResults(queryResult1, queryResult2));
					if(!iContLength)
						j++;
					}
				iNeedMoreRecordHandles = EFalse;
				}
			else
				{
				header = TSDPHeader(0x02, iRawTransactionId++, TUint16(ssParamsSize + iContLength + 1));//cont length byte);
				serviceRequest = TServiceRequest(/*UuidDes, */serviceUUID, aUuidType, KMaxNumRecordHandles);
				request.Zero();
				header.AppendHeader(request);
				serviceRequest.AppendRequest(request);
				request.Append(&iContLength, 1);
				request.Append(iContinuationBuffer, iContLength);
				FlogWriteString(request);
				iRawSock.Write(request, writeStat);
				User::WaitForRequest(writeStat);
				// Wait for reply
				//
				iRawSock.Read(queryResult1, readStat);
				User::WaitForRequest(readStat);
				User::LeaveIfError(readStat.Int());
				// Process results (fills up browse record handle array)
				//
				FlogReadString(queryResult1);
				User::LeaveIfError(ProcessServiceSearchResults(queryResult1, queryResult2));
				}
			loop = iContLength;
			}
		// the and attribute query on the first record handle for service class id list...
		//
		switch(iBrowseIter)
			{
		case 0:
			attrUUID = KSdpAttrIdServiceClassIDList;
			FLOG(_L("***********FIRST PHASE ATTRIBUTE SEARCH TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single attr id 0x%04x***********"), attrUUID));
			break;
		case 1:
			attrUUID = KSdpAttrIdSdpServerGroupID;
			FLOG(_L("***********SECOND PHASE ATTRIBUTE SEARCH TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single attr id 0x%04x***********"), attrUUID));
			break;
		case 2:
			attrUUID = KSdpAttrIdServiceClassIDList;
			FLOG(_L("***********THIRD PHASE ATTRIBUTE SEARCH TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single attr id 0x%04x***********"), attrUUID));
			break;
		default:
			User::Leave(KErrGeneral);
			break;
			}
		TUint i = 0;
		iContLength = 0; //make sure: new attribute request
		iContinuationStore.SetLength(0);
		for (TUint j = 0; j < KMaxNumBrowseUUIDs; j ++)
			{
			iBrowseUUID[i] = KUselessRecordHandle; //we want test database NOT to need this UUID!
			}
		iNumBrowseUUIDs = 0;
		while(i < iBrowseTestNumRecordHandlesStored)
			{
			TAttributeRequest attrRequest;
			if(iBrowseTestServiceRecordHandle[i]==KUselessRecordHandle)
				{
				i++;
				continue;
				}
			header = TSDPHeader(0x04, iRawTransactionId++, TUint16(KAttributeParamsSize + iContLength + 1));//cont length byte);
			attrRequest = TAttributeRequest(iBrowseTestServiceRecordHandle[i], 200, attrUUID);
			writeStat = KRequestPending;
			request.Zero();
			header.AppendHeader(request);
			attrRequest.AppendRequest(request);
			request.Append(&iContLength, 1);
			request.Append(iContinuationBuffer, iContLength);
			FlogWriteString(request);
			iRawSock.Write(request, writeStat);
			User::WaitForRequest(writeStat);
					
			// Wait for reply
			//
			readStat = KRequestPending;
			iRawSock.Read(queryResult1, readStat);
			User::WaitForRequest(readStat);
			User::LeaveIfError(readStat.Int());
			// Process results
			//
			FlogReadString(queryResult1);
			User::LeaveIfError(ProcessAttributeSearchResults(queryResult1, queryResult2));
			if(!iContLength)
				i++; //augment ready for next record handle only if no continuation.
			}
		// Increment tree level counter
		//
		iBrowseIter++;
		
		}
	else
		{
		header = TSDPHeader(0x06, iRawTransactionId++, TUint16(ssaParamsSize + iContLength + 1));
		switch(iBrowseIter)
			{
		case 0:
			serviceUUID = 0x1002;	// Public browse group
			attrUUID = KSdpAttrIdServiceClassIDList;
			FLOG(_L("***********FIRST PHASE SERVICE SEARCH ATTRIBUTE TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single UUID 0x%04x, and single attr id 0x%04x***********"), serviceUUID, attrUUID));
			break;
		case 1:	
			serviceUUID = 0x1001;	// Browse group descriptor class ID
			attrUUID = KSdpAttrIdSdpServerGroupID;
			FLOG(_L("***********SECOND PHASE SERVICE SEARCH ATTRIBUTE TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single UUID 0x%04x, and single attr id 0x%04x***********"), serviceUUID, attrUUID));
			break;
		case 2:
			serviceUUID = (TUint16)KGenericNetworkingUUID;	// Generic networking
			attrUUID = KSdpAttrIdServiceClassIDList;
			FLOG(_L("***********THIRD PHASE SERVICE SEARCH ATTRIBUTE TRANSACTION(S)**************"));
			FTRACE(FPrint(_L("***********uses single UUID 0x%04x, and single attr id 0x%04x***********"), serviceUUID, attrUUID));
			break;
		default:
			User::Leave(KErrGeneral);
			break;
			}

		TUint8 loop = 1;
		iNeedMoreRecordHandles = EFalse;
		while(loop)
			{
			TServiceAttributeRequest servAttrRequest;
			if(iBrowseIter==2)
				{
				iNeedMoreRecordHandles = ETrue;
				TInt j = 0;
				while(j<iSP->Count())
					{
					header = TSDPHeader(0x06, iRawTransactionId++, TUint16(ssaParamsSize + iContLength + 1));//cont length byte);
					servAttrRequest = TServiceAttributeRequest(iSP->At(j), aUuidType, 200, attrUUID);
					request.Zero();
					header.AppendHeader(request);
					servAttrRequest.AppendRequest(request);
					request.Append(&iContLength, 1);
					request.Append(iContinuationBuffer, iContLength);
					FlogWriteString(request);
					iRawSock.Write(request, writeStat);
					User::WaitForRequest(writeStat);
					// Wait for reply
					//
					readStat = KRequestPending;
					iRawSock.Read(queryResult1, readStat);
					User::WaitForRequest(readStat);
					User::LeaveIfError(readStat.Int());
					// Process results
					//
					FlogReadString(queryResult1);
					User::LeaveIfError(ProcessServiceAttributeSearchResults(queryResult1, queryResult2));
					if(!iContLength)
						j++;
					}
				iNeedMoreRecordHandles = EFalse;
				}
			else
				{
				header = TSDPHeader(0x06, iRawTransactionId++, TUint16(ssaParamsSize + iContLength + 1));//cont length byte);
				servAttrRequest = TServiceAttributeRequest(serviceUUID, aUuidType, 200, attrUUID);
				//Now add these to the query
				//
				TRequestStatus writeStat;
				TRequestStatus readStat;
				TBuf8<700> queryResult1;
				TBuf8<700> queryResult2;
				TBuf8<100> request;
				request.Zero();
				header.AppendHeader(request);
				servAttrRequest.AppendRequest(request);
				request.Append(&iContLength, 1);
				request.Append(iContinuationBuffer, iContLength);
				FlogWriteString(request);
				iRawSock.Write(request, writeStat);
				User::WaitForRequest(writeStat);
				// Wait for reply
				//
				readStat = KRequestPending;
				iRawSock.Read(queryResult1, readStat);
				User::WaitForRequest(readStat);
				User::LeaveIfError(readStat.Int());
				// Process results
				//
				FlogReadString(queryResult1);
				User::LeaveIfError(ProcessServiceAttributeSearchResults(queryResult1, queryResult2));
				
				}
			loop = iContLength;
			}
		// Increment tree level counter
		//
		iBrowseIter++;
		}
	}

//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--
//--



TInt CSdpVerificationTests::ProcessSearchResults(TDesC8& aResult1, TDesC8& aResult2)
	{
	// Process accordingly
	//
	TInt ret = KErrNone;
	if(iQueryState==EServiceRequest)
	{
		ret = ProcessServiceSearchResults(aResult1, aResult2);
	}
	else if(iQueryState==EServiceAttributeRequest)
	{
		ret = ProcessAttributeSearchResults(aResult1, aResult2);
	}
	else
	{
		ASSERT_DEBUG(iQueryState==EServiceSearchAttributeRequest);
		ret = ProcessServiceAttributeSearchResults(aResult1, aResult2);
	}
	return ret;
	}

TInt CSdpVerificationTests::ProcessServiceSearchResults(TDesC8& aResult1, TDesC8& aResult2)
	{

	TInt ret = KErrGeneral;

	// Have we found any records?
	//
	TBool haveRecords = ETrue;
	if (aResult1.Length() < KMinResponseSize)	
	{
		haveRecords = EFalse;
	}
	if(iTestNum == 1)
	{
		// Check the second result completed with records too
		if((haveRecords) && (aResult2.Length() < 8))
		{
			haveRecords = EFalse;
		}
	}

	// The test document dictates that size comparisons must be carried out.
	//

	TUint16 totalRecordCount = 0;
	TUint16 currentRecordCount = 0;
	TUint16 totalRecordCount2 = 0;
	TUint16 currentRecordCount2 = 0;
	if(haveRecords)
	{
		totalRecordCount = BigEndian::Get16(&aResult1[KTotalCountOffset]);
		currentRecordCount = BigEndian::Get16(&aResult1[KCurrentCountOffset]);
		/*
		...now done in FlogParsedPDUHeader(..)

		if(currentRecordCount > totalRecordCount)
		{
			return KErrGeneral;
		}
		TUint maxRecordCount = KMaxNumRecordHandles;
		if(iTestNum<KSdpBrowseTest)
		{
			maxRecordCount = iServiceTests[iTestNum].MaxRecordCount();
		}
		if(totalRecordCount > maxRecordCount)
		{
			return KErrGeneral;
		}
		*/
		// Check the second buffer as well if necessary
		//
		if(iTestNum==1)
		{
			totalRecordCount2 = BigEndian::Get16(&aResult2[KTotalCountOffset]);
			currentRecordCount2 = BigEndian::Get16(&aResult2[KCurrentCountOffset]);
			if(currentRecordCount2 > totalRecordCount2)
			{
				return KErrGeneral;
			}
			if(totalRecordCount2 > iServiceTests[iTestNum].MaxRecordCount())
			{
				return KErrGeneral;
			}
		}
	}


	// The rest depends on which tests is being run...
	//
	TInt recordHandleOffset = KRecordHandleOffset;
	if(iType==EServiceAttributeRequest)
		{
		if(haveRecords)
			{
			iAttributeTestServiceRecordHandle = BigEndian::Get32(&aResult1[recordHandleOffset]);
			}
		}
	switch(iTestNum)
	{
	case 1:
	case 0:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
		if(haveRecords)
		{
			// Check the returned record is correct 
			// Should only return the 1st record
			//
			if(currentRecordCount==1)
			{
				TUint32 serviceRecordHandle = BigEndian::Get32(&aResult1[recordHandleOffset]);
				if(serviceRecordHandle==KSdpMinServRecordHandle)
				{
					ret = KErrNone;
				}
				
				// Now check the second result
				if((iTestNum==1)&& (ret==KErrNone))
				{
					serviceRecordHandle = BigEndian::Get32(&aResult2[recordHandleOffset]);
					if(serviceRecordHandle!=KSdpMinServRecordHandle+1)
					{
						ret = KErrGeneral;
					}
				}
			}
		}
		break;
	case 2:
		if(haveRecords)
		{
			// This one should have the continuation state set
			// Offset of continuation byte =
			// (CurrentServiceRecordCount * 4) + 4 + HeaderSize
			TUint16	partial = TUint16((currentRecordCount * 4) + 4 + KSdpPduHeaderSize); 
			iContLength = aResult1[partial];
			TBuf8<20> cont;
			cont.SetLength(0);
			cont.Append(aResult1.Mid(partial));
			FlogContinuationString(cont);
			if (iContLength == 0)
			{
				// No continuation set. Is this the end or is there an error?
				// Are these handles correct?
				ret=KErrNone;
				for(TInt i=0; i<currentRecordCount; i++)
				{
					TUint32 serviceRecordHandle = BigEndian::Get32(&aResult1[recordHandleOffset]);
					recordHandleOffset += 4;	//Move to next handle;
					if(serviceRecordHandle<KSdpMinServRecordHandle+3)
					{
						ret = KErrGeneral;
					}
				}
			}
			else
			{
				// Check these UUIDs first
				//
				ret = KErrNone;
				for(TInt i=0; i<currentRecordCount; i++)
				{
					TUint32 serviceRecordHandle = BigEndian::Get32(&aResult1[recordHandleOffset]);
					recordHandleOffset += 4;	//Move to next handle;
					if(serviceRecordHandle<KSdpMinServRecordHandle+3)
					{
						ret = KErrGeneral;
					}
				}
				if(ret==KErrNone)
				{
					Mem::Copy(iContinuationBuffer, &aResult1[partial+1], KSdpContinuationStateMaxLength);
					TRAP(ret, SDPQueryL());
					// Clear out the continuation state buffer ready for next query
					//
					Mem::FillZ( iContinuationBuffer,KSdpContinuationStateMaxLength);
					iContLength = 0;
				}
			}
		}
		break;
	case 3:
		// Should get no records back here
		if(haveRecords)
		{	
			if((totalRecordCount==currentRecordCount) && (totalRecordCount==0))
			{
				ret=KErrNone;
			}
		}
		break;
	case 4:
	case 5:
		// This should be the SDP_ErrorResponse PDU 
		//
		if(!haveRecords)	// Should be no records here
		{
			if(aResult1[0] == 0x01)
			{
				// This is an error response.
				// Check the error code. It must either be 
				// an invalid size or invalid syntax.
				//
				TInt16 errorCode = BigEndian::Get16(&aResult1[KErrorCodeOffset]);
				if(((errorCode==EInvalidPduSize) && (iTestNum==4))
					||((errorCode==EInvalidRequestSyntax) && (iTestNum==5)))
				{
					ret = KErrNone;
				}
				// Otherwise, wrong response.
			}
		}
		break;
	case 53:
		if(haveRecords)
		{
			// Check the returned record is correct.
			//
			TUint16	partial = TUint16((currentRecordCount * 4) + 4 + KSdpPduHeaderSize); 

			if(iBrowseIter/*=*/<=2)
			{
				ret = KErrNone;
			}

			for (TUint i = iBrowseTestNumRecordHandlesStored; 
				 i < iBrowseTestNumRecordHandlesStored + currentRecordCount; 
				 i ++)
			{
				iBrowseTestServiceRecordHandle[i] = BigEndian::Get32(&aResult1[recordHandleOffset]);
				recordHandleOffset += 4;
				for(TUint j = 0; j<i; j++)
					{
					if(iBrowseTestServiceRecordHandle[i]==iBrowseTestServiceRecordHandle[j])
						//Check we are not repeating a handle
						//and if so set that handle to 'KUselessRecordHandle'
						//so that we ignore it!
						{
						iBrowseTestServiceRecordHandle[i] = KUselessRecordHandle; //indicates useless handle
						break;
						}
					}
			}

			if(iContLength)
				{
				Mem::Copy(iContinuationBuffer, &aResult1[partial+1], KSdpContinuationStateMaxLength);
				}
			iBrowseTestNumRecordHandlesStored += currentRecordCount;
		}
		break;
	default:
		break;
	}

	return ret;
	}


TInt CSdpVerificationTests::ProcessServiceAttributeSearchResults(TDesC8& aResult1, TDesC8& aResult2)
	{
	TInt ret = KErrGeneral;
	// Have got a response?
	//
	TBool haveResult = ETrue;
	if (aResult1.Length() < KMinResponseSize)	
	{
		haveResult = EFalse;
	}
	if(iTestNum == 33)
	{
		// Check the second result completed with records too
		if((haveResult) && (aResult2.Length() < KMinResponseSize))
		{
			haveResult = EFalse;
		}
	}
	// The test document dictates that size comparisons must be carried out.
	//
	TUint16 byteCount1 = 0;
	TUint16 byteCount2 = 0;
	if(haveResult)
	{
		TUint16 intendedByteCount = 200;
		if(iTestNum<KSdpBrowseTest)
		{
			intendedByteCount = iServiceAttributeTests[iTestNum - KNumSDPServiceTests - KNumSDPServiceAttributeTests].MaxByteCount();
		}
		byteCount1 = BigEndian::Get16(&aResult1[KAttributeByteCountOffset]);
		if(byteCount1 > intendedByteCount)
		{
			return KErrGeneral;
		}
		//Now check the second buffer if necessary
		//
		if(iTestNum==33)
		{
			byteCount2 = BigEndian::Get16(&aResult2[KAttributeByteCountOffset]);
			if(byteCount2 > iServiceAttributeTests[iTestNum - KNumSDPServiceTests - KNumSDPServiceAttributeTests].MaxByteCount())
			{
				return KErrGeneral;
			}
		}
	}
	
	// Now parse what we've got back
	//
	if(haveResult)
	{
		if(byteCount1==0)
		{
			if((iTestNum==29) || (iTestNum==30)	|| (iTestNum==31))
			{
				// the non-existing attribute/service test
				ret = KErrNone;
			}
		}
		else
		{
			#ifdef PRINT_BUILDER
				TRAP(ret,MSdpElementBuilder* builder = (MSdpElementBuilder*)new(ELeave)CPrintBuilder(iConsole));
				if (ret != KErrNone)
					{
					return ret;
					}
			#else
				MSdpElementBuilder* builder = this;
			#endif
			
			if(iTestNum>=KSdpBrowseTest)
			{
				TRAP(ret,builder = (MSdpElementBuilder*)new(ELeave)CBrowseResponseBuilder(iTestName,iBrowseIter/*,iTestNum - KSdpBrowseTest + 1, GetUuidSize()*/, *iSP));
				if (ret != KErrNone)
					{
					return ret;
					}
			}
			
			CElementParser* parser = NULL;
			TRAP(ret,parser = CElementParser::NewL(builder));
			if (ret != KErrNone)
				{
				return ret;
				}
			TRAP(ret,CleanupStack::PushL(parser));
			if (ret != KErrNone)
				{
				return ret;
				}
					
			// Get the offset of the continuation state
			TUint16	attributeListLength = BigEndian::Get16(&aResult1[KSdpPduHeaderSize]);
			TUint16 partial = TUint16(attributeListLength + KSdpPduHeaderSize + 2);
			iContLength = aResult1[partial];
			TBuf8<20> cont;
			cont.SetLength(0);
			cont.Append(aResult1.Mid(partial));
			FlogContinuationString(cont);

			if (iTestNum == 34)
			{
				// This one should have the continuation state set so do another query.
				iContinuationStore.Append(aResult1.Mid(KAttributeListOffset, attributeListLength));
				if (iContLength != 0)
				{
					Mem::Copy(iContinuationBuffer, &aResult1[partial+1], KSdpContinuationStateMaxLength);
					TRAPD(err, SDPQueryL());
					ret = err;
					// Clear out the continuation state buffer ready for next query
					//
					Mem::FillZ( iContinuationBuffer,KSdpContinuationStateMaxLength);
					iContLength = 0;
				}
				else
				{
					TRAPD(err, parser->ParseElementsL(iContinuationStore));
					ret = err;
					iContinuationStore.SetLength(0);
				}
			}
			else if(iTestNum>=KSdpBrowseTest)
			{
			// This one might have the continuation state set so prepare, 
			// if necessary, for another query.
				iContinuationStore.Append(aResult1.Mid(KAttributeListOffset, attributeListLength));
				if (iContLength != 0)
				{
					Mem::Copy(iContinuationBuffer, &aResult1[partial+1], KSdpContinuationStateMaxLength);
					ret = KErrNone;
				}
				else
				{
					TRAPD(err, parser->ParseElementsL(iContinuationStore));
					ret = err;
					iContinuationStore.SetLength(0);
				}
			}
			else
			{
				TRAPD(err,parser->ParseElementsL(aResult1.Mid(KAttributeListOffset, attributeListLength)));
				ret = err;
			}

			if((ret==KErrNone)&&(iTestNum == 33))
			{
				// We need to parse the second buffer as well.
				//
				if((byteCount2!=0))
				{
					iNeedAttributeId = ETrue;
					TUint16	attributeListLength2 = BigEndian::Get16(&aResult2[KSdpPduHeaderSize]);
					TRAPD(err,ret=parser->ParseElementsL(aResult2.Mid(KAttributeListOffset, attributeListLength2)));
					if (err)
						{
						ret = err;
						}
				}
			}
			if(iTestNum>=KSdpBrowseTest)
			{
				delete (CBrowseResponseBuilder*)builder;
				builder = 0;
			}
			#ifdef PRINT_BUILDER
			delete (CPrintBuilder*)builder;
			iConsole.Printf(_L("Press any key.\r\n"));
			iConsole.Getch();
			#endif
			
			CleanupStack::PopAndDestroy(parser);
		}
	}
	else
	{
		// What's left should be invalid behaviour tests
		// ie. the SDP_ErrorResponse PDU 
		if(aResult1[0] == 0x01)
		{
			// This is an error response.
			// Check the error code. It must either be 
			// an invalid size, invalid syntax or invalid record handle.
			//
			TInt16 errorCode = BigEndian::Get16(&aResult1[KErrorCodeOffset]);
				if(((errorCode==EInvalidPduSize) && (iTestNum==52))
					||((errorCode==EInvalidRequestSyntax) && (iTestNum==51)))
				{
					ret = KErrNone;
				}
			// Otherwise, wrong response.
		}
	}
	iContLength = 0; //reset
	return ret;
	}


TInt CSdpVerificationTests::ProcessAttributeSearchResults(TDesC8& aResult1, TDesC8& aResult2)
	{
	TInt ret = KErrGeneral;
	// Have got a response?
	//
	TBool haveResult = ETrue;
	if (aResult1.Length() < KMinResponseSize)	
	{
		haveResult = EFalse;
	}
	if(iTestNum == 7)
	{
		// Check the second result completed with records too
		if((haveResult) && (aResult2.Length() < KMinResponseSize))
		{
			haveResult = EFalse;
		}
	}
	// The test document dictates that size comparisons must be carried out.
	//
	TUint16 byteCount1 = 0;
	TUint16 byteCount2 = 0;
	if(haveResult)
	{
		TUint16 intendedByteCount = 200;
		if(iTestNum<KSdpBrowseTest)
		{
			intendedByteCount = iAttributeTests[iTestNum - KNumSDPServiceTests].MaxByteCount();
		}	
		byteCount1 = BigEndian::Get16(&aResult1[KAttributeByteCountOffset]);
		if(byteCount1 > intendedByteCount)
		{
			return KErrGeneral;
		}
		// Check the second buffer as well if necessary
		//
		if(iTestNum==7)
		{
			byteCount2 = BigEndian::Get16(&aResult2[KAttributeByteCountOffset]);
			if(byteCount2 > iAttributeTests[iTestNum - KNumSDPServiceTests].MaxByteCount())
			{
				return KErrGeneral;
			}
		}
	}
	
	// Now Parse what we've got back
	//
	if(haveResult)
	{
		if(byteCount1!=0)
		{
			MSdpElementBuilder* builder = this;
			if(iTestNum>=KSdpBrowseTest)
			{
				TRAP(ret,builder = (MSdpElementBuilder*)new(ELeave)CBrowseResponseBuilder(iTestName,iBrowseIter/*,iTestNum - KSdpBrowseTest + 1, GetUuidSize()*/, *iSP));
				if (ret != KErrNone)
					{
					return ret;
					}
			}
			
			CElementParser* parser = NULL;
			TRAP(ret,parser = CElementParser::NewL(builder));
			if (ret != KErrNone)
				{
				return ret;
				}
			
			TRAP(ret,CleanupStack::PushL(parser));			
			if (ret != KErrNone)
				{
				return ret;
				}
			
			// Get the offset of the continuation state
			TUint16	attributeListLength = BigEndian::Get16(&aResult1[KSdpPduHeaderSize]);
			TUint16 partial = TUint16(attributeListLength + KSdpPduHeaderSize + 2);
			iContLength = aResult1[partial];
			TBuf8<20> cont;
			cont.SetLength(0);
			cont.Append(aResult1.Mid(partial));
			FlogContinuationString(cont);
			if (iTestNum == 8)
			{
				// This one should have the continuation state set so do another query.
				iContinuationStore.Append(aResult1.Mid(KAttributeListOffset, attributeListLength));
				if (iContLength != 0)
				{
					Mem::Copy(iContinuationBuffer, &aResult1[partial+1], KSdpContinuationStateMaxLength);
					TRAPD(err, SDPQueryL());
					ret = err;
					// Clear out the continuation state buffer ready for next query
					//
					Mem::FillZ( iContinuationBuffer,KSdpContinuationStateMaxLength);
					iContLength = 0;
					}
				else
				{
					TRAPD(err, parser->ParseElementsL(iContinuationStore));
					ret = err;
					iContinuationStore.SetLength(0);
				}
			}
			else if(iTestNum>=KSdpBrowseTest)
			{
			// This one might have the continuation state set so prepare, 
			// if necessary, for another query.
				iContinuationStore.Append(aResult1.Mid(KAttributeListOffset, attributeListLength));
				if (iContLength != 0)
				{
					Mem::Copy(iContinuationBuffer, &aResult1[partial+1], KSdpContinuationStateMaxLength);
					ret = KErrNone; //only error when (?) iContLength is zero
				}
				else
				{
					TRAPD(err, parser->ParseElementsL(iContinuationStore));
					ret = err;
					iContinuationStore.SetLength(0);
				}
			}
			else
			{
				TRAPD(err, parser->ParseElementsL(aResult1.Mid(KAttributeListOffset, attributeListLength)));
				ret = err;
			}

			if((ret==KErrNone)&&(iTestNum == 7))
			{
				// We need to parse the second buffer as well.
				//
				if(byteCount2!= 0)
				{
					iNeedAttributeId = ETrue;
					TRAPD(err,ret=parser->ParseElementsL(aResult2.Mid(KAttributeListOffset)));
					if (err)
						{
						ret = err;
						}					
				}
			}
			if(iTestNum>=KSdpBrowseTest)
			{
				delete (CBrowseResponseBuilder*)builder;
				builder = 0;
			}
			CleanupStack::PopAndDestroy(parser);
			parser = 0;		
		}
	}
	else
	{
		// What's left should be invalid behaviour tests
		// ie. the SDP_ErrorResponse PDU 
		if(aResult1[0] == 0x01)
		{
			// This is an error response.
			// Check the error code. It must either be 
			// an invalid size, invalid syntax or invalid record handle.
			//
			TInt16 errorCode = BigEndian::Get16(&aResult1[KErrorCodeOffset]);
			if(((errorCode==EInvalidServiceRecordHandle) && (iTestNum==26))
				||((errorCode==EInvalidPduSize) && (iTestNum==28))
				||((errorCode==EInvalidRequestSyntax) && (iTestNum==27)))
			{
				ret = KErrNone;
			}
			// Otherwise, wrong response.
		}
	}
	return ret;
	}


void CSdpVerificationTests::SetUpServiceTestParams()
/*
	THE BIG SETUP.....
*/
	{
	// Fill up the service test structure (see SetUpAttributeParams() for desription)
/*	TUint8 validDes[3];
	validDes[0] = 0x35;
	validDes[1] = 0x03;
	validDes[2] = 0x19;
*/
	TUint8 invalidDes[3];
	invalidDes[0] = 0x35;
	invalidDes[1] = 0x03;
	invalidDes[2] = 0x09;

	// TEST 0 - Dial up networking should return the first handle
	iServiceTests[0] = TServiceRequest((TUint16)KDialUpNetworkingUUID, EUuid16, (TUint16)3);
	
	// TEST 1 - Search for 2 services (use test1 as well), should return the first 2 handles	
	iServiceTests[1] = TServiceRequest((TUint16)KFaxUUID, EUuid16, (TUint16)3);

	// TEST 2 - Search for Headset UUID can return any of the registered records. The max record
	// handles param is large so that continuation occurs
	iServiceTests[2] = TServiceRequest((TUint16)0x1108, EUuid16, (TUint16)200);
	
	// TEST 3 - SPP profile UUID has not been registered so should return with no records
	iServiceTests[3] = TServiceRequest((TUint16)KNonExistantUUID, EUuid16, (TUint16)1);

	// TEST 4 - UUID doesn't matter as size will be invalid.
	iServiceTests[4] = TServiceRequest((TUint16)0x1101, EUuid16, (TUint16)1);

	// TEST 5 - UUID doesn't matter as des will be invalid.
	iServiceTests[5] = TServiceRequest(invalidDes, (TUint16)0x1101, EUuid16, (TUint16)1);

	for(TInt i=KNumSDPServiceTests; i<25; i++)
	{
		iServiceTests[i] = TServiceRequest((TUint16)KDialUpNetworkingUUID, EUuid16, (TUint16)2);
	}

	// This one needs to return the second record handle
	iServiceTests[25] = TServiceRequest((TUint16)KFaxUUID, EUuid16, (TUint16)2);
	iServiceTests[26] = TServiceRequest((TUint16)KDialUpNetworkingUUID, EUuid16, (TUint16)2);
	iServiceTests[27] = TServiceRequest((TUint16)KDialUpNetworkingUUID, EUuid16, (TUint16)2);
	}

void CSdpVerificationTests::SetUpAttributeTestParams()
	{
	// Fill in the attribute request params.
	TUint8 validDes[3];
	validDes[0] = 0x35; // Type = 6(DES), Size = 5(size is contained in next 8 bits).
	validDes[1] = 0x03; // This is the next 8 bits: size = 3 
	validDes[2] = 0x09;	// Type = 1 (Unsigned int), size = 1 (2Bytes).

	TUint8 invalidDes[3];
	invalidDes[0] = 0x35;// Type = 6(DES), Size = 5(size is ontained in next 8 bits).
	invalidDes[1] = 0x03;// This is the next 8 bits: size = 3 
	invalidDes[2] = 0x19;// Type = 3 (UUID), size = 1 (2Bytes).

	// TEST 6 - Service for an existing attribute (0x0005) in the first record.
	//  
	iAttributeTests[0]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdBrowseGroupList); 
	
	// TEST 7 - Search for 2 attributes, see test 7 and also 0x0003 Service Id
	iAttributeTests[1]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdServiceID);

	// TEST 8 - Like test 1 but using continuation state (search for a very long URL).
	iAttributeTests[2]=TAttributeRequest(KSdpMinServRecordHandle + 1, 200, validDes, KSdpAttrIdDocumentationURL);
	
	// TEST 9 - Service ID
	iAttributeTests[3]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdServiceID);

	// TEST 10 - Protocol descriptor list
	iAttributeTests[4]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdProtocolDescriptorList);

	// TEST 11 - Service record state
	iAttributeTests[5]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdServiceRecordState);

	// TEST 12 - ServiceInfoTimetoLive
	iAttributeTests[6]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdServiceInfoTimeToLive);

	// TEST 13 - BrowseGroupList
	iAttributeTests[7]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdBrowseGroupList);

	// TEST 14 - LanguageBaseAttributeId
	iAttributeTests[8]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdLanguageBaseAttributeIDList);

	// TEST 15 - ServiceAvailability
	iAttributeTests[9]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdServiceAvailability);

	// TEST 16 - IconURL
	iAttributeTests[10]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdIconURL);

	// TEST 17 - ServiceName (offset from LanguageBaseAttributeIDList).
	iAttributeTests[11]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, (TUint16)(KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceName));

	// TEST 18 - Service Description
	iAttributeTests[12]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, (TUint16)(KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceDescription));

	// TEST 19 - ProviderName
	iAttributeTests[13]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, (TUint16)(KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetProviderName));

	// TEST 20 - Version number list
	iAttributeTests[14]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdSdpServerVersionNumberList);

	// TEST 21 - Service Database State
	iAttributeTests[15]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdSdpServerServiceDatabaseState);

	// TEST 22 - Bluetooth Profile DescriptorList
	iAttributeTests[16]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdBluetoothProfileDescriptorList);

	// TEST 23 - Documentation URL
	iAttributeTests[17]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdDocumentationURL);

	// TEST 24 - Client Executable URL
	iAttributeTests[18]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdClientExecutableURL);

	// TEST 25 - Non-existing attribute
	iAttributeTests[19]=TAttributeRequest(KSdpMinServRecordHandle+1, 200, validDes, KSdpAttrIdServiceRecordState);

	//TEST 26 - Invalid service record handle
	iAttributeTests[20]=TAttributeRequest(KSdpMinServRecordHandle-1, 200, validDes, KSdpAttrIdServiceRecordState);
	
	// TEST 27 - invalid syntax
	iAttributeTests[21]=TAttributeRequest(KSdpMinServRecordHandle, 200, invalidDes, KSdpAttrIdServiceClassIDList);

	// TEST 28 - invalid PDU size
	iAttributeTests[22]=TAttributeRequest(KSdpMinServRecordHandle, 200, validDes, KSdpAttrIdServiceClassIDList);

	}


void CSdpVerificationTests::SetUpServiceAttributeTestParams()
	{
	// see SetUpAttributeParams() for desription
	TUint8 validServiceDes[] = {0x35, 0x03, 0x19};
	TUint8 validAttributeDes[3];

	/*
	TUint8 validUUID128[16];
	validUUID128[0] = 0x0;
	validUUID128[1] = 0x0;
	validUUID128[2] = 0x0;
	validUUID128[3] = 0x0;
	//Bluetooth_Base_UUID
	validUUID128[4] = 0x00;
	validUUID128[5] = 0x00;
	validUUID128[6] = 0x10;
	validUUID128[7] = 0x00;
	validUUID128[8] = 0x80;
	validUUID128[9] = 0x00;
	validUUID128[10] = 0x00;
	validUUID128[11] = 0x80;
	validUUID128[12] = 0x5f;
	validUUID128[13] = 0x9b;
	validUUID128[14] = 0x34;
	validUUID128[15] = 0xfb;
	*/

	validAttributeDes[0] = 0x35;
	validAttributeDes[1] = 0x03;
	validAttributeDes[2] = 0x09;

	// TEST 29 - non-existing service (SPP), existing attribute.
	iServiceAttributeTests[0]=TServiceAttributeRequest((TUint16)0x1101, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceClassIDList );

	// TEST 30 - existing service, non-existing attribute
	iServiceAttributeTests[1]=TServiceAttributeRequest((TUint16)KFaxUUID, EUuid16,  KMaxAttributeListByteCount, KSdpAttrIdServiceRecordState);
	
	// TEST 31 - non-existing service, non-existing attribute
	iServiceAttributeTests[2]=TServiceAttributeRequest((TUint16)0x1101, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceRecordState) ;
	
	// TEST 32 - existing service and attribute
	iServiceAttributeTests[3]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdBrowseGroupList );
	
	// TEST 33 - two searches for existing services and attibutes (see test 33 as well).
	iServiceAttributeTests[4]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceID );
	
	// TEST 34 - continuation UUID16
#ifdef SEVEN_LAYERS_BUG
	iServiceAttributeTests[5]=TServiceAttributeRequest(
				(TUint16)KPublicBrowseGroupUUID,
				EUuid16,
				0xffff,//0x1e,//0x30,//KMaxAttributeListByteCount, 
				KSdpAttrIdServiceClassIDList);//(TUint32)0x0001);
#else
	iServiceAttributeTests[5]=TServiceAttributeRequest(
				//(TUint16)KPublicBrowseGroupUUID,
				0x1101,
				EUuid16,
				0x30,//KMaxAttributeListByteCount, 
				(TUint32)KSdpAttributeBigRange);
#endif
	// TEST 35 - service record state 
	iServiceAttributeTests[6]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceRecordState);
	
	// Test 36 - Service database state
	iServiceAttributeTests[7]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdSdpServerServiceDatabaseState);

	// TEST 37 - service info time to live
	iServiceAttributeTests[8]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceInfoTimeToLive);
	
	// TEST 38 - Service Id
	iServiceAttributeTests[9]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceID);
	
	// TEST 39 - Protocol descriptor list
	iServiceAttributeTests[10]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdProtocolDescriptorList);
	
	// TEST 40 - Browse group list
	iServiceAttributeTests[11]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdBrowseGroupList);
	
	// TEST 41 - Language base attribute id
	iServiceAttributeTests[12]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdLanguageBaseAttributeIDList);
	
	// TEST 42 - Service availability - 
	iServiceAttributeTests[13]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceAvailability);
	
	// TEST 43 - Icon URL
	iServiceAttributeTests[14]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdIconURL);
	
	// TEST 44 - Service Name
	iServiceAttributeTests[15]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, (TUint16)(KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceName));
	
	// TEST 45 - Service description
	iServiceAttributeTests[16]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, (TUint16)(KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetServiceDescription));
	
	// TEST 46 - Provider name
	iServiceAttributeTests[17]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, (TUint16)(KSdpAttrIdBasePrimaryLanguage+KSdpAttrIdOffsetProviderName));
	
	// TEST 47 - version number list
	iServiceAttributeTests[18]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdSdpServerVersionNumberList);
	
	// TEST 48 - Bluetooth profile descriptor list 
	iServiceAttributeTests[19]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdBluetoothProfileDescriptorList);
	
	// TEST 49 - Documentation URL
	iServiceAttributeTests[20]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdDocumentationURL);

	// TEST 50 - Client executable URL
	iServiceAttributeTests[21]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdClientExecutableURL);

	// TEST 51 - invalid request syntax (swap round the des values).
	iServiceAttributeTests[22]=TServiceAttributeRequest(validAttributeDes, (TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, validServiceDes, KSdpAttrIdClientExecutableURL);

	// TEST 52 - invalid PDU size
	iServiceAttributeTests[23]=TServiceAttributeRequest((TUint16)KDialUpNetworkingUUID, EUuid16, KMaxAttributeListByteCount, KSdpAttrIdServiceClassIDList);

	}

TBool CSdpVerificationTests::AnotherQuery()
	{
	if((iTestNum==1)||(iTestNum==7)||(iTestNum==33))
	{
		return ETrue;
	}
	return EFalse;
	}

TBool CSdpVerificationTests::Continue()
	{
	TBool ret = EFalse;
	if(iType==EServiceRequest)
	{
		ret = EFalse;
	}
	else if((iType==EServiceAttributeRequest)&&(iQueryState!=EServiceAttributeRequest))
	{
		iQueryState = EServiceAttributeRequest;
		ret = ETrue;
	}
	return ret;
	}

TInt CSdpVerificationTests::GetUuidSize()
	{
	switch(iUuidType)
		{
		case CSdpTestBase::EUuid16:
			return 16;
		case CSdpTestBase::EUuid32:
			return 32;
		case CSdpTestBase::EUuid128:
			return 128;
		default:
			return 0;
		}
	}

void CSdpVerificationTests::UpdateTestString()
	{		
	TInt uuidsize = GetUuidSize();
	TBuf<4> uuidBuf;
	uuidBuf.SetLength(0);
	switch(uuidsize)
		{
		case 16:
			uuidBuf.Append('1');
			uuidBuf.Append('6');
			break;
		case 32:
			uuidBuf.Append('3');
			uuidBuf.Append('2');
			break;
		case 128:
			uuidBuf.Append('1');
			uuidBuf.Append('2');
			uuidBuf.Append('8');
			break;
		default:
			break;
		}
	TChar ch = 'V';
	TInt num = 0;
	iTestName.SetLength(0);
	if(/*iTestNum>=KServiceRequestTest&&*/iTestNum<KServiceAttributeRequestTest)
			{
			num = iTestNum - KServiceRequestTest + 1;
			if(iTestNum>=KServiceRequestBITest)
				{
				ch = 'I';
				num = iTestNum - KServiceRequestBITest + 1;
				}
			iTestName.Append(_L("TC_SERVER_SS_UUID"));
			iTestName.AppendNum(uuidsize);
			iTestName.Append(_L("_B"));
			}
		else if(iTestNum>=KServiceAttributeRequestTest&&iTestNum<KServiceSearchAttributeRequestTest)
			{
			num = iTestNum - KServiceAttributeRequestTest + 1;
			if(iTestNum>=KServiceAttributeRequestBITest)
				{
				ch = 'I';
				num = iTestNum - KServiceAttributeRequestBITest + 1;
				}
			iTestName.Append(_L("TC_SERVER_SA_B"));
			}
		else if(iTestNum>=KServiceSearchAttributeRequestTest && iTestNum<KSdpBrowseTest)
			{
			num = iTestNum - KServiceSearchAttributeRequestTest + 1;
			if(iTestNum>=KServiceSearchAttributeRequestBITest)
				{
				ch = 'I';
				num = iTestNum - KServiceSearchAttributeRequestBITest + 1;
				}
			iTestName.Append(_L("TC_SERVER_SSA_UUID"));
			iTestName.AppendNum(uuidsize);
			iTestName.Append(_L("_B"));
			}
		else if(iTestNum>=KSdpBrowseTest && iTestNum<=KSdpLastTest)
			{
			num = iTestNum - KSdpBrowseTest + 1;
			iTestName.Append(_L("TC_SERVER_BRW_UUID"));
			iTestName.AppendNum(uuidsize);
			iTestName.Append(_L("_B"));
			}
		else
			{
			; //print nothing
			}
		iTestName.Append(ch);
		iTestName.Append('_');
		if(num<10)
			iTestName.Append('0');
		iTestName.AppendNum(num);
		iTestName.Append(_L("_C"));
	}

void CSdpVerificationTests::FlogTestId()
	{
	UpdateTestString();
	FTRACE(FPrint(_L("")));
	FTRACE(FPrint(_L("%S: "), &iTestName));
	}

#ifdef __FLOGGING__
void CSdpVerificationTests::FlogString(const TText* aLineStart, TDesC8& aString)
	{
	TInt len = aString.Length();
	TInt i = 0;
	while(len-i>=6)
		{
		RFileLogger::HexDump(KLogDir, KLogFile, EFileLoggingModeAppend, aLineStart, 0, aString.Mid(i, 6).Ptr(), 6);
		i+=6;
		}
	if(len-i>=0)
		{
		RFileLogger::HexDump(KLogDir, KLogFile, EFileLoggingModeAppend, aLineStart, 0, aString.Mid(i, len-i).Ptr(), len-i);
		}
	}
#else
void CSdpVerificationTests::FlogString(const TText* /*aLineStart*/, TDesC8& /*aString*/)
	{
	}
#endif


TInt CSdpVerificationTests::MaxCountFoundAfterServiceSearchPatternOrWhatYouWill(TDesC8& aString)
	{
	TInt ret = -1;
	TUint32 pos = 0;
	if(aString.Length()<7)
		return ret;
	TInt listType = aString[5];
	switch(listType)
		{
		case 0x35:
			if(aString.Length()<7)
				return ret;
			pos += 7+aString[6];
			break;
		case 0x36:
			if(aString.Length()<8)
				return ret;
			pos += 8+aString[7]+256*aString[6];
			break;
		case 0x37:
			if(aString.Length()<10)
				return ret;
			pos += 10+aString[9]+256*aString[8]+256*256*aString[7]+256*256*256*aString[6];
			break;
		default:
			return ret;
		}
	ret = aString[pos+1]+256*aString[pos];
	return ret;
	}

void CSdpVerificationTests::FlogParsedPDUHeader(TDesC8& aString)
	{
	TInt id = aString[2]+256*aString[1];
	FTRACE(FPrint(_L("SDP_Transaction Id: 0x%02x"), id));
	switch(aString[0])
		{
		case 1:
			if(iRequestTransId != id)
				{
				FTRACE(FPrint(_L("Request and Error Response Transaction Ids DO NOT MATCH: 0x%02x != 0x%02x ... Error code still printed below"), id, iRequestTransId));
				}
			if(aString.Length()>=7)
				{
				TInt errorCode = aString[6]+256*aString[5];
				switch(errorCode)
					{
					case 1:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Invalid/Unsupported SDP version")));
						break;
					case 2:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Invalid Srvice Record Handle")));
						break;
					case 3:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Invalid request syntax")));
						break;
					case 4:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Invalid PDU Size")));
						break;
					case 5:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Invalid Continuation State")));
						break;
					case 6:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Insufficient Resources to satisfy Request")));
						break;
					default:
						FTRACE(FPrint(_L("SDP_ErrorResponse - Reserved Value 0x%04x"), errorCode));
						break;
					}
				}
			else
				{
				FTRACE(FPrint(_L("SDP_ErrorResponse sent without parameters!")));
				User::Leave(KErrGeneral); //Qualified
				}
			if(iRequestTransId != id)
				{
				User::Leave(KErrGeneral); //Qualified
				}
			break;
		case 2:
			{
			FTRACE(FPrint(_L("SDP_ServiceSearchRequest")));
			iRequestTransId2 = iRequestTransId1;
			iRequestMaxCount2 = iRequestMaxCount1;
			iRequestTransId1 = id;
			iRequestMaxCount1 = MaxCountFoundAfterServiceSearchPatternOrWhatYouWill(aString);
			if(iTestNum==1)
				{
				iRequestTransId = iRequestTransId2;
				iRequestMaxCount = iRequestMaxCount2;
				}
			else
				{
				iRequestTransId = iRequestTransId1;
				iRequestMaxCount = iRequestMaxCount1;
				}
			if(iRequestMaxCount1>=0)
				{
				FTRACE(FPrint(_L("   MaximumServiceRecordCount = 0x%04x"), iRequestMaxCount1));
				}
			else
				{
				FTRACE(FPrint(_L("   .....sent without parameters!")));
				User::Leave(KErrGeneral);
				}
			}
			break;
		case 3:
			{
			FTRACE(FPrint(_L("SDP_ServiceSearchResponse")));
			if(iRequestTransId != id)
				{
				FTRACE(FPrint(_L("Request and Response Transaction Ids DO NOT MATCH: 0x%02x != 0x%02x ... ERROR!!!!!!!!"), id, iRequestTransId));
				//break;
				}
			if(aString.Length()>=9)
				{
				TInt total = aString[6]+256*aString[5];
				TInt current = aString[8]+256*aString[7];
				FTRACE(FPrint(_L("   TotalServiceRecordCount = 0x%04x, CurrentRecordCount = 0x%04x "), total, current));
				if(iRequestTransId == id)
					{
					if(iRequestMaxCount>=total&&total>=current)
						{
						FTRACE(FPrint(_L("       So: requested MaximumServiceRecordCount 0x%04x >= TotalServiceRecordCount, 0x%04x >= CurrentRecordCount, 0x%04x "), iRequestMaxCount, total, current));
						}
					else
						{
						FTRACE(FPrint(_L("       ERROR!!!! the following is NOT true: requested MaximumServiceRecordCount 0x%04x >= TotalServiceRecordCount, 0x%04x >= CurrentRecordCount, 0x%04x "), iRequestMaxCount, total, current));
						User::Leave(KErrGeneral); //Qualified
						}
					}
				}
			else
				{
				FTRACE(FPrint(_L("   .....sent without parameters!")));
				User::Leave(KErrGeneral); //Qualified
				}
			}
			if(iRequestTransId != id)
				{
				User::Leave(KErrGeneral); //Qualified
				}
			if(iTestNum==1)
				//Prepare for next response when we'll be looking 
				//at the LAST request's values.
				{
				iRequestTransId = iRequestTransId1;
				iRequestMaxCount = iRequestMaxCount1;
				}
			break;
		case 4:
			{
			iRequestTransId2 = iRequestTransId1;
			iRequestMaxCount2 = iRequestMaxCount1;
			iRequestTransId1 = id;
			FTRACE(FPrint(_L("SDP_ServiceAttributeRequest")));
			if(aString.Length()>=11)
				{
				TInt handle = aString[8]+256*aString[7]+256*256*aString[6]+256*256*256*aString[5];
				iRequestMaxCount1 = aString[10]+256*aString[9];
				FTRACE(FPrint(_L("   Record Handle = 0x%08x, MaximumAttributByteCount = 0x%04x"), handle, iRequestMaxCount1));
				if(iTestNum==7)
					{
					iRequestTransId = iRequestTransId2;
					iRequestMaxCount = iRequestMaxCount2;
					}
				else
					{
					iRequestTransId = iRequestTransId1;
					iRequestMaxCount = iRequestMaxCount1;
					}
				}
			else
				{
				FTRACE(FPrint(_L("   ....sent without parameters!")));
				User::Leave(KErrGeneral); //Qualified
				}
			}
			break;
		case 5:
			{
			FTRACE(FPrint(_L("SDP_ServiceAttributeResponse")));
			if(iRequestTransId != id)
				{
				FTRACE(FPrint(_L("Request and Response Transaction Ids DO NOT MATCH: 0x%02x != 0x%02x ... ERROR!!!!!!!!"), id, iRequestTransId));
				//break;
				}
			if(aString.Length()>=7)
				{
				TInt count = aString[6]+256*aString[5];
				if(iRequestTransId != id)
					{
					FTRACE(FPrint(_L("   AttributeListByteCount = 0x%04x"), count));
					}
				else
					{
					if(count <= iRequestMaxCount)
						{
						FTRACE(FPrint(_L("   AttributeListsByteCount = 0x%04x (<= requested MaxiumumAttributeByteCount, 0x%04x"), count, iRequestMaxCount));
						}
					else
						{
						FTRACE(FPrint(_L("   ERROR!!!!!  AttributeListsByteCount = 0x%04x > requested MaxiumumAttributeByteCount, 0x%04x"), count, iRequestMaxCount));
						User::Leave(KErrGeneral); //Qualified
						}
					}
				}
			else
				{
				FTRACE(FPrint(_L("   ....sent without parameters!")));
				User::Leave(KErrGeneral); //Qualified
				}
			}
			if(iRequestTransId != id)
				{
				User::Leave(KErrGeneral); //Qualified
				}
			if(iTestNum==7)
				//Prepare for next response when we'll be looking 
				//at the LAST request's values.
				{
				iRequestTransId = iRequestTransId1;
				iRequestMaxCount = iRequestMaxCount1;
				}
			break;
		case 6:
			{
			FTRACE(FPrint(_L("SDP_ServiceSearchAttributeRequest")));
			FTRACE(FPrint(_L("SDP_ServiceSearchRequest")));
			iRequestTransId2 = iRequestTransId1;
			iRequestMaxCount2 = iRequestMaxCount1;
			iRequestTransId1 = id;
			iRequestMaxCount1 = MaxCountFoundAfterServiceSearchPatternOrWhatYouWill(aString);
			if(iTestNum==33)
				{
				iRequestTransId = iRequestTransId2;
				iRequestMaxCount = iRequestMaxCount2;
				}
			else
				{
				iRequestTransId = iRequestTransId1;
				iRequestMaxCount = iRequestMaxCount1;
				}
			if(iRequestMaxCount1>=0)
				{
				FTRACE(FPrint(_L("   MaximumAttributeByteCount = 0x%04x"), iRequestMaxCount1));
				}
			else
				{
				FTRACE(FPrint(_L("   .....sent without parameters!")));
				User::Leave(KErrGeneral); //Qualified
				}
			}
			break;
		case 7:
			{
			FTRACE(FPrint(_L("SDP_ServiceSearchAttributeResponse")));
			if(iRequestTransId != id)
				{
				FTRACE(FPrint(_L("Request and Response Transaction Ids DO NOT MATCH: 0x%02x != 0x%02x ... ERROR!!!!!!!!"), id, iRequestTransId));
				//break;
				}
			if(aString.Length()>=7)
				{
				TInt count = aString[6]+256*aString[5];
				if(iRequestTransId != id)
					{
					FTRACE(FPrint(_L("   AttributeListByteCount = 0x%04x"), count));
					}
				else
					{
					if(count <= iRequestMaxCount)
						{
						FTRACE(FPrint(_L("   AttributeListsByteCount = 0x%04x (<= requested MaxiumumAttributeByteCount, 0x%04x"), count, iRequestMaxCount));
						}
					else
						{
						FTRACE(FPrint(_L("   ERROR!!!!!  AttributeListsByteCount = 0x%04x > requested MaxiumumAttributeByteCount, 0x%04x"), count, iRequestMaxCount));
						User::Leave(KErrGeneral); //Qualified
						}
					}
				}
			else
				{
				FTRACE(FPrint(_L("   ....sent without parameters!")));
				User::Leave(KErrGeneral); //Qualified
				}
			}
			if(iRequestTransId != id)
				{
				User::Leave(KErrGeneral); //Qualified
				}
			if(iTestNum==33)
				//Prepare for next response when we'll be looking 
				//at the LAST request's values.
				{
				iRequestTransId = iRequestTransId1;
				iRequestMaxCount = iRequestMaxCount1;
				}
			break;
		default:
			FTRACE(FPrint(_L("Reserved PDU Id: %02x"), aString[0]));
			User::Leave(KErrGeneral); //Qualified
			break;
		}
	}

void CSdpVerificationTests::FlogReadString(TDesC8& aString)
	{
	if(!aString.Length())
		return;
	FLOG(_L(""));
	;
	FlogTestId();
	FlogParsedPDUHeader(aString);
	FLOG(_L("Raw data RECEIVED........."));
	TPtrC start(_L("    Bytes READ - "));
	FlogString(&start[0], aString);
	FLOG(_L(""));
	}

void CSdpVerificationTests::FlogContinuationString(TDesC8& aString)
	{
	FLOG(_L("NB...."));
	TPtrC start(_L("    Continuation Bytes - "));
	FlogString(&start[0], aString);
	}

void CSdpVerificationTests::FlogWriteString(TDesC8& aString)
	{
	if(!aString.Length())
		return;
	FLOG(_L(""));
	FlogTestId();
	FlogParsedPDUHeader(aString);
	FLOG(_L("Raw data SENT........."));
	TPtrC start(_L("    Bytes WRITTEN - "));
	FlogString(&start[0], aString);
	TBuf8<20> contStr;
	contStr.SetLength(0);
	contStr.Append(&iContLength, 1);
	contStr.Append(iContinuationBuffer, iContLength);
	FlogContinuationString(contStr);
	FLOG(_L(""));
	}


		
void CSdpVerificationTests::WaitForStartSignalL()
{
	// Wait for sync info to be sent
	//
}

void CSdpVerificationTests::SendStartSignalL()
{
	// Send sync info
	//
}

void CSdpVerificationTests::PromptedGetch()
{
	// For putting stops between tests so user can check results as he goes
	//
	#ifdef STOPPING_MODE
	iConsole.Printf(_L("Press any key.\r\n"));
	iConsole.Getch();
	#endif
}



// this is the nearest thing to a real database. 
// originally for UPF4, but record 0 removed above.
	
CSdpDatabase* CSdpVerificationTests::BuildDbL()
	{
	//see builddb.cpp for copy of this
	return 0;
	}


void CSdpVerificationTests::PrintDb(CSdpDatabase& aDb, CConsoleBase& aConsole)
	{
	aConsole.Printf(_L("Printing Database...\n"));

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		aConsole.Printf(_L("\n...Printing Record 0x%x\n"), (*recIter).Handle());
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(aConsole);
			(*attrIter).AcceptVisitorL(*theVisitor); //Qualified
			delete theVisitor;
			}
		}
	}


MSdpElementBuilder* CSdpVerificationTests::BuildUintL(const TDesC8& aUint)
	{
	if(aUint.Length() != 2)
	{
		iNeedAttributeId = EFalse;
	}
	if (iNeedAttributeId)
	{
		iBuildingList = 0; //reset so only sublists are counted
		iNeedAttributeId = EFalse;
		// This is the Attribute ID level (the same for SA or SAS request)
		// First check the length
		//
		if(aUint.Length() != 2)
		{
			User::Leave(KErrGeneral);
		}
		// Now check the value;
		//
		TUint16 id = BigEndian::Get16(aUint.Ptr());
		FlogTestId();
		FTRACE(FPrint(_L("   Attribute ID - %d"), id));
	}
	else 
	{
		// First, get the correct size integer
//		TUint32 uint32 = 0;
		TUint16 uint16a = 0;
		TUint16 uint16b = 0;
		TUint8 uint8 = 0;
		if(iBuildingList<=0)
			{
			iNeedAttributeId = ETrue;
			}
		if(aUint.Length() == 4)
		{
//			uint32 = BigEndian::Get32(aUint.Ptr());
			uint16a = BigEndian::Get16(aUint.Ptr());
			TUint8* ptr = CONST_CAST(TUint8 *, aUint.Ptr());
			ptr+=2;
			uint16b = BigEndian::Get16(ptr);
			FlogTestId();
			FTRACE(FPrint(_L("   Attribute Uint32 Value - 0x%04x%04x"), uint16a, uint16b));
		}
		else if(aUint.Length() == 2)
		{
			uint16a = BigEndian::Get16(aUint.Ptr());
			FlogTestId();
			FTRACE(FPrint(_L("   Attribute Uint16 Value - 0x%x"), uint16a));
		}
		else if(aUint.Length() == 1)
		{
			uint8 = aUint[0];
			FlogTestId();
			FTRACE(FPrint(_L("   Attribute Uint8 Value - 0x%x"), uint8));
		}
		else
		{
			// We don't have 32, 16 or 8 bit Uint
			User::Leave(KErrGeneral);
		}
		
		(void)(uint16a != 0); // keep compiler happy by referencing uint16a as an r-value in urel
		(void)(uint16b != 0); // keep compiler happy by referencing uint16b as an r-value in urel
		(void)(uint8 != 0); // keep compiler happy by referencing uint8 as an r-value in urel
	}
	return this;
	}


MSdpElementBuilder* CSdpVerificationTests::BuildUUIDL(const TUUID& aUUID)
	{
	
	TPtrC8 uuid = aUUID.ShortestForm();
	TUint16 bigUuid = BigEndian::Get16(uuid.Ptr());
	FlogTestId();
	FTRACE(FPrint(_L("   Attribute UUID Value - 0x%x"), bigUuid));
	if(iBuildingList<=0)
		{
		iNeedAttributeId = ETrue;
		}
	return this;	
	}


MSdpElementBuilder* CSdpVerificationTests::BuildStringL(const TDesC8& aString)
	{
	if(iBuildingList<=0)
		{
		iNeedAttributeId = ETrue;
		}
	FlogTestId();
	FTRACE(FPrint(_L("   Attribute Value String - ")));
	for(TInt i = 0; i < aString.Length(); i ++)
	{
		FTRACE(FPrint(_L("   %c"), aString[i]));
	}
	return this;
	}

MSdpElementBuilder* CSdpVerificationTests::BuildDESL()
	{
	return this;
	}

MSdpElementBuilder* CSdpVerificationTests::BuildURLL(const TDesC8& aURL)
	{
//	TInt ret = KErrNone;
	TInt i = 0;
	TBuf8<654> urlBuf;
	if(iBuildingList<=0)
		{
		iNeedAttributeId = ETrue;
		}
	FlogTestId();
	FTRACE(FPrint(_L("   Attribute Value URL - ")));
	for(i = 0; i < aURL.Length(); i ++)
	{
		FTRACE(FPrint(_L("   %c"), aURL[i]));
	}
	
	switch(iTestNum)
		{
	case 8:
	case 34:
	case 35:
	case 36:
		// This should be part of a very long URL. Append it to the buffer until continuation has finished
		// Set up a  string to compare it to.
		//
		urlBuf.Append(_L8("http://"));
		for(i = 0; i < 160; i ++)
		{
			urlBuf.Append(_L8("url/"));
		}	
		urlBuf.Append(_L8("url.doc"));
		(void) aURL.Compare(urlBuf);

		// Reset the continuation buffer
		//??iContinuationStore.SetLength(0);
		break;
	default:
		break;
		}
	return this;
	}
		
MSdpElementBuilder* CSdpVerificationTests::StartListL()
	{
	FlogTestId();
	FTRACE(FPrint(_L("   New List (DES)")));
	iBuildingList++;
	return this;
	}

MSdpElementBuilder* CSdpVerificationTests::EndListL()
	{
	FlogTestId();
	FTRACE(FPrint(_L("   End Current List")));
	iBuildingList--;
	if(iBuildingList<=0)
		iNeedAttributeId=ETrue;
	return this;
	}


MSdpElementBuilder* CSdpVerificationTests::BuildNilL()
	{
	User::Leave(KErrGeneral);
	return this;
	}











void CSdpVerificationTests::BuildBrowseDbL(CConsoleBase&  aConsole)
	{
	// First thing is to get rid of the old one
	//
	iTheDb = CSdpDatabase::NewL();
	
	TBuf8<2> attrId;
	TBuf8<4> val;
	TBuf8<2> val1;
	CSdpServRecord *theRec = CSdpServRecord::NewL();
// all attribute IDs are BIG ENDian 

// First Record is the Public browse group service. 
	attrId.FillZ(2);
	val.FillZ(4);
	val1.FillZ(2);
	// Set Attr 0 (Record handle) to 0x00010000
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x00;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to BrowseGroupDescriptor
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(0x1001))
		->EndListL();

		// Set Attr 0x005 (browse group list) to public root
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16(0x1002))) // publicBrowseRoot
		->EndListL();

	// Set Attr 0x200 (group ID) to 0x1201 (Generic networking).
	attrId[0] = 0x02;
	attrId[1] = 0x00;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16((TUint16)KGenericNetworkingUUID))) 
		->EndListL();

	
	// Add the record into the database
	iTheDb->AddRecord(theRec);

// Record 2 - a member of the Browse group 0x1201 (Generic networking)
	theRec = CSdpServRecord::NewL();

	attrId.FillZ(2);
	// Set Attr 0 (Record handle) to 0x0001001
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x01;
	theRec->BuildUintL(attrId)->BuildUintL(val);


	// Set Attr 1 (service class list) to 0x1201
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID((TUint16)KDialUpNetworkingUUID))
		->EndListL();

	// Set Attr 0x005 (browse group list) to the generic networking browse group
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16((TUint16)KGenericNetworkingUUID))) 
		->EndListL();

	
	// Add the record into the database
	iTheDb->AddRecord(theRec);

// Record 3 - a member of the Browse group 0x1201 (Generic networking)
	theRec = CSdpServRecord::NewL();

	attrId.FillZ(2);
	// Set Attr 0 (Record handle) to 0x0001002
	val[0] = 0x00;
	val[1] = 0x01;
	val[2] = 0x00;
	val[3] = 0x02;
	theRec->BuildUintL(attrId)->BuildUintL(val);

	// Set Attr 1 (service class list) to LanAccessUsingPPP UUID (0x1102)
	attrId[0] = 0x00;
	attrId[1] = 0x01;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(0x1102))
		->EndListL();

	// Set Attr 0x005 (browse group list) to the generic networking browse group
	attrId[0] = 0x00;
	attrId[1] = 0x05;
	theRec->BuildUintL(attrId)->BuildDESL()
		->StartListL()
			->BuildUUIDL(TUUID(TUint16((TUint16)KGenericNetworkingUUID))) 
		->EndListL();

	
	// Add the record into the database
	iTheDb->AddRecord(theRec);

	CSdpDatabase& db = *iTheDb;
	PrintDb(db, aConsole);
	}



//
// Browse builder implementation
//
CBrowseResponseBuilder::CBrowseResponseBuilder(TDes& aTestName, TInt aBrowseIter/*, TInt aBrowseTest, TInt aUuidSize*/, CSdpSearchPattern& aBrowseSP)

:iTestName(aTestName),iBrowseIter(aBrowseIter),/*iBrowseTest(aBrowseTest),iUuidSize(aUuidSize),*/iNeedAttributeId(ETrue),iBuildingList(0),iBrowseSP(aBrowseSP)
	{
	
	}

CBrowseResponseBuilder::~CBrowseResponseBuilder()
	{
	}

MSdpElementBuilder* CBrowseResponseBuilder::BuildUintL(const TDesC8& aUint)
	{

	if(iNeedAttributeId)
	{	
		iNeedAttributeId = EFalse;
		if(aUint.Length() != 2)
		{
			User::Leave(KErrGeneral);
		}
		// Now check the value;
		//
		TUint16 id = BigEndian::Get16(aUint.Ptr());
		FTRACE(FPrint(_L("")));
		FTRACE(FPrint(_L("%S:"), &iTestName));
		FTRACE(FPrint(_L("	Browse Iter %d: Attribute ID - %d"),iBrowseIter + 1, id));

		switch(iBrowseIter)
		{
		case 0:
		case 2:
			if(id!=KSdpAttrIdServiceClassIDList)
			{	
				User::Leave(KErrGeneral);
			}
			break;
		case 1:
			if(id!=KSdpAttrIdSdpServerGroupID)
			{	
				User::Leave(KErrGeneral);
			}
			
			break;
		default:
			User::Leave(KErrGeneral);
			break;
		}
	
	}
	else
	{
		// Get the appropriate size uint
		//
		// First, get the correct size integer
//		TUint32 uint32 = 0;
		TUint16 uint16a = 0;
		TUint16 uint16b = 0;
		TUint8 uint8 = 0;
		if(iBuildingList<=0)
			{
			iNeedAttributeId = ETrue;
			}
		if(aUint.Length() == 4)
		{
//			uint32 = BigEndian::Get32(aUint.Ptr());
			uint16a = BigEndian::Get16(aUint.Ptr());
			TUint8* ptr = CONST_CAST(TUint8 *, aUint.Ptr());
			ptr+=2;
			uint16b = BigEndian::Get16(ptr);
			FTRACE(FPrint(_L("")));
			FTRACE(FPrint(_L("%S:"), &iTestName));
			FTRACE(FPrint(_L("	Browse Iter %d: Attribute Uint32 Value - 0x%04x%04x"), iBrowseIter + 1, uint16a, uint16b));
		}
		else if(aUint.Length() == 2)
		{
			uint16a = BigEndian::Get16(aUint.Ptr());
			FTRACE(FPrint(_L("")));
			FTRACE(FPrint(_L("%S:"), &iTestName));
			FTRACE(FPrint(_L("	Browse Iter %d: Attribute Uint32 Value - 0x%x"), iBrowseIter + 1, uint16a));
		}
		else if(aUint.Length() == 1)
		{
			uint8 = aUint[0];
			FTRACE(FPrint(_L("")));
			FTRACE(FPrint(_L("%S:"), &iTestName));
			FTRACE(FPrint(_L("	Browse Iter %d: Attribute Uint32 Value - 0x%x"), iBrowseIter + 1, uint8));
		}
		else
		{
			// We don't have 32, 16 or 8 bit Uint
			User::Leave(KErrGeneral);
		}

		(void)(uint16a != 0); // keep compiler happy by referencing uint16a as an r-value in urel
		(void)(uint16b != 0); // keep compiler happy by referencing uint16b as an r-value in urel
		(void)(uint8 != 0); // keep compiler happy by referencing uint8 as an r-value in urel
	}
	return this;
	}

MSdpElementBuilder* CBrowseResponseBuilder::BuildUUIDL(const TUUID& aUUID)
	{
	// Extract the underlying integer
	//
	TPtrC8 uuid = aUUID.ShortestForm();
	TUint16 bigUuid = BigEndian::Get16(uuid.Ptr());
	FTRACE(FPrint(_L("")));
	FTRACE(FPrint(_L("%S:"), &iTestName));
	FTRACE(FPrint(_L("   Browse Iter %d: Attribute UUID Value - 0x%x"), iBrowseIter + 1, bigUuid));
	if(iBuildingList<=0)
		{
		iNeedAttributeId = ETrue;
		}


	switch(iBrowseIter)
	{
	case 0:
		// Browse group list
		if(bigUuid==0x1001)
		{
		FTRACE(FPrint(_L("")));
		FTRACE(FPrint(_L("%S:"), &iTestName));
		FTRACE(FPrint(_L("   Browse Iter %d.... Browse Group Record Found!"), iBrowseIter + 1));
		}
		break;
	case 1:
		iBrowseSP.AddL(aUUID);
		FTRACE(FPrint(_L("")));
		FTRACE(FPrint(_L("%S:"), &iTestName));
		FTRACE(FPrint(_L("   Browse Iter %d.... Browse Group ID (0x%x) found!"), iBrowseIter + 1, bigUuid));
		break;
	default:
		break;
	}
	return this;
	}

MSdpElementBuilder* CBrowseResponseBuilder::BuildDESL()
	{
	return this;
	}

MSdpElementBuilder* CBrowseResponseBuilder::StartListL()
	{
	FTRACE(FPrint(_L("")));
	FTRACE(FPrint(_L("%S:   New List (DES)...."), &iTestName));
	iBuildingList++;
	return this;
	}

MSdpElementBuilder* CBrowseResponseBuilder::EndListL()
	{
	FTRACE(FPrint(_L("%S:   End Current List"), &iTestName));
	iBuildingList--;
	if(iBuildingList<=0)
		iNeedAttributeId=ETrue;
	return this;
	}

//
// Print out builder implementation
//
CPrintBuilder::CPrintBuilder(CConsoleBase& aConsole)
: iIndent(0), iConsole(aConsole)
	{
	
	}

CPrintBuilder::~CPrintBuilder()
	{
	}


void CPrintBuilder::HexDumpVT(const TDesC8& aDes)
	{
	for (TInt i = 0; i < aDes.Length(); ++i)
		{
		if (i%8 == 0)
			iConsole.Printf(_L("0x%04x:   "), i);
		iConsole.Printf(_L("0x%02x "), aDes[i]);
		if (i%8 == 7 || i == aDes.Length() - 1)
			iConsole.Printf(_L("\n"));
		}
	}



MSdpElementBuilder* CPrintBuilder::BuildUnknown(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got Unknown: <type %d, sizedesc %d, length %d>\n"), aType, aSizeDesc, aData.Length());
	HexDumpVT(aData);
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildNilL()
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got Nil: <>\n"));
	return this;
	};

MSdpElementBuilder* CPrintBuilder::BuildUintL(const TDesC8& aLongUint)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got Long Uint: <Length %d>\n"), aLongUint.Length());
	HexDumpVT(aLongUint);
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildIntL(const TDesC8& aLongInt)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got Long Int: <Length %d>\n"), aLongInt.Length());
	HexDumpVT(aLongInt);
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildUUIDL(const TUUID& aUUID)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got UUID: <length %d>\n"), aUUID.ShortestForm().Length());
	HexDumpVT(aUUID.ShortestForm());
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildBooleanL(TBool aBool)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got Boolean: <%d>\n"), aBool);
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildStringL(const TDesC8& aString)
	{
	TBuf<512> ustr;
	ustr.Copy(aString.Left(Min(aString.Length(), ustr.MaxLength())));
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got String: <length %d, Contents %S>\n"), ustr.Length(), &ustr);
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildDESL()
	{
	return StartListL();
	};

MSdpElementBuilder* CPrintBuilder::BuildDEAL()
	{
	return StartListL();
	};

MSdpElementBuilder* CPrintBuilder::StartListL(/*prob want list type here*/)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("<START LIST>\n"));
	iIndent += 2;
	return this;
	};
MSdpElementBuilder* CPrintBuilder::EndListL()
	{
	if (iIndent > 0)
		{
		iIndent-=2;
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("<END LIST>\n"));
		}
	else
		{
		iConsole.Printf(_L("WARNING: UNMATCHED END LIST!"));
		}
	return this;
	};
MSdpElementBuilder* CPrintBuilder::BuildURLL(const TDesC8& aString)
	{
	iConsole.SetPos(iIndent);
	iConsole.Printf(_L("Got URL: \n"));
	HexDumpVT(aString);
	return this;
	};




