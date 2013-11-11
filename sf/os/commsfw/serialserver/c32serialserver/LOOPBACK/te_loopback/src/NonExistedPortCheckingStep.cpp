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
// Example CTestStep derived implementation
// 
//

/**
 @file NonExistedPortCheckingStep.cpp
 @internalTechnology
*/
#include "NonExistedPortCheckingStep.h"
#include "Te_loopbackSuiteDefs.h"

CNonExistedPortCheckingStep::~CNonExistedPortCheckingStep()
/**
 * Destructor
 */
	{
	}

CNonExistedPortCheckingStep::CNonExistedPortCheckingStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KNonExistedPortCheckingStep);
	}

TVerdict CNonExistedPortCheckingStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_loopbackSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CNonExistedPortCheckingStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt portNum = 9; // Loopback support only 8 ports
		TBuf<50> msg;
		_LIT(KMsgFmt,"Comm port <%d> cannot be opened");
		HBufC* KNonExistedPortName = AllocPortNameL(portNum);
		CleanupStack::PushL(KNonExistedPortName);
		msg.Format(KMsgFmt, portNum);
		
		TestErrorCodeL(iPortList[0].port.Open(iCommServer, KNonExistedPortName->Des(), ECommExclusive, ECommRoleDCE), KErrNotSupported, msg);

		SetTestStepResult(EPass);
		CleanupStack::PopAndDestroy(KNonExistedPortName);
		}
	  return TestStepResult();
	}



TVerdict CNonExistedPortCheckingStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_loopbackSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

