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

#ifndef SMSSTACKBASETESTSTEPS_H
#define SMSSTACKBASETESTSTEPS_H

#include <e32base.h>
#include <test\testexecutestepbase.h>
#include <smsuaddr.h>
#include <gsmupdu.h>
#include <sacls.h>
#include <rscommon.h>
#include <c32root.h>
#include <etelmm.h>

class RSocketServ;
class RSocket;
class CSmsMessage;
class CSmsBuffer;
class CSmsPduDbMessage;
class CSmsStackTestUtils;
class TTestCase;

/**
  Base class housing SMS test Steps 
*/
class CSmsBaseTestStep : public CTestStep
    {
public:
    IMPORT_C virtual TVerdict doTestStepPreambleL();
    IMPORT_C virtual TVerdict doTestStepPostambleL();    
    
    IMPORT_C virtual ~CSmsBaseTestStep();

    enum TCodingScheme{ESevenBit=7, EEightBit};

    /* Functions grabbed from SMS Stack Test Utils */
    // Socket utils
    IMPORT_C void OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddr& aSmsAddr);
    IMPORT_C void OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily);

    IMPORT_C void ParseSettingsFromFileL();
    
    IMPORT_C void SetSimTSYTestNumberL(TInt aTestNumber);    
    IMPORT_C void SetTestNumberL(TInt aTestNumberProperty, TInt aTestNumber);
    IMPORT_C void SetTestNumberFromConfigurationFileL();

    IMPORT_C TInt GetIntegerFromConfigL(const TDesC& aKey);
    IMPORT_C TPtrC GetStringFromConfigL(const TDesC& aKey);
    
    IMPORT_C void ConnectSocketServerL(RSocketServ& aSocketServer);
    IMPORT_C void ConnectSocketServerLC(RSocketServ& aSocketServer);
    
    IMPORT_C void WaitForRecvL(RSocket& aSocket);
    IMPORT_C TBool TimedWaitForRecvL(RSocket& aSocket, TUint aDelay);
    
    IMPORT_C CSmsMessage* RecvSmsL(RSocket& aSocket, TInt aIoctl = KIoctlReadMessageSucceeded);
    IMPORT_C CSmsMessage* RecvSmsFailedL(RSocket& aSocket);

    IMPORT_C TSmsStatus::TSmsStatusValue RecvStatusReportL(TSmsServiceCenterAddress& aRecipientNumber, RSocket& aSocket);
    
    IMPORT_C CSmsMessage* CreateSmsMessageLC(CSmsPDU::TSmsPDUType aType, CSmsBuffer* aBuffer, const TDesC& aAddress);
    IMPORT_C CSmsMessage* CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType = CSmsPDU::ESmsSubmit);
    IMPORT_C CSmsMessage* CreateSmsMessageLC(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType = CSmsPDU::ESmsSubmit);
    //This method is used to create SMS message from test data
    IMPORT_C CSmsMessage* CreateSMSL();
    IMPORT_C CSmsMessage* CreateSmsWithStatusReportReqL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet);
    
    IMPORT_C TUint64 SetHighLowLimitsAndDiskSpaceLevelL(TUint aHighDrop, TUint aLowDrop, TUint aFreeDrop, TUint64 aMax=0x7fffffff);
    IMPORT_C void SetFreeDiskSpaceFromDropLevelL(TUint aFreeDrop, TUint64 aMax=0x7fffffff);
    IMPORT_C void SetFreeDiskSpaceL(TInt64 aNewFreeValue);
    IMPORT_C void ReleaseDiskSpaceL();
    IMPORT_C void SetLowHighLimitsInSmsuRscL(TInt64 aLowLimit, TInt64 aHighLimit);
    IMPORT_C void RemoveLowHighLimitsFromSmsuRscL();
    
    IMPORT_C void SendSmsL(const CSmsMessage* aSms, RSocket& aSocket, TInt aExpectedError=KErrNone, TInt aMaxRetries=3);
    IMPORT_C void SendSmsCancelL(CSmsMessage* aSms, RSocket& aSocket1, RSocket& aSocket2);
    IMPORT_C TInt SendSmsErrorL(CSmsMessage* aSms, RSocket& aSocket);
    IMPORT_C void SendCommandSmsL(CSmsMessage* aSms, RSocket& aSocket);
    IMPORT_C void SendTestMessageL(const TTestCase& aTestCase, RSocket& aSocket);
    IMPORT_C void SendSmsDontCheckReturnValueL(CSmsMessage* aSms, RSocket& aSocket);
    
    IMPORT_C void SendAndRecvTestMessageL(const TTestCase& aTestCase, RSocket& aSocket);
    IMPORT_C void SendAndRecvSms7BitL(const TDesC& aDes, RSocket& aSocket);
    IMPORT_C void DoSendAndRecvSmsL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RSocket& aSocket);

    IMPORT_C void PrintMessageDetailedL(const CSmsMessage* aSms);
    IMPORT_C void PrintMessageL(const CSmsMessage* aSms);
    
    IMPORT_C void TestSmsContentsL(CSmsMessage* aSms, const TDesC& aDes, TBool aIgnorePrintOutput = EFalse);
    IMPORT_C void TestMessageContentsL(CSmsMessage* aSms, const TTestCase& aTestCase);
    
    IMPORT_C void WriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket);
    IMPORT_C void WriteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket);
    IMPORT_C TInt CancelWriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket, TInt aDelay);
      
    IMPORT_C void ReadSmsStoreL(RSocket& aSocket, RPointerArray<CSmsMessage>& aMessages);
    IMPORT_C void ReadSmsStoreL(RSocket& aSocket, RPointerArray<CSmsMessage>& aMessages, TRequestStatus &aStatus);
    IMPORT_C TInt MakeReadSmsStoreRequestL(RSocket& aSocket);
    
    IMPORT_C TInt DeleteSmsL(const CSmsMessage& aSms, RSocket& aSocket, TInt aExpectedError=KErrNone);
    IMPORT_C void DeleteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket);
    
    IMPORT_C void SetIndicatorL(const TTestCase& aTestCase, CSmsMessage* aSms);
    IMPORT_C void FillDes(TDes& aDes,TInt aLength);
    
    IMPORT_C TBool DoSingleTestCaseL( const TDesC8& aSection, TInt aCount );
    IMPORT_C void ReadPduL( TBuf8<64> aSectionName, RPointerArray<CSmsPduDbMessage>& aArray );
    IMPORT_C void TestSendAndRecvMsgsWithDifferentTONL( const RPointerArray<CSmsPduDbMessage>& aArray,
                                                   const CArrayFixFlat<TInt>* aAlphabetArray,
                                                   const CArrayFixFlat<TInt>* aTypeOfNumberArray );
    IMPORT_C void TestSendAndReceiveIndicatorMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
                                               const CArrayFixFlat<TInt>* aIndicationStateArray,
                                               const CArrayFixFlat<TInt>* aDcsBits7To4Array,
                                               const CArrayFixFlat<TInt>* aIndicationTypeArray );
    
    IMPORT_C void ChangeReceiveModeL(RMobileSmsMessaging::TMobileSmsReceiveMode aNewRcvMode);
    
    IMPORT_C void UpdatePhonePowerStatusL(RProperty& aPhonePowerProperty, TSAPhoneStatus aNewStatus);
    
    IMPORT_C void DoESockMemoryLeakTestL();
    
public:
    CActiveScheduler* iScheduler;
    CSmsStackTestUtils* iSmsStackTestUtils;
    TSmsServiceCenterAddress iTelephoneNumber; // Test SIM
    TSmsServiceCenterAddress iServiceCenterNumber; // Radiolinja
    RSocketServ iSocketServer;
    
protected:
    IMPORT_C CSmsBaseTestStep();
    
protected:
    RFs iFs;
    TBool iPartOfMultiStepTestCase;
    TBool iNotLastTestStep;

private:
    class TESockSvrUnloadInfo
        {
        public:
            TCFModuleName iName;
            RootServer::TRSModuleState iState;
            TRequestStatus iStatus;
        };
    
    typedef RPointerArray<TESockSvrUnloadInfo> RUnloadInfoArray;

private:    
    void BuildESockCPMListL(RRootServ& aRootServer, RUnloadInfoArray& aInfo, TDes8& aDispList);
    
};

/*
 * Logging Macros
 */
#define TESTCHECKL(a, b, s) \
    { \
    if((a)!=(b)) \
        { \
        _LIT(KTemp, s); \
        ERR_PRINTF4(_L("<font color=FF0000>%S  failed: Got %d while expecting %d, leaving </font>"), &KTemp, a, b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a, s) \
    { \
    if (!(a)) \
        { \
        _LIT(KTemp, s); \
        ERR_PRINTF2(_L("<font color=FF0000>%S  failed: Condition was false, leaving</font>"), &KTemp); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b, s) \
    { \
    if((a)!=(b)) \
        { \
        _LIT(KTemp3, s); \
        ERR_PRINTF4(_L("<font color=FF0000>%S  failed: Got %d while expecting %d</font>"), &KTemp3, a, b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a, s) \
    { \
    if (!(a)) \
        { \
        _LIT(KTemp4, s); \
        ERR_PRINTF2(_L("<font color=FF0000>%S  failed: Condition was false</font>"), &KTemp4); \
        SetTestStepResult(EFail); \
        } \
    }

#endif // SMSSTACKBASETESTSTEPS_H
