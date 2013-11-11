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


#ifndef __TE_ETELISVCALLFUNC_H_
#define __TE_ETELISVCALLFUNC_H_

class TTestCallParams 
	{
public:
	CTelephony::TPhoneLine iLine;
	CTelephony::TCallParamsV1Pckg* iCallParamsV1Pckg;
	CTelephony::TTelAddress iTelAddress;
	TRequestStatus iExpStat;
	TInt iExpAPIErr;
	CTelephony::TCallId iCallId; 
	};

class CTestDialNewCall : public CEtelIsvTestStep
	{
public:
	CTestDialNewCall() ;
	~CTestDialNewCall(){} ;
	virtual enum TVerdict doTestStepL();
	};

class MActionParent;	
class CTestDialNewCallAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestDialNewCallAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony, TBool aUseMacros = ETrue);
	static CTestDialNewCallAct* NewL(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
	void ConstructL(); 
	~CTestDialNewCallAct();
	void TestDialNewCall(TTestCallParams& aTestCallParams, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestDialNewCallAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent, TBool aUseMacros);
private:	// data
	TTestCallParams* iTestCallParams;
	MActionParent* iParent;
	TBool iUseMacros;
	
	TPckgBuf<CTelephony::TCallParamsV1> iCallParamsBuff;
	};

class CTestAnswerIncomingCall : public CEtelIsvTestStep
	{
public:
	CTestAnswerIncomingCall(TInt& aCallCount) ;
	~CTestAnswerIncomingCall(){} ;
	virtual enum TVerdict doTestStepL();
private:
	TInt& iCallCount;
	};
	
	
class CTestAnswerIncomingCallAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestAnswerIncomingCallAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony);
	static CTestAnswerIncomingCallAct* NewL(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
	void ConstructL(); 
	~CTestAnswerIncomingCallAct();
	void TestAnswerIncomingCall(TTestCallParams& aTestCallParams, TBool aCheckConcurrency = EFalse); 
	void DoCancel();
	void RunL();
protected:
	CTestAnswerIncomingCallAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
private:	// data
	TTestCallParams* iTestCallParams;
	MActionParent* iParent; 
	};
	
class CTestHold : public CEtelIsvTestStep
	{
public:
	CTestHold() ;
	~CTestHold(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestHoldAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestHoldAct*  NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony);
	void ConstructL(); 
	~CTestHoldAct();
	void TestHold(TTestCallParams& aTestCallParams, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestHoldAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TCallId iCallId;
	};
	
class CTestResume : public CEtelIsvTestStep
	{
public:
	CTestResume() ;
	~CTestResume(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestResumeAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestResumeAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony);
	void ConstructL(); 
	~CTestResumeAct();
	void TestResume(TTestCallParams& aTestCallParams, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestResumeAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TCallId iCallId; 
	};
	
class CTestSwap : public CEtelIsvTestStep
	{
public:
	CTestSwap() ;
	~CTestSwap(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSwapAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestSwapAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony);
	void ConstructL(); 
	~CTestSwapAct();
	void TestSwap(TTestCallParams& aTestCallParams1, TTestCallParams& aTestCallParams2, TBool aCheckConcurrency = EFalse);        
	void DoCancel();
	void RunL();
protected:
	CTestSwapAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TCallId* iCallId1; 
	CTelephony::TCallId* iCallId2; 
	};
	
class CTestHangup : public CEtelIsvTestStep
	{
public:
	CTestHangup() ;
	~CTestHangup(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestHangupAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestHangupAct* NewLC(CEtelIsvTestStep* aTestStep,CTelephony* aTelephony, TBool aUseMacros = ETrue);
	static CTestHangupAct* NewL(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
	void ConstructL(); 
	~CTestHangupAct();
	void TestHangup(TTestCallParams& aTestCallParams, TBool aCheckConcurrency = EFalse);
	void DoCancel();
	void RunL();
protected:
	CTestHangupAct(CEtelIsvTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent, TBool aUseMacros);
private:	// data
	TTestCallParams* iTestCallParams;
	MActionParent* iParent;
	TBool iUseMacros;
	};
	
class CTestHoldResume2Calls : public CEtelIsvTestStep
	{
public:
	CTestHoldResume2Calls() ;
	~CTestHoldResume2Calls(){} ;
	virtual enum TVerdict doTestStepL();
	//TInt maxCallStates;
	};
	
class CTestGetCallDynamicCaps : public CEtelIsvTestStep	//test GetCallDynamicCaps()
	{
public:
	CTestGetCallDynamicCaps() ;
	~CTestGetCallDynamicCaps(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCallFuncFlightModeOnWithPublishSubscribe : public CEtelIsvTestStep
	{
public:
	CTestCallFuncFlightModeOnWithPublishSubscribe(TInt& aCallCount) ;
	~CTestCallFuncFlightModeOnWithPublishSubscribe(){} ;
	virtual enum TVerdict doTestStepL();
	
private:
	TInt& iCallCount;
	};

class CTestDeprecatedDataCallAPIs : public CEtelIsvTestStep
	{
public:
	CTestDeprecatedDataCallAPIs();
	virtual enum TVerdict doTestStepL();
	};

#endif
