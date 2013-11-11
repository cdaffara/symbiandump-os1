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
//

#include "ssmswppolicyproxy.h"

/**
Constructs and returns a pointer to CSsmSwpPolicyCliSession
*/
CSsmSwpPolicyCliSession* CSsmSwpPolicyCliSession::NewL()
	{
	CSsmSwpPolicyCliSession* self = new (ELeave) CSsmSwpPolicyCliSession;
	return self;
	}

/**
Sets the swppolicy dll handle and initializes the swppolicy. The policyhandle which is loaded in 
swppolicyresovler is passed to the swppolicyserver, where it is used to open the handle to swppolicy
and forward the calls from policy frame to the swppolicy.

@param aPolicyHandle The swppolicy policyhandle
@param aStatus		 Request status
*/
void CSsmSwpPolicyCliSession::CallSetDllHandleAndInitialize(TInt aPolicyHandle, TRequestStatus& aStatus)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyCliSession: Sets the swppolicy Dll Hanlde and Initializes it"));
	iSsmSwpPolicySession.CallSetDllHandleAndInitialize(aPolicyHandle, aStatus);
	}

/**
Cancels the Initialization of the swppolicy, if any Initialization request is pending.
@return KErrNone, if successful; otherwise one of the other system-wide error codes. 
*/
TInt CSsmSwpPolicyCliSession::CallInitializeCancel()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyCliSession: Cancel Initialization of the swp policy"));
	return (iSsmSwpPolicySession.CallInitializeCancel());
	}

/**
Prepares the commandlist in swppolicy
@param aSwp			Swp value
@param aStatus		Request status
*/
void CSsmSwpPolicyCliSession::CallPrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyCliSession: Call Prepare command list in swppolicy"));
	iSsmSwpPolicySession.CallPrepareCommandList(aSwp, aStatus); 
	}

/**
Cancels the preparation of the commandlist, if any PrepareCommandList request is pending.
@return KErrNone, if successful; otherwise one of the other system-wide error codes. 
*/
TInt CSsmSwpPolicyCliSession::CallPrepareCommandListCancel()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyCliSession: Cancel prepare command list in swppolicy"));
	return (iSsmSwpPolicySession.CallPrepareCommandListCancel());
	}

/**
Returns the commandlist to the client from swppolicy.
@return A pointer to commandlist if successfull, else returns NULL pointer.
*/
CSsmCommandList* CSsmSwpPolicyCliSession::CallCommandList()
	{
	CSsmCommandList* ssmCommandList = NULL;

	// To avoid a warning of variable not used.
	TInt err = KErrNone;

	// We are ignoring the error here, as we return a NULL pointer in case of error
	TRAP(err, ssmCommandList = iSsmSwpPolicySession.CallCommandListL());

	DEBUGPRINT2(_L("CSsmSwpPolicyCliSession: CallCommandList completed with %d"), err);
	return ssmCommandList;
	}

/**
Handles the value returned by Cle.
@param aSwp			swp value
@param aError		Error returned by cle as result of the command execution
@param aSeverity	Severity of the command
@param aStatus		Request status
*/
void CSsmSwpPolicyCliSession::CallHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyCliSession: Call HandleCleReturn Value of the swppolicy"));
	iSsmSwpPolicySession.CallHandleCleReturnValue(aSwp, aError, aSeverity, aStatus);
	}

/**
Cancels the HandleCleReturnValue, if any HandleCleReturnValue request is pending.
@return KErrNone, if successful; otherwise one of the other system-wide error codes. 
*/
TInt CSsmSwpPolicyCliSession::CallHandleCleReturnValueCancel()
	{
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Cancel Call HandleCleReturn Value of the swppolicy"));
	return (iSsmSwpPolicySession.CallHandleCleReturnValueCancel());
	}

/**
Connects to the ssmswppolicy server.

@leave KErrNoMemory if no memory.
*/
void CSsmSwpPolicyCliSession::ConnectL()
	{
	iSsmSwpPolicySession.ConnectL();
	DEBUGPRINT1(_L("CSsmSwpPolicyCliSession Connect Completed with KErrNone"));
	}

/**
Closes the RSsmSwpPolicySession handle.
*/
void CSsmSwpPolicyCliSession::Close()
	{
	iSsmSwpPolicySession.Close();
	}

/**
Returns ETrue if there is a connected session else returns EFalse.
*/
TBool CSsmSwpPolicyCliSession::IsConnected()
	{
	return iSsmSwpPolicySession.Handle() ? ETrue : EFalse;
	}

/**
Constructor
*/
CSsmSwpPolicyCliSession::CSsmSwpPolicyCliSession()
	{
	}

/**
Destructor
*/
CSsmSwpPolicyCliSession::~CSsmSwpPolicyCliSession()
	{
	Close();
	}



