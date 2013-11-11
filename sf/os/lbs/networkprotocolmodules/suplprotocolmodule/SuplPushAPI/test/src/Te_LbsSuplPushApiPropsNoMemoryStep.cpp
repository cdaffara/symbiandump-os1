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
// These are the memory allocation failure unit tests for the LbsSuplPushProps class
// 
//

#include <e32property.h>

#include "Te_LbsSuplPushApiPropsNoMemoryStep.h"
#include "lbssuplpushprops.h"

/**
 * Constructor. It must	call the SetTestStepName as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID. 
 */
CTe_LbsSuplPushApiPropsNoMemoryStep::CTe_LbsSuplPushApiPropsNoMemoryStep()
	{
	SetTestStepName(KLbsSuplPushApiPropsNoMemoryStep);
	}


/**
Override of the base class pure virtual function. Contains the test code to run. 

Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass. 

This test step simulates the memory errors while calling the LbsSuplPushProps::InitializeL. 
 
@return - The result of the test 

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepL
*/
TVerdict CTe_LbsSuplPushApiPropsNoMemoryStep::doTestStepL()
	{	
//This test works only for the debug build of the OS. Because the User::__DbgSetAllocFail call works only for the 
//udeb build of the OS. Please note that the test code itself must work both in the urel and udeb builds. 
	
	User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, 1);
	
	TInt* marker = new TInt(0);
	delete marker;
	
	if(marker) 
		{
		//The OS is in the urel mode. So, stops here.
		return TestStepResult();
		}
	else
		{
		marker = new TInt(0);
		delete marker;
		}
	
	for(TInt i=1; marker; i++)
		{
		User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, i);
	
		TRAPD(err, LbsSuplPushProps::InitializeL());
		marker = new TInt(0);
		delete marker;
		if(err==KErrNone && CheckProps()!=KErrNone)
			{
			return TestStepResult();
			}
		}
	
	return TestStepResult();
	}

