// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file testltsycallcontrolbase.h
//  

#ifndef TESTLTSYCALLCONTROLBASE_H_
#define TESTLTSYCALLCONTROLBASE_H_

//System include
#include "te_integration_stltsysuitestepbase.h"
#include "testltsycallshareddata.h"
#include "testltsymodel.h"


/**
 * This is the test step base class of Call Control test 
 * category which provides methods
 */

class CTestLtsyCallControlBase : public CTe_integration_stltsySuiteStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	virtual ~CTestLtsyCallControlBase();
	
	CTestLtsyCallControlBase(CTestLtsyModel& aModel);
	
protected:	
	virtual TVerdict doTestStepPreambleL();
	
	virtual TVerdict doTestStepPostambleL();
	
	virtual TVerdict doTestStepL();
	
	void OpenPhoneL();
	
	void OpenLineL();
	
	void CloseLine();
	
	void OpenCustomAPIL();
	
	void CloseCustomAPI();
	
	void CustomAPITerminateAllCallsL();
	
	void PhoneTerminateAllCallsL();
	
	void CreateThirtyPartyTelephonyL();

	void OpenCallOneL();
	
	void OpenCallTwoL();
	
	void CloseCallOne();
	
	void CloseCallTwo();	
	
	void DialCallOne();
	
	void CallOneDialBadNumberL();
	
	void CallOneDialSelfNumberL();
	
	void DialCallTwo();
	
	void HangupCallOneL();
	
	void HangupCallTwoL();
	
	void HoldCallOne();
	
	void HoldCallTwo();
	
	void ResumeCallOne();
	
	void ResumeCallTwo();
	
	void SwapCallOne();
	void NotifyCallOne();

	void SwapCallTwo();
	void NotifyCallTwo();
	
	void NotifyIncomingCallOneL();
	
	void NotifyIncomingCallTwoL();
	
	void AnswerIncomingCallOneL();
	
	void AnswerIncomingCallTwoL();
	
protected:
	
	CTestLtsyModel& iLtsyModel;
	};

_LIT(KTestLtsyCallControlBase,   "CTestLtsyCallControlBase");
#endif /*TESTLTSYCALLCONTROLBASE_H_*/
