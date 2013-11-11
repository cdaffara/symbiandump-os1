// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cmdlistexecutor.h"



/**
 Initiate command processing.
 */	
void CSsmCmdListExecutor::StartL( CSsmCommandList* aCmdList )
	{
	__ASSERT_DEBUG( (aCmdList->Count() > 0), User::Panic(KCleCmdListBadIdx, KErrArgument) );
	
	if( iRunning )
		{
		User::Leave( KErrInUse );
		}
	
	iInterCmdDelay = aCmdList->DelayBetweenCommands();
	if( iInterCmdDelay > 0 ) 
		{
		User::LeaveIfError( iTimer.CreateLocal() );
		// If delays are required. Do a delay before the first execution.
		iDoDelayToggle = ETrue;
		}

	iRunning = ETrue;
	iCommandList = aCmdList; // Ownership transferred after any possible leaves.
	iCommandCount = iCommandList->Count();
	
	iCommandIndex = 0;
	
	SetActive();
	TRequestStatus* trs = &iStatus;
	User::RequestComplete( trs, KErrNone );
	}



/**
 Successive calls to Execute, but only one per RunL in order for
 the scheduler/ server to remain responsive.
 */
void CSsmCmdListExecutor::RunL()
	{
	if( iStatus != KErrNone )
		{
		// If a command (or the timer) has completed with an error code, terminate list 
		// processing and supply the error code to the server.
		iRunning = EFalse;
		//cancel all remaining commands
		for (; iCommandIndex < iCommandCount; iCommandIndex++)
			{
			iCommandList->ExecuteCancel(iCommandIndex);
			}
		iObserver.ListExecutionComplete( iStatus.Int(), iSessionIndex, iCommandList->Severity(iCommandIndex-1) );
		}
	else if( iCommandIndex < iCommandCount )
		{
		DoExecute();
		}
	else
		{
		// The last command must have completed.
		iRunning = EFalse;
		iObserver.ListExecutionComplete( iStatus.Int(), iSessionIndex, iCommandList->Severity(iCommandIndex-1) );
		}
	}
	
	

/**
 Perform command execution and inter-command delays where required.
*/
void CSsmCmdListExecutor::DoExecute()
	{
	const TInt KMilliToMicro = 1000;
	
	// Are inter-command delays required?
	if( iInterCmdDelay > 0 )
		{
		// Do a delay or execute a command?
		if( iDoDelayToggle )
			{
			DEBUGPRINT2( _L("***** Performing inter-cmd delay of  %d ms"), iInterCmdDelay );
			iDoDelayToggle = EFalse;
			iTimer.After( iStatus, iInterCmdDelay * KMilliToMicro );
			
			SetActive();			
			}
		else
			{
			DEBUGPRINT2( _L("***** Exececute cmd no. %d"), iCommandIndex );
			iDoDelayToggle = ETrue;
			iCommandList->Execute( iCommandIndex++, iStatus );
			
			SetActive();
			}
		}
	else
		{
		// Execute the commands without delays.
		DEBUGPRINT2( _L("***** Execute cmd no. %d without a delay"), iCommandIndex );
		iCommandList->Execute( iCommandIndex++, iStatus );
		
		SetActive();		
		}
	
	}


/**
 Please note: At the time of writing, this is unreachable and therefore untestable code
 as there are no leaving calls in RunL(). However, if at some future point a leaving call 
 is added to RunL(), this function may require attention.
 
 The implementation provided notifies the server via ListExecutionComplete()
*/ 
TInt CSsmCmdListExecutor::RunError( TInt aError )
	{	
	iRunning = EFalse;
	iObserver.ListExecutionComplete( aError, iSessionIndex, iCommandList->Severity(iCommandIndex) );
	return KErrNone;
	}



/**
 Cancel the current command.
 Dispose of iCommandList.
*/
void CSsmCmdListExecutor::DoCancel()
	{
	if( iCommandList && iRunning )
		{
		iTimer.Close();
		
		TInt currentCommand = ( 0 == iCommandIndex ) ? 0 : ( iCommandIndex - 1 );
		iCommandList->ExecuteCancel( currentCommand );
		iCommandList->DeferredDelete();
		iCommandList = NULL;
		
		iRunning = EFalse;
		}	
	}
	
	
	
CSsmCmdListExecutor* CSsmCmdListExecutor::NewL( MCommandListExecutionObserver& aCmdListObserver, TInt aSessionIndex )
	{
	CSsmCmdListExecutor* self = NewLC( aCmdListObserver, aSessionIndex );
	CleanupStack::Pop();	
	return self;
	}
	
	
	
CSsmCmdListExecutor* CSsmCmdListExecutor::NewLC( MCommandListExecutionObserver& aCmdListObserver, TInt aSessionIndex )
	{
	CSsmCmdListExecutor* self = new(ELeave) CSsmCmdListExecutor( aCmdListObserver, aSessionIndex );
	CleanupStack::PushL( self );
	self->ConstructL();	
	return self;
	}



void CSsmCmdListExecutor::ConstructL()
	{
	CActiveScheduler::Add( this );
	}
	
	
	
CSsmCmdListExecutor::CSsmCmdListExecutor( MCommandListExecutionObserver& aCmdListObserver, TInt aSessionIndex  )
: CActive( EPriorityHigh ), iObserver( aCmdListObserver ), iSessionIndex( aSessionIndex )
	{
	}
	
	
	
CSsmCmdListExecutor::~CSsmCmdListExecutor()
	{
	Cancel();
	if (iCommandList)
		{
		iCommandList->DeferredDelete();
		iCommandList = NULL;
		}
	}


	
