// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of AO necessary to answer/hold/resume calls with Etel3rdParty.
// 
//

/**
 @file 
 @internalTechnology
*/
 

#ifndef __TE_CAPSCALLAO_H_
#define __TE_CAPSCALLAO_H_

#include <etel3rdparty.h>
#include <test/testexecutestepbase.h>

/** 
 * Base class for test AOs
 */
class CEtelIsvActBase : public CActive
	{
public:
	CEtelIsvActBase(CTestStep* aTestStep, CTelephony* aTelephony)
	:CActive(CActive::EPriorityStandard), iTelephony(aTelephony), iTestStep(aTestStep) {};

protected:
	CTelephony* iTelephony;
	CTestStep* iTestStep;
	};
 

/**
 * Specifies the action interface
 */
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


/**
 * Specifies the call parameters
 */

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


/**
 * The class defines AO to handle mobile orginated call creation
 */
class CTestDialNewCallAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestDialNewCallAct* NewLC(CTestStep* aTestStep, CTelephony* aTelephony);
	static CTestDialNewCallAct* NewL(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
	void ConstructL(); 
	~CTestDialNewCallAct();
	void TestDialNewCall(TTestCallParams& aTestCallParams);
	void DoCancel();
	void RunL();
protected:
	CTestDialNewCallAct(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
private:	// data
	TTestCallParams* iTestCallParams;
	MActionParent* iParent;
	};


/**
 * The class defines AO to handle hanging up a call. 
 */

class CTestHangupAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestHangupAct* NewLC(CTestStep* aTestStep, CTelephony* aTelephony);
	static CTestHangupAct* NewL(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
	void ConstructL(); 
	~CTestHangupAct();
	void TestHangup(TTestCallParams& aTestCallParams);
	void DoCancel();
	void RunL();
protected:
	CTestHangupAct(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent);
private:	// data
	TTestCallParams* iTestCallParams;
	MActionParent* iParent;
	};
	

/**
 * The class defines AO to handle putting the call on hold. 
 */
class CTestHoldAct : public CEtelIsvActBase
	{
public:	// functions
	static CTestHoldAct*  NewLC(CTestStep* aTestStep, CTelephony* aTelephony);
	void ConstructL(); 
	~CTestHoldAct();
	TInt TestHold(TTestCallParams& aTestCallParams);
	void DoCancel();
	void RunL();
protected:
	CTestHoldAct(CTestStep* aTestStep, CTelephony* aTelephony);
private:	// data
	CTelephony::TCallId iCallId;
	};

#endif // __TE_CAPSCALLAO_H_
