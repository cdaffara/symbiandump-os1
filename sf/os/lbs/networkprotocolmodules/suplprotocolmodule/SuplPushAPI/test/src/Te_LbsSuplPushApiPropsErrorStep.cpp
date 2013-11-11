// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// These are the environment error unit tests for the LbsSuplPushProps class
// 
//

#include <e32property.h>

#include "Te_LbsSuplPushApiPropsErrorStep.h"
#include "Te_LbsSuplErrorPattern.h"
#include "lbssuplpushprops.h"
#include "Te_LbsSuplPushRProperty.h"
#include "Te_LbsSuplCommon.h"

/**
 * Constructor. It must	call the SetTestStepName as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID. 
 */
CTe_LbsSuplPushApiPropsErrorStep::CTe_LbsSuplPushApiPropsErrorStep()
	{
	SetTestStepName(KLbsSuplPushApiPropsErrorStep);
	}

/**
Override of the base class virtual function. Contains the initialization code required for 
the test step. 

The error pattern is opened here.
 
@return - The result of the preamble.

@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushApiPropsStep::doTestStepPreambleL
*/
TVerdict CTe_LbsSuplPushApiPropsErrorStep::doTestStepPreambleL()
	{
	RTe_LbsSuplErrorPattern::OpenL();
	return CTe_LbsSuplPushApiPropsStep::doTestStepPreambleL();
	}

/**
Override of the base class pure virtual function. Contains the test code to run. 

Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass. 

This test step simulates the errors of the RProperty::Define calls. 
 
@return - The result of the test 

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepL
*/
TVerdict CTe_LbsSuplPushApiPropsErrorStep::doTestStepL()
	{
	TInt count=1;
	do	{
		RTe_LbsSuplErrorPattern::Reset();
		RTe_LbsSuplErrorPattern::AppendNextErrorL(Te_LbsSuplErrorId::ERProperty_Define,
				count, KErrGeneral, EFalse);
		
		CTe_LbsSuplPushApiPropsStep::doTestStepPreambleL();
		
		TRAPD(err, LbsSuplPushProps::InitializeL());
		if(RTe_LbsSuplErrorPattern::ErrorReached()==1 && err!=KErrGeneral)
			{
			SetTestStepResult(EFail);
			SetTestStepError(err);
			return TestStepResult();
			}
		
		count++;
		}
	while(RTe_LbsSuplErrorPattern::ErrorReached()==1);	
	  
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
Override of the base class virtual function. Contains the cleanup code to release 
the test step resources. 

The error pattern is closed here.
 
@return - The result of the postamble.

@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushApiPropsStep::doTestStepPostambleL
*/
TVerdict CTe_LbsSuplPushApiPropsErrorStep::doTestStepPostambleL()
	{
	RTe_LbsSuplErrorPattern::Close();
	return CTe_LbsSuplPushApiPropsStep::doTestStepPostambleL();
	}

