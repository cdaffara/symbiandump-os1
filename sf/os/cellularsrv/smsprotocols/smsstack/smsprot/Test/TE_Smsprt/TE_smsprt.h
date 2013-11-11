// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_SMSPRT_H
#define TE_SMSPRT_H

#include "TE_smsprtbase.h"
#include "logcheck.h"

class CSmsEventLogger;

class CTestSimpleTxAndRx : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};
	
	
class CTestBinaryTxAndRx : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};	


class CTestStatusReport : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxWithError : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxFailed : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMatchingToObserver : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestCommandMessages : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSimpleRx : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTest7bitMessTest : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestClassMessages : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRxConcatenated : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRxDuplicate : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRxRingToneA : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxRxConcatenated : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestParamStorage : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsStore : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsStoreList : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestDeleteSms : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestSmsAddr : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };

class CTestSocketBinding : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsEventLogger : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestBearerChange : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRestoreBearer : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRecvModeChange : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTsyCaps : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMSendSms : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMWriteSms : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMReadSms : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMReadSmsList : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMDeleteSms : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMSmsParams : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMeStoreDupAndMiss : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSimStoreDupAndMiss : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRxCphs : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestInvalidPDUs : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestStress : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestCombinedStore : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestParamsInter : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxRxAlphanumeric : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestRxSuspend : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEnumerateCorrupted : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestCancelling : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSendAndRecvMsgsWithDifferentTON : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSendAndReceiveIndicatorMsgs : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSendRecvMaxLengthMsg : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSendAndRecvMsgsWithDifferentPID : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsParamsErrorCases : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();

private:
	void TestSmsParamsErrorCasesL(
		RMobileSmsMessaging::TMobileSmspStoreValidParams aSmspEntryValidParams,
		RMobilePhone::TMobileTON aSmspEntryScTypeOfNumber,
		RMobilePhone::TMobileNPI aSmspEntryScNumberPlan,
		const TDesC8& aSmspEntryScTelNumber);
	};


class CTestResendFailedConcatinatedMessage : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMultipartWapMessage : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMOSESMessage : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestClockBack : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEnumerateNoMessages : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestIE : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEmailIE : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMaxReadTimeForSmsStoreList : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSameLogIdMultiplePDU	: public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSameLogIdSinglePDU : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestConcatenatedMessageLogging : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };


class CSmsLogChecker;

class CTestLog : public CActive
    {
public:
    static CTestLog* NewLC(CSmsStackTestUtils& aUtils, RFs& aFs, CSmsBaseTestStep* aTest, TInt aPriority);
    ~CTestLog();

    void StartL();

private:
    void ConstructL(RFs& aFs);
    CTestLog(CSmsStackTestUtils& aUtils, CSmsBaseTestStep* aTest, TInt aPriority);
    void RunL();
    void DoCancel() {};
    TInt RunError(TInt aError) {Complete(aError); return KErrNone;}
    void Complete(TInt aError) {iStatus = aError; CActiveScheduler::Stop();}
    void StartOriginalL();
    void StartCompareL(TInt aExpectError);

private:
    enum TState
        {
        EOriginal
        } iState;

    CSmsStackTestUtils& iUtils;
    CSmsBaseTestStep* iTest;

    CSmsLogChecker* iLogChecker;
    CSmsEventLogger* iEventLogger;
    CSmsMessage* iMessage;
    RSocket iSocket;

    RArray<TLogId> iAddedIds;
    TInt iNumberToAdd;
    TLogSmsPduData iData;
    };


class CTestEnumerationOfStatusReport : public CSmsBaseTestStep
    {
public:
    TVerdict doTestStepL();
    };


class CTestWriteStatusReportToSIM : public CSmsBaseTestStep
    {
public:
	TVerdict doTestStepL();
	};


class CTestTxSmsWithDelayedCompletion : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestSmsStoreReadCancel : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestBindWhenPoweredDownWithPublishSubscribe : public CSmsBaseTestStep
	{
public :
	TVerdict doTestStepL();
	};


class CTestObserverNotifiedWhenPoweredDownWithPublishSubscribe : public CSmsBaseTestStep
	{
public :
	TVerdict doTestStepL();
	};


class CTestSmsCNumberChangeWithPublishSubscribe : public CSmsBaseTestStep
	{
public :
	TVerdict doTestStepL();
	};


class CTestStatusReportTime : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestTx8BitWapWithStatus : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestSimStoreCorruptMessage : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestCorruptPduWithNegativeResponse : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepL();
	};


class CTestBootTimer : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepL();
	};


class CTestCSmsMessageTimeStamp : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepL();
	};


class CTestCSmsMessageWithDeliverPDU : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepL();
	};


class CTestCSmsMessageWithStatusReportPDU : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepL();
	};


class CTestCSmsMessageWithSubmitPDU : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepL();
	};


class CTestHandlePID40h : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestDiskSpaceMidRangeClass0Class2 : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	};


class CTestDiskSpaceLowRangeClass0Class2 : public CSmsBaseTestStep
	{
public:
    TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	};


class CTestCorruptPDUs : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestCongestedReceive : public CSmsBaseTestStep
	{
public:
	TVerdict doTestStepL();
	};


class CTestEncodingPDUonBoundary : public CSmsBaseTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestReceiveInvalidOrReservedIE : public CSmsBaseTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncoding7bitStandardGSM : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncoding8bitCodePage1252 : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncodingUnicode : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncoding7bitUnconvertibleGSM : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncoding7bitNonStandardGSM : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncoding7bitTurkishNationalLanguage : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestEncoding7bitTurkishAndUnconvertible : public CSmsPrtEncodingTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestReceivingMessageWithDifferentEncodings : public CSmsBaseTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestReceivingMessageAfterSocketClosure : public CSmsBaseTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestSimpleTxAndRxWithLoggingDisabled : public CSmsBaseTestStep
 	{
public:
 	virtual TVerdict doTestStepL();
 	};

class CTestEncoding7bitNationalLanguages : public CSmsPrtEncodingTestStep
    {
public:
    virtual enum TVerdict doTestStepL();
    };

class CTestEncodingMultiPDUwith7bitNationalLanguages : public CSmsPrtEncodingTestStep
    {
public:
    virtual enum TVerdict doTestStepL();
    };

class CTestEncodingDowngradedWith7bitNationalLanguages : public CSmsPrtEncodingTestStep
    {
public:
    virtual enum TVerdict doTestStepL();
    };

class CTestOptimumEncodingWithLockingAndShift7bitNationalLanguages : public CSmsPrtEncodingTestStep
    {
public:
    virtual enum TVerdict doTestStepL();
    };

class CTestOptimumEncodingWithLockingAndDowngrade7bitNationalLanguages : public CSmsPrtEncodingTestStep
    {
public:
    virtual enum TVerdict doTestStepL();
    };

class CTestOptimumEncodingWithLockingAndUnconvertible7bitNationalLanguages : public CSmsPrtEncodingTestStep
    {
public:
    virtual enum TVerdict doTestStepL();
    };

#endif // TE_SMSPRT_H
