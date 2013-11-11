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
// PsdAgtTestClosure.cpp
// This contains PsdAgt TestCase Test 13.1 - 13.2
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "networking/log.h"
#include "networking/teststep.h"
#include "TestStepPsdAgt.h"
#include "TestSuitePsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"

// RPacketContext
#include <etelpckt.h> 

// EPsdStartingConfiguration
#include <psdprog.h>

// EConnectionOpen
#include <connectprog.h>

// Class definitions
#include "PsdAgtTestOOM.h"


#define MaxAllocationFailures 1000

// Test case 15.1
enum TVerdict CPsdAgtTest15_1::doPsdAgtTestStepL( void )
	{
	TInt failCount=1;
	TInt ret=-1;

	TRealAgtNotify agtNotify; 
	
	agtNotify.SetTestCase(1501);

	while(failCount < MaxAllocationFailures)
		{
		// Mark heap and set allocation to fail
		__UHEAP_MARK;		
		__UHEAP_FAILNEXT(failCount);

		Log(_L("Incrementing __HEAP_FAILNEXT() to %d"),failCount);

		// Connect, most likely fail at some point
		TRAP(ret,NormalConnectOOML());
	
		User::After(200000); // Allow async libraries to unload

		// Check all heap is free'd
		__UHEAP_MARKEND;
		
		if(ret == 0)
			{
			// Allocation failure has been moved all the way through the test
			// Succeeded after failCount allocations
			return iTestStepResult;
			}
		else 
			{
			Log(_L("NormalConnectL : Failed with %d"),ret);

			// Reset test result to pass since the allocation failure most
			// likely caused it to fail
			iTestStepResult = EPass;

			// Try failing a little further into the process
			failCount++;
			}
		}

	// We must have reached our maximum number of allocation failures
	// There must be some other problem within the NormalConnect() function
	iTestStepResult=EFail;

	return iTestStepResult;
	}

// Test case 15.2
enum TVerdict CPsdAgtTest15_2::doPsdAgtTestStepL( void )
	{
	TInt failCount=1;
	TInt ret=-1;
	
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1502);
	SetGPRSModeL(agtNotify);
	
	while(failCount < MaxAllocationFailures)
		{
		// Mark heap and set allocation to fail
		__UHEAP_MARK;		
		__UHEAP_FAILNEXT(failCount);

		Log(_L("Incrementing __HEAP_FAILNEXT() to %d"),failCount);

		// Connect, most likely fail at some point
		TRAP(ret,NormalConnectOOML());
	
		User::After(200000); // Allow async libraries to unload

		// Check all heap is free'd
		__UHEAP_MARKEND;
		
			
		if(ret==0)
			{
			// Allocation failure has been moved all the way through the test
			// Succeeded after failCount allocations
			return iTestStepResult;
			}
		else 
			{
			// Reset test result to pass since the allocation failure most
			// likely caused it to fail
			iTestStepResult = EPass;
	
			// Try failing a little further into the process
			failCount++;
			}
		}

	// We must have reached our maximum number of allocation failures
	// There must be some other problem within the NormalConnect() function
	iTestStepResult=EFail;

	return iTestStepResult;
	}

// Test case 15.3
enum TVerdict CPsdAgtTest15_3::doPsdAgtTestStepL( void )
	{
	TInt failCount=1;
	TInt ret=-1;
	
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1503);

	while(failCount < MaxAllocationFailures)
		{
		// Mark heap and set allocation to fail
		__UHEAP_MARK;		
		__UHEAP_FAILNEXT(failCount);

		Log(_L("Incrementing __HEAP_FAILNEXT() to %d"),failCount);

		// Connect, most likely fail at some point
		TRAP(ret,ReConnectOOML());
	
		User::After(200000); // Allow async libraries to unload

		// Check all heap is free'd
		__UHEAP_MARKEND;
		
		
		if(ret==0)
			{
			// Allocation failure has been moved all the way through the test
			// Succeeded after failCount allocations
			return iTestStepResult;
			}
		else 
			{
			// Reset test result to pass since the allocation failure most
			// likely caused it to fail
			iTestStepResult = EPass;
	
			// Try failing a little further into the process
			failCount++;
			}
		}

	// We must have reached our maximum number of allocation failures
	// There must be some other problem within the ReConnectL() function
	iTestStepResult=EFail;

	return iTestStepResult;
	}

// Test case 15.4
enum TVerdict CPsdAgtTest15_4::doPsdAgtTestStepL( void )
	{
	TInt failCount=1;
	TInt ret=-1;
	
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1504);
	SetGPRSModeL(agtNotify);
	
	while(failCount < MaxAllocationFailures)
		{
		// Mark heap and set allocation to fail
		__UHEAP_MARK;		
		__UHEAP_FAILNEXT(failCount);

		Log(_L("Incrementing __HEAP_FAILNEXT()to %d"),failCount);

		// Connect, most likely fail at some point
		TRAP(ret,ReConnectOOML());
	
		User::After(200000); // Allow async libraries to unload

		// Check all heap is free'd
		__UHEAP_MARKEND;
		
			
		if(ret==0)
			{
			// Allocation failure has been moved all the way through the test
			// Succeeded after failCount allocations
			return iTestStepResult;
			}
		else 
			{
			// Reset test result to pass since the allocation failure most
			// likely caused it to fail
			iTestStepResult = EPass;
	
			// Try failing a little further into the process
			failCount++;
			}
		}

	// We must have reached our maximum number of allocation failures
	// There must be some other problem within the NormalConnect() function
	iTestStepResult=EFail;

	return iTestStepResult;
	}
