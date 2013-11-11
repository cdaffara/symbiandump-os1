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
// Definition of all test objects required to verify capability check on 
// ETel3rdParty call functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/
 
#ifndef __CALLFUNCTSTEPS_H__
#define __CALLFUNCTSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_etelisvcapsstepbase.h"
#include "te_capscallao.h"

//
class CHoldTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CHoldTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallParamsV1 iCallParams;
	CTelephony::TCallParamsV1Pckg iCallParamsPckg;
	};

_LIT(KCapsTestHoldStr,"HoldTestStep");

//

class CDialNewCallTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CDialNewCallTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallParamsV1 iCallParams;
	CTelephony::TCallParamsV1Pckg iCallParamsPckg;
	CTelephony::TCallId iCallId;
	};

_LIT(KCapsTestDialNewCallStr,"DialNewCallTestStep");

//

class CGetCallDynamicCaps : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCallDynamicCaps();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallCapsV1 iCallCaps;
	CTelephony::TCallCapsV1Pckg iCallCapsPckg;
	};

_LIT(KCapsTestGetCallDynamicCapsStr,"GetCallDynamicCapsTestStep");

//

class CGetCallStatus : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCallStatus();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallStatusV1 iCallStatus;
	CTelephony::TCallStatusV1Pckg iCallStatusPckg;
	};

_LIT(KCapsTestGetCallStatusStr,"GetCallStatusTestStep");

//

class CResumeTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CResumeTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallParamsV1 iCallParams;
	CTelephony::TCallParamsV1Pckg iCallParamsPckg;
	};

_LIT(KCapsTestResumeStr,"ResumeTestStep");

//

class CSwapTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CSwapTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

	};

_LIT(KCapsTestSwapStr,"SwapTestStep");

//
class CHangUpTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CHangUpTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

	};

_LIT(KCapsTestHangUpStr,"HangUpTestStep");

//
class CAnswerIncomingCallTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CAnswerIncomingCallTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallId iCallId;
	};

_LIT(KCapsTestAnswerIncomingCallStr,"AnswerIncomingCallTestStep");

//
#endif // __CALLFUNCTSTEPS_H__
