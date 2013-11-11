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
//

/**
 @file
*/


#ifndef _TE_ETELISVBASE_H_
#define _TE_ETELISVBASE_H_

#include <e32base.h>
#include <es_sock.h>
#include <f32file.h>
#include <sacls.h>
#include "e32test.h"
#include "c32comm.h"
#include <simtsy.h>
#include <etel3rdparty.h>
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

/**
This class provides a dummy request so that 
the main thread in the Stress Test does not hang
waiting for the requests by the internal AOs in 
CTelephonyFunctions to be dealt with.
*/
class CMyAsyncOneShot : public CAsyncOneShot
	{
public:
	static CMyAsyncOneShot* NewL();
	~CMyAsyncOneShot();

	void StartOneShot();
	
protected:
	void DoCancel();
	void RunL();

private:
	CMyAsyncOneShot();
	void ConstructL();
	/**
	Timer to provide a dummy asynchronous call for this class.
	Not used to actually do anything else.
	*/
	RTimer iTimer;
	};

class CEtelIsvTestStep : public CTestStep
{
public:
	~CEtelIsvTestStep();
	CEtelIsvTestStep();
 	virtual TVerdict doTestStepPreambleL( void );
	virtual TVerdict doTestStepPostambleL( void );
	void PrintIndicatorValues(TInt aIndicator, TInt aIndicatorCaps);
	void PrintBatteryInfo(CTelephony::TBatteryInfoV1& aBatteryInfo);
	void PrintLockInfo(CTelephony::TIccLock aLock, TInt aLockStatus, TInt aLockSetting);
	void PrintNetworkRegStatus(TInt aNetworkRegStatus);
	void PrintNetworkInfo(TInt aNetworkMode, TInt aNetworkAccess, TInt aNetworkStatus, TInt aNetworkBandInfo);
	void SetTestNumber(TInt aTestNumber);
	void SignalSimTsyToReduceTimersL();

	CTelephony* iTelephony;
	RProperty iTestNumberProperty;
	CActiveScheduler* iScheduler;
	CMyAsyncOneShot* iMyAsyncOneShot;
	
	// Some of the tests support "Buffer Mode". Default setting is EFalse.
	// In "Buffer Mode" the buffer package (TPckgBuff) is passed in to API method
	// as a descriptor reference, otherwise the raw packge (TPckg) is used.
	// This is to verify if ETel3rdParty suppports buffer packages (DEF091177). 
	TBool iBufferMode;

private:
};

class CTestCanceller : public CAsyncOneShot
	{
public:
	CTestCanceller(CActive*);
	~CTestCanceller();
private:
	void RunL();
private:
	CActive* iRetrieve;// not owned by this
	};

class CEtelIsvActBase : public CActive
	{
public:
	CEtelIsvActBase(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	inline CTestCanceller* RetrieveCanceller() {return iCanceller;};
	virtual	void CallMe(TInt& aCallCount);	
protected:
	CTestCanceller* iCanceller;
	CTelephony* iTelephony;
	CEtelIsvTestStep* iTestStep;
	TRequestStatus iExpStat;
	};
 
class MActionParent
	{
public:

enum TActionEvent
	{
	EDialNewCall,
	EHangup,
	EAnswerIncomingCall
	};
	
	virtual void ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent) = 0;
	};

#endif // _TE_ETELISVBASE_H_
