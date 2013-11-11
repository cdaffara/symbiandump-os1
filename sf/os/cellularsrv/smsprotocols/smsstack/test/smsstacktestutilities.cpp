// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Utility functions for Sms Stack test harnesses
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsstacktestutilitiesTraces.h"
#endif

#include "smsstacktestutilities.h"

#include <commsdattypesv1_1.h>
#include <commsdattypesv1_1_internal.h>
#include <logcli.h>
#include <e32math.h>
#include <e32test.h>
#include <f32file.h>
#include <logwraplimits.h>

#include "smsustrm.h"
#include "Gsmumsg.h"
#include "smsuaddr.H"
#include "gsmubuf.h"
#include "smspdudb.h"
#include "SmsuTimer.h"
#include "smsstacklog.h"

using namespace CommsDat;

// Check a boolean is true
#define LOCAL_CHECKPOINT(a) iTestStep->testBooleanTrue((a), (TText8*)__FILE__, __LINE__)

// Check a boolean is true if not return error code b
#define LOCAL_CHECKPOINT_CODE(a, b) iTestStep->testBooleanTrueWithErrorCode((a), (b), (TText8*)__FILE__, __LINE__)
#define LOCAL_CHECKPOINT_COMPAREL(p1, p2, p3) iTestStep->TestCheckPointCompareL((p1), (p2), (p3), (TText8*)__FILE__, __LINE__)

// TestExecute style Printf
#define PRINTF1(p1)							iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define PRINTF2(p1, p2)						iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))
#define PRINTF3(p1, p2, p3)					iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3))
#define PRINTF4(p1, p2, p3, p4)				iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4))
#define PRINTF5(p1, p2, p3, p4, p5)			iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5))
#define PRINTF6(p1, p2, p3, p4, p5, p6)		iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6))
#define PRINTF7(p1, p2, p3, p4, p5, p6, p7)	iTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

class CTSmsRegTstActive : public CActive
	{
public:
	CTSmsRegTstActive();
	~CTSmsRegTstActive();
	void StartL();
protected:
	void DoCancel();
	void RunL();
	};

CTSmsRegTstActive::CTSmsRegTstActive():
	CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CTSmsRegTstActive::~CTSmsRegTstActive()
	{
	Cancel();
	}

void CTSmsRegTstActive::DoCancel()
	{
	TRequestStatus* s=&iStatus;
	User::RequestComplete(s, KErrNone);
	}

void CTSmsRegTstActive::StartL()
	{
	SetActive();
	}

void CTSmsRegTstActive::RunL()
	{
	CActiveScheduler::Stop();
	}


EXPORT_C CSmsStackTestUtils* CSmsStackTestUtils::NewL(CTestStep* aTestStep, RFs& aFs)
	{
	CSmsStackTestUtils* self = new (ELeave) CSmsStackTestUtils(aTestStep, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSmsStackTestUtils::ConstructL()
	{
	iCharConv = CCnvCharacterSetConverter::NewL();
	}

EXPORT_C CSmsStackTestUtils::~CSmsStackTestUtils()
	{
	delete iCharConv;
	}

CSmsStackTestUtils::CSmsStackTestUtils(CTestStep* aTestStep, RFs& aFs)
: iTestStep(aTestStep), iFs(aFs)
	{
	}


EXPORT_C void CSmsStackTestUtils::OpenSmsSocketLC(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily)
/**
 *  Initialise an RSocket for SMS
 *  @note aSocket is pushed to CleanupStack.
 *  @param aSocketServer reference to the socket server object
 *  @param aSocket The socket that will be opened
 *  @param aFamily The sms address family
 */
	{
	OpenSmsSocketL(aSocketServer, aSocket, aFamily);
	CleanupClosePushL(aSocket);
	}

EXPORT_C void CSmsStackTestUtils::OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily)
/**
 *  Initialise an RSocket for SMS, aSocket is NOT pushed to CleanupStack.
 *  @param aSocketServer reference to the socket server object
 *  @param aSocket The socket that will be opened
 *  @param aFamily The sms address family
 */
	{
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(aFamily);
	OpenSmsSocketL(aSocketServer, aSocket, smsaddr);
	}

EXPORT_C void CSmsStackTestUtils::OpenSmsSocketLC(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddr& aSmsAddr)
/**
 *  Initialise an RSocket for SMS.
 *  @note aSocket is pushed to CleanupStack
 *  @param aSocketServer reference to the socket server object
 *  @param aSocket The socket that will be opened
 *  @param aSmsAddr The sms address to bind to
 */
	{
	OpenSmsSocketL(aSocketServer, aSocket, aSmsAddr);
	CleanupClosePushL(aSocket);
	}

EXPORT_C void CSmsStackTestUtils::OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddr& aSmsAddr)
/**
 *  Initialise an RSocket for SMS, aSocket is NOT pushed to CleanupStack.
 *  @param aSocketServer reference to the socket server object
 *  @param aSocket The socket that will be opened
 *  @param aSmsAddr The sms address to bind to
 */
	{
	TInt ret=aSocket.Open(aSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	PRINTF2(_L("Socket Open Return Value : %d"),ret);
	LOCAL_CHECKPOINT(ret == KErrNone);
	CleanupClosePushL(aSocket);
	ret=aSocket.Bind(aSmsAddr);
	PRINTF2(_L("Socket Bind Return Value : %d"),ret);
	LOCAL_CHECKPOINT(ret == KErrNone);
	
	TProtocolDesc desc;
	aSocket.Info(desc);
	PRINTF2(_L("Protocol name: %S"), &desc.iName);
	
    TTimeIntervalMicroSeconds32 InitPause=9000000;  //Required Pause to Allow SMSStack to Complete its Async Init
    User::After(InitPause);                         //call to the TSY and finish its StartUp.

	CleanupStack::Pop(&aSocket);
	}

EXPORT_C void CSmsStackTestUtils::SendSmsAndChangeBearerL(CSmsMessage* aSms, RSocket& aSocket, RMobileSmsMessaging::TMobileSmsBearer aBearer)
/**
 *  Stream aSms out to the socket server. The idea is to change bearer midway through
 *  sending the PDUs making up the complete message.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @param aBearer Bearer setting that is changed to CommDb
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
	{
	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << *aSms);
	LOCAL_CHECKPOINT(ret == KErrNone);
	TRAP(ret,writestream.CommitL());
	LOCAL_CHECKPOINT(ret == KErrNone);
	
    // Create comms database object
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
    CleanupStack::PushL(db);

 	//Send message and change bearer
	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	aSocket.Ioctl(KIoctlSendSmsMessage, status, &sbuf, KSolSmsProv);

	//Wait couple of seconds to ensure first pdus of message have been sent
	User::After(2500000);

    // Change bearer
    CMDBField<TUint32>* smsBearerField = new(ELeave) CMDBField<TUint32>(KCDTIdSMSBearer);
    CleanupStack::PushL(smsBearerField);
    smsBearerField->SetRecordId(1); //it's GlobalSettingsRecord
    *smsBearerField = aBearer;
    smsBearerField->ModifyL(*db);
    CleanupStack::PopAndDestroy(smsBearerField);
    CleanupStack::PopAndDestroy(db);

	User::WaitForRequest(status);
	PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"), status.Int());
	LOCAL_CHECKPOINT(status.Int() == KErrNone);

	CSmsBuffer& smsbuffer = (CSmsBuffer&)aSms->Buffer();
	TInt len = smsbuffer.Length();
	HBufC* hbuf = HBufC::NewL(len);
	TPtr ptr = hbuf->Des();
	smsbuffer.Extract(ptr,0,len);
	PRINTF2(_L("SMS contains %d characters"),len);
	delete hbuf;
	}

EXPORT_C void CSmsStackTestUtils::ChangeBearerL(RMobileSmsMessaging::TMobileSmsBearer aBearer)
/**
Change the bearer in CommDB global settings.

@param aBearer The new bearer setting.
@return none
*/
	{
    // Start a CommDB session
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(db);
    
    // Change bearer in global settings
    CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
    TRAPD(err, globalSettingsRecord.LoadL(*db));
    if (err != KErrNone)
        {
        PRINTF2(_L("Could not load global settings. Error = %d\n"), err);
        User::Leave(err);
        }
    ((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iSMSBearer = aBearer;
    globalSettingsRecord.ModifyL(*db);
    
    PRINTF2(_L("Setting bearer in global settings to %d\n"), aBearer); 
    
    CleanupStack::PopAndDestroy(db);
	}

EXPORT_C void CSmsStackTestUtils::GetBearerL(RMobileSmsMessaging::TMobileSmsBearer& aBearer)
/**
Get the bearer from CommDB global settings.

@param aBearer The bearer setting retrieved from global settings.
@return none
*/
    {
    // Start a CommDB session
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(db);
    
    // Load global settings
    CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
    TRAPD(err, globalSettingsRecord.LoadL(*db));
    if (err != KErrNone)
        {
        PRINTF2(_L("Could not load global settings. Error = %d\n"), err);
        User::Leave(err);
        }
    TInt tempBearer = ((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iSMSBearer;
    aBearer = static_cast<RMobileSmsMessaging::TMobileSmsBearer>(tempBearer);
    PRINTF2(_L("Got bearer from CommDB. Bearer = %d\n"), aBearer);
    
    CleanupStack::PopAndDestroy(db);
    }

EXPORT_C void CSmsStackTestUtils::DisableLogging()
 	{
 	CTSmsRegTstActive* testActive = new(ELeave)CTSmsRegTstActive();
 	CleanupStack::PushL(testActive);
 	testActive->StartL();	
 
 	CLogWrapper* logWrapper=CLogWrapper::NewL(iFs);
 	CleanupStack::PushL(logWrapper);
 	CLogClient& logClient = static_cast<CLogClient&>(logWrapper->Log());
 	TLogConfig config;
 	logClient.GetConfig(config, testActive->iStatus);
 	CActiveScheduler::Start();
 	LOCAL_CHECKPOINT(testActive->iStatus.Int() == KErrNone);
 
 	// Disable logging
 	config.iMaxEventAge = 0;
 	config.iMaxLogSize = 0;
 	config.iMaxRecentLogSize = 0;
 
 	testActive->StartL();
 	logClient.ChangeConfig(config, testActive->iStatus);
 	CActiveScheduler::Start();
 	LOCAL_CHECKPOINT(testActive->iStatus.Int() == KErrNone);
 	
 	CleanupStack::PopAndDestroy(2, testActive); // testActive, logWrapper
	}
 
 EXPORT_C void CSmsStackTestUtils::EnableLogging()
    {
    CTSmsRegTstActive* testActive = new(ELeave)CTSmsRegTstActive();
    CleanupStack::PushL(testActive);
    testActive->StartL();	
    
    CLogWrapper* logWrapper=CLogWrapper::NewL(iFs);
    CleanupStack::PushL(logWrapper);
    CLogClient& logClient = static_cast<CLogClient&> (logWrapper->Log());
    TLogConfig config;
    logClient.GetConfig(config, testActive->iStatus);
    CActiveScheduler::Start();
    LOCAL_CHECKPOINT(testActive->iStatus.Int() == KErrNone);
    
    // Enable logging - default values
    config.iMaxEventAge = 2592000;
    config.iMaxLogSize  = 1000; 
    config.iMaxRecentLogSize = 20; 
    
    testActive->StartL();	
    logClient.ChangeConfig(config, testActive->iStatus);
    CActiveScheduler::Start();
    LOCAL_CHECKPOINT(testActive->iStatus.Int() == KErrNone);
    
    CleanupStack::PopAndDestroy(2, testActive); // testActive, logWrapper
    }		
	
EXPORT_C void CSmsStackTestUtils::GetLogEventL(CLogEvent& aLogEvent, TInt aLogServerId)
/**
 *  Get SMS log event from event logger.
 *  @param aLogEvent reference to the CLogEvent object
 *  @param aLogServerId number of log event
 */
	{
	CTSmsRegTstActive* testActive = new(ELeave)CTSmsRegTstActive();
	CleanupStack::PushL(testActive);

	CLogWrapper* logWrapper=CLogWrapper::NewL(iFs);
	CleanupStack::PushL(logWrapper);

	aLogEvent.SetId(aLogServerId);
	testActive->StartL();
	logWrapper->Log().GetEvent(aLogEvent,testActive->iStatus);
	CActiveScheduler::Start();
	LOCAL_CHECKPOINT(testActive->iStatus.Int() == KErrNone);
	
    CleanupStack::PopAndDestroy(2, testActive); // testActive, logWrapper
	}

EXPORT_C TLogId CSmsStackTestUtils::AddLogEventL(CSmsMessage& aSmsMessage,TLogSmsPduData& aSmsPDUData)
/**
 *  Add SMS log event to the event logger.
 *  @param aSmsMessage SM mesage
 *  @param aLogSmsPduData initialised structure  with data relevant to logging
 */
	{
	CTSmsRegTstActive* testActive = new(ELeave)CTSmsRegTstActive();
	CleanupStack::PushL(testActive);

	CLogWrapper* logWrapper=CLogWrapper::NewL(iFs);
	CleanupStack::PushL(logWrapper);

	TLogId id=NULL;

	if(logWrapper->ClientAvailable())
	{
		testActive->StartL();


		CLogEvent* logevent=CLogEvent::NewL();
		CleanupStack::PushL(logevent);

		TBuf<KLogMaxDirectionLength> direction;
		User::LeaveIfError(logWrapper->Log().GetString(direction,R_LOG_DIR_OUT));


		logevent->SetDirection(direction);
		logevent->SetEventType(KLogShortMessageEventTypeUid);
		logevent->SetNumber(aSmsMessage.ToFromAddress());

		logevent->SetSubject(_L("test"));
		logevent->SetStatus(_L("status"));
		TPckg<TLogSmsPduData> packeddata(aSmsPDUData);
		logevent->SetDataL(packeddata);
		
		PRINTF2(_L("iStatus is %d"), testActive->iStatus.Int());
		logWrapper->Log().AddEvent(*logevent,testActive->iStatus);
		CActiveScheduler::Start();
		PRINTF2(_L("iStatus is %d"), testActive->iStatus.Int());
		LOCAL_CHECKPOINT(testActive->iStatus.Int() == KErrNone);
		id=logevent->Id();
		CleanupStack::PopAndDestroy(logevent);
	}

    CleanupStack::PopAndDestroy(2, testActive); // testActive, logWrapper
	return id;
	}


EXPORT_C void CSmsStackTestUtils::CopyConfigFileL(const TDesC& aFromSmsConfigFile, CSmsPduDatabase* aPduDb)
	{
	PRINTF3(_L("Copying %S to %S..."), &aFromSmsConfigFile, &KSimTsyConfigFileName);
	TBool createdDb = EFalse;

	if (aPduDb == NULL)
		{
		aPduDb = CSmsPduDatabase::NewL(iFs);
		CleanupStack::PushL(aPduDb);
		createdDb = ETrue;
		}

	aPduDb->RewriteFileL(aFromSmsConfigFile, KGmsSmsConfigFileDir, KSimTsyConfigFileName);
	PRINTF1(_L("done"));

	if (createdDb)
		CleanupStack::PopAndDestroy(aPduDb);
	}


EXPORT_C CSmsMessage* CSmsStackTestUtils::CreateSmsMessageLC(CSmsPDU::TSmsPDUType aType, const TDesC& aDestination, const TDesC& aMessage)
	{
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	buffer->InsertL(0, aMessage);

	CSmsMessage* message = CSmsMessage::NewL(iFs, aType, buffer);
	CleanupStack::PushL(message);

	message->SetToFromAddressL(aDestination);

	return message;
	}

EXPORT_C void CSmsStackTestUtils::AddSmsRxL(CTestConfig& aConfigFile, TInt aTestNumber, CSmsMessage& aMessage, const TDesC& aServiceCenter) const
	{
	_LIT8(KSmspTestUtilsSmsRx, "SmsRx");
	_LIT8(KSmspTestUtilsSmsRxFormat, "%S, %S");

	CTestConfigSection& section = GetSectionL(aConfigFile, aTestNumber);

	CArrayFixFlat<TGsmSms>* pdus = EncodeMessageLC(aMessage);

	const TInt pduCount = pdus->Count();

	for (TInt i = 0; i < pduCount; i++)
		{
		const TGsmSms& pdu = (*pdus)[i];

		HBufC8* pduBuf = ConvertToHexLC(pdu.Pdu());

		HBufC8* sc = HBufC8::NewLC(aServiceCenter.Length());
		sc->Des().Copy(aServiceCenter);

		const TInt maxLen = KSmspTestUtilsSmsRxFormat().Length() + pduBuf->Length() + sc->Length();
		HBufC8* smsRx = HBufC8::NewLC(maxLen);

		smsRx->Des().AppendFormat(KSmspTestUtilsSmsRxFormat, pduBuf, sc);

		CTestConfigItem* item = CTestConfigItem::NewLC(section, KSmspTestUtilsSmsRx, *smsRx);

		User::LeaveIfError(section.Items().Append(item));

		CleanupStack::Pop(item);
		CleanupStack::PopAndDestroy(smsRx);
		CleanupStack::PopAndDestroy(sc);
		CleanupStack::PopAndDestroy(pduBuf);
		}

	CleanupStack::PopAndDestroy(pdus);
	}

EXPORT_C CTestConfigSection& CSmsStackTestUtils::GetSectionL(CTestConfig& aConfigFile, TInt aTestNumber) const
	{
	_LIT8(KSmspTestUtilsTest, "test%d");

	TBuf8<16> test;
	test.AppendFormat(KSmspTestUtilsTest, aTestNumber);
	CTestConfigSection* section = aConfigFile.Section(test);

	if (section == NULL)
		{
		User::Leave(KErrNotFound);
		}

	return *section;
	}

EXPORT_C void CSmsStackTestUtils::AddSmsTxL(CTestConfig& aConfigFile, TInt aTestNumber, CSmsMessage& aMessage, const TDesC& aServiceCenter, TInt aMsgRef, CSmsMessage* aSubmitReport, TInt aExpectedError) const
	{
	_LIT8(KSmspTestUtilsSmsTx, "SmsTx");
	_LIT8(KSmspTestUtilsSmsTxFormat, "%S, %S, %d, %S, %d");

	CTestConfigSection& section = GetSectionL(aConfigFile, aTestNumber);
	CArrayFixFlat<TGsmSms>* pdus = EncodeMessageLC(aMessage);

	HBufC8* submitReport = NULL;

	if (aSubmitReport == NULL)
		{
		submitReport = KNullDesC8().AllocLC();
		}
	else
		{
		CArrayFixFlat<TGsmSms>* srPdus = EncodeMessageLC(*aSubmitReport);
		const TGsmSms& pdu = (*srPdus)[0];
		submitReport = ConvertToHexLC(pdu.Pdu());
		CleanupStack::Pop(submitReport);
		CleanupStack::PopAndDestroy(srPdus);
		CleanupStack::PushL(submitReport);
		}

	const TInt pduCount = pdus->Count();

	for (TInt i = 0; i < pduCount; i++)
		{
		const TGsmSms& pdu = (*pdus)[i];
		HBufC8* pduBuf = ConvertToHexLC(pdu.Pdu());

		HBufC8* sc = HBufC8::NewLC(aServiceCenter.Length());
		sc->Des().Copy(aServiceCenter);

		const TInt maxLen = KSmspTestUtilsSmsTxFormat().Length() + pduBuf->Length() + sc->Length() + submitReport->Length();
		HBufC8* smsTx = HBufC8::NewLC(maxLen);

		smsTx->Des().AppendFormat(KSmspTestUtilsSmsTxFormat, pduBuf, sc, aMsgRef, submitReport, aExpectedError);

		CTestConfigItem* item = CTestConfigItem::NewLC(section, KSmspTestUtilsSmsTx, *smsTx);

		User::LeaveIfError(section.Items().Append(item));
			
		CleanupStack::Pop(item);
        CleanupStack::PopAndDestroy(3, pduBuf); // smsTx, sc, pduBuf
		}

	CleanupStack::PopAndDestroy(2, pdus); // pdus, submitReport
	}

EXPORT_C HBufC8* CSmsStackTestUtils::ConvertToHexLC(const TDesC8& aDes) const
	{
	const TInt len = aDes.Length();

	HBufC8* hBuf = HBufC8::NewLC(len*2);
	TPtr8 value(hBuf->Des());

	for (TInt i = 0; i < len; i++)
		{
		value.AppendFormat(_L8("%02X"), aDes[i]);
		}
	return hBuf;
	}

CArrayFixFlat<TGsmSms>* CSmsStackTestUtils::EncodeMessageLC(CSmsMessage& aMessage) const
	{
	CArrayFixFlat<TGsmSms>* array = new (ELeave) CArrayFixFlat<TGsmSms>(1);
	CleanupStack::PushL(array);
	aMessage.EncodeMessagePDUsL(*array);
	return array;
	}

EXPORT_C TSmsFirstOctet CSmsStackTestUtils::FirstDeliverOctet(TBool aMMS, TBool aRP, TBool aUDHI, TBool aSRI)
	{
	TSmsFirstOctet firstOctet(0);

	if (!aMMS)
		firstOctet = firstOctet | TSmsFirstOctet::ESmsNoMoreMessagesToSend;

	if (aRP)
		firstOctet = firstOctet | TSmsFirstOctet::ESmsReplyPathExists;

	if (aUDHI)
		firstOctet = firstOctet | TSmsFirstOctet::ESmsUDHIHeaderPresent;

	if (aSRI)
		firstOctet = firstOctet | TSmsFirstOctet::ESmsStatusReportRequested;

	return firstOctet;
	}

EXPORT_C void CSmsStackTestUtils::DeliverWithoutUserDataL(TGsmSms& aPdu, TSmsFirstOctet aFO, const TGsmSmsTelNumber& aOA, TSmsProtocolIdentifier aPID, TSmsDataCodingScheme aDCS, const TTime& aSCTS, TInt aUTCOffset)
	{
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	pdu.SetLength(pdu.MaxLength());
	TUint8* ptr0 = &pdu[0];
	TUint8* ptr1 = ptr0;

	//Prepare the OA
	CSmsAddress* addr = CSmsAddress::NewL(*iCharConv, iFs);
	CleanupStack::PushL(addr);
	addr->SetParsedAddressL(aOA);

	//Prepare the service center time stamp
	TSmsServiceCenterTimeStamp time;
	time.SetTime(aSCTS);
	time.SetTimeOffset(aUTCOffset);

	//Encode the PDU. Unfortunately this relies on GSMU.
	ptr1 = aFO.EncodeL(ptr1);
	ptr1 = addr->EncodeL(ptr1);
	ptr1 = aPID.EncodeL(ptr1);
	ptr1 = aDCS.EncodeL(ptr1);
	ptr1 = time.EncodeL(ptr1);
	pdu.SetLength(ptr1 - ptr0);
	aPdu.SetPdu(pdu);

	CleanupStack::PopAndDestroy(addr);
	}

/**
 *  This is a unit test for class TSmsServiceCenterTimeStamp.  It is added to this utility
 *  class because it has access to TSmsServiceCenterTimeStamp.  TSmsServiceCenterTimeStamp is not exported
 *  out of the gsmsu.dll.  This is why this project compiles with gsmsuelem.cpp.
 */
EXPORT_C void CSmsStackTestUtils::CreateTSmsServiceCenterTimeStampL()
	{
	//Prepare the service center time stamp
	TSmsServiceCenterTimeStamp serviceCenterTime;
	TTime utcTime;
	TInt offset=0;
	utcTime.UniversalTime();

	//	Set and Get
	serviceCenterTime.SetTime(utcTime);
	serviceCenterTime.SetTimeOffset(offset);

	TTime testTimeValue=serviceCenterTime.Time();
	TInt testOffsetValue=serviceCenterTime.TimeOffset();

	LOCAL_CHECKPOINT(testTimeValue==utcTime);
	LOCAL_CHECKPOINT(testOffsetValue==offset);

	//boundary value checks
	//Offset is stored in number of quarter hours.
	//Max/Min value = +/- 79
	//This is the maximum value specified in the spec 3GPP TS23.040
	//The biggest value allowed by the production code is KSmsMaxTimeZoneNumQuarterHours=79
	//This is the maximum value that can be stored in the PDU when encoded.  Actual max byte
	//storage space available.  The test will check that this maximum will be allowable.
	//Values beyond 79 will cause the thread to panic
	TInt positiveValue=79;
	TInt negativeValue=-79;

	serviceCenterTime.SetTimeOffset(positiveValue);
	testOffsetValue=serviceCenterTime.TimeOffset();
	LOCAL_CHECKPOINT(testOffsetValue==positiveValue);

	serviceCenterTime.SetTimeOffset(negativeValue);
	testOffsetValue=serviceCenterTime.TimeOffset();
	LOCAL_CHECKPOINT(testOffsetValue==negativeValue);

	//Try past the end.  This panics the test case.  It has been done as a check to test
	// a panic will be raised.
/*	TInt overFlowValue=80;
	serviceCenterTime.SetTimeOffset(overFlowValue);

	TInt underFlowValue=-99;
	serviceCenterTime.SetTimeOffset(underFlowValue);
*/
	}

EXPORT_C void CSmsStackTestUtils::DecodeTSmsServiceCenterTimeStampL()
	{
	//Decode timestamp
	TTimeIntervalSeconds oneSecond(1);
	const TInt KTimeStampSize=7;
	TInt ret=0;
	TBuf8<KTimeStampSize> encodedPdu;
	encodedPdu.SetLength(encodedPdu.MaxLength());

	//---------------------------------------------------
	//Decode a valid service center time stamp
	//Expected UTC = 29/02/2004, 15h30m0s, offset=3hrs
	//Local Time   = 29/02/2004, 18h30m0s
	TDateTime utcDateTime(2004,EFebruary,28,15,30,0,0);
	TTime expectedDecodedTime(utcDateTime);

	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x20;		//month=Feb
	encodedPdu[2]=0x92;		//day=29th
	encodedPdu[3]=0x81;		//hour=18h
	encodedPdu[4]=0x03;		//minute=30min
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0x21;		//timeZone=12x15min=3hrs

	//Decode the service center time stamp.  This must equal the expected
	TGsmuLex8 pduTime(encodedPdu);
	TSmsServiceCenterTimeStamp serviceCenterTimeStamp;
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	TTime time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	TInt offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==12);

	//---------------------------------------------------
	//Decode a negative offset
	//Expected UTC = 29/02/2004, 15h30m0s, offset=-5hrs
	//Local Time   = 29/02/2004, 10h30m0s

	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x20;		//month=Feb
	encodedPdu[2]=0x92;		//day=29
	encodedPdu[3]=0x01;		//hour=10
	encodedPdu[4]=0x03;		//minute=30
	encodedPdu[5]=0x00;		//second
	encodedPdu[6]=0x0A;		//timeZone=-5hrs=15minx20, 4th bit is 1 for negative


	//Decode the service center time stamp.  This must equal the expected
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==-20);

	//---------------------------------------------------
	//Decode time stamp over new year
	//
	//Expected UTC = 01/01/2005, 00h15m0s, offset=-1h15m
	//Local Time   = 31/12/2004, 23h00m0s
	utcDateTime=TDateTime(2005,EJanuary,0,0,15,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x21;		//month=December=12
	encodedPdu[2]=0x13;		//day=31
	encodedPdu[3]=0x32;		//hour=23hrs
	encodedPdu[4]=0x00;		//minute=0
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0x58;		//timeZone is -5.
								//	-5x15min=-1hr15min

	//Decode the service center time stamp.  This must equal the expected
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==-5);

	//---------------------------------------------------
	//Decode over leap year

	//Expected UTC = 29/02/2004, 21h00m0s, offset=+5h 30min 0s
	//Local Time   = 01/03/2004, 02h30m0s
	utcDateTime=TDateTime(2004,EFebruary,28,21,0,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	//	local time is 01/03/2004   2hrs 30
	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x30;		//month=March
	encodedPdu[2]=0x10;		//day=1st
	encodedPdu[3]=0x20;		//hour=2hrs
	encodedPdu[4]=0x03;		//minute=30min
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0x22;		//timeZone is 5hrs30min.
							//	22x15min=5hr30min

	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==22);

	//---------------------------------------------------
	//DecodeL a corrupt time stamp
	encodedPdu[0]=0x40;		//year
	encodedPdu[1]=0x02;		//month = 20
	encodedPdu[2]=0x96;		//day = 69
	encodedPdu[3]=0x51;		//hour
	encodedPdu[4]=0x03;		//minute
	encodedPdu[5]=0;			//second
	encodedPdu[6]=0x20;		//timezone=12

	//Decode should fail trying to recreate the time.
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrGeneral);

	//---------------------------------------------------
	//DecodeL a zero offset

	//Expected UTC = 15/06/2004, 14h20, offset = 0
	//Local Time   = 15/06/2004, 14h20
	utcDateTime=TDateTime(2004,EJune,14,14,20,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x60;		//month=06
	encodedPdu[2]=0x51;		//day=15
	encodedPdu[3]=0x41;		//hour=14
	encodedPdu[4]=0x02;		//minute=20
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0x00;		//timezone=0

	//Decode should bring back the same date.  ie. local time==utc time
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==0);

	//---------------------------------------------------
	//DecodeL a maximum possitive offset

	//Expected UTC = 15/06/2004, 01h20, offset = 19h45min
	//Local Time   = 15/06/2004, 21h05
	utcDateTime=TDateTime(2004,EJune,14,01,20,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x60;		//month=06
	encodedPdu[2]=0x51;		//day=15
	encodedPdu[3]=0x12;		//hour=21
	encodedPdu[4]=0x50;		//minute=05
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0x97;		//timezone=79x15min=19h45
							//offset = 1185 minutes = 19h45min

	//Decode should bring back the same date.  ie. local time==utc time
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==79);

	//---------------------------------------------------
	//DecodeL a maximum negative offset

	//Expected UTC = 15/06/2004, 23h45, offset = -19h45min
	//Local Time   = 15/06/2004, 04h00
	utcDateTime=TDateTime(2004,EJune,14,23,45,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x60;		//month=06
	encodedPdu[2]=0x51;		//day=15
	encodedPdu[3]=0x40;		//hour=4
	encodedPdu[4]=0x00;		//minute=0
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0x9F;		//timezone=-79x15min=-19h45
							//offset = -1185 minutes = 19h45min

	//Decode should bring back the same date.  ie. local time==utc time
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);

	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==-79);
	
	//---------------------------------------------------
	//DecodeL a timestamp with an invalid offset 0xFF.

	//Expected UTC = 15/06/2004, 14h20, offset = 0
	//Local Time   = 15/06/2004, 14h20
	utcDateTime=TDateTime(2004,EJune,14,14,20,0,0);
	expectedDecodedTime=TTime(utcDateTime);
	
	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x60;		//month=06
	encodedPdu[2]=0x51;		//day=15
	encodedPdu[3]=0x41;		//hour=14
	encodedPdu[4]=0x02;		//minute=20
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0xFF;		//timezone off set is invalid, so set to zero
	
	//Decode should bring back the same date.  ie. local time==utc time
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);
	
	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==0);
	
	//---------------------------------------------------
	//DecodeL a timestamp with an invalid offset 0xAF

	//Expected UTC = 15/06/2004, 14h20, offset = 0
	//Local Time   = 15/06/2004, 14h20
	utcDateTime=TDateTime(2004,EJune,14,14,20,0,0);
	expectedDecodedTime=TTime(utcDateTime);
	
	encodedPdu[0]=0x40;		//year=2004
	encodedPdu[1]=0x60;		//month=06
	encodedPdu[2]=0x51;		//day=15
	encodedPdu[3]=0x41;		//hour=14
	encodedPdu[4]=0x02;		//minute=20
	encodedPdu[5]=0x00;		//second=0
	encodedPdu[6]=0xAF;		//timezone off set is invalid, so set to zero
	
	//Decode should bring back the same date.  ie. local time==utc time
	pduTime=TGsmuLex8(encodedPdu);
	serviceCenterTimeStamp.DecodeL(pduTime,ret);
	LOCAL_CHECKPOINT(ret==KErrNone);

	//Compare the times
	time=serviceCenterTimeStamp.Time();
	LOCAL_CHECKPOINT(time==expectedDecodedTime);
	
	//Check the offset
	offset=serviceCenterTimeStamp.TimeOffset();
	LOCAL_CHECKPOINT(offset==0);
	}


EXPORT_C void CSmsStackTestUtils::EncodeTSmsServiceCenterTimeStampL()
	{
	//Encode timestamp
	const TInt KServiceCenterBuffSizeInPDU=7;			//Buffer is a 14 semi-octet buffer, 7 byte
	TBuf8<KServiceCenterBuffSizeInPDU> encodedPdu;
	encodedPdu.SetLength(encodedPdu.MaxLength());
	TUint8* ptr = &encodedPdu[0];

	//---------------------------------------------------
	//Expected UTC = 03/03/2005, 12h00, offset = 3hrs
	//Local Time   = 03/03/2005, 15h00
	TMonth month=EMarch;
	TDateTime utcDateTime(2005,month,2,12,0,0,0);
	TTime expectedDecodedTime(utcDateTime);

	//Prepare the service center time stamp
	TSmsServiceCenterTimeStamp time;
	time.SetTime(expectedDecodedTime);

	//Set offset=+12, +3hrs
	time.SetTimeOffset(12);

	//Encode the PDU.
	time.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	TUint8 semioctet = encodedPdu[0];	//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = encodedPdu[1];			//March
	LOCAL_CHECKPOINT(semioctet==0x30);
	semioctet = encodedPdu[2];			//3rd
	LOCAL_CHECKPOINT(semioctet==0x30);
	semioctet = encodedPdu[3];			//15h
	LOCAL_CHECKPOINT(semioctet==0x51);
	semioctet = encodedPdu[4];			//min
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[5];			//sec
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x21);


	//---------------------------------------------------
	//Expected UTC = 01/03/2005, 01h00, offset = -3hrs
	//Local Time   = 28/02/2005, 22h00
	encodedPdu.Delete(0,encodedPdu.MaxLength());
	encodedPdu.SetLength(encodedPdu.MaxLength());
	ptr = &encodedPdu[0];

	utcDateTime=TDateTime(2005,month,0,1,0,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	//Prepare the service center time stamp
	time.SetTime(expectedDecodedTime);

	//Set offset=-12, -3hrs
	time.SetTimeOffset(-12);

	//Encode the PDU.
	time.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	semioctet = encodedPdu[0];			//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = encodedPdu[1];			//Feb
	LOCAL_CHECKPOINT(semioctet==0x20);
	semioctet = encodedPdu[2];			//28th
	LOCAL_CHECKPOINT(semioctet==0x82);
	semioctet = encodedPdu[3];			//22h
	LOCAL_CHECKPOINT(semioctet==0x22);
	semioctet = encodedPdu[4];			//min
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[5];			//sec
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x29);


	//---------------------------------------------------
	//Expected UTC = 31/12/2005, 23h45min55s, offset = +3h15min
	//Local Time   = 01/01/2006, 03h00min55s
	encodedPdu.Delete(0,encodedPdu.MaxLength());
	encodedPdu.SetLength(encodedPdu.MaxLength());
	ptr = &encodedPdu[0];
	month=EDecember;
	utcDateTime=TDateTime(2005,month,30,23,45,55,0);
	expectedDecodedTime=TTime(utcDateTime);

	//Prepare the service center time stamp
	time.SetTime(expectedDecodedTime);

	//Set offset=13, 3hrs15min
	time.SetTimeOffset(13);

	//Encode the PDU.
	time.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	semioctet = encodedPdu[0];			//2006
	LOCAL_CHECKPOINT(semioctet==0x60);
	semioctet = encodedPdu[1];			//Jan
	LOCAL_CHECKPOINT(semioctet==0x10);
	semioctet = encodedPdu[2];			//1st
	LOCAL_CHECKPOINT(semioctet==0x10);
	semioctet = encodedPdu[3];			//3h
	LOCAL_CHECKPOINT(semioctet==0x30);
	semioctet = encodedPdu[4];			//00min
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[5];			//55sec
	LOCAL_CHECKPOINT(semioctet==0x55);
	semioctet = encodedPdu[6];			//offset = 13 x 15min bocks
	LOCAL_CHECKPOINT(semioctet==0x31);

	//---------------------------------------------------
	//Check zero offset

	//Expected UTC = 12/07/2005, 01h00, offset = 0hrs
	//Local Time   = 12/07/2005, 01h00
	encodedPdu.Delete(0,encodedPdu.MaxLength());
	encodedPdu.SetLength(encodedPdu.MaxLength());
	ptr = &encodedPdu[0];

	month=EJuly;
	utcDateTime=TDateTime(2005,month,11,1,0,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	//Prepare the service center time stamp
	time.SetTime(expectedDecodedTime);

	//Set offset=0
	time.SetTimeOffset(0);

	//Encode the PDU.
	time.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	semioctet = encodedPdu[0];			//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = encodedPdu[1];			//July
	LOCAL_CHECKPOINT(semioctet==0x70);
	semioctet = encodedPdu[2];			//12th
	LOCAL_CHECKPOINT(semioctet==0x21);
	semioctet = encodedPdu[3];			//01h
	LOCAL_CHECKPOINT(semioctet==0x10);
	semioctet = encodedPdu[4];			//min
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[5];			//sec
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x00);

	//---------------------------------------------------
	//Check boundary value, maximum positive

	//Expected UTC = 12/07/2005, 01h00, offset = 19h45min
	//Local Time   = 12/07/2005, 20h45
	encodedPdu.Delete(0,encodedPdu.MaxLength());
	encodedPdu.SetLength(encodedPdu.MaxLength());
	ptr = &encodedPdu[0];

	month=EJuly;
	utcDateTime=TDateTime(2005,month,11,1,0,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	//Prepare the service center time stamp
	time.SetTime(expectedDecodedTime);

	//Set offset=0
	time.SetTimeOffset(79);

	//Encode the PDU.
	time.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	semioctet = encodedPdu[0];			//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = encodedPdu[1];			//July
	LOCAL_CHECKPOINT(semioctet==0x70);
	semioctet = encodedPdu[2];			//12th
	LOCAL_CHECKPOINT(semioctet==0x21);
	semioctet = encodedPdu[3];			//20h
	LOCAL_CHECKPOINT(semioctet==0x02);
	semioctet = encodedPdu[4];			//45min
	LOCAL_CHECKPOINT(semioctet==0x54);
	semioctet = encodedPdu[5];			//sec
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x97);


	//---------------------------------------------------
	//Check boundary value, maximum positive

	//Expected UTC = 12/07/2005, 01h00, offset = -19h45min
	//Local Time   = 11/07/2005, 05h15
	encodedPdu.Delete(0,encodedPdu.MaxLength());
	encodedPdu.SetLength(encodedPdu.MaxLength());
	ptr = &encodedPdu[0];

	month=EJuly;
	utcDateTime=TDateTime(2005,month,11,1,0,0,0);
	expectedDecodedTime=TTime(utcDateTime);

	//Prepare the service center time stamp
	time.SetTime(expectedDecodedTime);

	//Set offset=0
	time.SetTimeOffset(-79);

	//Encode the PDU.
	time.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	semioctet = encodedPdu[0];			//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = encodedPdu[1];			//July
	LOCAL_CHECKPOINT(semioctet==0x70);
	semioctet = encodedPdu[2];			//11th
	LOCAL_CHECKPOINT(semioctet==0x11);
	semioctet = encodedPdu[3];			//05h
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = encodedPdu[4];			//15min
	LOCAL_CHECKPOINT(semioctet==0x51);
	semioctet = encodedPdu[5];			//sec
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = encodedPdu[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x9F);
	}

/**
 *  This is a unit test for class TSmsValidityPeriod.
 *  TSmsValidityPeriod is not exported out of the gsmsu.dll.
 *  We build gsmuelem.cpp to get access.
 */
EXPORT_C void CSmsStackTestUtils::CreateTSmsValidityPeriodL()
	{
	//Get the current UTC offset
	TTime currUTCTime;
	currUTCTime.UniversalTime();
	TTimeIntervalSeconds currUTCOffset=User::UTCOffset();

	//---------------------------------------------------
	//Time()
	//setup expected time for this validity period
	//UTC = 2005/03/31 10hr45min55sec, offset=135sec
	//Validity period = 3days
	TMonth month=EMarch;
	TDateTime utcDateTime(2005,month,30,10,45,55,0);
	TTime utcTime(utcDateTime);
	TInt offset=135;	// 135 seconds
	TTimeIntervalSeconds utcOffset(offset);
	User::SetUTCTimeAndOffset(utcTime,utcOffset);

	//---------------------------------------------------
	//Create
	//Format is set to an absolute format
	TSmsFirstOctet validityPeriodFormat=TSmsFirstOctet::ESmsVPFSemiOctet;
	TSmsValidityPeriod validityPeriod(validityPeriodFormat);

	//---------------------------------------------------
	//Set the validity period
	TInt expectedIntervalPeriod(3*24*60+15); // 3days15min=4320 minutes
	TTimeIntervalMinutes validityTimeInterval(expectedIntervalPeriod);
	validityPeriod.SetTimeIntervalMinutes(validityTimeInterval);
	TTimeIntervalMinutes actualInterval=validityPeriod.TimeIntervalMinutes();
	LOCAL_CHECKPOINT(expectedIntervalPeriod==actualInterval.Int());

	//update utc, validity period is unchanged.
	TDateTime nextDateTime(2005,month,30,12,45,55,0);
	TTime nextTime(nextDateTime);
	User::SetUTCTime(nextTime);

	TTime actualValidityTime=validityPeriod.Time();
	LOCAL_CHECKPOINT(actualValidityTime!=nextTime);

	User::SetUTCTimeAndOffset(currUTCTime,currUTCOffset);
	}

/**
 *  Test ability to encode a TSmsValidityPeriod inside a validity period inside a pdu
 */
EXPORT_C void CSmsStackTestUtils::EncodeTSmsValidityPeriodL()
	{
	//Get the current UTC offset
	TTime currUTCTime;
	currUTCTime.UniversalTime();
	TTimeIntervalSeconds currUTCOffset=User::UTCOffset();

	//Encode
	const TInt KServiceCenterBuffSizeInPDU=7;			//Buffer is a 14 semi-octet buffer, 7 byte
	TBuf8<KServiceCenterBuffSizeInPDU> serviceCenterBuffer;
	serviceCenterBuffer.SetLength(serviceCenterBuffer.MaxLength());
	TUint8* ptr = &serviceCenterBuffer[0];

	//---------------------------------------------------
	//Set the UTC time
	//UTC 					= 03/03/2005, 12h00min30sec, offset = +3hrs = 10800 seconds
	//Validity = 2hrs
	//Expected local time 	= 03/03/2005, 17h00min30sec
	TTimeIntervalSeconds utcOffset(10800);
	TMonth month=EMarch;
	TDateTime utcDateTime(2005,month,2,12,0,30,0);
	TTime utcTime(utcDateTime);
	User::SetUTCTimeAndOffset(utcTime,utcOffset);

	//Create the validity period
	TSmsFirstOctet validityPeriodFormat=TSmsFirstOctet::ESmsVPFSemiOctet;
	TSmsValidityPeriod validityPeriod(validityPeriodFormat);

	//Set the validity period to 2hrs, = 120 minutes
	TInt expectedIntervalPeriod=120;
	TTimeIntervalMinutes validityTimeInterval(expectedIntervalPeriod);
	validityPeriod.SetTimeIntervalMinutes(validityTimeInterval);

	//Encode the PDU.
	validityPeriod.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	TUint8 semioctet = serviceCenterBuffer[0];	//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = serviceCenterBuffer[1];			//March
	LOCAL_CHECKPOINT(semioctet==0x30);
	semioctet = serviceCenterBuffer[2];			//3rd
	LOCAL_CHECKPOINT(semioctet==0x30);
	semioctet = serviceCenterBuffer[3];			//17h
	LOCAL_CHECKPOINT(semioctet==0x71);
	semioctet = serviceCenterBuffer[4];			//0min
	LOCAL_CHECKPOINT(semioctet==0x00);
	semioctet = serviceCenterBuffer[5];			//30sec
	LOCAL_CHECKPOINT(semioctet>=2 && semioctet<=4);	// 1 second window each side of 3
	semioctet = serviceCenterBuffer[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x21);			//3hrs=15minx12

	//Try a negative offset
	//UTC 					= 03/03/2005, 12h00min30sec, offset = -6hrs = -21600 seconds
	//Validity = 5days
	//Expected local time 	= 08/03/2005, 06h00min30sec

	utcOffset=TTimeIntervalSeconds(-21600);		// -6hrs validity
	month=EMarch;
	utcDateTime=TDateTime(2005,month,2,12,0,30,0);
	utcTime=TTime(utcDateTime);
	User::SetUTCTimeAndOffset(utcTime,utcOffset);

	//Create the validity period
	//validityPeriodFormat=TSmsFirstOctet::ESmsVPFSemiOctet;
	//validityPeriod=TSmsValidityPeriod(validityPeriodFormat);

	//Set the validity period to 5days, = 5*24*60=7200 minutes
	expectedIntervalPeriod=7200;
	validityTimeInterval=TTimeIntervalMinutes(expectedIntervalPeriod);
	validityPeriod.SetTimeIntervalMinutes(validityTimeInterval);

	//Encode the PDU
	validityPeriod.EncodeL(ptr);

	//	This is the expected structure in bytes for the time above
	semioctet = serviceCenterBuffer[0];	//2005
	LOCAL_CHECKPOINT(semioctet==0x50);
	semioctet = serviceCenterBuffer[1];			//March
	LOCAL_CHECKPOINT(semioctet==0x30);
	semioctet = serviceCenterBuffer[2];			//3rd
	LOCAL_CHECKPOINT(semioctet==0x80);
	semioctet = serviceCenterBuffer[3];			//6h
	LOCAL_CHECKPOINT(semioctet==0x60);
	semioctet = serviceCenterBuffer[4];			//0min
	LOCAL_CHECKPOINT(semioctet==0);
	semioctet = serviceCenterBuffer[5];			//30sec
	LOCAL_CHECKPOINT(semioctet>=2 && semioctet<=4);	// 1 second window each side of 3
	semioctet = serviceCenterBuffer[6];			//offset
	LOCAL_CHECKPOINT(semioctet==0x4A);		//-6hrs = -6*60=360 minutes 15x24 minutes

	User::SetUTCTimeAndOffset(currUTCTime,currUTCOffset);
	}

/**
 *  Test ability to decode a validity period inside a pdu into a TSmsValidityPeriod.
 */
EXPORT_C void CSmsStackTestUtils::DecodeTSmsValidityPeriodL()
	{
	//Get the current UTC offset
	TTime currUTCTime;
	currUTCTime.UniversalTime();
	TTimeIntervalSeconds currUTCOffset=User::UTCOffset();

	//Decode timestamp
	const TInt KTimeStampSize=7;
	//TInt ret=0;
	TBuf8<KTimeStampSize> timeStamp;
	timeStamp.SetLength(timeStamp.MaxLength());

	//---------------------------------------------------
	//	In integer values - local time
	timeStamp[0]=64;		//year=2004
	timeStamp[1]=32;		//month=Feb
	timeStamp[2]=146;		//day=29th
	timeStamp[3]=81;		//hour=15h
	timeStamp[4]=3;			//minute=30min
	timeStamp[5]=0;			//second=0
	timeStamp[6]=33;		//timeZone=12x15min=3hrs

	//UTC time is 29 January 2004, 12.30
	//This should give a TTimeIntervalMinutes of 31 Days = 44640 minutes
	TMonth month=EJanuary;
	TDateTime utcDateTime(2004,month,28,12,30,0,0);
	//Setup UTC time and offset
	TTime utcTime(utcDateTime);
	//+3hrs offset = 10800 seconds
	TTimeIntervalSeconds utcOffset(10800);
	User::SetUTCTimeAndOffset(utcTime,utcOffset);

	//Create buffer
	TGsmuLex8 timeStampPosOffsetBuffer(timeStamp);
	//Create validity period
	TSmsFirstOctet validityPeriodFormat=TSmsFirstOctet::ESmsVPFSemiOctet;
	TSmsValidityPeriod validityPeriod(validityPeriodFormat);

	//Decode
	validityPeriod.DecodeL(timeStampPosOffsetBuffer);

	//Now I have a TValidityPeriod
	//Check validity period format
	//ValidityPeriodFormat
	LOCAL_CHECKPOINT(validityPeriod.ValidityPeriodFormat()==TSmsFirstOctet::ESmsVPFSemiOctet);

	//Check the expected Interval
	TTimeIntervalMinutes expectedInterval(44640);
	//Give or take a minute
	TTimeIntervalMinutes actualInterval=validityPeriod.TimeIntervalMinutes();
	LOCAL_CHECKPOINT(actualInterval.Int()==expectedInterval.Int()+1||actualInterval.Int()==expectedInterval.Int()-1||actualInterval.Int()==expectedInterval.Int());
	User::SetUTCTimeAndOffset(currUTCTime,currUTCOffset);
	}


EXPORT_C void CSmsStackTestUtils::AppendUserDataL(TGsmSms& aPdu, const CSmsUserData& aUD, TInt aUDL /* = KErrNotFound */)
/**
 *  Appends an encoded CSmsUserData to the PDU
 *  
 *  @pre aPdu contains an appropriate Data Coding Scheme
 *  @param aPdu Partially constructed TGsmSms, lacking the user data
 *  @param aUD The User Data to append to aPdu
 *  @param aUDL OPTIONAL Value to set the TP-UDL in the PDU. Only used if >= 0
 */
	{
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu(aPdu.Pdu());
	const TInt origLen = pdu.Length();
	pdu.SetLength(pdu.MaxLength());
	TUint8* ptr0 = &pdu[origLen];
	TUint8* ptr1 = ptr0;

	ptr1 = aUD.EncodeL(ptr1);

	if (aUDL >= 0 && aUDL <= 0x99)
		{
		*ptr0 = static_cast<TInt8>(aUDL);
		}

	pdu.SetLength((ptr1 - ptr0) + origLen);
	aPdu.SetPdu(pdu);
	}

EXPORT_C void CSmsStackTestUtils::CompareDCSL(const CSmsPDU& aPdu, TSmsDataCodingScheme aDCS)
/**
 *  TODO
 */
	{
	LOCAL_CHECKPOINT(aPdu.DataCodingSchemePresent());
	LOCAL_CHECKPOINT(aPdu.Alphabet() == aDCS.Alphabet());
	LOCAL_CHECKPOINT(aPdu.Bits7To4() == aDCS.Bits7To4());

	TSmsDataCodingScheme::TSmsClass class1, class2;
	const TBool hasClass1 = aPdu.Class(class1);
	const TBool hasClass2 = aPdu.Class(class2);

	LOCAL_CHECKPOINT((hasClass1 && hasClass2) || (!hasClass1 && !hasClass2));

	if (hasClass1)
		{
		LOCAL_CHECKPOINT(class1 == class2);
		}
	}

EXPORT_C void CSmsStackTestUtils::ComparePIDL(const CSmsPDU& aPDU, TSmsProtocolIdentifier aPID)
/**
 *  TODO
 */
	{
	LOCAL_CHECKPOINT(aPDU.ProtocolIdentifierPresent());
	LOCAL_CHECKPOINT(aPDU.PIDType() == aPID.PIDType());
	LOCAL_CHECKPOINT(aPDU.TelematicDeviceIndicator() == aPID.TelematicDeviceIndicator());

	if (aPDU.TelematicDeviceIndicator()==TSmsProtocolIdentifier::ESmsTelematicDevice)
		{
		LOCAL_CHECKPOINT(aPDU.TelematicDeviceType() == aPID.TelematicDeviceType());
		}
	}

EXPORT_C void CSmsStackTestUtils::CompareUDL(const CSmsPDU& aPDU, const CSmsUserData& aUD)
/**
 *  TODO
 */
	{
	LOCAL_CHECKPOINT(aPDU.UserDataPresent());
	const CSmsUserData& pduUD = aPDU.UserData();

	TInt count = pduUD.NumInformationElements();

	LOCAL_CHECKPOINT(count == aUD.NumInformationElements());

	while (count--)
		{
		const CSmsInformationElement& ie1 = pduUD.InformationElement(count);
		const CSmsInformationElement& ie2 = aUD.InformationElement(count);
		LOCAL_CHECKPOINT(ie1.Identifier() == ie2.Identifier());
		LOCAL_CHECKPOINT(ie1.Data() == ie2.Data());
		}

	LOCAL_CHECKPOINT(pduUD.Body() == aUD.Body());
	}

EXPORT_C void CSmsStackTestUtils::PrintAndTestDecodeL(const TGsmSms& aPdu, TSmsDataCodingScheme aExpectedDCS, TSmsProtocolIdentifier aExpectedPID, const CSmsUserData& aExpectedUD, TBool aIsMobileTerminated, TInt aExpectedError)
/**
 *  TODO
 */
	{
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG) 
    LogSmsIfPDUL(_L8("TestPduDbToolsL PDU: "), aPdu.Pdu(), EFalse);
    TBuf<40> timestamp;
    SmsTimeStampL(timestamp);
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CSMSSTACKTESTUTILS_PRINTANDTESTDECODEL_1, "%S",timestamp);
#endif

	CSmsPDU* pdu = NULL;
	TRAPD(err, pdu = CSmsPDU::NewL(aPdu, *iCharConv, iFs, aIsMobileTerminated));

	if (err == KErrNone)
		CleanupStack::PushL(pdu);

	LOCAL_CHECKPOINT(err == aExpectedError);

	if (err == KErrNone)
		{
		CompareDCSL(*pdu, aExpectedDCS);
		ComparePIDL(*pdu, aExpectedPID);
		CompareUDL(*pdu, aExpectedUD);
		}

	if (err == KErrNone)
		CleanupStack::PopAndDestroy(pdu);
	}

EXPORT_C TInt CSmsStackTestUtils::StoreParamsL(CMobilePhoneSmspList& aSmspList, RSocket& aSocket, TBool aCancelAfterRequest)
/**
 *  Stream Sms parameters out to the socket server
 *  @param aSmspList has Sms parameters list that is streamed to the socket server
 *  @param aSocket is used to stream the aSmspList to the socket server
 *  @param aCancelAfterRequest Boolean parameter tells if the request is wanted to be canceled
 *  @return TInt : The completing status of the request
 */
	{
	// Write parameters
	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << aSmspList);
	LOCAL_CHECKPOINT(ret == KErrNone);
	TRAP(ret,writestream.CommitL());
	LOCAL_CHECKPOINT(ret == KErrNone);

	TRequestStatus status;
	aSocket.Ioctl(KIoctlWriteSmsParams,status,NULL, KSolSmsProv);
    if(aCancelAfterRequest)
        aSocket.CancelIoctl();
	User::WaitForRequest(status);

	PRINTF2(_L("StoreParamsL - storeSmsParams returned %d"), status.Int());
	return status.Int();
	}

EXPORT_C TInt CSmsStackTestUtils::RetrieveParamsL(CMobilePhoneSmspList& aSmspList, RSocket& aSocket, TBool aCancelAfterRequest)
/**
 *  Retrieve Sms parameters
 *  @param aSmspList pointer to the Sms parameters list to be read into
 *  @param aSocket is used to stream the aSmspList from the socket server
 *  @param aCancelAfterRequest Boolean parameter tells if the request is wanted to be canceled
 *  @return TInt : The completing status of the request
 */
	{
	TInt ret;
	TRequestStatus status;

	//
	// Retrieve Sms parameters from Phone
	//
	aSocket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    if(aCancelAfterRequest)
        aSocket.CancelIoctl();
	User::WaitForRequest(status);

    if(status.Int() == KErrNone && !aCancelAfterRequest)
        {
	    RSmsSocketReadStream readstream(aSocket);
	    TRAP(ret,readstream >> aSmspList);
	    LOCAL_CHECKPOINT(ret == KErrNone);
        }

    return status.Int();
	}

EXPORT_C void CSmsStackTestUtils::MakeParametersReadRequestL(RSocket& aSocket)
/**
 *  Make SMS parameters read request to the SMS Stack.
 *  @param aSocket that is used to make request to the stack.
 */
	{
	TRequestStatus status;

    // Make read SMS params request to the SMS Stack
    aSocket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    User::WaitForRequest(status);
	PRINTF2(_L("Retrieve SMS parameters returned %d"), status.Int());
	if(status.Int() != KErrNone)
		{
		User::Leave(status.Int());
		}
	}

EXPORT_C void CSmsStackTestUtils::StoreParamsLeaveIfErrorL(CMobilePhoneSmspList& aSmspList,RSocket& aSocket)
/**
 *  Stream Sms parameters out to the socket server and make request.
 *  @param aSmspList has Sms parameters list that is streamed to the socket server
 *  @param aSocket is used to stream the aSmspList to the socket server
 */
	{
	// Write parameters
	RSmsSocketWriteStream writestream(aSocket);
	writestream << aSmspList;
	writestream.CommitL();

	TRequestStatus status;
	aSocket.Ioctl(KIoctlWriteSmsParams,status,NULL, KSolSmsProv);
	User::WaitForRequest(status);
	PRINTF2(_L("Store SMS parameters returned %d"), status.Int());
	if(status.Int() != KErrNone)
		{
		User::Leave(status.Int());
		}
	}

EXPORT_C TInt CSmsStackTestUtils::TestParameters(CMobilePhoneSmspList& aLeft,CMobilePhoneSmspList& aRight)
/**
 *  Compare sms parameters taken from chunk to parameters retrieved from SIM
 *  @param aLeft sms parameters taken from the Dummychunk
 *  @param aRight sms parameters retrieved from the SIM
 *  @return TInt : The error code
 */
	{
    TInt ret(KErrNone);

    if(aLeft.Enumerate() == aRight.Enumerate())
        {
        for(TInt storeIndex=0; storeIndex<aLeft.Enumerate(); storeIndex++)
            {
            const RMobileSmsMessaging::TMobileSmspEntryV1*  leftEntry(NULL);
            TBool found=EFalse;

            TRAPD(myLeftErr, leftEntry = &aLeft.GetEntryL(storeIndex));
            if (myLeftErr != KErrNone)
            	{
            	return myLeftErr;
            	}
            if (leftEntry == NULL)
            	{
            	return KErrArgument;
            	}

            for(TInt writeIndex=0; writeIndex<aRight.Enumerate(); writeIndex++)
                {
                const RMobileSmsMessaging::TMobileSmspEntryV1*  rightEntry(NULL);
                
                TRAPD(myRightErr, rightEntry = &aRight.GetEntryL(writeIndex));
	            if (myRightErr != KErrNone)
	            	{
	            	return myRightErr;
	            	}
	            if (rightEntry == NULL)
	            	{
	            	return KErrArgument;
	            	}

                if(leftEntry->iIndex == rightEntry->iIndex)
                    {
                    found=ETrue;
					if((leftEntry->iValidParams != rightEntry->iValidParams) ||
						(leftEntry->iText != rightEntry->iText))
						{
						ret = KErrCorrupt;
						break;
						}
					if(leftEntry->iValidParams & RMobileSmsMessaging::KDestinationIncluded)
						{
						ret = TestPhoneNumbers(leftEntry->iDestination,rightEntry->iDestination);
						if(ret)
							break;
						}
					if(leftEntry->iValidParams & RMobileSmsMessaging::KSCAIncluded)
						{
						ret = TestPhoneNumbers(leftEntry->iServiceCentre,rightEntry->iServiceCentre);
						if(ret)
							break;
						}
					if((leftEntry->iValidParams & RMobileSmsMessaging::KProtocolIdIncluded) &&
						(leftEntry->iProtocolId != rightEntry->iProtocolId))
						ret = KErrCorrupt;
					if((leftEntry->iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded) &&
						(leftEntry->iValidityPeriod != rightEntry->iValidityPeriod))
						ret = KErrCorrupt;
					if((leftEntry->iValidParams & RMobileSmsMessaging::KDcsIncluded) &&
						(leftEntry->iDcs != rightEntry->iDcs))
						ret = KErrCorrupt;
                    break;
                    }
                }

            if(!found || ret != KErrNone)
                {
                ret = KErrCorrupt;
                break;
                }
            }
        }
    else
        ret = KErrCorrupt;

    if(ret == KErrNone)
		PRINTF1(_L("SMS Parameters Ok!") );
    else
		PRINTF2(_L("ERROR: SMSP lists doesn't match: %d"), ret );

    return ret;
	}

EXPORT_C TInt CSmsStackTestUtils::TestPhoneNumbers(const RMobilePhone::TMobileAddress& aLeftNumber,const RMobilePhone::TMobileAddress& aRightNumber)
/**
 *  Compare phone numbers
 *  @param aLeftNumber reference to TMobileAddress object
 *  @param aRightNumber reference to TMobileAddress object
 */
	{
	TInt ret(KErrNone);
	TBuf<TGsmSmsTelNumberMaxLen> leftNumber;
	TBuf<TGsmSmsTelNumberMaxLen> rightNumber;

	//Take '+' sign away first
	leftNumber.Copy(aLeftNumber.iTelNumber);
	if(leftNumber.Length() > 0 && leftNumber[0] == '+')
		{
		// Remove "+" sign from telephony number
		leftNumber.Delete(0,1);
		leftNumber.Trim();
		}
	//Take '+' sign away first
	rightNumber.Copy(aRightNumber.iTelNumber);
	if(rightNumber.Length() > 0 && rightNumber[0] == '+')
		{
		// Remove "+" sign from telephony number
		rightNumber.Delete(0,1);
		rightNumber.Trim();
		}
	if((leftNumber.Compare(rightNumber) != 0) ||
       (aLeftNumber.iTypeOfNumber != aRightNumber.iTypeOfNumber) ||
       (aLeftNumber.iNumberPlan != aRightNumber.iNumberPlan))
		{
        ret = KErrCorrupt;
        }
	return(ret);
	}

EXPORT_C void CSmsStackTestUtils::PrintSmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aEntryToTsy)
/**
 *  Print a single SMS parameter entry, induvidual content depends on valid flags
 *  @param aEntryToTsy reference to TMobileSmspEntryV1 object
 */
    {
    PRINTF2( _L("Index %d"), aEntryToTsy.iIndex);
    PRINTF2( _L("Text %S"),  &aEntryToTsy.iText);
    PRINTF2( _L("Validity Flags %d"), aEntryToTsy.iValidParams);

    // Protocol ID
    if (aEntryToTsy.iValidParams & RMobileSmsMessaging::KProtocolIdIncluded)
        {
        PRINTF2( _L("Protocol ID is %d"),
                                    aEntryToTsy.iProtocolId);
        }

    //DCS
    if (aEntryToTsy.iValidParams & RMobileSmsMessaging::KDcsIncluded)
        {
        PRINTF2( _L("Default Ccding Scheme is %d"),
                                    aEntryToTsy.iDcs);
        }

   //validity
    if (aEntryToTsy.iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded)
        {
        PRINTF2( _L("Validity Period is %d"),
                                    aEntryToTsy.iValidityPeriod);
        }

    // destination number
    if (aEntryToTsy.iValidParams & RMobileSmsMessaging::KDestinationIncluded)
        {
        PRINTF2( _L("Destination Type Number is %d"),
                                    aEntryToTsy.iDestination.iTypeOfNumber);
        PRINTF2( _L("Destination Number Plan is %d"),
                                    aEntryToTsy.iDestination.iNumberPlan);
        PRINTF2( _L("Destination Number is %S"),
                                    &aEntryToTsy.iDestination.iTelNumber);
        }
    // service center number
    if (aEntryToTsy.iValidParams & RMobileSmsMessaging::KSCAIncluded)
        {
        PRINTF2( _L("Service Center Type Number is %d"),
                                    aEntryToTsy.iServiceCentre.iTypeOfNumber);
        PRINTF2( _L("Service Center Number Plan is %d"),
                                    aEntryToTsy.iServiceCentre.iNumberPlan);
        PRINTF2( _L("Service Center Number is %S"),
                                    &aEntryToTsy.iServiceCentre.iTelNumber);
        }
    }

EXPORT_C void CSmsStackTestUtils::PrintSmspList(CMobilePhoneSmspList& aSmspList)
/**
 *  Print a SMS parameter list
 *  @param aSmspList reference to CMobilePhoneSmspList object
 */
    {
    TInt entries;
    entries = aSmspList.Enumerate();

    if (entries != 1)
        {
        PRINTF2(_L("The Sms Parameter List has %d parameter entries "), entries );
        }
    else
        {
        PRINTF2(_L("The Sms Parameter List has %d parameter entry "), entries );
        }

    RMobileSmsMessaging::TMobileSmspEntryV1 entryToTsy;

    for (TInt i = 0; i < entries; i++)
        {
        PRINTF1(_L(""));
        TRAPD(err, entryToTsy = aSmspList.GetEntryL(i));

        if (err == KErrNone)
            {
            PrintSmspEntry(entryToTsy);
            }
        else
            {
            //if leave occurs print to the console
            PRINTF2(_L("SmspList.GetEntryL() left: possible cause memory allocation %d"), err);
            break;
            }
        }
    }

//
// CTestGetSmsList
//

EXPORT_C CTestGetSmsList* CTestGetSmsList::NewL(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils)
	{
	CTestGetSmsList* smsListGetter = new(ELeave) CTestGetSmsList(aPriority, aSocketServer, aTestUtils);
	CleanupStack::PushL(smsListGetter);
	smsListGetter->ConstructL();
	CleanupStack::Pop(smsListGetter);
	return smsListGetter;
	}

EXPORT_C CTestGetSmsList::CTestGetSmsList(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils)
: CSmsuActiveBase(aPriority), //parent construction
iSocketServer(aSocketServer),
iTestUtils(aTestUtils)
	{
	}


void CTestGetSmsList::ConstructL()
	{
	iTestUtils.OpenSmsSocketLC(iSocketServer,iSocket,ESmsAddrLocalOperation);
	CleanupStack::Pop(&iSocket);
	}

EXPORT_C void CTestGetSmsList::GetSmsList(TRequestStatus& aStatus)
/**
 *  Starts the active object: Puts a Ioctl request on a socket to enumerate the number
 *  of messsages in the store
 *  @param	aStatus, copied to iStatus of this active object
 *  
 *   *
 */
	{
	Queue(aStatus);
	iMessageCount()=0;
	iSocket.Ioctl(KIoctlEnumerateSmsMessages, iStatus,&iMessageCount, KSolSmsProv);
	SetActive();
	iState = EEnuming;
	}

void CTestGetSmsList::DoRunL()
/**
 *  Active object completion function
 *  Active while there are still messages to read from the socket
 */
	{
	iTestUtils.PRINTF1(_L("Messages enumerated"));
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EEnuming:
			{
			iRemainingMessages = iMessageCount();
			iTestUtils.PRINTF2(_L("%d messages in the store"), iRemainingMessages);
			}

		case EReading:
			{
			if (iRemainingMessages > 0)
				{
				iRemainingMessages--;
				CSmsBuffer* buffer=CSmsBuffer::NewL();
				CSmsMessage* smsMessage=CSmsMessage::NewL(iTestUtils.iFs, CSmsPDU::ESmsDeliver,buffer);
				CleanupStack::PushL(smsMessage);

				RSmsSocketReadStream readstream(iSocket);
				readstream >> *smsMessage;
				CleanupStack::PopAndDestroy(smsMessage);//operation was done only to clear the socket, we are not interested in the message content

				iSocket.Ioctl(KIoctlReadMessageSucceeded, iStatus, NULL, KSolSmsProv);
				SetActive();
				iState = EReading;
				}
			}
		}
	}

void CTestGetSmsList::DoCancel()
	{
	iSocket.CancelIoctl();
	Complete(KErrCancel);
	}


//
//         SmsReceiver AO
//

EXPORT_C CTestReceiveMessages* CTestReceiveMessages::NewL(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils)
	{
	CTestReceiveMessages* smsReceiver = new(ELeave) CTestReceiveMessages(aPriority, aSocketServer, aTestUtils);
	CleanupStack::PushL(smsReceiver);
	smsReceiver->ConstructL();
	CleanupStack::Pop(smsReceiver);
	return smsReceiver;
	}

CTestReceiveMessages::CTestReceiveMessages(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils)
/**
 *  Constructor
 */
: CSmsuActiveBase(aPriority), //parent construction
iSocketServer(aSocketServer),
iTestUtils(aTestUtils)
	{
	}


void CTestReceiveMessages::ConstructL()
	{
	iTestUtils.OpenSmsSocketLC(iSocketServer,iSocket,ESmsAddrRecvAny);
	CleanupStack::Pop(&iSocket);
	}

EXPORT_C void CTestReceiveMessages::Receive(TRequestStatus& aStatus, TInt aNumberOfMessages)
/**
 *  Starts the AO by putting a request on the socket receive sms messages
 *  This function will normally be used when we start using the active object
 *  Active while the number of received messages is > aNumberOfmessages
 *  @param aNumberOfMessages the number of messages to be received
 */
	{
	Queue(aStatus);
	if (iNumberOfReceivedMessages >= aNumberOfMessages)
		Complete(KErrNone);
	else
		{
		iNumberOfMessagesToReceive=aNumberOfMessages;
		if (!IsActive())
			Receive();
		}
	}

EXPORT_C void CTestReceiveMessages::Receive()
/**
 *  Reposts the receive request -note Receive(aStatus, aNunmberOfMessages) has to be called prior to this one
 *  
 */
	{
	iMessageBuf()=KSockSelectRead;
	iSocket.Ioctl(KIOctlSelect,iStatus,&iMessageBuf,KSOLSocket);
	iState = EReceiving;
	SetActive();
	}


void CTestReceiveMessages::DoRunL()
/**
 *  Active object completion function.completes once in each state for each message received
 *  
 */
	{
	User::LeaveIfError(iStatus.Int());

	switch(iState)
		{
		case (EReceiving):
			{
			iTestUtils.PRINTF1(_L(" Message received"));

			CSmsBuffer* buffer=CSmsBuffer::NewL();
			CSmsMessage* smsMessage=CSmsMessage::NewL(iTestUtils.iFs, CSmsPDU::ESmsSubmit,buffer);
			CleanupStack::PushL(smsMessage);
			RSmsSocketReadStream readstream(iSocket);
			readstream >> *smsMessage;
			CleanupStack::PopAndDestroy(smsMessage);//operation was done only to clear the socket, we are not interested in the message content
			iSocket.Ioctl(KIoctlReadMessageSucceeded, iStatus, NULL, KSolSmsProv);
			SetActive();
			iState = EReceived;
			break;
			}
		case (EReceived):
			{
			iNumberOfReceivedMessages++;
			iTestUtils.PRINTF2(_L("%d Messages received"), iNumberOfReceivedMessages);
			iState=EReceiving;

			if (iNumberOfMessagesToReceive!=iNumberOfReceivedMessages || iReport == NULL)
				Receive();

			break;
			}
		}
	}

void CTestReceiveMessages::DoCancel()
	{
	iSocket.CancelIoctl();
	Complete(KErrCancel);
	}


TInt CTestReceiveMessages::NumberReceived()
/**
 *  Returns the number of received sms messages
 */
	{
	return iNumberOfReceivedMessages;
	}

//
//         SmsReceiverEnumerator wrapping AO
//


EXPORT_C CTestReceiveEnumerateMessages* CTestReceiveEnumerateMessages::NewL(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils)
	{
	CTestReceiveEnumerateMessages* smsReceiverEnumerator = new(ELeave) CTestReceiveEnumerateMessages(aPriority, aSocketServer, aTestUtils);
	CleanupStack::PushL(smsReceiverEnumerator);
	smsReceiverEnumerator->ConstructL();
	CleanupStack::Pop(smsReceiverEnumerator);
	return smsReceiverEnumerator;
	}

CTestReceiveEnumerateMessages::CTestReceiveEnumerateMessages(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils)
/**
 *  Constructor
 *   *
 */
: CSmsuActiveBase(aPriority), //parent construction
iSocketServer(aSocketServer),
iTestUtils(aTestUtils)
	{
	}


void CTestReceiveEnumerateMessages::ConstructL()
	{
	iEnumerator = CTestGetSmsList::NewL(KActiveObjectPriority, iSocketServer, iTestUtils);
	iReceiver = CTestReceiveMessages::NewL(KActiveObjectPriority, iSocketServer, iTestUtils);
	ConstructTimeoutL();
	}

EXPORT_C void CTestReceiveEnumerateMessages::Start(TInt aEnumExpected, TInt aRecvExpected, TInt aRecvInitial)
/**
 *  Starts the active object state machine in order to have the following
 *  -receive iRecvExpected messages
 *  -Enumerate messages in store and verify there are iEnumExpected and continue receiving
 *  -Receive the rest of the messages (iRecvExpected in total)
 *  -Request to receive another message that should time out as only 5 messages are received
 *  -Flag error if that last message is received
 */
	{
	iEnumExpected=aEnumExpected;
	iRecvExpected=aRecvExpected;
	iRecvInitial=aRecvInitial;

	iReceiver->Receive(iStatus, aRecvInitial);
	iState = ERecvInitial;
	SetActive();
	CActiveScheduler::Start();
	}

void CTestReceiveEnumerateMessages::Complete(TInt aErr)
	{
	iStatus=aErr;
	CActiveScheduler::Stop();
	}

void CTestReceiveEnumerateMessages::DoRunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch(iState)
		{
		case ERecvInitial:
			{
			iEnumerator->GetSmsList(iStatus);
			iReceiver->Receive();
			iState=EEnumerating;
			SetActive();
			break;
			}
		case EEnumerating:
			{
			if(iEnumerator->iMessageCount()==TUint(iEnumExpected))
				iTestUtils.PRINTF1(_L("**enumerated the right number of messages"));
			else
				User::Leave(KErrCorrupt);
			iReceiver->Receive(iStatus, 5);
			SetActive();
			iState=ESecondReceiving;
			break;
			}
		case ESecondReceiving:
			{
			iReceiver->Receive(iStatus, 1 + iRecvExpected);
			iState=ETryingToReceiveOneMore;
			TimedSetActive(15000000);
			break;
			}

		case ETryingToReceiveOneMore:
			{
			User::Leave(KErrOverflow);
			break;
			}

		}

	}

void CTestReceiveEnumerateMessages::DoCancel()
/**
 *  Calls cancel on the active objects this owns
 *  
 */
	{
	TimedSetActiveCancel();
	TInt err = KErrCancel;

	switch(iState)
		{
		case ERecvInitial:
			iReceiver->Cancel();
			break;
		case EEnumerating:
			iEnumerator->Cancel();
			break;
		case ESecondReceiving:

			iReceiver->Cancel();
			break;

		case ETryingToReceiveOneMore:

			iReceiver->Cancel();

			if (TimedOut())
				err = KErrNone;

			break;
		}

	Complete(err);
	}


EXPORT_C CTestReceiveEnumerateMessages::~CTestReceiveEnumerateMessages()
/**
 *  destructor
 */
 	{
	delete iReceiver;
	delete iEnumerator;
	}
