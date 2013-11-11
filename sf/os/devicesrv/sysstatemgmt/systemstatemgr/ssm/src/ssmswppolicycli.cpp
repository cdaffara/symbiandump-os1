// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmswppolicycli.h"
#include "ssmswppolicysrv.h"
#include "ssmswppolicyconst.h"
#include "ssmdebug.h"

/**
 Constructs the client-side ssmswppolicy server session.
*/
RSsmSwpPolicySession::RSsmSwpPolicySession()
	: RSessionBase(), iVersion(KSsmSwpPolicyCliVersionMajor, KSsmSwpPolicyCliVersionMinor, KSsmSwpPolicyCliVersionBuild)
	{
	} //lint !e1401 not initialized by constructor (iSpare)
	
/**
 Connects to the ssmswppolicy server. If the server is not found, this function will attempt to start it
 and retry connecting to it.

@leave KErrNoMemory if no memory.
*/
void RSsmSwpPolicySession::ConnectL()
	{
	// Guard against multiple calls.
	if(KNullHandle != iHandle)
		{
		DEBUGPRINT1(_L("RSsmSwpPolicySession: Client already has a Connected session"));
		User::Leave(KErrAlreadyExists);
		}

	TInt err = CreateSession(KSsmSwpPolicySrvName, iVersion, KSsmSwpPolicySrvMsgSlots);
	if((KErrNotFound == err) || (KErrServerTerminated == err))
		{
		err = CSsmSwpPolicyServer::StartSsmSwpPolicySrv(KSsmSwpPolicySrvName);

		DEBUGPRINT2(_L("Starting SsmSwpPolicySrv completed with %d"),err);
		if (KErrNone !=  err && KErrAlreadyExists != err)
			{
			User::Leave(err);
			}

		User::LeaveIfError(CreateSession(KSsmSwpPolicySrvName, iVersion, KSsmSwpPolicySrvMsgSlots));
		DEBUGPRINT1(_L("RSsmSwpPolicySession Connect Completed with KErrNone"));
		}
	else
		{
		DEBUGPRINT2(_L("RSsmSwpPolicySession Connect Completed with %d"), err);
		User::LeaveIfError(err);
		}
	}

#ifdef TEST_SSM_SERVER
/**
 For testing purposes
 Connects to ssmswppolicy server with the specified name. If the server is not found, this function will attempt to start it
 and retry connecting to it.
 
@param aServerName The name of the ssmswppolicy server to connect to
@leave KErrNoMemory if no memory.
*/
void RSsmSwpPolicySession::ConnectL(const TDesC& aServerName)
	{
	// Guard against multiple calls.
	if(KNullHandle != iHandle)
		{
		User::Leave(KErrAlreadyExists);
		}

	TInt err = CreateSession(aServerName, iVersion, KSsmSwpPolicySrvMsgSlots);
	if((KErrNotFound == err) || (KErrServerTerminated == err))
		{
		err = CSsmSwpPolicyServer::StartSsmSwpPolicySrv(aServerName);

		DEBUGPRINT2(_L("Starting SsmSwpPolicySrv completed with %d"),err);
		if (KErrNone !=  err && KErrAlreadyExists != err)
			{
			User::Leave(err);
			}
		
		User::LeaveIfError(CreateSession(aServerName, iVersion, KSsmSwpPolicySrvMsgSlots));
		DEBUGPRINT1(_L("RSsmSwpPolicySession Connect Completed with KErrNone"));
		}
	else
		{
		DEBUGPRINT2(_L("RSsmSwpPolicySession Connect Completed with %d"), err);
		User::LeaveIfError(err);
		}
	}
#endif	// TEST_SSM_SERVER

/**
Closes the RSession handle.
*/
void RSsmSwpPolicySession::Close()
	{
	RSessionBase::Close();
	}

/**
Sets the swppolicy dll handle and initializes the swppolicy. The policyhandle which is loaded in 
swppolicyresovler is passed to the swppolicyserver, where it is used to open the handle to swppolicy
and forward the calls from policy frame to the swppolicy.

@param aPolicyHandle The swppolicy policyhandle
@param aStatus		 Request Status
*/
void RSsmSwpPolicySession::CallSetDllHandleAndInitialize(TInt aPolicyHandle, TRequestStatus& aStatus)
	{
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Sets the swppolicy Dll Hanlde and Initializes it"));
	SendReceive(ESsmPolicySrvSetSwpPolicyDllHandleAndInitialize, TIpcArgs(aPolicyHandle), aStatus);
	}

/**
Cancels the Initialization of the swppolicy, if any Initialization request is pending.
@return KErrNone, if successful; otherwise one of the other system-wide error codes. 
*/
TInt RSsmSwpPolicySession::CallInitializeCancel()
	{
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Cancel Initialization of the swp policy"));
	return (SendReceive(ESsmPolicySrvInitializeCancel));
	}

/**
Prepares the commandlist in swppolicy
@param aSwp		Swp value
@param aStatus	Request Status 
*/
void RSsmSwpPolicySession::CallPrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Call Prepare command list in swppolicy"));
	const TPckgC<TSsmSwp> swpBuf(aSwp);
	SendReceive(ESsmPolicySrvPrepareCommandList, TIpcArgs(&swpBuf), aStatus);
	}

/**
Cancels the preparation of the commandlist, if any preparecommandlist request is pending.
@return KErrNone, if successful; otherwise one of the other system-wide error codes. 
*/
TInt RSsmSwpPolicySession::CallPrepareCommandListCancel()
	{
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Cancel prepare command list in swppolicy"));
	return (SendReceive(ESsmPolicySrvPrepareCommandListCancel));
	}

/**
Returns the commandlist to the client from swppolicy.
@return A pointer to commandlist
*/
CSsmCommandList* RSsmSwpPolicySession::CallCommandListL()
	{
	// Ensure any memory that might already have been allocated is disposed of. 
	// Transfer the streamed cmd list from the CBuf.
	RBuf8 cmdListBuf;
	cmdListBuf.CreateL(KSsmSwpPolicyStreamBufMaxSize);
	CleanupClosePushL(cmdListBuf);

	TInt err = SendReceive(ESsmPolicySrvCommandList, TIpcArgs(&cmdListBuf));
	DEBUGPRINT2(_L("RSsmSwpPolicySession: CallCommandList completed with %d"), err);

	CSsmCommandList* cmdList = NULL;

	if(err == KErrNone)
		{
		// Supply RBuf to the readstream. Stream and reconstruct the command list.
		RDesReadStream readStream(cmdListBuf);
		CleanupClosePushL(readStream);
		cmdList = CSsmCommandList::NewLC();
		cmdList->InternalizeL( readStream);
		CleanupStack::Pop(cmdList);
		CleanupStack::PopAndDestroy(&readStream);
		}

	CleanupStack::PopAndDestroy(&cmdListBuf);
	return cmdList;
	}

/*
Handles the value returned by Cle.
@param aSwp			swp value
@param aError		Error returned by cle as result of the command execution
@param aSeverity	Severity of the command
@param aStatus		Request Status
*/
void RSsmSwpPolicySession::CallHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	const TPckgC<TSsmSwp> swpBuf(aSwp);
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Call HandleCleReturn Value of the swppolicy"));
	SendReceive(ESsmPolicySrvHandleCleReturnValue, TIpcArgs(&swpBuf, aError, aSeverity), aStatus);
	}

/**
Cancels the HandleCleReturnValue, if any HandleCleReturnValue request is pending.
@return KErrNone, if successful; otherwise one of the other system-wide error codes. 
*/
TInt RSsmSwpPolicySession::CallHandleCleReturnValueCancel()
	{
	DEBUGPRINT1(_L("RSsmSwpPolicySession: Cancel Call HandleCleReturn Value of the swppolicy"));
	return (SendReceive(ESsmPolicySrvHandleCleReturnValueCancel));
	}



	
	
