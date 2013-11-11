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

#ifndef __SSMSWPPOLICY_H__
#define __SSMSWPPOLICY_H__

#include <e32cmn.h>

class CSsmCommandList;
class TSsmSwp;

/**
 The 2'nd UID value for System Wide Property policy DLLs.
 @publishedPartner
 @released
 */
const TInt KSsmSwpPolicyDllTypeUidValue=0x2000D765;

/**
 The 2'nd UID for System Wide Property policy DLLs.
 @publishedPartner
 @released
 */
const TUid KSsmSwpPolicyDllTypeUid={KSsmSwpPolicyDllTypeUidValue};

/**
 The protocol for System Wide Property policy DLLs.
 These are not ECOM DLLs because they need to work before the ECOM server is started.
 These DLLs can only be loaded from the ROM file system (from Z:).
 @publishedPartner
 @released
 */
class MSsmSwpPolicy
  	{
public:

	enum TResponse
		{
		/* The requested system value is not permissible */
		ENotAllowed,
		
		/* Value ok */
		EAllowed
		};
	
	/** 
	Used to determine if an incoming request should be accepted or rejected.
	@param aSwp Contains information about the new request.
	@param aMessage Contains information about the requesting client process. DLLs should not call RMessagePtr2::Complete.
	@return The decision from the the virtual implementation
	*/
	virtual TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage) = 0;
	
	/**	
	This function is guaranteed to be called before this Swp Policy is used.
	It is intended for e.g. opening resource files, initialize	hardware or 
	talk to a domestic OS.
	@code
		//minimal implemementation of this function would be
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
	@endcode
	@param aStatus to complete when the initialization operation has finished
	*/
	virtual void Initialize(TRequestStatus& aStatus) = 0;

	/**	
	Used to inform the DLL to Cancel the pending asynchronous Initialize operation.
	*/
	virtual void InitializeCancel() = 0;
	
	/** 
	Used to create the command list associated with a Swp change.
	@param aSwp		Contains the key that identifies the command list to create. Also
					contains the new value to be set to the Swp in question.
	@param aStatus  Used to signify command list is prepared
	*/ 
	virtual void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus) = 0;
	
	/**	
	Used to inform the DLL to Cancel the pending asynchronous PrepareCommandList operation.
	*/
	virtual void PrepareCommandListCancel() = 0;
	
	/**
	Used to retrieve the command list once the  PrepareCommandList has completed. 
	Ownership of the returned command list is transferred to the caller.
	@return The command list created during the preceding PrepareCommandList step
	*/
	virtual CSsmCommandList* CommandList() = 0;
	
	/**
	Handle the return value from the CLE. This gives the policy a chance to do some
	Error handling, in case execution of the command list should fail in any way.
	@param aSwp Contains information about the request that has been executed
	@param aError Contains the error-code that resulted from the Command-list execution
	@param aSeverity Contains the severity of the failed command in case the Command-list execution ended with an error
	@param aStatus to complete when aError has been dealt with 
	*/
	virtual void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus) = 0;

	/**	
	Used to inform the DLL to Cancel the pending asynchronous HandleCleReturnValue operation.
	*/
	virtual void HandleCleReturnValueCancel() = 0;
	
	/** 
	It is expected that Release will usually just call "delete this" on the object, 
	but this will depend on the implementation of each policy. 
	*/
	virtual void Release() = 0;
	
private:
	//for future use
	virtual void MSsmSwpPolicy_Reserved_1() {}
	virtual void MSsmSwpPolicy_Reserved_2() {}
	virtual void MSsmSwpPolicy_Reserved_3() {}
	};
	
#endif
