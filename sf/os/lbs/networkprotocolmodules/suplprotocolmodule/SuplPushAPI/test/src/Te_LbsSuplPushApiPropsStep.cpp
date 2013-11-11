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
// These are the unit tests for the LbsSuplPushProps class
// 
//

#include <e32property.h>

#include "Te_LbsSuplPushApiPropsStep.h"
#include "lbssuplpushprops.h"

/**
 * Constructor. It must	call the SetTestStepName as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID. 
 */
CTe_LbsSuplPushApiPropsStep::CTe_LbsSuplPushApiPropsStep()
	{
	SetTestStepName(KLbsSuplPushApiPropsStep);
	}


/**
 * Destructor.
 */
CTe_LbsSuplPushApiPropsStep::~CTe_LbsSuplPushApiPropsStep()
	{
	//Intentionally left blank
	}

/**
Overrides the base class pure virtual function. Contains the initialization code required for 
the test step. 

Deletes all the properties involved.
 
@return - always EPass. If error happens, it leaves.

@leave If a error happens, it leaves with one of the system error codes.

@see CTe_LbsSuplPushApiPropsStep::doTestStepPreambleL
*/
TVerdict CTe_LbsSuplPushApiPropsStep::doTestStepPreambleL()
	{
	
	//Removing all the properties to provide a clean environment
	TInt err =  RProperty::Delete(KLbsSuplPushSmsBusyKey);	
	if (err==KErrNone || err==KErrNotFound)
		{
		err =  RProperty::Delete(KLbsSuplPushSmsInitKey);
		}
	if (err==KErrNone || err==KErrNotFound)
		{
		err =  RProperty::Delete(KLbsSuplPushSmsAckKey);
		}
	if (err==KErrNone || err==KErrNotFound)
		{
		err =  RProperty::Delete(KLbsSuplPushWapBusyKey);
		}
	if (err==KErrNone || err==KErrNotFound)
		{
		err =  RProperty::Delete(KLbsSuplPushWapInitKey);
		}
	if (err==KErrNone || err==KErrNotFound)
		{
		err =  RProperty::Delete(KLbsSuplPushWapAckKey);
		}
	
	if(err!=KErrNone && err!=KErrNotFound)
		{
		User::LeaveIfError(err);
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
Override of the base class pure virtual function. Contains the test code to run. 

This test step defines the props when they all are deleted and then again, when they all are
defined. In both cases the function must succeed and the properties must be defined correctly.

@return - The result of the test 

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepL
*/
TVerdict CTe_LbsSuplPushApiPropsStep::doTestStepL()
	{
	TRAPD(err, LbsSuplPushProps::InitializeL());
	if(err!=KErrNone)
		{
		SetTestStepResult(EFail);
		SetTestStepError(err);
		return TestStepResult();
		}
	
	//Check that the properties are defined
	if (CheckProps()!=KErrNone)
		{
		return TestStepResult();
		}
	
	//Define all props again
	TRAP(err, LbsSuplPushProps::InitializeL());
	if(err!=KErrNone)
		{
		SetTestStepResult(EFail);
		SetTestStepError(err);
		return TestStepResult();
		}
	
	//Try to define alreafy existing props
	TRAP(err, LbsSuplPushProps::InitializeL());
	if(err!=KErrNone)
		{
		SetTestStepResult(EFail);
		SetTestStepError(err);
		return TestStepResult();
		}
	
	//Check that the properties are defined
	CheckProps();
	return TestStepResult();
	}


/**
Overrides the base class virtual function.
 
@return - The result of the postamble.

@see CTestStep::doTestStepPostambleL
*/
TVerdict CTe_LbsSuplPushApiPropsStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
Checks that all properties are defined correctly and deletes them.

@return KErrNone if successfull, error code - otherwise. 
*/
TInt CTe_LbsSuplPushApiPropsStep::CheckProps()
	{
	TInt err =  RProperty::Delete(KLbsSuplPushSmsBusyKey);	
	if (err==KErrNone)
		{
		err =  RProperty::Delete(KLbsSuplPushSmsInitKey);
		}
	if (err==KErrNone)
		{
		err =  RProperty::Delete(KLbsSuplPushSmsAckKey);
		}
	if (err==KErrNone)
		{
		err =  RProperty::Delete(KLbsSuplPushWapBusyKey);
		}
	if (err==KErrNone)
		{
		err =  RProperty::Delete(KLbsSuplPushWapInitKey);
		}
	if (err==KErrNone)
		{
		err =  RProperty::Delete(KLbsSuplPushWapAckKey);
		}
	
	if(err!=KErrNone)
		{
		SetTestStepError(err);
		SetTestStepResult(EFail);
		}
	
	return err;
	}

