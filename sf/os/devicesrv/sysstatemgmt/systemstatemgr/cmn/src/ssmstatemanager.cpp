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
//

#include <ssm/ssmstatemanager.h>
#include "ssmclisrv.h"

/**
 Connect this session without pre-allocating any async message slots.
 This version of @c Connect signifies that the session has no dedicated pool of asynchronous message 
 objects but uses the global pool, reducing the kernel memory required to create the session. 
 This is the best version to use unless either:
	1. The session never sends asynchronous messages, in which case you should specify '0', OR
	2. The session makes guarantees on at least one asynchronous API that it cannot fail with KErrNoMemory - 
	   in which case the maximum number of concurrent asynchronous messages that may 
	   be outstanding on a single session must be used.
	   
@return		KErrNone or a system wide errorcode
 */
 EXPORT_C TInt RSsmStateManager::Connect()
 	{
 	const TInt KTryAllocSlotsFromSystemPoolInRuntime = -1;
 	return Connect(KTryAllocSlotsFromSystemPoolInRuntime);
 	}

/**
 Connect this session with pre-allocated async message slots.
 
 In theory a maximum of 3 slots is required for state transition requests because 
 the server can only have two requests queued but must still be able to handle an incoming 
 request to ESsmFail. There is no upper limit for the number of slots that might be 
 required for Swp change requests. 
 
 @return	 KErrNone or a system wide errorcode
*/
EXPORT_C TInt RSsmStateManager::Connect(TInt aAsyncMessageSlots)
	{
	return DoConnect(KSsmServerName, Version(), aAsyncMessageSlots);	
	}

/**
 This exists for testing purposes only.
 @internalComponent
 */
TInt RSsmStateManager::DoConnect(const TDesC& aServerName, TVersion aVersion, TInt aAsyncMessageSlots)
	{
	if(!Handle())
		{
		return CreateSession(aServerName, aVersion, aAsyncMessageSlots);	
		}

	return KErrAlreadyExists;
	} //lint !e1746 Suppress parameter 'aVersion' could be made const reference

/**
 Close this session.
 */
EXPORT_C void RSsmStateManager::Close()
	{
	RSessionBase::Close();
	}
	
/**
 Used as part of connect requests to ensure compatibility between client and server binaries.
 @internalComponent
 */		
TVersion RSsmStateManager::Version() const
	{
	return(TVersion(KSsmServMajorVersionNumber, KSsmServMinorVersionNumber, KSsmServBuildVersionNumber));
	}

/**
Issues a synchronous request for a transition from current system state to @c aRequest.
The request does not return until the transition has been started (or been rejected).

@param aRequest	The requested target system state together with a reason.
@return			Returns either @c KErrNone if the request was accepted or @c KErrNotSupported 
				if the request could not be allowed. If the SendReceive operation 
				failed, a system wide errorcode is returned.
*/	
EXPORT_C TInt RSsmStateManager::RequestStateTransition(TSsmStateTransition aRequest)
	{
	if(Handle())
		{
		const TUint32 stateAsInt = aRequest.State().Int();
		const TInt reason = aRequest.Reason();
		return SendReceive(ERequestStateTransition, TIpcArgs(stateAsInt, reason));
		}

	return KErrDisconnected;
	} //lint !e1746 Suppress parameter 'aRequest' could be made const reference

/**
Issues an asynchronous request for a transition from current system state to @c aRequest.
The @c TRequestStatus will be completed when the transition has been started (or 
earlier if it is rejected by the policy).

@param aRequest	The requested target system state together with a reason.
@param aStatus	The request status object used to contain the completion status of the request.
				The completion status will be either @c KErrNone when the tansition is started or 
				@c KErrNotSupported if the request could not be allowed. 
				One of the system wide error-codes if the @c SendReceive operation failed.
*/	
EXPORT_C void RSsmStateManager::RequestStateTransition(TSsmStateTransition aRequest, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		const TUint32 stateAsInt = aRequest.State().Int();
		const TInt reason = aRequest.Reason();
		SendReceive(ERequestStateTransition, TIpcArgs(stateAsInt, reason), aStatus);
		return;
		}

	TRequestStatus* clientStatus = &aStatus;
	User::RequestComplete(clientStatus, KErrDisconnected);
	} //lint !e1746 Suppress parameter 'aRequest' could be made const reference

/**
Issues a synchronous request to cancel all pending transitions which have been requested 
from this session. Refer to the @c TRequestStatus that was passed into the async  
@c RequestStateTransition to see if a request was Cancelled.

It is not possible to Cancel an ongoing system state transition with this function
If you want to abort a transition that is already in progress you need to request 
another transition, it is then up to the system state policy DLL implementation to finish 
the ongoing transition as early as possible and start on the new transition.
*/	
EXPORT_C void RSsmStateManager::RequestStateTransitionCancel()
	{
	if(Handle())
		{
		const TInt kErrNone = SendReceive(ERequestStateTransitionCancel);
		} //lint !e529 Suppress symbol not subsequently referenced. Because synchronous IPC never fails
	}

/**
Issues a synchronous request to change the value of a System Wide Property.
The request does not return until the request has been started (or been rejected).

@param aSwp	The identifier for the Swp and its new value.
@return		Returns either @c KErrNone if the request was accepted or @c KErrNotSupported 
			if the request could not be allowed. If the SendReceive operation 
			failed, a system wide errorcode is returned.
*/	
EXPORT_C TInt RSsmStateManager::RequestSwpChange(TSsmSwp aSwp)
	{
	if(Handle())
		{
		TUint32 key   = aSwp.Key();
		TInt value = aSwp.Value();
		return SendReceive(ERequestSwpChange, TIpcArgs(key, value));
		}

	return KErrDisconnected;
	} //lint !e1746 Suppress parameter 'aSwp' could be made const reference

/**
Issues an asynchronous request to change the value of a System Wide Property.
The @c TRequestStatus will be completed when the request has been started (or been rejected).

@param aSwp		The identifier for the Swp and its new value.
@param aStatus	The request status object used to contain the completion status of the request.
				The completion status will be either @c KErrNone if the request was accepted or 
				@c KErrNotSupported if the request could not be allowed. 
				One of the system wide error-codes if the @c SendReceive operation failed.
*/	
EXPORT_C void RSsmStateManager::RequestSwpChange(TSsmSwp aSwp, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		TUint32 key   = aSwp.Key();
		TInt value = aSwp.Value();
		SendReceive(ERequestSwpChange, TIpcArgs(key, value), aStatus);
		return;
		}
	
	TRequestStatus* clientStatus = &aStatus;
	User::RequestComplete(clientStatus, KErrDisconnected);
	} //lint !e1746 Suppress parameter 'aSwp' could be made const reference

/**
Request to Cancel any request originating from this session for which the @c TRequestStatus
has not yet been completed.
*/	
EXPORT_C void RSsmStateManager::RequestSwpChangeCancel()
	{
	if(Handle())
		{
		const TInt kErrNone = SendReceive(ERequestSwpChangeCancel);
		} //lint !e529 Suppress symbol not subsequently referenced. Because synchronous IPC never fails
	}

/**
Establish a mapping between the identifier key of a System Wide Property and the DLL that implements 
the MSsmSwpPolicy interface for the System Wide Property in question.

These DLLs can only be loaded from ROM (Z:). There will be no attempt to load or verify the existence of
the given file until the first time it is used (i.e. the first time there is a call to @c RequestSwpChange())

@param aSwp		 The identifier key of a System Wide Property
@param aFilename The filename of the MSsmSwpPolicy DLL for this property. Should be given with no path, 
				 with or without extension.
@return			 KErrNone or a system wide errorcode 
@internalTechnology
*/
EXPORT_C TInt RSsmStateManager::RegisterSwpMapping(TUint aSwpKey, const TDesC& aFilename)
	{
	if(Handle())
		{
	    TIpcArgs args(aSwpKey, &aFilename);
	    return SendReceive(ERequestRegisterSwpMapping, args);
		}
	
	return KErrDisconnected;
	} //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference
