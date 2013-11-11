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

#include "fireandforget.h"
#include "timeoutwaiter.h"
#include "ssmdebug.h"

_LIT(KApStartDLL, "apstart.dll");
_LIT( KSysMonProxyDLL, "loadsysmon.dll" );
typedef CApaStarter* (*TFuncNewL)( void );
typedef MSsmLoadSysMon* (*TFuncCreateL)( void );

void CFireAndForget::FireD()
	{
	SetActive();

	TRequestStatus* pTrs = &iStatus;
	User::RequestComplete( pTrs, KErrNone );
	}

/**
 Start the app or process and delete oneself.
 */
void CFireAndForget::RunL()
	{
	// create timer only if timeout > KSsmStartSafeNoTimeout and iTimeoutWaiter is NULL
	const TInt32 timeout = iStartupProperties->Timeout();
	if( timeout > KSsmStartSafeNoTimeout && !iTimeoutWaiter)
		{
		iTimeoutWaiter = CTimeoutWaiter::NewL( timeout, *this );
		}

	switch( iState )
		{
		case EFAndFIdle:
			{
			iState = EFAndFStateRetrying;
			InvokeL();
			}
			break;

		case EFAndFStateRetrying:
			{
			CleanupOrRetryL();
			}
			break;

		default:
			User::Leave( KErrUnknown );
			break;
		}
	}

TBool CFireAndForget::GoodStart()
	{
	TBool goodStart( ETrue );
	TInt err = KErrNone;

	if(iStatus == KErrAlreadyExists && (iStartupProperties->IsMonitoringRequired()))
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
			DEBUGPRINT2A("*** CFireAndForget - Tried to Open process which is already running with %d", err);
			}
		}
	iStatus = ( iStatus == KErrAlreadyExists ) ? KErrNone : iStatus.Int(); // In the case of pre-existing servers.

	// Relevant if StartApp
	err = GetProcessHandle(iProcess, iThreadId);
	if( iStatus.Int() != KErrNone || err != KErrNone )
		{
		goodStart = EFalse;

		if(KErrNone == err && iProcess.Handle() && (EExitPending == iProcess.ExitType()))
			{
			TPtrC fileName = iStartupProperties->FileName();
			DEBUGPRINT3A("*** CFireAndForget - terminating with err: %d for app/process %S", iStatus.Int(), &fileName);
			iProcess.Terminate( iStatus.Int() );
			iProcess.Close();
			}
		}	
	else if(KErrNone == err && iProcess.Handle() && (EExitPanic == iProcess.ExitType()))		 
		{
		goodStart = EFalse;
		//We can't use the 'exit reason' if the process has panicked as this is the 
		//panic 'reason' and may be '0' which cannot be distinguished from KErrNone
		iStatus = KErrGeneral;
		}

	return goodStart;
	}

TInt CFireAndForget::GetProcessHandle( RProcess& aProcess, const TThreadId& aThreadId )
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

void CFireAndForget::CleanupOrRetryL()
	{
	if( GoodStart() )
		{
		// Rendezvous succeeded.
		// Monitor the process/app if needed
		StartMonitorIfRequiredL();
		delete this;
		}
	else
		{		
		if( --iRetries < 0)
			{
			// We have not succeeded and retries, if any, have been exhausted.
			TPtrC fileName = iStartupProperties->FileName();
			DEBUGPRINT2(_L("*** CFireAndForget - retries exhausted for app/process %S"), &fileName);
			delete this;
			}
		else
			{
			InvokeL();
			}
		}
	}

void CFireAndForget::InvokeL()
	{
	TPtrC fileName = iStartupProperties->FileName();
	DEBUGPRINT3(_L("*** CFireAndForget::InvokeL() - iRetries is %d for app/process %S"), iRetries, &fileName);

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
									iStartupProperties->StartInBackground(), iThreadId, iStatus ));
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

void CFireAndForget::DoCancel()
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
	}

TInt CFireAndForget::RunError( TInt aError )
	{
	DEBUGPRINT2A("*** CFireAndForget::RunError error %d", aError);

	delete this;
	
	// We've dealt with the error here, so return KErrNone always
	aError = KErrNone;
	return aError;
	}

CFireAndForget* CFireAndForget::NewL( const CSsmStartupProperties& aStartupProperties )
	{
	CFireAndForget* self = NewLC( aStartupProperties );
	CleanupStack::Pop();
	return self;
	}

CFireAndForget* CFireAndForget::NewLC( const CSsmStartupProperties& aStartupProperties  )
	{
	CFireAndForget* self = new(ELeave) CFireAndForget();
	CleanupStack::PushL( self );
	self->ConstructL( aStartupProperties );
	return self;
	}

void CFireAndForget::ConstructL( const CSsmStartupProperties& aStartupProperties )
	{
	if ( aStartupProperties.FileName() == KNullDesC ) 
		{
		User::Leave(KErrArgument);
		}
	iStartupProperties = CSsmStartupProperties::NewL( aStartupProperties );
	
	// We need to decrement the retries in case of application\process failure and since we need
	// the retries for monitoring it is stored as a separate member variable
	iRetries = aStartupProperties.Retries();
	// create iApaStarter, we are not using apaStarter of SS as 
	// this object will be running even after SS is finished.
	LoadApStartLibL();
	}

CFireAndForget::CFireAndForget()
	: CActive( EPriorityStandard ),
	iThreadId( KSsmStartSafeNullThreadId )
	{
	CActiveScheduler::Add( this );
	}

CFireAndForget::~CFireAndForget()
	{
	Cancel();

	delete iTimeoutWaiter;
	delete iApaStarter;
	iApStartLib.Close();
	iProcess.Close();
	iSysMonProxyLib.Close();
	delete iStartupProperties;
	}

/**
 From MTimeoutWaiterNotification
*/
void CFireAndForget::NotifyTimeout()
	{
	// Relevant if StartApp
	TInt err = GetProcessHandle(iProcess, iThreadId);
	if( KErrNone == err )
		{
		TPtrC fileName = iStartupProperties->FileName();
		DEBUGPRINT2(_L("*** CFireAndForget - timeout exhausted for app/process %S"), &fileName);
		iProcess.Terminate( KErrTimedOut ); // should invoke RunL once more.
		delete this;
		}
	}

/**
Load the library. Locate and call the ordinal corresponding to CApStart::NewL().

Note: We do not leave in the case of being unable to load the libray, but assume Apparc is not present.
      iApaStarter is checked for NULL before use passim and appropriate error code supplied if it is. 
      The return code is derived from the loader-server (Base) and not closely specified in RLibrary docs or code.
*/
void CFireAndForget::LoadApStartLibL()
	{
	 if( KErrNone != iApStartLib.Load(KApStartDLL) )
	 	{
	 	return;	
	 	}

	TFuncNewL apStartNewL = reinterpret_cast<TFuncNewL>( iApStartLib.Lookup(1) );
	iApaStarter = apStartNewL();
	}

/**
If Monitoring is required for the started process/app, load SysMonCli and
launch the monitor
*/
void CFireAndForget::StartMonitorIfRequiredL()
	{
	if (iStartupProperties->IsMonitoringRequired())
		{
		// Load the dll interfacing between us and the System Monitor component, if installed.
		MSsmLoadSysMon* sysMonCli = NULL;
		if( KErrNone != iSysMonProxyLib.Load(KSysMonProxyDLL) )
		 	{
		 	return;	
		 	}
		
		TFuncCreateL sysMonProxyCreateL = reinterpret_cast<TFuncCreateL>( iSysMonProxyLib.Lookup(1) );
		sysMonCli = sysMonProxyCreateL();
		CleanupStack::PushL(sysMonCli);
		sysMonCli->OpenL();
		TRAPD( err, sysMonCli->MonitorL( *iStartupProperties, iProcess ) );
		sysMonCli->Close();
		CleanupStack::PopAndDestroy();
		if( KErrNone != err )
			{
			iProcess.Terminate( err );
			User::Leave( err );
			}
		}
	}
