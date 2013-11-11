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


#ifndef __TE_ETELISVLINEINFO_H_
#define __TE_ETELISVLINEINFO_H_

class CTestGetLineStatus : public CEtelIsvTestStep	//tests GetLineInfo()
	{
public:
	CTestGetLineStatus(TInt& aCallCount) ;
	~CTestGetLineStatus(){} ;
	virtual enum TVerdict doTestStepL();
private:
	TInt& iCallCount;
	TPckgBuf<CTelephony::TCallStatusV1> iStatusV1Buff;
	};
	
class CTestGetCallInfo : public CEtelIsvTestStep	//tests GetCallInfo()
	{
public:
	CTestGetCallInfo() ;
	~CTestGetCallInfo(){} ;
	virtual enum TVerdict doTestStepL();
	void TestResults(CTelephony::TCallInfoV1& aActualCallInfo, CTelephony::TCallInfoV1& aExpCallInfo, CTelephony::TRemotePartyInfoV1& aActualRemotePartyInfo,  CTelephony::TRemotePartyInfoV1& aExpRemotePartyInfo);

private:
	TPckgBuf<CTelephony::TCallInfoV1> iCallInfoV1Buff;
	TPckgBuf<CTelephony::TRemotePartyInfoV1> iRemoteInfoV1Buff;
	TPckgBuf<CTelephony::TCallSelectionV1> iCallSelectionV1Buff;
	};
		

//AO classes used in test steps
class TTestCallParams;
class CTestDialNewCallAct;
class CTestHangupAct;
class CTestAnswerIncomingCallAct;

class CNotifyLineStatusBaseAct: public CEtelIsvActBase, public MActionParent 
	{
public:
	static void PrintLineStatus(CEtelIsvTestStep* aTestStep, TDes& aLine, const CTelephony::TCallStatus aStatus);
	static void PrintGetCallInfo(CEtelIsvTestStep* aTestStep,  CTelephony::TCallSelect aCallSelect, CTelephony::TCallInfoV1& aCallInfoV1, CTelephony::TRemotePartyInfoV1& aRemoteInfoV1);
	
	enum TTestToRun
		{
		EGetLineStatus,
		EGetCallInfo,
		EGetCallStateAndDynamicCaps
		};
	
	virtual void StartTestL(TTestCallParams& aTestCallParams, TTestToRun aTestToRun)=0;
protected:
	CNotifyLineStatusBaseAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
	void CallGetCallInfo(const CTelephony::TCallSelect aSelect, CTelephony::TCallStatus aExpStatus);
protected:
	TTestCallParams* iTestCallParams;
	CTelephony::TCallStatusV1 iCallStatusV1;
	CTelephony::TCallStatusV1Pckg iCallStatusV1Pckg;
	TBool iFinish;
	TTestToRun iTestToRun;
	CTelephony::TNotificationEvent iEvent;
	CTestHangupAct* iHangupAct;	
	};

class CNotifyLineStatusMOAct : public CNotifyLineStatusBaseAct 
	{
public:	// functions
	
	static CNotifyLineStatusMOAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony);
	void ConstructL(); 
	~CNotifyLineStatusMOAct();
	void StartTestL(TTestCallParams& aTestCallParams, TTestToRun aTestToRun);	//from CNotifyLineStatusBaseAct
	void DoCancel();
	void RunL();
	void ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent);	//from MActionParent
protected:
	CNotifyLineStatusMOAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data	
	CTestDialNewCallAct* iDialNewCallAct;
	};

class CNotifyLineStatusMTAct : public CNotifyLineStatusBaseAct 
	{
public:	// functions
	
	static CNotifyLineStatusMTAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony, TInt& aCallCount);
	void ConstructL(); 
	~CNotifyLineStatusMTAct();
	void StartTestL(TTestCallParams& aTestCallParams, TTestToRun aTestToRun);	//from CNotifyLineStatusBaseAct
	void DoCancel();
	void RunL();
	void ActionParent(const TRequestStatus aCallStatus, const TActionEvent aEvent);	//from MActionParent
protected:
	CNotifyLineStatusMTAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aCallCount);
private:	// data	
	CTestAnswerIncomingCallAct* iAnswerIncomingCallAct;
	TInt& iCallCount;
	};
	
class CTestLineFuncFlightModeOnWithPublishSubscribe : public CEtelIsvTestStep
	{
public:
	CTestLineFuncFlightModeOnWithPublishSubscribe() ;
	~CTestLineFuncFlightModeOnWithPublishSubscribe(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CNotifyLineStatusNoCapsMTAct : public CNotifyLineStatusMTAct 
 	{
 public:	
 	static CNotifyLineStatusNoCapsMTAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony, TInt& aCallCount);
 	void RunL();
 protected:
 	CNotifyLineStatusNoCapsMTAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, TInt& aCallCount);
 	};

class CTestGetLineStatusNoCaps : public CEtelIsvTestStep	//tests GetLineInfo()
 	{
 public:
 	CTestGetLineStatusNoCaps(TInt& aCallCount) ;
 	~CTestGetLineStatusNoCaps(){} ;
 	virtual enum TVerdict doTestStepL();
 private:
 	TInt& iCallCount;
 	};	

#endif
