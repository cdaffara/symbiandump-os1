// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CsdAgtTestClosure.cpp
// This contains CsdAgt TestCase Test 4.1
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "log.h"
#include "teststep.h"
#include "TestStepCsdAgt.h"
#include "TestSuiteCsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"

// EConnectionOpen
#include <csdprog.h>

// Class definitions
#include "CsdAgtTestOOM.h"

#define MaxAllocationFailures 1500
#define MaxConsecutiveSuccess 5		//maximum number of consecutive successful operations
									//indicating that from now on the __HEAP_FAILNEXT()
									//cannot cause a failure anymore

/**
 * Test case 4.1
 *
 */
enum TVerdict CCsdAgtTest4_1::doCsdAgtTestStepL( void )
	{
	TInt failCount=500;
	TInt successCount=0;
	TInt ret=-1;

	TRealAgtNotify AgtNotify; 

	AgtNotify.SetTestCase(10401);
	SetGSMModeL(AgtNotify);

	while(failCount < MaxAllocationFailures && successCount < MaxConsecutiveSuccess)
		{
		// Mark heap and set allocation to fail
		__UHEAP_MARK;
		__UHEAP_FAILNEXT(failCount);

	    Log(_L("Incrementing __HEAP_FAILNEXT(%d) of %d"), failCount, MaxAllocationFailures);

		// Connect, most likely fail at some point
		TRAP(ret,NormalConnectL());
	
		User::After(200000); // Allow async libraries to unload

		// Check all heap is free'd
		__UHEAP_MARKEND;

		if(ret == KErrNone)
			{
			// Allocation failure has been moved all the way through the test
			// Succeeded after failCount allocations
			//return iTestStepResult;

			// Clear __UHEAP_FAILNEXT
			__UHEAP_RESET;
			
			//Operation may succeed even in OOM test due to interuption 
			//in propagating the exception, so let it to continue to the 
			//maximum number of iterations. (the next iteration might fail)
			Log(_L("Operation completed @ iteration %d"), failCount);

			//Mark the success
			//At this stage we don't know if it's a real success
			successCount++;
			}
		else if(ret == KErrNoMemory)
			{
			// Reset test result to pass since the allocation failure most
			// likely caused it to fail
			iTestStepResult = EPass;

			//Start counting the consecutive successful operations again
			successCount=0;
			}
		else 
			{
			// Failure other than Memory failure this would be the result of a bug
			iTestStepResult = EFail;
			return iTestStepResult;
			}
		Log(_L("Run completed with %d after failcount of %d and successcount of %d"), ret, failCount, successCount);

		// Try failing a little further into the process
		failCount++;
		}

	if (failCount == MaxAllocationFailures)
		{
		// We have reached our maximum number of allocation failures
		// There must be some other problem within the NormalConnect() function
		iTestStepResult=EFail;
		}

	return iTestStepResult;
	}
