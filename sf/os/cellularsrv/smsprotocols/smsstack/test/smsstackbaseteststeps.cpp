// Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "smsstackbaseteststeps.h"

#include <testconfigfileparser.h>
#include <simtsy.h>
#include <commsdattypesv1_1.h>
#include <gsmubuf.h>
#include <Gsmumsg.h>
#include <gsmuset.h>
#include <es_sock.h>
#include <smsustrm.h>
#include <c32root.h>
#include <sacls.h>
#include <e32math.h>
#include <smspver.h>

#include "smsstacktestutilities.h"
#include "smsstacktestconsts.h"
#include "smsstacktestcase.h"
#include "smspdudb.h"

using namespace CommsDat;

EXPORT_C CSmsBaseTestStep::CSmsBaseTestStep()
    {
    // empty
    }

EXPORT_C CSmsBaseTestStep::~CSmsBaseTestStep()
    {
    // empty       
    }

/**
    Creates the file server session and marks the heap
*/
EXPORT_C TVerdict CSmsBaseTestStep::doTestStepPreambleL()
    {
    RDebug::Print(_L("%S"), &TestStepName());
    
    User::LeaveIfError(iFs.Connect());  
    
    __UHEAP_MARK;
    
    SetTestNumberFromConfigurationFileL();
    
    iSmsStackTestUtils = CSmsStackTestUtils::NewL(this,  iFs);
    
    iScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iScheduler);
    
    TRAPD(ret, ParseSettingsFromFileL());
    TESTCHECK(ret, KErrNone, "ParseSettingsFromFileL");
    

    if (!iPartOfMultiStepTestCase)
        {
        ConnectSocketServerL(iSocketServer);
        }
    
    return TestStepResult();
    }

/**
    Closes file server session and unmarks the heap 
*/
EXPORT_C TVerdict CSmsBaseTestStep::doTestStepPostambleL()
    {
    if (!iPartOfMultiStepTestCase)
        {
        iSocketServer.Close();
        }
    
    delete iScheduler;
    iScheduler = NULL;
    
    delete iSmsStackTestUtils;
    iSmsStackTestUtils = NULL;
    
    __UHEAP_MARKEND;
    
    iFs.Close();
    
    if ( !iPartOfMultiStepTestCase && !iNotLastTestStep)
        {
        DoESockMemoryLeakTestL();
        }
    
    return TestStepResult();
    }
    
// TODO Method not needed - should be removed and replaced where appropriate
EXPORT_C void CSmsBaseTestStep::ParseSettingsFromFileL()
    {
    CTestConfig* configFile = CTestConfig::NewLC(iFs,KGmsSmsConfigFileDir,KGmsSmsConfigFileName);
    
    const CTestConfigSection* cfgFile = configFile->Section(KSetupTelNumbers);
    TESTCHECKCONDITIONL(cfgFile!=NULL, "Open SMS configuration file");

    const CTestConfigItem* item = cfgFile->Item(KServiceCenter,0);
    TESTCHECKCONDITIONL(item!=NULL, "Read ServiceCentre value from the configuration file");

    iServiceCenterNumber.Copy(item->Value());

    item = cfgFile->Item(KTelefoneNumber,0);
    TESTCHECKCONDITIONL(item!=NULL, "Read TelephoneNumber value from the configuration file");
    
    iTelephoneNumber.Copy(item->Value());

    CleanupStack::PopAndDestroy(configFile);
    }

/**
 * Read the test number from  the configuration file.
 * The test number is passed via property KUidPSSimTsyCategory. 
 * The SIM tsy uses test number to parse correct script from config.txt
 * @return ETrue, if the test number has been assigned successfully, otherwise EFalse.
 */
EXPORT_C void CSmsBaseTestStep::SetTestNumberFromConfigurationFileL()
    {
    TInt testNumber (0);
    if(GetIntFromConfig( ConfigSection(), KTestCaseNumber, testNumber))
        {
        SetSimTSYTestNumberL(testNumber);
        }
    }

EXPORT_C void CSmsBaseTestStep::SetSimTSYTestNumberL(TInt aTestNumber)
/**
    Set the SIM TSY Test number 
    @param aTestNumber is the test number in SIM TSY config file
*/
    {   
    SetTestNumberL(KPSSimTsyTestNumber, aTestNumber);
    }

/**
 *  The test number is set with property aTestNumberProperty. This will notify the SIM tsy.
 *  SIM tsy uses test number to parse correct script from the configuration file.
 *  @param aTestNumberProperty The test number property UID
 *  @param aTestNumber The test number corresponding the test case
 */
EXPORT_C void CSmsBaseTestStep::SetTestNumberL(TInt aTestNumberProperty, TInt aTestNumber)
    {
    //
    // Set the SIM.TSY test number...
    //
    RProperty testNumberProperty;
    User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, aTestNumberProperty));
    CleanupClosePushL(testNumberProperty);

    TRequestStatus status;
    testNumberProperty.Subscribe(status);
    INFO_PRINTF3(_L("Setting SimTsy test number property (0x%X) to %d"), aTestNumberProperty, aTestNumber);
    User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory, aTestNumberProperty, aTestNumber));
    User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrNone, "Setting SimTsy test number property ");

    TInt testNumberCheck;
    User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
    TESTCHECK(aTestNumber, testNumberCheck, "Checking correct test number");

    CleanupStack::PopAndDestroy(&testNumberProperty);
    }
    
EXPORT_C void CSmsBaseTestStep::ConnectSocketServerL(RSocketServ& aSocketServer)
    {
    INFO_PRINTF1(_L("Connecting to the Socket Server..."));
    
    TInt  ret = aSocketServer.Connect(KSocketMessageSlots);
    if (ret != KErrNone)
        {
        ERR_PRINTF2(_L("Connecting to socket server failed [ret=%d]"), ret);
        }
    
    INFO_PRINTF1(_L("Clearing private data..."));

    _LIT(KWapReassemblyStoreName, "C:\\Private\\101F7989\\sms\\wapreast.dat");
    ret = iFs.Delete(KWapReassemblyStoreName);
    if( ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound )
        {
        ERR_PRINTF2(_L("Deleted WAP reassembly Store failed [ret=%d]"), ret);
        }

    _LIT(KReassemblyStoreName, "C:\\Private\\101F7989\\sms\\smsreast.dat");
    ret = iFs.Delete(KReassemblyStoreName);
    if( ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound )
        {
        ERR_PRINTF2(_L("Deleted reassembly Store failed [ret=%d]"), ret);
        }

    _LIT(KSegmentationStoreName, "C:\\Private\\101F7989\\sms\\smssegst.dat");
    ret = iFs.Delete(KSegmentationStoreName);
    if( ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound )
        {
        ERR_PRINTF2(_L("Deleted segmentation Store failed [ret=%d]"), ret);
        }

    _LIT(KSmsClass0PreallocatedStoreName, "C:\\Private\\101F7989\\sms\\smsclass0preallocated.dat");
    ret = iFs.Delete(KSmsClass0PreallocatedStoreName);
    if( ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound )
        {
        ERR_PRINTF2(_L("Deleted SmsClass0Preallocated Store failed [ret=%d]"), ret);
        }

    _LIT(KSmsClass0ReassemblyStoreName, "C:\\Private\\101F7989\\sms\\smsclass0reast.dat");
    ret = iFs.Delete(KSmsClass0ReassemblyStoreName);
    if( ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound )
        {
        ERR_PRINTF2(_L("Deleted SmsClass0Reassembly Store failed [ret=%d]"), ret);
        }

    _LIT(KSmsUtilityResourceFileName, "C:\\Private\\101F7989\\sms\\smsu.rsc");
    ret = iFs.Delete(KSmsUtilityResourceFileName);
    if( ret != KErrNone && ret != KErrNotFound && ret != KErrPathNotFound )
        {
        ERR_PRINTF2(_L("Deleted SMS stack resource file failed [ret=%d]"), ret);
        }
    }

EXPORT_C void CSmsBaseTestStep::ConnectSocketServerLC(RSocketServ& aSocketServer)
    {
    ConnectSocketServerL(aSocketServer);
    CleanupClosePushL(aSocketServer);
    }
    
EXPORT_C CSmsMessage* CSmsBaseTestStep::CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
    {
    CSmsMessage* smsMessage = CreateSmsMessageLC(aDes, aAlphabet, aType);
    CleanupStack::Pop(smsMessage);
    return smsMessage;
    }

EXPORT_C CSmsMessage* CSmsBaseTestStep::CreateSmsMessageLC(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
    {
    CSmsBuffer* buffer=CSmsBuffer::NewL();
    CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, aType, buffer);
    CleanupStack::PushL(smsMessage);

    TSmsUserDataSettings smsSettings;
    smsSettings.SetAlphabet(aAlphabet);
    smsSettings.SetTextCompressed(EFalse);
    smsMessage->SetUserDataSettingsL(smsSettings);

    smsMessage->SetToFromAddressL(iTelephoneNumber);
    smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
    buffer->InsertL(0,aDes);
    return smsMessage;
    }

EXPORT_C CSmsMessage* CSmsBaseTestStep::CreateSmsWithStatusReportReqL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet)
/**
 *  Create a uninitialised SMS message with Status Report request
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
    {
    CSmsMessage* smsMessage=CreateSmsMessageLC(aDes, aAlphabet);

    //Set Status report request
    CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
    submitPdu.SetStatusReportRequest(ETrue);

    CleanupStack::Pop(smsMessage);
    return smsMessage;
    }

EXPORT_C CSmsMessage* CSmsBaseTestStep::RecvSmsL(RSocket& aSocket, TInt aIoctl)
/**
 *  Receive an Sms
 *  @param aSocket is used to stream the sms message from the socket server
 *  @return CSmsMessage* :Sms message from Sms stack
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 */
    {
    CSmsBuffer* buffer=CSmsBuffer::NewL();
    CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
    CleanupStack::PushL(smsMessage);

    RSmsSocketReadStream readstream(aSocket);
    TRAPD(ret, readstream >> *smsMessage);
 
    TRequestStatus status;
    
    // aDesc arguments passed to Ioctl() method have been changed to NULL to keep it compliant with
    // usage rules and to prevent OOM tests from failing
    if(ret==KErrNone)
        {
        aSocket.Ioctl(aIoctl, status, NULL, KSolSmsProv);
        User::WaitForRequest(status);
        CleanupStack::Pop(smsMessage);
        TESTCHECK(status.Int(), KErrNone, "Notifying the SMS stack that message was received successfully");
        INFO_PRINTF1(_L("Message received successfully"));
        }
    //An error has occured, no message has been received
    else
        {
        aSocket.Ioctl(KIoctlReadMessageFailed, status, NULL, KSolSmsProv);
        User::WaitForRequest(status);
        TESTCHECK(status.Int(), KErrNone, "Notifying the SMS stack that message was not received");
        ERR_PRINTF2(_L("Receiving message failed %d"), ret);
        User::Leave(ret);
        }
    return smsMessage;  
    }

EXPORT_C CSmsMessage* CSmsBaseTestStep::RecvSmsFailedL(RSocket& aSocket)
/**
 *  Receive an Sms, first nack the receive several times before succeeding
 *  @param aSocket is used to stream the sms message from the socket server
 *  @return CSmsMessage* :Sms message from Sms stack
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 *  @leave Leaves if nack of receiving is completed with error code
 *  @leave Leaves if ack of receiving is completed with error code
 */
    {
    CSmsBuffer* buffer=CSmsBuffer::NewL();
    CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
    CleanupStack::PushL(smsMessage);
    RSmsSocketReadStream readstream(aSocket);

    TPckgBuf<TUint> sbuf;
    TRequestStatus status;

    for(TInt i=0; i<10; i++)
        {
        TRAPD(ret,readstream >> *smsMessage);
        TEST(ret == KErrNone);
        aSocket.Ioctl(KIoctlReadMessageFailed, status, &sbuf, KSolSmsProv);
        User::WaitForRequest(status);
        TEST(status.Int() == KErrNone);
        }

    TRAPD(ret,readstream >> *smsMessage);
    TEST(ret == KErrNone);
    aSocket.Ioctl(KIoctlReadMessageSucceeded, status, NULL, KSolSmsProv);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);

    CleanupStack::Pop(smsMessage);
    return smsMessage;
    }

EXPORT_C void CSmsBaseTestStep::SendSmsL(const CSmsMessage* aSms, RSocket& aSocket, TInt aExpectedError/*=KErrNone*/, TInt aMaxRetries/*=3*/)
/**
 *  Stream aSms out to the socket server
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @param aExpectedError The error expected from the send request
 *  @param aMaxRetries The max number of retries is send request does not match expected error
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
    {
    INFO_PRINTF1(_L("Sending SMS... "));
    PrintMessageL(aSms);
    
    TBool tryAgain = ETrue;
    TInt sendTry (0);
    TRequestStatus status = KErrNone;

    while( tryAgain && sendTry < aMaxRetries )
        {
        RSmsSocketWriteStream writestream(aSocket);
        TRAPD(ret,writestream << *aSms);
        TRAP(ret, writestream.CommitL());

        TPckgBuf<TUint> sbuf;
        aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
        User::WaitForRequest(status);
        INFO_PRINTF3(_L("SendSmsL [status=%d, aExpectedError=%d]"), status.Int(), aExpectedError);
        if ( status.Int() != aExpectedError )
            {
            tryAgain = ETrue;
            INFO_PRINTF1(_L("Try again... "));
            ++sendTry;
            }
        else tryAgain = EFalse;
        }
    
    TESTCHECKL(status.Int(), aExpectedError, "Sending Message with an expected error");
    }

EXPORT_C void CSmsBaseTestStep::SendSmsCancelL(CSmsMessage* aSms, RSocket& aSocket1, RSocket& aSocket2)
/**
 *  Stream Sms out to the socket server by two RSmsSocketWriteStream object.
 *  The first request is canceled and then the second request is completed with error code.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket1 The socket used with message that will be canceled
 *  @param aSocket2 The socket used with message that will be completed with error code
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with KErrNone
 */
    {
    RSmsSocketWriteStream writestream(aSocket1);
    TRAPD(ret,writestream << *aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream.CommitL());
    TEST(ret == KErrNone);

    TPckgBuf<TUint> sbuf;
    TRequestStatus status1,status2;

    //stream to socket2
    RSmsSocketWriteStream writestream2(aSocket2);
    TRAP(ret,writestream2 << *aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream2.CommitL());
    TEST(ret == KErrNone);

    aSocket1.Ioctl(KIoctlSendSmsMessage,status1,&sbuf, KSolSmsProv);
    aSocket2.Ioctl(KIoctlSendSmsMessage,status2,&sbuf, KSolSmsProv);

    User::After(2000000);

    // Test cancel first
    aSocket1.CancelIoctl();
    User::WaitForRequest(status1);
    TEST(status1.Int()==KErrCancel);

    User::After(50000);


    User::WaitForRequest(status2);

    INFO_PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"),status2.Int());
    PrintMessageL(aSms);

    //Ensure the request is completed with error code ;)
    TEST(status2.Int() != KErrNone);
    INFO_PRINTF2(_L("Sending failed! %d"), status2.Int());
    }

EXPORT_C TInt CSmsBaseTestStep::SendSmsErrorL(CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream aSms out to the socket server. Sending is completed with error code.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @return error code
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with KErrNone
 */
    {
    RSmsSocketWriteStream writestream(aSocket);
    TRAPD(ret,writestream << *aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream.CommitL());
    TEST(ret == KErrNone);

    TPckgBuf<TUint> sbuf;
    TRequestStatus status;

    User::After(50000);
    // test cancel first
    aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
    aSocket.CancelIoctl();
    User::WaitForRequest(status);
    TEST(status.Int()==KErrCancel);

    //Now send again, completed with error
    TRAP(ret,writestream << *aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream.CommitL());
    TEST(ret == KErrNone);

    aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
    User::WaitForRequest(status);
    INFO_PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"), status.Int());
    PrintMessageL(aSms);
    INFO_PRINTF2(_L("Sending failed! %d"), status.Int());
    TEST(status.Int() != KErrNone);
    return status.Int();
    }

EXPORT_C void CSmsBaseTestStep::SendCommandSmsL(CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream command message out to the socket server and wait for the return status
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
    {
    RSmsSocketWriteStream writestream(aSocket);
    TRAPD(ret,writestream << *aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream.CommitL());
    TEST(ret == KErrNone);

    TRequestStatus status;
    TPckgBuf<TUint> sbuf;
    aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf,KSolSmsProv);

    User::WaitForRequest(status);

    INFO_PRINTF2(_L("SendCommandSmsL, sendSms returned %d"), status.Int());
    User::After(1000000);
    TEST(status.Int() == KErrNone);

    }

EXPORT_C void CSmsBaseTestStep::SendAndRecvTestMessageL(const TTestCase& aTestCase, RSocket& aSocket)
/**
 *  Send a test message. This method is used to send and receive different DCS type messages
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
    {
    SendTestMessageL(aTestCase, aSocket);

    WaitForRecvL(aSocket);
    CSmsMessage* smsMessage = RecvSmsL(aSocket);
    CleanupStack::PushL(smsMessage);
    
    TestMessageContentsL(smsMessage,aTestCase);
    CleanupStack::PopAndDestroy(smsMessage);
    }

EXPORT_C void CSmsBaseTestStep::SendTestMessageL(const TTestCase& aTestCase, RSocket& aSocket)
/**
 *  Send a test message
 *  Assumes recv is already done.
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
    {
    CSmsMessage* smsMessage = CreateSmsMessageLC(aTestCase.iMsg, TSmsDataCodingScheme::ESmsAlphabet7Bit);

    CSmsPDU& pdu = smsMessage->SmsPDU();
    CSmsUserData& userData = pdu.UserData();

    if (aTestCase.iMatchType == ESmsAddrMatchIEI)
        userData.AddInformationElementL(aTestCase.iIdentifierMatch,_L8("98"));

    if (aTestCase.iTestSmsClass)
        {
        pdu.SetBits7To4(TSmsDataCodingScheme::ESmsDCSTextUncompressedWithClassInfo);
        pdu.SetClass(ETrue,aTestCase.iSmsClass);
        }

    if (aTestCase.iTestValidityPeriod && pdu.Type()==CSmsPDU::ESmsSubmit)
        {
        CSmsSubmit* submitPdu = REINTERPRET_CAST(CSmsSubmit*,&pdu);
        submitPdu->SetValidityPeriod(aTestCase.iValidityPeriod);
        }

    if (aTestCase.iTestIndicators && pdu.Type()==CSmsPDU::ESmsSubmit)
        {
        SetIndicatorL(aTestCase, smsMessage);
        }

    SendSmsL(smsMessage, aSocket);
    CleanupStack::PopAndDestroy(smsMessage);
    }

EXPORT_C void CSmsBaseTestStep::SendSmsDontCheckReturnValueL(CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream aSms out to the socket server and don't check return value.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with KErrNone
 */
    {
    RSmsSocketWriteStream writestream(aSocket);
    writestream << *aSms;
    writestream.CommitL();

    TPckgBuf<TUint> sbuf;
    TRequestStatus status;
    aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
    User::WaitForRequest(status);
    INFO_PRINTF2(_L("Send SMS message returned %d"), status.Int());
    if(status.Int() != KErrNone)
        {
        User::Leave(status.Int());
        }
    }

EXPORT_C void CSmsBaseTestStep::SendAndRecvSms7BitL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send and receive one 7bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendAndRecvSmsL leaves
 */
    {
    DoSendAndRecvSmsL(aDes,TSmsDataCodingScheme::ESmsAlphabet7Bit, aSocket);
    }

EXPORT_C void CSmsBaseTestStep::DoSendAndRecvSmsL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RSocket& aSocket)
/**
 *  Send and recv one sms,
 *  then check that the sent message corresponds with the received message
 *  @param aDes contains the text that will be inserted to the pdu at CreateSmsMessageL
 *  @param aAlphabet describes the alphabet of the pdu that will be created at CreateSmsMessageL
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
    {
    CSmsMessage* smsMessage=CreateSmsMessageLC(aDes, aAlphabet);
    SendSmsL(smsMessage, aSocket);
    CleanupStack::PopAndDestroy(smsMessage); //destroyed because created again in RecvSmsL

    WaitForRecvL( aSocket);
    smsMessage = RecvSmsL( aSocket);
    CleanupStack::PushL(smsMessage);

    TestSmsContentsL(smsMessage,aDes);

    // TODO: is this a way to go?
    User::After(1000000);

    CleanupStack::PopAndDestroy(smsMessage);
    }

EXPORT_C void CSmsBaseTestStep::PrintMessageDetailedL(const CSmsMessage* aSms)
    {
    TPtrC from = aSms->ToFromAddress();
    INFO_PRINTF2(_L("ToFromAddress: %S"), &from);
    TPtrC sc = aSms->ServiceCenterAddress();
    INFO_PRINTF2(_L("ServiceCenterAddress: %S"), &sc);

    if(aSms->Storage() == CSmsMessage::ESmsSIMStorage)
        {
        INFO_PRINTF1(_L("Store: SIM"));
        }
    else if (aSms->Storage() == CSmsMessage::ESmsPhoneStorage)
        {
        INFO_PRINTF1(_L("Store: Phone"));
        }
    else if (aSms->Storage() == CSmsMessage::ESmsCombinedStorage)
        {
        INFO_PRINTF1(_L("Store: Combined"));
        }
    else
        {
        INFO_PRINTF1(_L("Store: Unknown"));
        }

    switch (aSms->Status())
        {
        case RMobileSmsStore::EStoredMessageUnread:
            INFO_PRINTF1(_L("Status: Unread"));
            break;

        case RMobileSmsStore::EStoredMessageRead:
            INFO_PRINTF1(_L("Status: Read"));
            break;

        case RMobileSmsStore::EStoredMessageUnsent:
            INFO_PRINTF1(_L("Status: Unsent"));
            break;

        case RMobileSmsStore::EStoredMessageSent:
            INFO_PRINTF1(_L("Status: Sent"));
            break;

        case RMobileSmsStore::EStoredMessageDelivered:
            INFO_PRINTF1(_L("Status: Delivered"));
            break;

        case RMobileSmsStore::EStoredMessageUnknownStatus:
        default:
            INFO_PRINTF1(_L("Status: Unknown"));
            break;
        }
    PrintMessageL(aSms);
    }

EXPORT_C void CSmsBaseTestStep::PrintMessageL(const CSmsMessage* aSms)
/**
 *  Print the content of SMS to the console
 */
    {
    if (aSms == NULL)
        {
        return;
        }
    
    CSmsBuffer& smsbuffer = (CSmsBuffer&)aSms->Buffer();
    const TInt len = smsbuffer.Length();
    HBufC* hbuf = HBufC::NewL(len);
    
    TPtr ptr = hbuf->Des();
    smsbuffer.Extract(ptr, 0, len);

    for (TInt j = 0; j < len; ++j)
        {
        if (ptr[j] < 0x20  ||  ptr[j] > 0xFF)
            {
            // Non-displayable character, print "." instead
            ptr[j] = 0x007F;
            }
        }
    INFO_PRINTF2(_L("SMS contains: %S"), &ptr);

    delete hbuf;
    }

EXPORT_C TSmsStatus::TSmsStatusValue CSmsBaseTestStep::RecvStatusReportL(TSmsServiceCenterAddress& aRecipientNumber, RSocket& aSocket)
/**
 * Receive a Status report
 * @param aRecipientNumber The supposed recipient number
 * @param aSocket is used to stream the sms message from the socket server
 */
    {
    //Receive SMS
    INFO_PRINTF1(_L("waiting for incoming status report...") );
    WaitForRecvL(aSocket);
    CSmsMessage* smsMessage = RecvSmsL(aSocket);

    //Check the status report
    CleanupStack::PushL(smsMessage);
    TBool isSR = (smsMessage->Type()==CSmsPDU::ESmsStatusReport);

    if (isSR)
        {
        INFO_PRINTF1(_L("Received status report"));
        TSmsServiceCenterAddress telephoneNumber=smsMessage->ToFromAddress();
        TEST(telephoneNumber==aRecipientNumber);
        INFO_PRINTF2(_L("Message delivered to %S"), &telephoneNumber);
        }
    else
        {
        INFO_PRINTF1(_L("Received SMS is NOT a Status report!"));
        }

    TEST(isSR);
    
    //Get the status report
    CSmsStatusReport& statusReport = STATIC_CAST(CSmsStatusReport&, smsMessage->SmsPDU()); 
    TSmsStatus::TSmsStatusValue status = statusReport.Status();
    
    CleanupStack::PopAndDestroy(smsMessage);
    return status;
    }

EXPORT_C void CSmsBaseTestStep::WaitForRecvL(RSocket& aSocket)
/**
 *  Wait for an Sms to be received
 *  @param aSocket The status is return to this socket
 *  @leave Leaves if receiving is completed with error code
 */
    {
    INFO_PRINTF1(_L("Waiting for incoming SMS...") );
    TPckgBuf<TUint> sbuf;
    sbuf()=KSockSelectRead;
    TRequestStatus status;
    aSocket.Ioctl(KIOctlSelect,status,&sbuf,KSOLSocket);
    User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrNone, "Waiting for incoming SMS");
    }

EXPORT_C void CSmsBaseTestStep::TestSmsContentsL(CSmsMessage* aSms, const TDesC& aDes, TBool aIgnorePrintOutput)
/**
 *  Check that aSms contains text that matches to aDes
 *  @param aSms SMS message that has been come from SMS stack
 *  @param aDes SMS message's text that is defined at client side
 *  @leave Leaves if aSms doesn't match to aDes
 */
    {
    CSmsBufferBase& smsBuffer=aSms->Buffer();
    TInt bufLen=smsBuffer.Length();
    
    INFO_PRINTF2(_L("Length of buffer is : %d"),bufLen);
        
    HBufC* textBuf=HBufC::NewL(bufLen);
    CleanupStack::PushL(textBuf);
    TPtr textPtr(textBuf->Des());
    smsBuffer.Extract(textPtr,0,bufLen);

    INFO_PRINTF1(_L("Comparing messages..."));
    TInt  compareResult = textPtr.Compare(aDes);

    if (!aIgnorePrintOutput)
        {
        TInt  bufLen2 = aDes.Length();
        HBufC*  textBuf2 = HBufC::NewL(aDes.Length());
        CleanupStack::PushL(textBuf2);
        TPtr textPtr2(textBuf2->Des());
        TInt  index;
        
        for (index = 0;  index < bufLen;  index++)
            {
            if (textPtr[index] < 0x20  ||  textPtr[index] > 0xff)
                {
                textPtr[index] = 0x007f;
                }
            }
        INFO_PRINTF2(_L("%S"), &textPtr);

        INFO_PRINTF1(_L("with expected"));

        textPtr2.Copy(aDes);
        for (index = 0;  index < bufLen2;  index++)
            {
            if (textPtr2[index] < 0x20  ||  textPtr2[index] > 0xff)
                {
                textPtr2[index] = 0x007f;
                }
            }
        INFO_PRINTF2(_L("%S"), &textPtr2);

        CleanupStack::PopAndDestroy(textBuf2);
        }
        
    if (compareResult != 0)
        {
        SetTestStepResult(EFail);
        ERR_PRINTF1(_L("Message content does not match the provided string")); 
        }
    else
        {
        INFO_PRINTF1(_L("Message content matches the provided string"));  
        }
    
    CleanupStack::PopAndDestroy(textBuf);
    }

EXPORT_C void CSmsBaseTestStep::TestMessageContentsL(CSmsMessage* aSms, const TTestCase& aTestCase)
/**
 *  Check the sms matches the expected test result
 *  @param aSms has the message to be tested with aTestCase.iMsg
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if TSmsClass isn't defined at the pdu
 *  @leave Leaves if class of pdu doesn't match to supposed class (aTestCase.iSmsClass)
 */
    {
    TestSmsContentsL(aSms,aTestCase.iMsg);
    CSmsPDU& pdu = aSms->SmsPDU();

    if (aTestCase.iTestSmsClass)
        {
        TSmsDataCodingScheme::TSmsClass smsClass;
        TBool isDefined = pdu.Class(smsClass);
        TEST(isDefined);
        TEST(smsClass == aTestCase.iSmsClass);
        }
    if (aTestCase.iTestIndicators)
        {
        TEST(pdu.Bits7To4() == aTestCase.iBits7To4);
        TEST(pdu.IndicationType() == aTestCase.iIndicationType);
        TEST(pdu.IndicationState() == aTestCase.iIndicationState);
        }

    }

EXPORT_C void CSmsBaseTestStep::WriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket)
/**
 *  This method stores SMS messages to the SMS storage.
 *  @param aSms SMS message that will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if store request is completed with error code
 */
    {
    INFO_PRINTF1(_L("Write message to SIM"));
    TRequestStatus status;
    RSmsSocketWriteStream writestream(aSocket);

    TRAPD(ret,writestream << aSms);
    TESTCHECK(ret, KErrNone, "Write SMS to stream");
    TRAP(ret,writestream.CommitL());
    TESTCHECK(ret, KErrNone, "Commit the stream writing");

    aSocket.Ioctl(KIoctlWriteSmsMessage,status,NULL,KSolSmsProv);
    User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrNone, "Write message to SIM");
    }

EXPORT_C void CSmsBaseTestStep::WriteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket)
/**
 *  This method stores SMS messages to the SMS storage.
 *  @param aSms SMS message that will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 */
    {
    TRequestStatus status;
    RSmsSocketWriteStream writestream(aSocket);
    
    writestream << aSms;
    writestream.CommitL();
    
    aSocket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
    User::WaitForRequest(status);
    INFO_PRINTF2(_L("Write SMS message returned %d"), status.Int());
    if(status.Int() != KErrNone)
        {
        User::Leave(status.Int());
        }
    }

EXPORT_C void CSmsBaseTestStep::ReadSmsStoreL(RSocket& aSocket, RPointerArray<CSmsMessage>& aMessages)
/**
 *  This method retrieves SMS messages from SMS storage and print them out.
 *  @param aSocket Used to stream SMS messages from the socket server
 *  @param aMessages Sms messages will be streamed to this array
 *  @leave Leaves if first request is NOT completed with KErrCancel
 *  @leave Leaves if second request is completed with error code
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 *  @leave Leaves if nack of reading is completed with error code
 *  @leave Leaves if ack of reading is completed with error code
 */
    {
    TRequestStatus status;
    ReadSmsStoreL(aSocket, aMessages, status);
    }

EXPORT_C void CSmsBaseTestStep::ReadSmsStoreL(RSocket& aSocket, RPointerArray<CSmsMessage>& aMessages, TRequestStatus& aStatus)
/**
 *  This method retrieves SMS messages from SMS storage and print them out.
 *  @param aSocket Used to stream SMS messages from the socket server
 *  @param aMessages Sms messages will be streamed to this array
 *  @param aStatus Status of request to enumerate messages from store
 *  @leave Leaves if first request is NOT completed with KErrCancel
 *  @leave Leaves if second request is completed with error code
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 *  @leave Leaves if nack of reading is completed with error code
 *  @leave Leaves if ack of reading is completed with error code
 */
    {
    INFO_PRINTF1(_L("Enumerating messages"));
    TPckgBuf<TUint> sbuf;
    sbuf()=0;
    
    // Enumerate messages from store - NOTE - sometimes SIM.TSY returns
    // KErrInUse (strange timing things!). In this case wait and repeat.
    const TInt KMaxAttempts = 3;
    TInt attempts = 0;
    do
        {
        User::After(2000000);   // Wait a couple of seconds...

        aSocket.Ioctl(KIoctlEnumerateSmsMessages, aStatus, &sbuf, KSolSmsProv);
        User::WaitForRequest(aStatus);
        
        INFO_PRINTF3(_L("Enumerating completed [status=%d, attempts=%d]"), aStatus.Int(), ++attempts);        
        } while( aStatus.Int() == KErrInUse && attempts < KMaxAttempts );
    
    if( aStatus.Int() == KErrNone )
        {
        // sbuf() includes the count of messages on Store
        TInt count = sbuf();
        INFO_PRINTF2(_L("%d enumerated messages"), count);
        
        RSmsSocketReadStream readstream(aSocket);
        
        //Read each message from the stream
        for(TInt i=0; i< count; ++i)
            {
            CSmsBuffer* buffer=CSmsBuffer::NewL();
            CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
            CleanupStack::PushL(smsmessage);
        
            TRAPD(ret,readstream >> *smsmessage);
            TEST(ret == KErrNone);
            aSocket.Ioctl(KIoctlReadMessageSucceeded, aStatus, NULL, KSolSmsProv);
            User::WaitForRequest(aStatus);
            TEST(aStatus.Int() == KErrNone);
            
            PrintMessageDetailedL(smsmessage);
            
            User::LeaveIfError(aMessages.Append(smsmessage));
            CleanupStack::Pop(smsmessage);
            }
        }
    else
        {
        ERR_PRINTF2(_L("Enumerating failed [status=%d]"), aStatus.Int());
        SetTestStepResult(EFail);
        }
    }

EXPORT_C TInt CSmsBaseTestStep::DeleteSmsL(const CSmsMessage& aSms, RSocket& aSocket, TInt aExpectedError /*KErrNone*/)
/**
 *  This method deletes SMS message from the SMS storage.
 *  @param aSms SMS message that will be deleted
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @return TInt :error code
 */
    {
    INFO_PRINTF2(_L("Delete message from store [aExpectedError=%d]"), aExpectedError);

    TRequestStatus status;

    RSmsSocketWriteStream writestream(aSocket);
    TRAPD(ret,writestream << aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream.CommitL());
    TEST(ret == KErrNone);

    aSocket.Ioctl(KIoctlDeleteSmsMessage, status, NULL, KSolSmsProv);
    User::WaitForRequest(status);
    TInt error = status.Int();
    
    INFO_PRINTF2(_L("Delete SMS message - returned %d"), error);    

    if( error != aExpectedError )
        {
        ERR_PRINTF3(_L("Delete SMS message failed [aExpectedError=%d, error=%d]"), aExpectedError, error);
        SetTestStepResult(EFail);
        }
    return error;
    }

EXPORT_C void CSmsBaseTestStep::DeleteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket)
/**
 *  This method deletes SMS message from the SMS storage.
 *  @param aSms SMS message that will be deleted
 *  @param aSocket Used to stream SMS message to the sms stack
 */
    {
    TRequestStatus status;
    RSmsSocketWriteStream writestream(aSocket);
    writestream << aSms;
    writestream.CommitL();
    aSocket.Ioctl(KIoctlDeleteSmsMessage, status, NULL, KSolSmsProv);
    User::WaitForRequest(status);
    INFO_PRINTF2(_L("Delete SMS returned %d"), status.Int());
    if(status.Int() != KErrNone)
        {
        User::Leave(status.Int());
        }
    }

EXPORT_C void CSmsBaseTestStep::SetIndicatorL(const TTestCase& aTestCase, CSmsMessage* aSms)
/**
 *  
 */
    {
    TSmsDataCodingScheme::TSmsAlphabet alphabet;
    if (aTestCase.iBits7To4 == TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2)
        alphabet = TSmsDataCodingScheme::ESmsAlphabetUCS2;
    else
        alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;

    TSmsUserDataSettings smsSettings;
    smsSettings.SetAlphabet(alphabet);
    smsSettings.SetTextCompressed(EFalse);
    aSms->SetUserDataSettingsL(smsSettings);

    CSmsPDU& pdu = aSms->SmsPDU();
    pdu.SetBits7To4(aTestCase.iBits7To4);
    pdu.SetIndicationType(aTestCase.iIndicationType);
    pdu.SetIndicationState(aTestCase.iIndicationState);

    }

EXPORT_C void CSmsBaseTestStep::FillDes(TDes& aDes,TInt aLength)
/**
 *  Fill aDes with randomly generated 7bit data
 *  @param aDes will be filled with random data
 *  @param aLength has the length to be set to aDes
 */
    {
    TText baseChar='A';

    aDes.SetLength(aLength);
    for (TInt i=0; i<aLength; i++)
        {
        aDes[i]=TText(baseChar + i%26);
        }
    }

EXPORT_C TInt CSmsBaseTestStep::MakeReadSmsStoreRequestL(RSocket& aSocket)
/**
 *  This method retrieves SMS messages from SMS storage.
 *  Main purpose is to test out of memory conditions.
 *  @param aSocket Used to stream SMS messages from the sms stack
 *  @param aMessages reference to CSmsMessage pointer array.
 */
    {
    TRequestStatus status;
    TPckgBuf<TUint> sbuf;
    sbuf()=0;

    //Now enumerate messages from SIM
    aSocket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
    User::WaitForRequest(status);
    INFO_PRINTF2(_L("Read SMS returned %d"), status.Int());
    if(status.Int() != KErrNone)
        {
        User::Leave(status.Int());
        }

    //sbuf() includes the count of messages on SIM
    return sbuf();
    }

EXPORT_C TBool CSmsBaseTestStep::TimedWaitForRecvL(RSocket& aSocket, TUint aDelay)
/**
 *  Wait for up to the delay for an Sms to be received
 *  @param aSocket The status is return to this socket
 *  @param aDelay Maximum time to wait for receipt
 *  @return ETrue if data received
 *  @leave Leaves if receiving is completed with error code
 */
    {
    TRequestStatus timerStatus;
    RTimer timer;
    timer.CreateLocal();
    timer.After(timerStatus, TTimeIntervalMicroSeconds32(aDelay));

    TPckgBuf<TUint> sbuf;
    sbuf()=KSockSelectRead;
    TRequestStatus ioctlStatus;
    aSocket.Ioctl(KIOctlSelect, ioctlStatus, &sbuf, KSOLSocket);
    User::WaitForRequest(timerStatus, ioctlStatus);
    TBool retval;
    
    if(ioctlStatus.Int() != KErrNone)
        {
        aSocket.CancelIoctl();
        User::WaitForRequest(ioctlStatus);
        retval = EFalse;
        }
    else
        {
        timer.Cancel();
        User::WaitForRequest(timerStatus);
        retval = ETrue;
        }
    timer.Close();
    return retval;
    }

EXPORT_C TInt CSmsBaseTestStep::CancelWriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket, TInt aDelay)
/**
 *  This method stores SMS messages to the SMS storage.
 *  @param aSms SMS message that will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if store request is completed with error code
 */
    {
    INFO_PRINTF1(_L("Write message"));

    TRequestStatus status;

    RSmsSocketWriteStream writestream(aSocket);
    TRAPD(ret,writestream << aSms);
    TEST(ret == KErrNone);
    TRAP(ret,writestream.CommitL());
    TEST(ret == KErrNone);

    aSocket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
    INFO_PRINTF1(_L("Cancel"));
    User::After(aDelay);
    aSocket.CancelIoctl();
    User::WaitForRequest(status);
    INFO_PRINTF2(_L("WriteSmsToSimL - returned %d"), status.Int());

    return status.Int();
    }

EXPORT_C TBool CSmsBaseTestStep::DoSingleTestCaseL( const TDesC8& aSection, TInt aCount )
    {
    CTestConfig* configFile = CTestConfig::NewLC(iFs, KGmsSmsConfigFileDir, KConfigFilename);

    TBuf8<64> sectionName(aSection);
    sectionName.AppendNum(aCount);
    const CTestConfigSection* section = NULL;
    TBool found( ETrue );

    if ( ( section = configFile->Section( sectionName ) ) != NULL )
        {
        if ( aSection == KTestSendAndRecvMsgsWithDifferentTON )
            {
            TInt num( 1 );
            CArrayFixFlat<TInt>* alphabetArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
            CleanupStack::PushL(alphabetArray);
            CArrayFixFlat<TInt>* typeOfNumberArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
            CleanupStack::PushL(typeOfNumberArray);

            TBuf8<32> itemName( KAlphabet );
            itemName.AppendNum( num );

            TInt param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
            while ( param != KErrNotFound )
                {
                alphabetArray->AppendL(param);
                itemName = KAlphabet;
                itemName.AppendNum( ++num );
                param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
                }
            num = 1;
            itemName = KTypeOfNumber;
            itemName.AppendNum( num );

            param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
            while ( param != KErrNotFound )
                {
                typeOfNumberArray->AppendL(param);
                itemName = KTypeOfNumber;
                itemName.AppendNum( ++num );
                param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
                }

            RPointerArray<CSmsPduDbMessage> array;
            CleanupResetAndDestroyPushL(array);
            ReadPduL( sectionName, array );
            TestSendAndRecvMsgsWithDifferentTONL( array, alphabetArray, typeOfNumberArray );
            CleanupStack::PopAndDestroy(&array);
            CleanupStack::PopAndDestroy(typeOfNumberArray);
            CleanupStack::PopAndDestroy(alphabetArray);
            }
        else if ( aSection == KTestSendAndReceiveIndicatorMsgs )
            {
            TInt num( 1 );
            CArrayFixFlat<TInt>* indicationTypeArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
            CleanupStack::PushL(indicationTypeArray);
            CArrayFixFlat<TInt>* dcsBits7To4Array = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
            CleanupStack::PushL(dcsBits7To4Array);
            CArrayFixFlat<TInt>* indicationStateArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
            CleanupStack::PushL(indicationStateArray);

            TBuf8<32> itemName( KIndicationType );
            itemName.AppendNum( num );

            TInt param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
            while ( param != KErrNotFound )
                {
                indicationTypeArray->AppendL(param);
                itemName = KIndicationType;
                itemName.AppendNum( ++num );
                param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
                }
            num = 1;
            itemName = KDCSBits7To4;
            itemName.AppendNum( num );

            param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
            while ( param != KErrNotFound )
                {
                dcsBits7To4Array->AppendL(param);
                itemName = KDCSBits7To4;
                itemName.AppendNum( ++num );
                param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
                }
            num = 1;
            itemName = KIndicationState;
            itemName.AppendNum( num );

            param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
            while ( param != KErrNotFound )
                {
                indicationStateArray->AppendL(param);
                itemName = KIndicationState;
                itemName.AppendNum( ++num );
                param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
                }

            RPointerArray<CSmsPduDbMessage> array;
            CleanupResetAndDestroyPushL(array);
            ReadPduL( sectionName, array );
            TestSendAndReceiveIndicatorMsgsL( array, indicationStateArray, dcsBits7To4Array, indicationTypeArray );
            CleanupStack::PopAndDestroy(&array);
            CleanupStack::PopAndDestroy(indicationStateArray);
            CleanupStack::PopAndDestroy(dcsBits7To4Array);
            CleanupStack::PopAndDestroy(indicationTypeArray);
            }
        else
            found = EFalse;
        }
    else
        found = EFalse;

    CleanupStack::PopAndDestroy(configFile);//configFile
    return found;
    }

EXPORT_C void CSmsBaseTestStep::TestSendAndRecvMsgsWithDifferentTONL( const RPointerArray<CSmsPduDbMessage>& aArray,
                                                   const CArrayFixFlat<TInt>* aAlphabetArray,
                                                   const CArrayFixFlat<TInt>* aTypeOfNumberArray )
    {
    INFO_PRINTF1(_L("Send and receive messages with different type of number"));
    
    const TInt testNumber = 39;
    SetSimTSYTestNumberL(testNumber);
    
    RSocketServ socketServer;
    ConnectSocketServerLC(socketServer);

    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TSmsDataCodingScheme::TSmsAlphabet alphabet = ( TSmsDataCodingScheme::TSmsAlphabet )aAlphabetArray->At(0);

    TInt bufLen=aArray[0]->iSmsMessage->Buffer().Length();
    HBufC* textBuf=HBufC::NewL(bufLen);
    CleanupStack::PushL(textBuf);
    TPtr testText(textBuf->Des());
    aArray[0]->iSmsMessage->Buffer().Extract(testText,0,bufLen);

    CSmsMessage* sendMessage=CreateSmsMessageLC(testText,alphabet);
    CSmsSubmit& submitPdu=(CSmsSubmit&)sendMessage->SmsPDU();
    TGsmSmsTelNumber gsmSmsTelNumber;
    gsmSmsTelNumber.iTelNumber.Append(iTelephoneNumber);
    if(gsmSmsTelNumber.iTelNumber.Length() > 0 && gsmSmsTelNumber.iTelNumber[0] == '+')
    {
        // Remove "+" sign from telephony number
        gsmSmsTelNumber.iTelNumber.Delete(0,1);
        gsmSmsTelNumber.iTelNumber.Trim();
    }

    for ( TInt i = 0; i < aTypeOfNumberArray->Count(); i++ )
        {
        gsmSmsTelNumber.iTypeOfAddress.SetTON( ( TGsmSmsTypeOfNumber )aTypeOfNumberArray->At(i) );
        sendMessage->SetParsedToFromAddressL(gsmSmsTelNumber);

        if ( ( TGsmSmsTypeOfNumber )aTypeOfNumberArray->At(i) == EGsmSmsTONSubscriberNumber )
            {
            submitPdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);
            submitPdu.SetShortMessageType(TSmsProtocolIdentifier::ESmsReturnCallMesage);
            }
        INFO_PRINTF2(_L("Send SMS message. Type of number is %d"), aTypeOfNumberArray->At(i) );
        SendSmsL(sendMessage,socket);

        INFO_PRINTF1(_L("Waiting for incoming SMS...") );
        WaitForRecvL(socket);
        CSmsMessage* recvMessage = RecvSmsL(socket);
        delete recvMessage;
        }

    CleanupStack::PopAndDestroy(4, &socketServer); // socketServer, socket, textBuf, sendMessage
    }

EXPORT_C void CSmsBaseTestStep::TestSendAndReceiveIndicatorMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
                                               const CArrayFixFlat<TInt>* aIndicationStateArray,
                                               const CArrayFixFlat<TInt>* aDcsBits7To4Array,
                                               const CArrayFixFlat<TInt>* aIndicationTypeArray )
/**
 *  @test Send and receive different indicator messages
 *  TODO ESmsDCSMessageWaitingIndicationDiscardMessage test doesn't work with Nokia 6210 because phone
 *  doesn't route incoming message to the MM TSY.
 */
    {

    INFO_PRINTF1(_L("Send and receive different indicator messages"));
    
    const TInt testNumber = 40;
    SetSimTSYTestNumberL(testNumber);
    
    RSocketServ socketServer;
    ConnectSocketServerLC(socketServer);

    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    for ( TInt i = 0; i < aArray.Count(); i++ )
        {

        TSmsDataCodingScheme::TSmsIndicationState indicationState =
            ( TSmsDataCodingScheme::TSmsIndicationState )aIndicationStateArray->At(i);
        TSmsDataCodingScheme::TSmsDCSBits7To4 dcsBits7To4Array =
            ( TSmsDataCodingScheme::TSmsDCSBits7To4 )aDcsBits7To4Array->At(i);
        TSmsDataCodingScheme::TSmsIndicationType indicationType =
            ( TSmsDataCodingScheme::TSmsIndicationType )aIndicationTypeArray->At(i);

        TInt bufLen=aArray[i]->iSmsMessage->Buffer().Length();
        HBufC* textBuf=HBufC::NewL(bufLen);
        CleanupStack::PushL(textBuf);
        TPtr testText(textBuf->Des());
        aArray[i]->iSmsMessage->Buffer().Extract(testText,0,bufLen);

        TTestCase msg(testText, indicationType, dcsBits7To4Array, indicationState );

        SendAndRecvTestMessageL(msg,socket);

        CleanupStack::PopAndDestroy(textBuf);
        }

    CleanupStack::PopAndDestroy(2, &socketServer); // socketServer, socket
    }

EXPORT_C void CSmsBaseTestStep::ReadPduL( TBuf8<64> aSectionName, RPointerArray<CSmsPduDbMessage>& aArray )
    {
    CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs, aSectionName, KConfigFilename, KConfigFileDir);
    CleanupStack::PushL(db);

    db->GetMessageL(aArray, CSmsPDU::ESmsSubmit);

    CleanupStack::PopAndDestroy(db);
    }

EXPORT_C CSmsMessage* CSmsBaseTestStep::CreateSmsMessageLC(CSmsPDU::TSmsPDUType aType, CSmsBuffer* aBuffer, const TDesC& aAddress)
    {
    CSmsMessage* msg = CSmsMessage::NewL(iFs, aType, aBuffer);
    CleanupStack::PushL(msg);

    if (aAddress.Length() != 0)
        {
        msg->SetToFromAddressL(aAddress);
        }

    return msg;
    }

/*
These functions were grabbed from SMSStackTestUtils.dll
*/
EXPORT_C void CSmsBaseTestStep::OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily)
/**
 *  Initialise an RSocket for SMS, aSocket is NOT pushed to CleanupStack.
 *  @param aSocketServer reference to the socket server object
 *  @param aSocket The socket that will be opened
 *  @param aFamily The sms address family
 *  @param aDualSimAware Flag indicating that client is dual-SIM aware
 */
    {
    TSmsAddr smsaddr;
    smsaddr.SetSmsAddrFamily(aFamily);
    OpenSmsSocketL(aSocketServer, aSocket, smsaddr);
    }

EXPORT_C void CSmsBaseTestStep::OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddr& aSmsAddr)
/*
 *  Initialise an RSocket for SMS, aSocket is NOT pushed to CleanupStack.
 *  @param aSocketServer reference to the socket server object
 *  @param aSocket The socket that will be opened
 *  @param aSmsAddr The sms address to bind to
 */
    {
    INFO_PRINTF2(_L("Opening and binding socket [addrFamily=%d]"), aSmsAddr.SmsAddrFamily());
    
    TInt ret=aSocket.Open(aSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
    TESTCHECK(ret, KErrNone, "Opening socket");

    ret=aSocket.Bind(aSmsAddr);
    TESTCHECK(ret, KErrNone, "Binding to the socket");
    
    TProtocolDesc desc;
    aSocket.Info(desc);
    INFO_PRINTF2(_L("Protocol name: %S"), &desc.iName);
	
    TTimeIntervalMicroSeconds32 InitPause=9000000;  //Required Pause to Allow SMSStack to Complete its Async Init
    User::After(InitPause);                         //call to the TSY and finish its StartUp.

    }

/*
    Utility for creating a SMS message from test data (ini) file
*/
EXPORT_C CSmsMessage* CSmsBaseTestStep::CreateSMSL()
    {
    INFO_PRINTF1(_L("Creating SMS...") );
    
    TInt codingScheme;
    GetIntFromConfig(ConfigSection(), _L("messageCoding"), codingScheme);
    
    TCodingScheme dataCodingScheme = (TCodingScheme)codingScheme;   
    
    TSmsDataCodingScheme::TSmsAlphabet alphabet;
    if (dataCodingScheme == ESevenBit)
        {   
        //8 bit coding scheme
        alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
        }

    else 
        {
        alphabet = TSmsDataCodingScheme::ESmsAlphabet8Bit;
        }       
    
    TPtrC messageText;
    GetStringFromConfig(ConfigSection(), _L("message"), messageText);
    CSmsMessage *sms = CreateSmsMessageL(messageText, alphabet);
    
    INFO_PRINTF1(_L("SMS created") );
    PrintMessageL(sms);
    
    return sms;
    }

/**
 * Sets high and low limits in .RSC file and then reserves disk space to match requested levels.
 * 
 * Checks the current free space and then sets the high and low marks
 * to be aHighDrop MB and aLowDrop MB below the current free space 
 * level.
 * 
 * Then diskspace is reserved to aFreeDrop MB below the current free
 * space level.
 * 
 * If the current free space level is greater then aMax then the 
 * current free space level is set to aMax
 * 
 * If the current free space level is less than aLowDrop MB then this
 * method leaves with KErrArgument.
 * 
 * @param   aHighDrop   The number of MB below the current free space level for the high level mark (in the .RSC file)
 * @param   aLowDrop    The number of MB below the current free space level for the low level mark (in the .RSC file)  
 * @param   aFreeDrop   The number of MB below the current free space level for the low level mark
 * @param   aMax        The maximum level for the high limit allowed
 * 
 * @return  The max current free space level used.
 * 
 * @leave   KErrArgument if the current free diskspace level is less than aLowDrop MB
 * @leave   KErrArgument if aMax is not greater than aLowDrop MB
 * @leave   KErrArgument if aHighDrop >= aLowDrop  
 */
EXPORT_C TUint64 CSmsBaseTestStep::SetHighLowLimitsAndDiskSpaceLevelL(TUint aHighDrop, TUint aLowDrop, TUint aFreeDrop, TUint64 aMax/*=0x7fffffff*/)
    {
    INFO_PRINTF5(_L("Setting High-Low limits and Free Diskspace levels [aHighDrop=%u, aLowDrop=%u, aFreeDrop=%u, aMax=%ld]"), aHighDrop, aLowDrop, aFreeDrop, aMax);
    
    __ASSERT_ALWAYS( (aMax > (aLowDrop*1024*1024)), User::Leave(KErrArgument));
    __ASSERT_ALWAYS( (aLowDrop > aHighDrop), User::Leave(KErrArgument));
    
    ReleaseDiskSpaceL();
   
    TVolumeInfo  volumeInfo;
    User::LeaveIfError(iFs.Volume(volumeInfo, EDriveC));
    INFO_PRINTF2(_L("  Drive C currently has %ld bytes free."), volumeInfo.iFree);
    
    TUint64 current = volumeInfo.iFree;
    if( current < (aLowDrop*1024*1024) )
        {
        INFO_PRINTF1(_L("  Drive C already has too little free space!"));
        User::Leave(KErrArgument);
        }
    if( current > aMax )
        {
        current = aMax;
        }
    TUint64 high = current - (aHighDrop*1024*1024);
    TUint64 low  = current - (aLowDrop*1024*1024);
        
    SetLowHighLimitsInSmsuRscL(low, high);
    if( aFreeDrop != 0 )
        {
        TUint64 free = current - (aFreeDrop*1024*1024);
        SetFreeDiskSpaceL(free);
        }
    
    return current;
    }

/**
 * Reserves disk space to match requested free space - a number MB below the current free space level.
 * 
 * Checks the current free space and diskspace is reserved to aFreeDrop
 * MB below the current free space level.
 * 
 * If the current free space level is greater then aMax then the 
 * current free space level is set to aMax
 * 
 * If the current free space level is less than aLowDrop MB then this
 * method leaves with KErrArgument.
 * 
 * @param   aFreeDrop   The number of MB below the current free space level for the low level mark
 * @param   aMax        The maximum level for the high limit allowed
 * 
 * @return  The max current free space level used.
 * 
 * @leave   KErrArgument if the current free diskspace level is less than aLowDrop MB
 * @leave   KErrArgument if aMax is not greater than aLowDrop MB
 * @leave   KErrArgument if aHighDrop >= aLowDrop  
 */
EXPORT_C void CSmsBaseTestStep::SetFreeDiskSpaceFromDropLevelL(TUint aFreeDrop, TUint64 aMax/*=0x7fffffff*/)
    {
    INFO_PRINTF3(_L("Setting Free Diskspace level [aFreeDrop=%u, aMax=%ld]"), aFreeDrop, aMax);
    
    if( aFreeDrop == 0)
        {
        return;
        }
    
    TVolumeInfo  volumeInfo;
    User::LeaveIfError(iFs.Volume(volumeInfo, EDriveC));
    TUint64 current = volumeInfo.iFree;
    if( current > aMax )
        {
        current = aMax;
        }    
    TUint64 free = current - (aFreeDrop*1024*1024);
    SetFreeDiskSpaceL(free);    
    }

/**
 *  Reserves disk space so that a specified amount of free disk space is
 *  available.
 *
 *  @param aNewFreeValue  Amount of free space required.
 */
EXPORT_C void CSmsBaseTestStep::SetFreeDiskSpaceL(TInt64 aNewFreeValue)
    {
    
#ifndef _DEBUG
    (void)aNewFreeValue; // added to aviod compiler warning, as only used in test mode
    
    ERR_PRINTF1(_L("Unexpected call: CSMSTestSteps::SetFreeDiskSpaceL() is expected to be called only in DEBUG mode."));
    User::Leave(KErrNotSupported);
#else
    INFO_PRINTF2(_L("Setting Drive C free disk space to %ld bytes."), aNewFreeValue);    
    
    __ASSERT_DEBUG( (aNewFreeValue <= 0x7fffffff), User::Leave(KErrArgument));

    TInt err = RProperty::Set(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey, (TInt)aNewFreeValue);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("RProperty::Set() failure [err=%d]"), err);
        User::Leave(err);
        }
#endif           
    } // CSMSTestSteps::SetFreeDiskSpaceL


/**
 *  Release all reserved disk space.
 */
EXPORT_C void CSmsBaseTestStep::ReleaseDiskSpaceL()
    {    
#ifndef _DEBUG
    ERR_PRINTF1(_L("Unexpected call: CSMSTestSteps::ReleaseDiskSpaceL() is expected to be called only in DEBUG mode."));
    User::Leave(KErrNotSupported);
#else
    INFO_PRINTF1(_L("CSMSTestSteps::ReleaseDiskSpaceL()"));
    
    TVolumeInfo  volumeInfo;
    User::LeaveIfError(iFs.Volume(volumeInfo, EDriveC));
    TUint64 current = volumeInfo.iFree;
    if( current > 0x7fffffff )
        {
        current = 0x7fffffff;
        }
    SetFreeDiskSpaceL(current);
#endif    
    }

 /**
  *  Set high and low limits in .RSC file. When the SMS Stack starts the limits
  *  will be loaded as if set by the licensee.
  *
  *  @param aLowLimit   Low limit value.
  *  @param aHighLimit  High limit value.
  *
  *  @note Only works in debug mode for security reasons.
  */ 
EXPORT_C void CSmsBaseTestStep::SetLowHighLimitsInSmsuRscL(TInt64 aLowLimit, TInt64 aHighLimit)
    {
    INFO_PRINTF3(_L("Setting high and low .RSC limits to %ld and %ld."),
                 aHighLimit, aLowLimit);

    __ASSERT_ALWAYS(aLowLimit  < 0x7fffffff, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(aHighLimit < 0x7fffffff, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(aLowLimit  < aHighLimit, User::Leave(KErrArgument));

    //
    // Data for the SMSU resource file. The low limit is written at position
    // 20 and the high limit at position 24.
    //
    const TInt  smsuRscSize = 34;
    TChar  smsuRscData[smsuRscSize] =
                {0x6b, 0x4a, 0x1f, 0x10, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x19, 0xfd, 0x48, 0xe8,
                 0x01, 0x04, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12,
                 0x87, 0x65, 0x43, 0x21, 0x14, 0x00, 0x18, 0x00, 
                 0x1c, 0x00};
                              
    smsuRscData[20] = (aLowLimit  >>  0) & 0xff;
    smsuRscData[21] = (aLowLimit  >>  8) & 0xff;
    smsuRscData[22] = (aLowLimit  >> 16) & 0xff;
    smsuRscData[23] = (aLowLimit  >> 24) & 0xff;
    smsuRscData[24] = (aHighLimit >>  0) & 0xff;
    smsuRscData[25] = (aHighLimit >>  8) & 0xff;
    smsuRscData[26] = (aHighLimit >> 16) & 0xff;
    smsuRscData[27] = (aHighLimit >> 24) & 0xff;

    TBuf8<smsuRscSize>  smsuRscBuffer;

    for (TInt index = 0;  index < smsuRscSize;  index++)
        {
        smsuRscBuffer.Append(smsuRscData[index]);
        }

    //
    // Ensure the target directory exists...
    //
    TInt  ret;
    
    ret = iFs.MkDir(KSMSUResourceDir);
    if (ret != KErrNone  &&  ret != KErrAlreadyExists)
        {
        User::Leave(ret);
        }

    //
    // Write the RSC file to the private C:\ directory...
    //
    RFile  file;

    User::LeaveIfError(file.Replace(iFs, KSMSUResourceFile, EFileWrite));
    CleanupClosePushL(file);
    User::LeaveIfError(file.Write(smsuRscSize, smsuRscBuffer));
    CleanupStack::PopAndDestroy(&file);
    } // CSmsBaseTestStep::SetLowHighLimitsInSmsuRscL


/**
 *  Removes the low and high limits and returns the SMSU.RSC to defauls.
 */
EXPORT_C void CSmsBaseTestStep::RemoveLowHighLimitsFromSmsuRscL()
    {
    INFO_PRINTF1(_L("Removing .RSC limits."));

    //
    // Remove the RSC file...
    //
    TInt  ret;
    
    ret = iFs.Delete(KSMSUResourceFile);
    if (ret != KErrNone  &&  ret != KErrNotFound)
        {
        User::Leave(ret);
        }
    } // CSmsBaseTestStep::RemoveLowHighLimitsFromSmsuRscL

EXPORT_C void CSmsBaseTestStep::ChangeReceiveModeL(RMobileSmsMessaging::TMobileSmsReceiveMode aNewRcvMode)
    {

    // Create comms database object
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(db);

    INFO_PRINTF1(_L("Testing recvMode change to EReceiveUnstoredClientAck"));

    // EReceiveUnstoredClientAck
    CMDBField<TUint32>* smsReceiveModeField = new(ELeave) CMDBField<TUint32>(KCDTIdSMSReceiveMode);
    CleanupStack::PushL(smsReceiveModeField);
    smsReceiveModeField->SetRecordId(1); //it's GlobalSettingsRecord
    *smsReceiveModeField = aNewRcvMode;
    smsReceiveModeField->ModifyL(*db);
    CleanupStack::PopAndDestroy(smsReceiveModeField);
    CleanupStack::PopAndDestroy(db);
    }

EXPORT_C TInt CSmsBaseTestStep::GetIntegerFromConfigL(const TDesC& aKey)
    {
    TInt value (0);
    if(!GetIntFromConfig( ConfigSection(), aKey, value))
        {
        ERR_PRINTF2( _L("%S is missing in the configuration file!"),  &aKey);
        User::Leave(KErrNotFound);     
        }
    return value;
    }

EXPORT_C TPtrC CSmsBaseTestStep::GetStringFromConfigL(const TDesC& aKey)
    {
    TPtrC value;
    if(!GetStringFromConfig(ConfigSection(), aKey, value))
        {
        ERR_PRINTF2(_L("%S is missing in the configuration file!"), &aKey);
        User::Leave(KErrNotFound);
        }
    return value;
    }

EXPORT_C void CSmsBaseTestStep::UpdatePhonePowerStatusL(RProperty& aPhonePowerProperty, TSAPhoneStatus aNewStatus)
    {
    INFO_PRINTF2(_L("Updating phone power status [aNewStatus=%d]"), aNewStatus);
    
    TInt currentStatus;
    User::LeaveIfError(aPhonePowerProperty.Get(currentStatus));
    
    while( currentStatus != aNewStatus  )
        {
        TRequestStatus status;
        aPhonePowerProperty.Subscribe(status);
        User::LeaveIfError(aPhonePowerProperty.Set(KUidSystemCategory, KUidPhonePwr.iUid, aNewStatus));
        User::After(5 * 1000000);   // sleep 5 secs;
        User::WaitForRequest(status);
        TEST(status.Int() == KErrNone);
        User::LeaveIfError(aPhonePowerProperty.Get(currentStatus));
        }
    }

// Build a list of instances of CPMs running ESOCKSVR.DLL
void CSmsBaseTestStep::BuildESockCPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList)
    {
    TRSIter iter;
    TCFModuleName modName;
    TRSModuleInfo modInfo;
    aInfo.ResetAndDestroy();
    aDispList.SetLength(0);
    
    while (aRootServer.EnumerateModules(iter, modName) == KErrNone)
        {
        if (aRootServer.GetModuleInfo(modName, modInfo) == KErrNone  &&
                modInfo.iParams.iDll.MatchF(_L("*ESOCKSVR.DLL")) >= 0)
            {
            TESockSvrUnloadInfo* unloadInfo = new(ELeave) TESockSvrUnloadInfo;
            unloadInfo->iName.Copy(modInfo.iParams.iName);
            unloadInfo->iState=modInfo.iParams.iState;
            unloadInfo->iStatus=KErrNone;
            
            aDispList.Append(unloadInfo->iName);
            aDispList.Append(_L8(" "));
            
            TInt err = aInfo.Append(unloadInfo);
            TESTCHECKL(err, KErrNone, "Appending unloading info");
            }
        }
    }

EXPORT_C void CSmsBaseTestStep::DoESockMemoryLeakTestL()
    {
    INFO_PRINTF1(_L("Checking for ESock memory leaks..."));
    TInt  ret, startLeakCounter;

    // Find the current number of leaked cells in ESock
    ret = RProperty::Get(KUidCommsProcess, KUidCommsModuleLeakCounter, startLeakCounter);
    if (ret == KErrNotFound)
        {
        // No variable to monitor, ESock is probably not in debug mode.
        INFO_PRINTF1(_L("<font size=2 color=00FFCC><B>ESock is probably not in debug mode.</B></font>"));       
        return;
        }
    
    TESTCHECKL(ret, KErrNone, "Get the Comms Module Leak Counter")

    // With the Staged Start-up Architecture it is likely that ESOCK modules continue to load after the ECoreComponentsStarted
    // state which releases StartC32() and RSocketServ::Connect(). So here we wait until the RootServer is fully configured before
    // starting the shutdown, which avoids various races (modules may not be bound yet, or even loaded at all by the Configurator
    // and hence invisible to this unloading code)
    RProperty configurationProperty;
    CleanupClosePushL(configurationProperty);
    
    configurationProperty.Attach(KUidSystemCategory, KUidC32StartPropertyKey.iUid); // needs the KEY
    TInt propertyValue = EInitialising; // set to safe state
    TInt propertyResult = configurationProperty.Get(propertyValue);
    TRequestStatus propertyStatus;
    while(propertyValue < EConfigurationComplete)
        {
        configurationProperty.Subscribe(propertyStatus);
        
        if(configurationProperty.Get(propertyValue) == KErrNone && propertyValue == EConfigurationComplete)
            {
            configurationProperty.Cancel();
            }
        User::WaitForRequest(propertyStatus);
        }
    
    RRootServ  rootserver;
    User::LeaveIfError(rootserver.Connect());
    CleanupClosePushL(rootserver);

    // Find all instances of CPMs running ESOCKSVR.DLL
    RUnloadInfoArray unloadArray(16);
    CleanupClosePushL(unloadArray);
    TBuf8<256> modList;
    
    BuildESockCPMListL(rootserver, unloadArray, modList);

    // Start by asking all of the ESOCK threads to unload when there are no sessions
    TInt index = unloadArray.Count();
    while( index-- > 0 )
        {
        TESockSvrUnloadInfo* info = unloadArray[index];
        rootserver.UnloadCpm(info->iStatus, info->iName, EGraceful);
        }

    // Start polling to see when they all complete unloading
    const TInt KPollPeriod = 2000 * 1000;
    TUint maxPolls = 15;
    
    RUnloadInfoArray pollUnloadArray(16);
    BuildESockCPMListL(rootserver, pollUnloadArray, modList);
    
    while( maxPolls-- > 0 && pollUnloadArray.Count() > 0 )
        {
        User::After(KPollPeriod);
        BuildESockCPMListL(rootserver, pollUnloadArray, modList);
        }
    pollUnloadArray.ResetAndDestroy();
    
    // Cancel any remaining unloads and eat the events
    index = unloadArray.Count();
    while( index-- >0 )
        {
        TESockSvrUnloadInfo* info = unloadArray[index];
        rootserver.CancelUnloadCpm(info->iName);
        User::WaitForRequest(info->iStatus);
        }

    // See what's left
    BuildESockCPMListL(rootserver, unloadArray, modList);

    TInt err = KErrNone; // Will use this to catch first error below
    TInt count = unloadArray.Count();
    
    if(count > 0)
        {
        // No more waiting; we order immediate unloads
        for(TInt i = 0; i < count; ++i)
            {
            TESockSvrUnloadInfo* info = unloadArray[i];
            rootserver.UnloadCpm(info->iStatus, info->iName, EImmediate);
            }
            
        // Wait for them all to return
        for(TInt i = 0; i < count; ++i)
            {
            TESockSvrUnloadInfo* info = unloadArray[i];
            User::WaitForRequest(info->iStatus);
            }
        
        // Display the status of any remaining modules
        for(TInt i = count - 1; i >= 0; --i)
            {
            TESockSvrUnloadInfo* info = unloadArray[i];
            TBuf16<32> tmp;
            tmp.Copy(info->iName);
            if(info->iStatus.Int() != KErrNone && info->iStatus.Int() != KErrRSModuleNotLoaded)
                {
                err = info->iStatus.Int();
                INFO_PRINTF4(_L("%S cannot be gracefully or immediatly unloaded due to error %d and is stuck in state %d"), &tmp, info->iStatus.Int(), info->iState);
                }
            else
                {
                INFO_PRINTF4(_L("%S could not be gracefully unloaded and had to be immediatly unloaded while it was stuck in state %d, status %d"), &tmp, info->iState, info->iStatus.Int());
                }
            }
        }
    CleanupStack::PopAndDestroy(3, &configurationProperty); // configurationProperty, rootserver, unloadArray
    
    // Leave with last seen error, if any
    TESTCHECKL(err, KErrNone, "Unloading CPM modules");
    
    // Get the latest number of leaked cells in ESock
    TInt  finalLeakCounter;

    ret = RProperty::Get(KUidCommsProcess, KUidCommsModuleLeakCounter, finalLeakCounter);
    TESTCHECKL(ret, KErrNone, "Getting Comms Module leak counter");

    TESTCHECKCONDITION(finalLeakCounter <= startLeakCounter, "A memory leak has been detected inside ESock");
    
    // TODO: reviewers - why is it here?
    // Restart C32...
    _LIT(KDummyCMI, "");
    WarmBootC32(KDummyCMI);
    }
