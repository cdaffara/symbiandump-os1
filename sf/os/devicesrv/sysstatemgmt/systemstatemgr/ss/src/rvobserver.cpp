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

#include "rvobserver.h"
#include <ssm/ssmstartupproperties.h>
#include <ssm/ssmcmd.hrh>
#include "timeoutwaiter.h"
#include "rvobservernotification.h"
#include "ssconst.h"
#include "ssmdebug.h"
	
/**
 We are either starting for the first time, or an app or process has rendezvouzed.
 If the latter, check if it has done so without error and behave as appropriate.
*/
void CRvObserver::RunL()
	{
	switch( iObserverState )
		{
		case ERvObserverStateNotInvoked:
			{
			iObserverState = ERvObserverStateAwaitingRv;
			InvokeL();
			}
			break;
			
		case ERvObserverStateAwaitingRv:
			{
			NotifyOrRetryL();
			}
			break;
			
		default:
			User::Leave( KErrUnknown );
			break;
		}
	}



/**
 Leaves here occur in the context of RunL, therefore error notification is performed via RunError()
 */
void CRvObserver::InvokeL()
	{
	switch( iStartupProperties->CommandType() )
		{
		case ESsmCmdStartApp:
			{
			if( !iApaStarter )
				{
				User::Leave( KErrNotSupported );
				}
			SetActive();
			
			TRAPD( err, iApaStarter->StartAppL( iStartupProperties->FileName(), 
									iStartupProperties->Args(), 
									iStartupProperties->Viewless(), 
									iStartupProperties->StartInBackground(), iThreadId, iStatus );
				);
			
			if( KErrNone != err )
				{
				// In the case of error here, we know that Apparc has not called Rendezvous()
				TRequestStatus* trs = &iStatus;
				User::RequestComplete( trs, err );
				}
			
			}
			break;
		
		case ESsmCmdStartProcess:
			{
			SetActive(); 
			
			TInt err = iProcess.Create( iStartupProperties->FileName(), iStartupProperties->Args() );
			
			if( KErrNone != err )
				{
				// In case of error here, copmplete self with err.
				TRequestStatus* trs = &iStatus;
				User::RequestComplete( trs, err );
				
				return;
				}	
			
			iProcess.Rendezvous( iStatus );
			// Rendezvous() can complete with KErrNoMemory, hence:-
			if( iStatus == KRequestPending )
				{
				iProcess.Resume();
				}
		
			}
			break;
	
		default:
			User::Leave( KErrArgument );
		}
	
	
	if( iTimeoutWaiter && !iTimeoutWaiter->IsActive())
		{
		iTimeoutWaiter->ActuateTimer();
		}
	}



/**
 Calls here are only relevant in the case of StartApp.
*/
TInt CRvObserver::GetProcessHandle( RProcess& aProcess, const TThreadId& aThreadId )
	{
	TInt err = KErrNone;
	if( ESsmCmdStartApp == iStartupProperties->CommandType() )
		{
		RThread thread;
		err = thread.Open(aThreadId);
		if( KErrNone == err )
			{
			err = thread.Process(aProcess);
			}
		thread.Close();
		}
	return err;
	}

/**
 Leaves here occur in the context of RunL, therefore error notification is performed via RunError()
*/
void CRvObserver::NotifyOrRetryL()
	{
	if( GoodStart() )
		{
		// Rendezvous succeeded. If ESsmCmdStartApp, the process handle has been supplied in GoodStart
		iStarter.NotifyCompletion( iStatus.Int() );
		}
	else
		{
		//Launches the application if it is not timedout or retries if not exhausted.
		--iRetries; 
		if( iRetries < 0 || ( iRetries < 0 && iTimeOut ) )
			{
			iTimeOut = EFalse;
			// We have not succeeded and retries, if any, have been exhausted.
			iStarter.NotifyCompletion( iStatus.Int() );
			}
		else
			{
			InvokeL();
			}
		}
	}



/**
 Has the process rv'd with an error? If so terminate it.
 Has the process been panicked with KErrNone? If so translate that error to KErrGeneral;
 If the process has panicked or been killed, the Exit reason should be conveyed via iStatus.
 Any leave here occurs in the context of RunL, therefore any such notifications are performed via RunError().
 An error code of KErrAlreadyExists is presumed to imply that an attempt has been made to start a 
 pre-existing server. This is translated to KErrNone which allows CSsmStartSafe::StartAndMonitorL()
 to apply monitoring to the running server.
*/
TBool CRvObserver::GoodStart()
	{
	TBool goodStart( ETrue );
	TInt err = KErrNone;
	if(iStatus == KErrAlreadyExists && iStartupProperties->IsMonitoringRequired())
		{
		// This is done only when trying to start an already running server(process), as when we try to start an application 
		// which is already running, it doesn't return KErrAlreadyExists.  It would be started as another instance of same application.
		TFullName searchTerm(iStartupProperties->FileName());
		_LIT(KSearchAny, "*");
		searchTerm += KSearchAny;
 	    TFindProcess find(searchTerm);
		TFullName name;
		err = find.Next(name);
		if(err == KErrNone)
			{
			err = iProcess.Open(find);
			DEBUGPRINT2A("Tried to Open process which is already running with %d", err);
			}
		}
	// In the case of pre-existing servers.
	iStatus = ( iStatus == KErrAlreadyExists ) ? KErrNone : iStatus.Int(); 

	// In the case of StartApp
	err = GetProcessHandle( iProcess, iThreadId );

	if( iStatus.Int() != KErrNone || err != KErrNone )
		{
		goodStart = EFalse;
		if(KErrNone == err && iProcess.Handle() && (EExitPending == iProcess.ExitType()))
			{
			iProcess.Terminate( iStatus.Int() );
			iProcess.Close();
			}
		}	
	else if(err == KErrNone &&  iProcess.Handle() && (EExitPanic == iProcess.ExitType()))
		{
		goodStart = EFalse;
		//We can't use the 'exit reason' if the process has panicked as this is the 
		//panic 'reason' and may be '0' which cannot be distinguished from KErrNone
		iStatus = KErrGeneral;
		}
	
	return goodStart;
	}



void CRvObserver::DoCancel()
	{
	if( iTimeoutWaiter )
		{
		iTimeoutWaiter->Cancel();
		}
	// Relevant if StartApp
	TInt err = GetProcessHandle(iProcess, iThreadId); 
	if( KErrNone == err )
		{
		err = iProcess.RendezvousCancel( iStatus );
		}
	
	// Docs say iStatus should be completed with KErrCancel, but I am observing KErrNone, therefore:-
	TInt factoredCompletionCode = ( KErrNone == err ) ? KErrCancel : err ;
	iStarter.NotifyCancellation( factoredCompletionCode );
	}



TInt CRvObserver::RunError( TInt aError )
	{
	DEBUGPRINT2A("CRvObserver::RunError called with error %d", aError);
	iStarter.NotifyCompletion( aError );
	return KErrNone;
	}



void CRvObserver::RvAndObserveL( CSsmStartupProperties* aStartupProperties )
	{
	// Are we already running?
	if( ERvObserverStateNotInvoked != iObserverState )
		{
		User::Leave( KErrInUse );
		}
	
	if( (ESsmCmdStartApp == aStartupProperties->CommandType()) && !iApaStarter  )
		{
		User::Leave( KErrNotSupported );
		}
	
	iStartupProperties = aStartupProperties;
	iRetries = iStartupProperties->Retries();
	
	// If timeout is zero (or negative), we will wait indefinitely.
	if( iStartupProperties->Timeout() > KSsmStartSafeNoTimeout )
		{
		iTimeoutWaiter = CTimeoutWaiter::NewL( iStartupProperties->Timeout(), *this );
		}
	
	SetActive();
	TRequestStatus* trs = &iStatus;
	User::RequestComplete( trs, KErrNone );
	}



/**
 From MTimeoutWaiterNotification
*/
void CRvObserver::NotifyTimeout()
	{
	// Relevant if StartApp
	TInt err = GetProcessHandle(iProcess, iThreadId); 
	if( KErrNone == err )
		{
		// No more retries would be done
		iProcess.Terminate( KErrTimedOut ); 
		iTimeOut = ETrue;
		}
	}



CRvObserver* CRvObserver::NewL( RProcess& aProcess, MRvObserverNotification& aObserved, CApaStarter* aApaStarter )
	{
	CRvObserver* self = NewLC( aProcess, aObserved, aApaStarter );
	CleanupStack::Pop();
	return self;
	}



CRvObserver* CRvObserver::NewLC( RProcess& aProcess, MRvObserverNotification& aObserved, CApaStarter* aApaStarter )
	{
	CRvObserver* self = new(ELeave) CRvObserver( aProcess, aObserved, aApaStarter );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}



CRvObserver::~CRvObserver()
	{
	Cancel();
	delete iTimeoutWaiter;
	}



CRvObserver::CRvObserver( RProcess& aProcess, MRvObserverNotification& aObserved, CApaStarter* aApaStarter )
: CActive( EPriorityStandard ), 
  iApaStarter( aApaStarter ),
  iStarter( aObserved ), 
  iThreadId( KSsmStartSafeNullThreadId ),
  iProcess( aProcess ),
  iTimeOut(EFalse)
	{
	CActiveScheduler::Add( this );
	}



void CRvObserver::ConstructL()
	{
	}

