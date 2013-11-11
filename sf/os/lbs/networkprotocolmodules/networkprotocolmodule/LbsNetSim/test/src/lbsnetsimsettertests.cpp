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
 @file lbsnetsimsettertests.cpp
*/
#include "lbsnetsimsettertests.h"
#include "Te_lbsnetsimtestSuiteDefs.h"

#include <lbs/lbsnetcommon.h>

CLbsSetterTests::~CLbsSetterTests()
/**
 * Destructor
 */
	{
	}

CLbsSetterTests::CLbsSetterTests()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSetterTests);
	}

TVerdict CLbsSetterTests::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	__UHEAP_MARK;
	
	iTest.ConnectL(NULL);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsSetterTests::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	if (TestStepResult()==EPass)
		{
		
		TBool success = ETrue;
		
		if (success)
			{
			TPosition position;
			success = iTest.SetReferenceLocation(position);
			}
			
		if (success)
			{
			success = iTest.SetResponseTime(10, ETrue);
			if (success)
				{
				success = iTest.SetResponseTime(10, EFalse);
				}
			}
			
		if (success)
			{
			success = iTest.SetResponseError(KErrAbort, ETrue);
			if (success)
				{
				success = iTest.SetResponseError(KErrAbort, EFalse);
				}
			}
			
		if (success)
			{
			TLbsAssistanceDataGroup group = EAssistanceDataReferenceTime | EAssistanceDataReferenceLocation;
			success = iTest.SetDefaultAssitanceDataFilter(group);
			}

		if (success)
			{
			TLbsAssistanceDataGroup group = EAssistanceDataReferenceTime | EAssistanceDataReferenceLocation;
			success = iTest.SetMoreAssitanceDataFilter(group);
			}
			
		if (success)
			{
			success = iTest.ClearAssistanceDataFilters();
			}
			
		if (success)
			{
			success = iTest.SetAssistanceDataProvider(TUid::Uid(0));
			}

		if (success)
			{
			success = iTest.SetEmergenyStatus(ETrue);
			if (success)
				{
				success = iTest.SetEmergenyStatus(EFalse);
				}
			}

		if (success)
			{
			success = iTest.SetRoamingStatus(ETrue);
			if (success)
				{
				success = iTest.SetRoamingStatus(EFalse);
				}
			}
	
		if (success)
			{
			TLbsNetPosRequestQuality quality;
			success = iTest.SetQuality(quality);
			}

		if (success)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
		
	  return TestStepResult();
	}



TVerdict CLbsSetterTests::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iTest.Close();
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
