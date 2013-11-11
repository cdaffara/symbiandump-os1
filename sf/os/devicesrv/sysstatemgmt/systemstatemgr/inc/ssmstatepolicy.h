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

#ifndef __SSMSTATEPOLICY_H__
#define __SSMSTATEPOLICY_H__

#include <e32def.h>
#include <e32cmn.h>

class CSsmCommandList;
class TSsmState;
class TSsmStateTransition;

/**
 The UID value for System State Policy DLLs.
 @publishedPartner
 @released
 */
const TInt KSsmStatePolicyDllTypeUidValue=0x2000D75D;

/**
 The UID for System State Policy DLLs.
 @publishedPartner
 @released
 */
const TUid KSsmStatePolicyDllTypeUid={KSsmStatePolicyDllTypeUidValue};

/**
Filenames of System State Policy DLLs must be named on the pattern
KSsmStatePolicyFilenamePrefix + TSsmState::MainState() + KSsmStatePolicyFilenamePostfix.
The State must be formatted as a right-aligned hexadecimal number with the fixed width of 4 digits.
These DLLs can only be loaded from the ROM file system (from Z:).
@publishedPartner
@released
*/
_LIT(KSsmStatePolicyFilenamePrefix, "ssm.state.policy.");

/**
@see KSsmStatePolicyFilenamePrefix
@publishedPartner
@released
*/
_LIT(KSsmStatePolicyFilenamePostfix, ".dll");

/**
The protocol for System State Policy DLLs.
These are not ECOM DLLs because they need to work before the ECOM server is started.
 
The first function in System State Policy DLLs must have an exported  function with 
following signature that returns an instance of the @c MSsmStatePolicy implementation:

@code
IMPORT_C static MSsmStatePolicy* NewL();
@endcode

@publishedPartner
@released
*/
class MSsmStatePolicy
  	{
public:

	enum TResponse
		{
		/* The requested system state is not possible to reach from current system state */
		ENotAllowed,
		
		/* No state transition is currently ongoing */
		EDefinitelyAllowed,
		
		/* The requested transition has higher priority than the currently queued transition,
		discard the queued transition and put the new request into the queue. Its up to the
		policy implementation to decide if the new request will cause the current transition
		to finish earlier than previously planned. */
		ECurrentRemainReplaceQueued,
		
		/* The requested transition must action immediately, e.g. phone failure. Cancel the
		currently ongoing transition and immediately start executing this new transition request.*/
		EReplaceCurrentClearQueue
		};
	
	/**	
	This function is guaranteed to be called before this System State Policy is used.
	It is intended for e.g. opening resource files or if you need to initialize 
	hardware or talk to a domestic OS.
	@code
	//minimal implemementation of this function would be
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	@endcode
	@param aStatus to complete when the initialization operation has finished
	*/
	virtual void Initialize(TRequestStatus& aStatus) = 0;

	/**	
	Used to inform the policy DLL to Cancel an asynchronous Initialize operation.
	*/
	virtual void InitializeCancel() = 0;
	
	/** 
	Used to determine if an incoming request should be accepted or rejected.
	@param aRequest Contains information about the new request
	@param aCurrent Contains NULL or the first accepted but not yet completed transition request
	@param aQueued	Contains NULL or a second accepted but not yet started transition request
	@param aMessage Contains information about the requesting client process. DLLs should not call RMessagePtr2::Complete.
	@return The decision from the the virtual implementation
	*/
	virtual TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage) = 0;
	
	/** 
	Used to create the command list associated with a sub state transition.
	@param aState	Contains the state that identifies the command list to create
	@param aReason	Contains the reason as given by the request
	@param aStatus to complete when the operation has finished
	*/ 
	virtual void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus) = 0;

	/**	
	Used to inform the policy DLL that the pending asynchronous PrepareCommandList operation 
	needs to be cancelled.
	*/
	virtual void PrepareCommandListCancel() = 0;
	
	/**	
	Used to retrieve the command list once the  PrepareCommandList has completed. 
	Ownership of the returned command list is transferred to the caller.
	@return The command list created during the preceding PrepareCommandList step
	*/
	virtual CSsmCommandList* CommandList() = 0;

	/** 
	Used to determine the next sub state transition.
	@param aCurrentTransition	Contains the last executed state transition.
	@param aReason				Contains the reason as given by the request
	@param aError				Contains the completion code from the last executed sub-state transition
	@param aSeverity			Contains the severity of the failed command in case the sub-state transition ended with an error
	@param aNextState			The next System State to head for, if there is one
	@return 	ETrue if aNextState contains another System State to head for, or 
				EFalse if there is no further transitions to do.
	*/
	virtual TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState) = 0;

	/** 
	It is expected that Release will usually just call "delete this" on the object, 
	but this will depend on the implementation of each policy. 
	*/
	virtual void Release() = 0;
	
private:
	//for future use, do not override these
	virtual void MSsmStatePolicy_Reserved_1() {}
	virtual void MSsmStatePolicy_Reserved_2() {}
	virtual void MSsmStatePolicy_Reserved_3() {}
	};
	
#endif
