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


#ifndef __TE_ETELISVNOTIFIERS_H_
#define __TE_ETELISVNOTIFIERS_H_

#include <etel3rdparty.h>
#include "TE_EtelIsvCallFunc.h"


/**
 * This class implements a state machine that executes a series of asynchronous
 * requests.
 */
class CTestNotifyChange : public CEtelIsvTestStep
	{
public:
	CTestNotifyChange() ;
	~CTestNotifyChange(){} ;
	virtual enum TVerdict doTestStepL();
private:
	TPckgBuf<CTelephony::TFlightModeV1> iNotifierStatusBuff;
	TPckgBuf<CTelephony::TFlightModeV1> iFmNotifierStatusBuff;
	TPckgBuf<CTelephony::TIndicatorV1> iIndNotifierStatusBuff;
	TPckgBuf<CTelephony::TBatteryInfoV1> iBatNotifierStatusBuff;
	TPckgBuf<CTelephony::TSignalStrengthV1> iSigNotifierStatusBuff;
	TPckgBuf<CTelephony::TIccLockInfoV1> iLocNotifierStatusBuff;
	TPckgBuf<CTelephony::TCallStatusV1> iLinNotifierStatusBuff;
	TPckgBuf<CTelephony::TCallStatusV1> iFaxDataLinExpNotifierStatusBuff;
	TPckgBuf<CTelephony::TCallStatusV1> iCalNotifierStatusBuff;
	TPckgBuf<CTelephony::TRemotePartyInfoV1> iRemNotifierStatusBuff;
	TPckgBuf<CTelephony::TNetworkRegistrationV1> iRegNotifierStatusBuff;
	TPckgBuf<CTelephony::TNetworkInfoV1> iNetNotifierStatusBuff;
	TPckgBuf<CTelephony::TNetworkInfoV2> iNetNotifierStatusBuffV2;
	};
	
class CTestNotifyChangeCancel : public CEtelIsvTestStep
	{
public:
	CTestNotifyChangeCancel() ;
	~CTestNotifyChangeCancel(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestCallAndNotify : public CEtelIsvTestStep
	{
public:
	CTestCallAndNotify() ;
	~CTestCallAndNotify(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestRegisterNotifierAct : public CEtelIsvActBase, public MActionParent
	{
public:	// functions
	static CTestRegisterNotifierAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& iNumberNotifierTestsActive, CTelephony::TNotificationEvent aEvent);
	void ConstructL();
	~CTestRegisterNotifierAct();
	void StartNotifier(TDes8& aNotifierData, TRequestStatus aExpStat, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
	void MakeCallL();
	
	void SetNetworkInfoVersion(TInt aVersion);

protected:
	CTestRegisterNotifierAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aNumberNotifierTestsActive, CTelephony::TNotificationEvent aEvent);
	void SendNotifyRequest(TBool aCheckConcurrency = EFalse);
	void ProcessFlightModeChangeNotification();
	void ProcessIndicatorNotification();
	void ProcessBatteryInfoNotification();
	void ProcessSignalStrengthNotification();
	void ProcessLockInfoNotification();
	void ProcessLineStatusNotification();
	void ProcessCallStatusNotification();
	void ProcessRemotePartyInfoNotification();
	void ProcessNetworkRegistrationStatusNotification();
	void ProcessCurrentNetworkInfoNotification();
	void InvokeNotifyConditionL();
	void HangupCallL();
	void ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent);	//from MActionParent
public:
	RArray<TPtr8> iExpNotificationCompleteList;
private:	// data
	TDes8* iNotifierData;
	CTelephony::TNotificationEvent iEvent;
	TInt& iNumberNotifierTestsActive;
	TInt iNotificationCount;
	TInt iNumberNotificationsBeforeFinish;
	CTestDialNewCallAct* iDialNewCallAct;
	CTestHangupAct* iHangupAct;
	CTelephony::TCallParamsV1 iCallParams;
	TTestCallParams iTestCallParams;
	CTelephony::TCallParamsV1Pckg iCallParamsPckg;
	TBool iCallsActive;
	/**
	Version of TNetworkInfo being tested.
	*/
	TInt iNetworkInfoVersion;
	};
	
class CTestCancelRegisterNotifierAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestCancelRegisterNotifierAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent);
	void ConstructL();
	~CTestCancelRegisterNotifierAct();
	void StartNotifier(TDes8& aNotifierPckg, TRequestStatus aExpStat);
	void DoCancel();
	void RunL();
protected:
	CTestCancelRegisterNotifierAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent);
public:
	RArray<TPtr8> iExpNotificationCompleteList;
private:
	TDes8* iNotifierPckg;
	CTelephony::TNotificationEvent iEvent;
	CTelephony::TCallParamsV1 iCallParams;
	TTestCallParams iTestCallParams;
	CTelephony::TCallParamsV1Pckg iCallParamsPckg;
	};
	
class CTestNotifyCallClosureAct : public CEtelIsvActBase, public MActionParent
	{
public:	// functions
	static CTestNotifyCallClosureAct* NewLC(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent);
	void ConstructL();
	~CTestNotifyCallClosureAct();
	void StartNotifier(TDes8& aNotifierPckg, TRequestStatus aExpStat);
	void DoCancel();
	void RunL();
	void MakeCallL();
protected:
	CTestNotifyCallClosureAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, CTelephony::TNotificationEvent aEvent);
	void SendNotifyRequest();
	void InvokeNotifyCondition();
	void HangupCallL();
	void ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent);	//from MActionParent
public:
	RArray<TPtr8> iExpNotificationCompleteList;
private:	// data
	TDes8* iNotifierPckg;
	CTelephony::TNotificationEvent iEvent;
	TInt iNotificationCount;
	TInt iNumberNotificationsBeforeFinish;
	CTestDialNewCallAct* iDialNewCallAct;
	CTestHangupAct* iHangupAct;
	CTelephony::TCallParamsV1 iCallParams;
	TTestCallParams iTestCallParams;
	CTelephony::TCallParamsV1Pckg iCallParamsPckg;
	TBool iCallsActive;
	};
	
#endif // __TE_ETELISVNOTIFIERS_H_
