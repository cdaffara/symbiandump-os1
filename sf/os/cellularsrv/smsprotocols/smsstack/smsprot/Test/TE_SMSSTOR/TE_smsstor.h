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
//

#include "TE_smsstorbase.h"
#include "smspclass0stor.h"

#ifndef __TE_SMSSTOR_H_
#define __TE_SMSSTOR_H_


class CSmsSegmentionStorePurge : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSegmentionStorePurge1 : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSegmentionStorePurge2 : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSmsSegmentionStorePurge3 : public CSmsStorTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };

class CSmsSegmentionStorePurge4 : public CSmsStorTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };

class CReassemblystore3 : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};



class CReassemblystore2 : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CEncodePDUs : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CInternalizeExternalizeSMS: public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CPublicMethodTest : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CLowMemoryTest : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CCorruptionTest : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};
	

class CStatusReportTest : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSmsPermanentFileStoreTest : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CPreallocatedFileTest : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CClass0ReassemblyStoreTest2;

class CTestMSmsComm2 : public MSmsComm, public CTimer
	{
public:
	static CTestMSmsComm2* NewL(RFs& aFs, CClass0ReassemblyStoreTest2& aStoreTest);
	~CTestMSmsComm2() { delete iSmsMessage; }
	CSmsMessage* GetTestMessage() {	return iSmsMessage; }
	void DeleteTestMessage();
	void EnableGuardTimerTest(TBool aBool) {iTestGuardTimer = aBool;}

public:
	//Implementation of MSmsComm
	TInt ProcessMessageL(const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes);
	void DeletePDUs(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray, MSmsMessageObserver* aObserver);
	void MessageSendCompleted(TInt aStatus);
	void ModemNotificationCompleted(TInt aStatus,RPhone::TModemDetection aState);
	void PhoneEnumerationCompleted(TInt aStatus);
	TBool ClosingDown() const;
	void CloseNowWrap();
    inline TBool ObserverIsPresent(const MSmsMessageObserver& aObserver) const;
	virtual const RMobilePhone::TMobilePhoneNetworkInfoV1& NetworkInfo()const;
	virtual TBool NetworkInfoAvailable()const;
	TBool IsPowerUp()const;
	void DiskSpaceMonitorStateChange(TSmsDiskSpaceMonitorStatus aStatus);
	//Implementation of CTimer
	void RunL();

protected:
	CTestMSmsComm2(RFs& aFs, CClass0ReassemblyStoreTest2& aStoreTest);

private:
	RFs& iFs;
	CClass0ReassemblyStoreTest2& iStoreTest;
	CSmsMessage* iSmsMessage;
	TBool iTestGuardTimer;
	RMobilePhone::TMobilePhoneNetworkInfoV1 iMobilePhoneNetworkInfo;
	};

class CClass0ReassemblyStoreTest2 : public CSmsStorTestStep
	{
public:
	virtual TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

public:
	void TestGuardTimerMessageL();
	void StopScheduler();

private:
	void OpenStoreL();
	void CloseStore();
	void DeleteClass0ReassemblyStoreFiles();
	void CreatePDUsForMessageL(TInt aNoOfPDUs, TInt aMsgRef);
	CSmsMessage* CreateMessageL(TInt aMsgPDUIndex);
	void TestSingleSegmentMessageL();
	void TestNumberOfMessageExceedingL();
	void TestMaxPDUSegExceedingL();
	void TestMultiSegmentMessageL();
	void TestUpdationOfFieldsL();
	void TestCodeCoverageForCopyDuplicateToMasterL();
	void TestCodeCoverageForRevertOperationsL();
	void TestGuardTimerL();
	void TestSmsContentsL(CSmsMessage& aSms1, CSmsMessage& aSms2);
	void CorruptMasterHeaderInfoL();
	void TestAddMessageTillHeapAllocDoesNotFailL();
	void TestDeleteMessageTillHeapAllocDoesNotFailL();

private:
	CClass0SmsReassemblyStore* iClass0ReassemblyStore;
	CTestMSmsComm2* iTestMSmsComm2;
	CArrayFix<TGsmSms>* iSmsArray;
	CActiveScheduler* iScheduler;
	TInt iGuardTimerTestStage;
	};


class CSmsReassemblyStorePerformanceTest : public CSmsStorTestStep, MSmsComm
	{
public:
	virtual TVerdict doTestStepL();

private:
	void RunPerformanceTestL(CSmsReassemblyStore* aReassemblyStore,
							 const TDesC& aTestName, TInt aMsgSize,
	                         TInt aPduCount, TInt aStoreLevel);

	//
	// Virtual methods from MSmsComm which is required to gain access to
	// CSmsReassemblyStore functions. All panic as they should never be called.
	//
	virtual TInt ProcessMessageL(const CSmsMessage& /*aSmsMessage*/, const TSmsAddr* /*aOriginalSmsAddr*/,
								 const CSmsMessage* /*aOriginalSmsMessage*/, TDes& /*aDes*/)
		{
		User::Invariant();
		return 0;
		};

	virtual void DeletePDUs(const CArrayFix<TGsmSmsSlotEntry>& /*aSlotArray*/,
							MSmsMessageObserver* /*aObserver*/)
		{
		User::Invariant();
		};

	virtual void MessageSendCompleted(TInt /*aStatus*/)
		{
		User::Invariant();
		};

	virtual void PhoneInitializationCompleted(TInt /*aStatus*/)
		{
		User::Invariant();
		};

	virtual void ModemNotificationCompleted(TInt /*aStatus*/, RPhone::TModemDetection /*aState*/)
		{
		User::Invariant();
		};

	virtual void PhoneEnumerationCompleted(TInt /*aStatus*/)
		{
		User::Invariant();
		};

	virtual TBool ClosingDown() const
		{
		User::Invariant();
		return EFalse;
		};

	virtual void CloseNowWrap()
		{
		User::Invariant();
		};

	virtual TBool ObserverIsPresent(const MSmsMessageObserver& /*aObserver*/) const
		{
		User::Invariant();
		return EFalse;
		};

    virtual const RMobilePhone::TMobilePhoneNetworkInfoV1& NetworkInfo()const
        {
        User::Invariant();
        return iDummyNetworkInfoV1;
        };
    
    virtual TBool NetworkInfoAvailable()const
		{
		User::Invariant();
		return EFalse;
		};

	virtual TBool IsPowerUp()const
		{
		User::Invariant();
		return EFalse;
		};

    virtual void DiskSpaceMonitorStateChange(TSmsDiskSpaceMonitorStatus /*aStatus*/)
        {
        User::Invariant();
        };

private:
	RMobilePhone::TMobilePhoneNetworkInfoV1  iDummyNetworkInfoV1;
	};
	
#endif // __TE_SMSSTOR_H_
