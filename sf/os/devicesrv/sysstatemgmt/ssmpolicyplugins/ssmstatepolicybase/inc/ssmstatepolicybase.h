/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration of CSsmStatePolicyBase class.
*
*/

#ifndef C_SSMSTATEPOLICYBASE_H
#define C_SSMSTATEPOLICYBASE_H

#include <ssm/ssmstatepolicy.h>

#include "ssmpolicybase.h"

/**
*  Base class for SSM state policy plugins
*
*/
class CSsmStatePolicyBase : public CSsmPolicyBase, public MSsmStatePolicy
    {

public:

    /**
    * First phase constructor.
    */
    IMPORT_C CSsmStatePolicyBase();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CSsmStatePolicyBase();

protected: // From MSsmStatePolicy

	/**
    * This function is guaranteed to be called before this System State Policy
    * is used. It is intended for e.g. opening resource files or if you need to
    * initialize hardware or talk to a domestic OS.
	*/
    IMPORT_C virtual void Initialize( TRequestStatus& aStatus );

	/**
    * Used to inform the policy DLL to Cancel an asynchronous Initialize
    * operation. Typically used to inform a MCommandListProvider that any open
    * resource files should be closed.
	*/
    IMPORT_C virtual void InitializeCancel();

	/**
    * Used to determine if an incoming request should be accepted or rejected.
	*
	* @param aRequest Contains information about the new request
	* @param aCurrent Contains NULL or the first accepted but not yet completed transition request
	* @param aQueued Contains NULL or a second accepted but not yet started transition request
    * @param aMessage Message sent by SSM server, used to check if the client has 'ECapabilityPowerMgmt'
	* @return The decision
	*/
    IMPORT_C virtual TResponse TransitionAllowed(
        const TSsmStateTransition& aRequest,
        TSsmStateTransition const* aCurrent,
        TSsmStateTransition const* aQueued,
        const RMessagePtr2& aMessage );

	/**
    * Used to create the command list associated with a minor state transition.
	*
    * @param aState	Contains the state that identifies the command list to create
	* @param aReason Contains the reason as given by the request
	* @param aStatus to complete when the operation has finished
	*/
    IMPORT_C virtual void PrepareCommandList(
        TSsmState aState,
        TInt aReason,
        TRequestStatus& aStatus );

	/**
    * Used to inform the policy DLL that the pending asynchronous
    * PrepareCommandList operation needs to be cancelled.
	*/
    IMPORT_C virtual void PrepareCommandListCancel();

	/**
    * Used to retrieve the command list once the  PrepareCommandList has
    * completed. Ownership of the returned command list is transferred to the
    * caller.
    *
    * @return The command list created during the preceding PrepareCommandList step
	*/
    IMPORT_C virtual CSsmCommandList* CommandList();

	/**
    * It is expected that Release will usually just call "delete this" on the
    * object, but this will depend on the implementation of each policy.
	*/
    IMPORT_C virtual void Release();

protected:

	/**
    * Used to determine if an incoming request to change state should be accepted or rejected.
	*/
    virtual TBool IsAllowedTargetState( const TSsmStateTransition& aRequest ) const = 0;

	/**
    * On return contains target substate - modified or requested.
	*/
    virtual TUint16 TargetSubState( const TUint16 aRequestedSubState ) = 0;

    };

#endif // C_SSMSTATEPOLICYBASE_H
