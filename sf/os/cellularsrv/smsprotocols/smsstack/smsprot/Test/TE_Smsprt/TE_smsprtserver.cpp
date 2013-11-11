// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file creates and starts the TE_smsprt DLL.
// 
//

/**
 @file
*/

#include "TE_smsprtserver.h"
#include "TE_smsprt.h"
#include "smsstacktestcase.h"
#include "te_smsprt_R6.h"
#include "Te_StatusReportSchemeSteps.h"
#include "Te_SmsTestSteps.h"
#include "TE_smsmondsk.h"

_LIT(KServerName,"TE_SMSPRT");


CSmsPrtTestServer* CSmsPrtTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CSmsPrtTestServer * server = new (ELeave) CSmsPrtTestServer();
	CleanupStack::PushL(server);
	// CSmsStackTestServer intermediate base class call
	server->InitializeTsyAndPhonesL();
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 *  Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	TInt err = StartC32();
	if (err != KErrNone)
		{
		User::Leave(err);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSmsPrtTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAP(err,server = CSmsPrtTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
		
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 *  @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;

	return err;
    }


CTestStep* CSmsPrtTestServer::CreateTestStep(const TDesC& aStepName)
/**
 *  @return - A CTestStep derived instance
 *  Implementation of CTestServer pure virtual
 */
	{
	CTestStep*  testStep = NULL;

	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//
	if (aStepName == _L("TestSimpleTxAndRx"))
		{
		testStep = new CTestSimpleTxAndRx ;
		}
	else if (aStepName == _L("TestBinaryTxAndRx"))
		{
		testStep = new CTestBinaryTxAndRx ;
		}				
	else if (aStepName == _L("TestStatusReport"))
		{
		testStep = new CTestStatusReport ;
		}
	else if (aStepName == _L("TestTxWithError"))
		{
		testStep = new CTestTxWithError ;
		}
	else if (aStepName == _L("TestTxFailed"))
		{
		testStep = new CTestTxFailed ;
		}
	else if (aStepName == _L("TestMatchingToObserver"))
		{
		testStep = new CTestMatchingToObserver ;
		}
	else if (aStepName == _L("TestCommandMessages"))
		{
		testStep = new CTestCommandMessages ;	
		}
	else if (aStepName == _L("TestSimpleRx"))
		{
		testStep = new CTestSimpleRx ;
		}
	else if (aStepName == _L("Test7bitMessTest"))
		{
		testStep = new CTest7bitMessTest ;	
		}
	else if (aStepName == _L("TestClassMessages"))
		{
		testStep = new CTestClassMessages ;
		}
	else if (aStepName == _L("TestRxConcatenated"))
		{
		testStep = new CTestRxConcatenated ;	
		}
	else if (aStepName == _L("TestRxDuplicate"))
		{
		testStep = new CTestRxDuplicate ;
		}
	else if (aStepName == _L("TestRxRingToneA"))
		{
		testStep = new CTestRxRingToneA ;	
		}
	else if (aStepName == _L("TestTxRxConcatenated"))
		{
		testStep = new CTestTxRxConcatenated ;	
		}
	else if (aStepName == _L("TestParamStorage"))
		{
		testStep = new CTestParamStorage ;
		}
	else if (aStepName == _L("TestSmsStore"))
		{
		testStep = new CTestSmsStore ;	
		}
	else if (aStepName == _L("TestSmsStoreList"))
		{
		testStep = new CTestSmsStoreList ;
		}
	else if (aStepName == _L("TestDeleteSms"))
		{
		testStep = new CTestDeleteSms ;	
		}
    else if (aStepName == _L("TestSmsAddr"))
        {
        testStep = new CTestSmsAddr; 
        }
	else if (aStepName == _L("TestSocketBinding"))
		{
		testStep = new CTestSocketBinding ;	
		}
	else if (aStepName == _L("TestSmsEventLogger"))
		{
		testStep = new CTestSmsEventLogger ;
		}
	else if (aStepName == _L("TestBearerChange"))
		{
		testStep = new CTestBearerChange ;
		}
	else if (aStepName == _L("TestRestoreBearer"))
		{
		testStep = new CTestRestoreBearer;
		}
	else if (aStepName == _L("TestRecvModeChange"))
		{
		testStep = new CTestRecvModeChange ;		
		}
	else if (aStepName == _L("TestTsyCaps"))
		{
		testStep = new CTestTsyCaps ;	
		}
	else if (aStepName == _L("TestOOMSendSms"))
		{
		testStep = new CTestOOMSendSms ;	
		}
	else if (aStepName == _L("TestOOMWriteSms"))
		{
		testStep = new CTestOOMWriteSms ;	
		}
	else if (aStepName == _L("TestOOMReadSms"))
		{
		testStep = new CTestOOMReadSms ;	
		}
	else if (aStepName == _L("TestOOMReadSmsList"))
		{
		testStep = new CTestOOMReadSmsList ;	
		}
	else if (aStepName == _L("TestOOMDeleteSms"))
		{
		testStep = new CTestOOMDeleteSms ;	
		}
	else if (aStepName == _L("TestOOMSmsParams"))
		{
		testStep = new CTestOOMSmsParams ;	
		}
	else if (aStepName == _L("TestMeStoreDupAndMiss"))
		{
		testStep = new CTestMeStoreDupAndMiss ;	
		}
	else if (aStepName == _L("TestSimStoreDupAndMiss"))
		{
		testStep = new CTestSimStoreDupAndMiss ;		
		}
	else if (aStepName == _L("TestRxCphs"))
		{
		testStep = new CTestRxCphs ;	
		}
	else if (aStepName == _L("TestInvalidPDUs"))
		{
		testStep = new CTestInvalidPDUs ;	
		}
	else if (aStepName == _L("TestStress"))
		{
		testStep = new CTestStress ;	
		}
	else if (aStepName == _L("TestCombinedStore"))
		{
		testStep = new CTestCombinedStore ;	
		}
	else if (aStepName == _L("TestParamsInter"))
		{
		testStep = new CTestParamsInter ;	
		}
	else if (aStepName == _L("TestTxRxAlphanumeric"))
		{
		testStep = new CTestTxRxAlphanumeric ;	
		}
	else if (aStepName == _L("TestRxSuspend"))
		{
		testStep = new CTestRxSuspend ;	
		}
	else if (aStepName == _L("TestEnumerateCorrupted"))
		{
		testStep = new CTestEnumerateCorrupted ;	
		}
	else if (aStepName == _L("TestCancelling"))
		{
		testStep = new CTestCancelling ;	
		}
	else if (aStepName == _L("TestSendAndRecvMsgsWithDifferentTON"))
		{
		testStep = new CTestSendAndRecvMsgsWithDifferentTON ;	
		}
	else if (aStepName == _L("TestSendAndReceiveIndicatorMsgs"))
		{
		testStep = new CTestSendAndReceiveIndicatorMsgs ;	
		}
	else if (aStepName == _L("TestSendRecvMaxLengthMsg"))
		{
		testStep = new CTestSendRecvMaxLengthMsg ;	
		}
	else if (aStepName == _L("TestSendAndRecvMsgsWithDifferentPID"))
		{
		testStep = new CTestSendAndRecvMsgsWithDifferentPID ;	
		}
	else if (aStepName == _L("SmsParamsErrorCases"))
		{
		testStep = new CTestSmsParamsErrorCases ;	
		}
	else if (aStepName == _L("TestResendFailedConcatinatedMessage"))
		{
		testStep = new CTestResendFailedConcatinatedMessage ;	
		}
	else if (aStepName == _L("TestMultipartWapMessage"))
		{
		testStep = new CTestMultipartWapMessage ;	
		}
	else if (aStepName == _L("TestMOSESMessage"))
		{
		testStep = new CTestMOSESMessage ;	
		}
	else if (aStepName == _L("TestClockBack"))
		{
		testStep = new CTestClockBack ;	
		}
	else if (aStepName == _L("TestEnumerateNoMessages"))
		{
		testStep = new CTestEnumerateNoMessages ;	
		}
	else if (aStepName == _L("TestIE"))
		{
		testStep = new CTestIE ;	
		}
	else if (aStepName == _L("TestEmailIE"))
		{
		testStep = new CTestEmailIE ;
		}
	else if (aStepName == _L("TestMaxReadTimeForSmsStoreList"))
		{
		testStep = new CTestMaxReadTimeForSmsStoreList;
		}
	else if (aStepName == _L("TestSameLogIdMultiplePDU"))
		{
		testStep = new CTestSameLogIdMultiplePDU;
		}
	else if (aStepName == _L("TestSameLogIdSinglePDU"))
		{
		testStep = new CTestSameLogIdSinglePDU;
		}
	else if (aStepName == _L("TestConcatenatedMessageLogging" ))
		{
		testStep = new CTestConcatenatedMessageLogging;
		}
	else if (aStepName == _L("TestEnumerationOfStatusReport" ))
		{
		testStep = new CTestEnumerationOfStatusReport;
		}
 	else if (aStepName == _L("TestWriteStatusReportToSIM" ))
		{
		testStep = new CTestWriteStatusReportToSIM;
		}
	else if (aStepName == _L("TestTxSmsWithDelayedCompletion" ))
		{
		testStep = new CTestTxSmsWithDelayedCompletion;
		}
	else if (aStepName == _L("TestSmsStoreReadCancel" ))
		{
		testStep = new CTestSmsStoreReadCancel;
		}
	else if (aStepName == _L("TestBindWhenPoweredDownWithPublishSubscribe" ))
		{
		testStep = new CTestBindWhenPoweredDownWithPublishSubscribe;
		}
	else if (aStepName == _L("TestObserverNotifiedWhenPoweredDownWithPublishSubscribe" ))
		{
		testStep = new CTestObserverNotifiedWhenPoweredDownWithPublishSubscribe;
		}
	else if (aStepName == _L("TestSmsCNumberChangeWithPublishSubscribe" ))
		{
		testStep = new CTestSmsCNumberChangeWithPublishSubscribe;
		}
	else if (aStepName == _L("TestStatusReportTime" ))
		{
		testStep = new CTestStatusReportTime;
		}
	else if (aStepName == _L("TestTx8BitWapWithStatus"))
		{
		testStep = new CTestTx8BitWapWithStatus;
		}
	else if(aStepName == _L("TestSimStoreCorruptMessage"))
		{
		testStep = new CTestSimStoreCorruptMessage ;
		}
	else if (aStepName == _L("TestCorruptPduWithNegativeResponse"))
		{
        testStep = new CTestCorruptPduWithNegativeResponse;
		}
	else if (aStepName == _L("TestBootTimer"))
		{
		testStep = new CTestBootTimer;
		}
	else if (aStepName == _L("TestCSmsMessageTimeStamp"))
		{
	    testStep = new CTestCSmsMessageTimeStamp;
		}
	else if (aStepName == _L("TestCSmsMessageWithDeliverPDU"))
		{
		testStep = new CTestCSmsMessageWithDeliverPDU;
		}
	else if (aStepName == _L("TestCSmsMessageWithStatusReportPDU"))
		{
	    testStep = new CTestCSmsMessageWithStatusReportPDU;
	    }
	else if (aStepName == _L("TestCSmsMessageWithSubmitPDU"))
		{
	    testStep = new CTestCSmsMessageWithSubmitPDU;
		}
	else if (aStepName == _L("TestHandlePID40h"))
		{
		testStep = new CTestHandlePID40h;	
		}
	//SMS R6 test steps
	else if (aStepName== _L("TestSinglePDUHyperLinks"))
		{
		testStep = new CTestSinglePDUHyperLinks();
		}
	else if (aStepName== _L("TestMultiplePDUHyperLinks"))
		{
		testStep = new CTestMultiplePDUHyperLinks();
		}
	else if (aStepName== _L("TestSinglePDUReplyAddress"))
		{
		testStep = new CTestSinglePDUReplyAddress();
		}
	else if (aStepName== _L("TestMultiplePDUReplyAddress"))
		{
		testStep = new CTestMultiplePDUReplyAddress();
		}
	else if (aStepName== _L("TestSpecialMessageWaitingOperations"))
		{
		testStep = new CTestSpecialMessageOperations();
		}
	else if (aStepName== _L("TestSinglePDUSpecialMessageWaiting"))
		{
		testStep = new CTestSinglePDUSpecialMessageWaiting();
		}
	else if (aStepName== _L("TestMultiplePDUSpecialMessageWaiting"))
		{
		testStep = new CTestMultiplePDUSpecialMessageWaiting();
		}
	else if (aStepName== _L("TestEnhancedVoiceMailOperations"))
		{
		testStep = new CTestEnhancedVoiceMailOperations();
		}
	else if (aStepName== _L("TestSinglePDUEnhancedVoiceMailInformation"))
		{
		testStep = new CTestSinglePDUEnhancedVoiceMailInformation();
		}
	else if (aStepName== _L("TestMultiplePDUEnhancedVoiceMailInformation"))
		{
		testStep = new CTestMultiplePDUEnhancedVoiceMailInformation();
		}
	else if (aStepName== _L("TestEnhancedVoiceMailDeleteOperations"))
		{
		testStep = new CTestEnhancedVoiceMailDeleteOperations();
		}
	else if (aStepName== _L("TestSinglePDUEnhancedVoiceMailDeleteConfirmation"))
		{
		testStep = new CTestSinglePDUEnhancedVoiceMailDeleteConfirmation();
		}
	else if (aStepName== _L("TestMultiplePDUEnhancedVoiceMailDeleteConfirmation"))
		{
		testStep = new CTestMultiplePDUEnhancedVoiceMailDeleteConfirmation();
		}
	else if (aStepName== _L("Test7BitDefaultAlphabet"))
		{
		testStep = new CTest7BitDefaultAlphabet();
		}
	else if (aStepName== _L("TestZeroLengthIE"))
		{
		testStep = new CTestZeroLengthIE();
		}
	else if (aStepName== _L("TestAutomaticDeletionInDCS"))
		{
		testStep = new CTestAutomaticDeletionInDCS();
		}
	else if (aStepName== _L("TestAutomaticDeletionInDCSAndInEsk"))
		{
		testStep = new CTestAutomaticDeletionInDCSAndInESK();
		}
	else if (aStepName== _L("TestAutomaticDeletionTypeZeroSMS"))
		{
		testStep = new CTestAutomaticDeletionTypeZeroSMS();
		}
	else if (aStepName== _L("TestAutomaticDeletionTypeZeroAndSetInESK"))
		{
		testStep = new CTestAutomaticDeletionTypeZeroAndSetInESK();
		}
	else if (aStepName== _L("TestOOMSendR6Sms"))
	    {
	    testStep = new CTestOOMSendR6Sms();
	    }
	else if (aStepName== _L("EnhancedVoiceMessageBoundaryTest"))
	    {
	    testStep = new CTestEnhancedVoiceMessageBoundary();
	    }
	else if (aStepName== _L("EnhancedVoiceMessageBoundaryTest1"))
	    {
	    testStep = new CTestEnhancedVoiceMessageBoundary1();
	    }
	else if (aStepName== _L("CSubmitReportDefaultsTo7BitDecoding"))
	    {
	    testStep = new CTestSubmitReportDefaultsTo7BitDecoding();
	    }
	else if (aStepName== _L("CDeliverReportDefaultsTo7BitDecoding"))
	    {
	    testStep = new CTestDeliverReportDefaultsTo7BitDecoding();
	    }
	else if (aStepName== _L("CStatusReportDefaultsTo7BitDecoding"))
	    {
	    testStep = new CTestStatusReportDefaultsTo7BitDecoding();
	    }
	else if (aStepName== _L("TestSendingTPSRRSchemeInAllPDUs"))
	    {
	    testStep = new CTestSendingTPSRRSchemeInAllPDUs();
	    }
	else if (aStepName== _L("TestSendingTPSRRSchemeInLastPDU"))
	    {
	    testStep = new CTestSendingTPSRRSchemeInLastPDU();
	    }
	else if (aStepName== _L("TestReceivingLastTPSRR"))
	    {
	    testStep = new CTestReceivingLastTPSRR();
	    }
	else if (aStepName== _L("TestReceivingTPSRRAllPDU"))
	    {
	    testStep = new CTestReceivingTPSRRAllPDU();
	    }
	else if (aStepName== _L("TestReceivingTPSRRSomePDU"))
	    {
	    testStep = new CTestReceivingTPSRRSomePDU();
	    }

	else if (aStepName== _L("TestSendingSMSCSchemeSinglePDUNoBitSet"))
	    {
	    testStep = new CTestSendingSMSCSchemeSinglePDUNoBitSet();
	    }
	else if (aStepName== _L("TestSendingSMSCSchemeSinglePDUSomeBitsSet"))
	    {
	    testStep = new CTestSendingSMSCSchemeSinglePDUSomeBitsSet();
	    }
	else if (aStepName== _L("TestSendingSMSCSchemeMultiplePDUsSomeBitsSet"))
	    {
	    testStep = new CTestSendingSMSCSchemeMultiplePDUsSomeBitsSet();
	    }
	else if (aStepName== _L("TestReceivingLastSMSC"))
	    {
	    testStep = new CTestReceivingLastSMSC();
	    }
	else if (aStepName== _L("TestReceivingSMSCAllPDUs"))
	    {
	    testStep = new CTestReceivingSMSCAllPDU();
	    }
	else if (aStepName== _L("TestReceivingSMSCSomePDUs"))
	    {
	    testStep = new CTestReceivingSMSCSomePDU();
	    }
	else if (aStepName== _L("TestReceivingSMSCMultiplePDU"))
	    {
	    testStep = new CTestReceivingSMSCMultiplePDU();
	    }
	else if (aStepName== _L("TestReceivingSMSCSinglePDU"))
	    {
	    testStep = new CTestReceivingSMSCSinglePDU();
	    }
	else if (aStepName== _L("TestReceivingDeliversWithTPSRR"))
	    {
	    testStep = new CTestReceivingDeliversWithTPSRR();
	    }
	else if (aStepName== _L("TestDeliversWithTPSRRScheme"))
	    {
	    testStep = new CTestDeliversWithTPSRRScheme();
	    }
	//SMS Test  Steps	    
	else if (aStepName== _L("LoadSMSProtocol"))
	    {
	    testStep = new CLoadSMSProtocol(iSocketServer);
	    }
	else if (aStepName== _L("DeleteSARStores"))
	    {
	    testStep = new CDeleteSARStores();
	    }
	else if (aStepName== _L("ReceiveSMS"))
	    {
	    testStep = new CReceiveSMS(iSocketServer);
	    }
	else if (aStepName== _L("SendSMS"))
	    {
	    testStep = new CSendSMS(iSocketServer);
	    }
	else if (aStepName== _L("CloseSMSProtocol"))
	    {
	    testStep = new CCloseSMSProtocol(iSocketServer);
	    }	    
	else if (aStepName== _L("SetDiskMonitorLimits"))
	    {
	    testStep = new CSetDiskMonitorLimits();
	    }	    
	else if (aStepName== _L("SetDiskSpace"))
	    {
	    testStep = new CSetDiskSpace();
	    }	    
	else if (aStepName== _L("FreeDiskSpace"))
	    {
	    testStep = new CFreeDiskSpace();
	    }	    
	else if (aStepName== _L("TestDiskSpaceMidRangeClass0Class2"))
	    {
	    testStep = new CTestDiskSpaceMidRangeClass0Class2();
	    }
	else if (aStepName== _L("TestDiskSpaceLowRangeClass0Class2"))
	    {
	    testStep = new CTestDiskSpaceLowRangeClass0Class2();
	    }
	else if (aStepName== _L("TestCorruptPDUs"))
	    {
	    testStep = new CTestCorruptPDUs();
	    }
	else if (aStepName== _L("TestCongestedReceive"))
	    {
	    testStep = new CTestCongestedReceive();
	    }
	else if (aStepName== _L("ForwardSystemTime"))
	    {
	    testStep = new CForwardSystemTime();
	    }
	else if (aStepName== _L("SendReceiveSMSWhileServerInOOM"))
	    {
	    testStep = new CSendReceiveSMSWhileServerInOOM(iSocketServer);
	    }
	    
	else if (aStepName== _L("Class0StoreEnabled"))
	    {
	    testStep = new CClass0StoreEnabled(iSocketServer);
	    }	 
	    
	else if (aStepName== _L("EnumerateInOODCondition"))
	    {
	    testStep = new CEnumerateInOODCondition(iSocketServer);
	    }	      
	else if (aStepName== _L("TestEncodingPDUonBoundary"))
		{
		testStep = new CTestEncodingPDUonBoundary();
		}
	else if (aStepName== _L("TestReceiveInvalidOrReservedIE"))
		{
		testStep = new CTestReceiveInvalidOrReservedIE();
		}
	else if (aStepName== _L("TestEncoding7bitStandardGSM"))
		{
		testStep = new CTestEncoding7bitStandardGSM();
		}
	else if (aStepName== _L("TestEncoding8bitCodePage1252"))
		{
		testStep = new CTestEncoding8bitCodePage1252();
		}
	else if (aStepName== _L("TestEncodingUnicode"))
		{
		testStep = new CTestEncodingUnicode();
		}
	else if (aStepName== _L("TestEncoding7bitUnconvertibleGSM"))
		{
		testStep = new CTestEncoding7bitUnconvertibleGSM();
		}
	else if (aStepName== _L("TestEncoding7bitNonStandardGSM"))
		{
		testStep = new CTestEncoding7bitNonStandardGSM();
		}
	else if (aStepName== _L("TestEncoding7bitTurkishNationalLanguage"))
		{
		testStep = new CTestEncoding7bitTurkishNationalLanguage();
		}
	else if (aStepName== _L("TestEncoding7bitTurkishAndUnconvertible"))
		{
		testStep = new CTestEncoding7bitTurkishAndUnconvertible();
		}
	else if (aStepName== _L("TestReceivingMessageWithDifferentEncodings"))
		{
		testStep = new CTestReceivingMessageWithDifferentEncodings();
		}
	else if (aStepName== _L("TestReceivingMessageAfterSocketClosure"))
		{
		testStep = new CTestReceivingMessageAfterSocketClosure();
		}
 	else if (aStepName== _L("TestSimpleTxAndRxWithLoggingDisabled"))
 		{
 		testStep = new CTestSimpleTxAndRxWithLoggingDisabled();
 		}
    else if (aStepName== _L("TestEncoding7bitNationalLanguages"))
        {
        testStep = new CTestEncoding7bitNationalLanguages();
        }
    else if (aStepName== _L("TestEncodingMultiPDUwith7bitNationalLanguages"))
        {
        testStep = new CTestEncodingMultiPDUwith7bitNationalLanguages();
        }
    else if (aStepName== _L("TestEncodingDowngradedWith7bitNationalLanguages"))
        {
        testStep = new CTestEncodingDowngradedWith7bitNationalLanguages();
        }
    else if (aStepName== _L("TestOptimumEncodingWithLockingAndShift7bitNationalLanguages"))
        {
        testStep = new CTestOptimumEncodingWithLockingAndShift7bitNationalLanguages();
        }
    else if (aStepName== _L("TestOptimumEncodingWithLockingAndDowngrade7bitNationalLanguages"))
        {
        testStep = new CTestOptimumEncodingWithLockingAndDowngrade7bitNationalLanguages();
        }
    else if (aStepName== _L("TestOptimumEncodingWithLockingAndUnconvertible7bitNationalLanguages"))
        {
        testStep = new CTestOptimumEncodingWithLockingAndUnconvertible7bitNationalLanguages();
        }
   	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}

	return testStep;
	}
