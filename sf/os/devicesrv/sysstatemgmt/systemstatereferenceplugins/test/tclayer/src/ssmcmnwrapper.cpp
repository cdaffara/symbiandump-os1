// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains wrapper helper functions for test code to control the test wrappers 
// TSsmStateTransition, TSsmState and RSsmStateManager wrapper methods
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmstatetransition.h>
#include <test/testexecutestepbase.h>

#include "ssmcmnwrapper.h"

/*
 * RSsmStateManager test wrapper functions
 */

TInt RSsmStateManager::Connect()
	{
	TInt ret = SsmCmnWrapper::SsmStateManagerNextConnectReturn();
	if (ret == KErrNone)
		{
		// Will return success so need to connect the session
		// Use an RMutex as the handle to ensure that we are wrapping any uses of the handle
		// i.e. attempting to SendAndReceive() on an RMutex will panic
		RMutex tempMutex;
		ret = tempMutex.CreateLocal();
		if(ret != KErrNone)
			{
			// return the error, the test will fail as it's an unexpected result
			}
		else
			{
			// Transfer ownership of tempMutex's handle to this RSsmStateManager
			SetHandle(tempMutex.Handle());
			// Set the test framework value
			SsmCmnWrapper::SetSsmStateManagerHandle(tempMutex.Handle());
			}
		// Don't close tempMutex before retuning as RSsmStateManager now owns the handle
		}
	return ret;
	}

TInt RSsmStateManager::RequestStateTransition(TSsmStateTransition aTransition)
	{
	TInt ret = SsmCmnWrapper::GetSsmStateManagerNextStateRequestReturn();
	if(ret == KErrNone)
		{
		SsmCmnWrapper::SetLastStateTransition(aTransition);
		}
	return ret;
	}

TInt RSsmStateManager::RequestSwpChange(TSsmSwp aSwp)
	{
	TInt ret = SsmCmnWrapper::GetSsmStateManagerNextSwpRequestReturn();
	if(ret == KErrNone)
		{
		SsmCmnWrapper::SetLastSwpTransition(aSwp);
		}
	return ret;
	}

/**
 * Sets the value that should be returned by RSsmStateManager::Connect()
 * 
 * @test
 * @internalComponent
 */
void SsmCmnWrapper::SetSsmStateManagerNextConnectReturn(TInt aRet)
	{
	iSsmStateManagerNextConnectReturn = aRet;
	}

/**
 * Gets the value that should be returned by RSsmStateManager::Connect()
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::SsmStateManagerNextConnectReturn()
	{
	return iSsmStateManagerNextConnectReturn;
	}

/**
 * Sets the value of the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
void SsmCmnWrapper::SetSsmStateManagerHandle(TInt aHandle)
	{
	iSsmStateManagerHandle = aHandle;
	}


/**
 * Gets the value of the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::SsmStateManagerHandle()
	{
	return iSsmStateManagerHandle;
	}

/**
 * Gets the value that should be returned by RSsmStateManager::RequestStateTransition()
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::GetSsmStateManagerNextStateRequestReturn()
	{
	return iSsmStateManagerNextRequestReturn;
	}

/**
 * Sets the value that should be returned by RSsmStateManager::RequestStateTransition()
 * 
 * @test
 * @internalComponent
 */
void SsmCmnWrapper::SetSsmStateManagerNextStateRequestReturn(TInt aRet)
	{
	iSsmStateManagerNextRequestReturn = aRet;
	}

/**
 * Sets the value of the last state transition for the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
void SsmCmnWrapper::SetLastStateTransition(TSsmStateTransition& aTransition)
	{
	iSsmStateManagerStateTransition = aTransition;
	}
	
/**
 * Gets the last main state value request of the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
TUint16 SsmCmnWrapper::GetLastMainStateRequest()
	{
	return iSsmStateManagerStateTransition.State().MainState();
	}

/**
 * Gets the last sub state value request of the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
TUint16 SsmCmnWrapper::GetLastSubStateRequest()
	{
	return iSsmStateManagerStateTransition.State().SubState();
	}

/**
 * Gets the last reason value request of the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::GetLastReasonRequest()
	{
	return iSsmStateManagerStateTransition.Reason();
	}
	
/**
 * Gets the return value for the next SWP transition requested
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::GetSsmStateManagerNextSwpRequestReturn()
	{
	return iSsmStateManagerNextSwpRequestReturn;
	}

/**
 * Sets the return value for the next SWP transition requested
 * 
 * @test
 * @internalComponent
 */
void SsmCmnWrapper::SetSsmStateManagerNextSwpRequestReturn(TInt aRet)
	{
	iSsmStateManagerNextSwpRequestReturn = aRet;
	}

/**
 * Sets the last SWP transition requested
 * 
 * @test
 * @internalComponent
 */
void SsmCmnWrapper::SetLastSwpTransition(TSsmSwp& aSwp)
	{
	iSsmStateManagerSwpTransition = aSwp;
	}

/**
 * Gets the last SWP transition requested
 * 
 * @test
 * @internalComponent
 */
TSsmSwp SsmCmnWrapper::GetLastSwpTransition()
	{
	return iSsmStateManagerSwpTransition;
	}

/**
 * Holds the value to be returned by calls to RSsmStateManager::RequestSwpTransition()
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::iSsmStateManagerNextSwpRequestReturn = KErrNone;

/**
 * Holds the value used in the last SWP transition request
 * 
 * @test
 * @internalComponent
 */
TSsmSwp SsmCmnWrapper::iSsmStateManagerSwpTransition(0,0);

/**
 * Holds the value to be returned by calls to RSsmStateManager::Connect()
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::iSsmStateManagerNextConnectReturn = KErrNone;

/**
 * Holds the value of the test handle for RSsmStateManager
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::iSsmStateManagerHandle = KNullHandle;

/**
 * Holds the value of the last transition requested
 * 
 * @test
 * @internalComponent
 */ 
TSsmStateTransition SsmCmnWrapper::iSsmStateManagerStateTransition;

/**
 * Holds the value to be returned by calls to RSsmStateManager::RequestStateTransition()
 * 
 * @test
 * @internalComponent
 */
TInt SsmCmnWrapper::iSsmStateManagerNextRequestReturn = KErrNone;
