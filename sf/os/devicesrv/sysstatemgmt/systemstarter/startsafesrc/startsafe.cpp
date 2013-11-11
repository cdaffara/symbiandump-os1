// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <startupproperties.h>
#include "startsafe.h"
#include "restartsys.h"
#include "apastarter.h"

#include "sysstartpanic.h"

_LIT(KApStartDLL, "apstart.dll");
typedef CApaStarter* (*TFuncNewL)();

const TInt KStartSafeMilliToMicro = 1000;
const TInt KStartSafeRestartTimeout = 5000000;
const TUint64 KStartSafeNullThreadId = 0;


/**
Used to create an instance of CStartSafe class
@return An instance of CStartSafe
@internalTechnology
@deprecated
*/
EXPORT_C CStartSafe* CStartSafe::NewL()
	{
	CStartSafe* startSafe = new(ELeave) CStartSafe();
	CleanupStack::PushL( startSafe );
	
	startSafe->ConstructL();
	
	CleanupStack::Pop( startSafe );
	return startSafe;	
	}

CStartSafe::CStartSafe()
	{
	}

 
void CStartSafe::ConstructL()
	{
	LoadApStartLibL();		
	}
	
	
CStartSafe::~CStartSafe()
	{
	delete iApaStarter;
		
	iApStartLib.Close();
	}



/**
Used to start a process or application and initiate monitoring. 
Use this for EFireAndForget or EWaitForStart. The case of EDeferredWaitForStart
is treated as for EFireAndForget.
The number-of-retries parameter in aStartupProperties applies to the startup procedure.
If monitoring is not successfully setup, the started process is killed and this function
leaves with the error code supplied by the system-monitor.

@param aStartupProperties Startup properties provided by the caller.
@param aProcess The newly created process.
@param aNumRetried The number of retries made in starting the process. 
@leave KErrArgument if the monitor flag is not set in aStartupProperties. 
@leave Another of the system-wide error codes

@internalTechnology
@deprecated
*/
EXPORT_C void CStartSafe::StartAndMonitorL( const CStartupProperties &aStartupProperties, RProcess& aProcess, TInt& aNumRetried )
	{

	if( !aStartupProperties.Monitored() )
		{
		User::Leave( KErrArgument );	
		}


	RSysMonSession sysMon;
	CleanupClosePushL( sysMon );
	sysMon.OpenL();
	
	StartL( aStartupProperties, aProcess, aNumRetried );
	TRAPD( err, sysMon.MonitorL( aStartupProperties, aProcess ) );
	
	if( KErrNone != err )
		{
		aProcess.Kill( err );
		User::Leave( err );	
		}
	
	CleanupStack::PopAndDestroy( &sysMon );
	}



/**
Used to start a process or application using EFireAndForget or EWaitForStart.
The case of EDeferredWaitForStart is treated as EFireAndForget.
 
@param aStartupProperties Startup properties provided by the caller.
@param aProcess The newly created process.
@param aNumRetried Number of retries made for starting the process. 
@leave KErrNoMemory if no memory. 
@leave KErrArgument if the TStartMethod enumeration in aStartupProperties is out of range. 
@leave KErrTimedOut if the Application or process failed to rendezvous within the interval specified in aStartupProperties. 
@leave KErrAbort if the startup and rendezvous was unsuccessful _and the call to restart the system was also unsuccessful. 
Otherwise,one of the system-wide error codes
@internalTechnology
@deprecated
*/
EXPORT_C void CStartSafe::StartL( const CStartupProperties &aStartupProperties, RProcess& aProcess, TInt& aNumRetried )	
	{
	
	TInt err = KErrNone;	
	
	aNumRetried = 0;
	
	switch( aStartupProperties.StartMethod() )
	 	{ 	
	 	
	 	case EDeferredWaitForStart:
	 	case EFireAndForget:
	 		TRAP( err, DoFireAndForgetL( aStartupProperties, aProcess ) );
 	 		break;

	 	case EWaitForStart:
	 		TRAP( err, DoWaitForStartL( aStartupProperties, aProcess, aNumRetried ) );					
			break;
			
		default:
			User::Leave( KErrArgument);
			break;
	 	}
	 	
	 
	if ( KErrNone != err ) 
		{
		InstituteRestartL( aStartupProperties );
		
		User::Leave( err );
		}
	
	}
	
	

/**
Used to start a process or application using EDeferredWaitForStart. 
Note: Retries are not attempted in this asynchronous form of the function.

@param aStartupProperties Startup properties provided by the caller.
@param aProcess The newly created process.
@param aCommandTrs The request-status which is completed by the process rendezvous, or by this function in case of error.
@leave KErrNotSupported if ApStart is not installed on the device.
@leave KErrArgument if the CStartupProperties StartupType is not recognised, otherwise one of the system-wide error codes
@internalTechnology
@deprecated 
*/
EXPORT_C void CStartSafe::StartL( const CStartupProperties &aStartupProperties, RProcess& aProcess, TRequestStatus& aCommandTrs )
	{
	
	TInt err = KErrNone;
	
	
	switch( aStartupProperties.StartupType() )
		{
		case EStartApp:
			{	
			// Guard against the case where there is no Apparc
			if( !iApaStarter )
				{
				User::Leave( KErrNotSupported );	
				}
				
			TThreadId threadId( KStartSafeNullThreadId );
	
			TRAP( err, 
					{
					iApaStarter->StartAppL( aStartupProperties.FileName(), 
											aStartupProperties.Args(), 
											aStartupProperties.Viewless(),
										 	aStartupProperties.StartInBackground(), 
											threadId, aCommandTrs );
					} );
					
			// In the case of error here, we know Apparc has _not called RProcess::Rendezvous( aCommandTrs ) 
			// on the process yet, so  we must complete the TRS.	
			if ( KErrNone != err )
				{	
	 			TRequestStatus* errorTrs = &aCommandTrs;
				User::RequestComplete( errorTrs, err );
				
				InstituteRestartL( aStartupProperties );
				}
				
			// In the case of error here, we know Apparc _has called RProcess::Rendezvous( aCommandTrs ) on the process,
			// so do not complete the TRS, just leave.
			TRAP( err, GetProcessHandleL(aProcess, threadId) );
			
			if( KErrNone != err )
				{
				InstituteRestartL( aStartupProperties );
				}

			}
			break;
		
		case EStartProcess:
			{
		 	err = aProcess.Create( aStartupProperties.FileName(), aStartupProperties.Args() );
		
			if ( KErrNone == err )
				{		
				aProcess.Rendezvous( aCommandTrs );
				// aProcess has not been Resumed yet
				// but Rendezvous _can complete with KErrNoMemory, hence:-
				if( aCommandTrs == KRequestPending ) 
					{
					aProcess.Resume();
					}
				else
					{
					aProcess.Kill( KErrGeneral );
					
					InstituteRestartL( aStartupProperties );
					// If a restart is specified, this Leave will not be called, but if a restart is not specified,
					// InstituteRestartL() returns, therefore communicate the error by leaving with the TRS's value.
					User::Leave( aCommandTrs.Int() );
					}
					
				}
			else
				{
				InstituteRestartL( aStartupProperties );
				// If a restart is specified, this Leave will not be called, but if a restart is not specified, 
				// InstituteRestartL() returns, therefore communicate the error by leaving with err.
				User::Leave( err );
				}
					
			}		
			break;
			
		default:
			User::Leave( KErrArgument );
			break;	
		}

	}
	


/**
Waits for AppArc Server initialise.
Note: This function is synchronous.
@param aStatus Command Status
@return TInt system error code
@internalTechnology
@deprecated 
*/			
EXPORT_C void CStartSafe::InitAppArcServer( TRequestStatus& aStatus )
	{
	
	TRequestStatus* pStatus = &aStatus;
	
	// Guard against the case where there is no Apparc.
	if( !iApaStarter )
		{
		User::RequestComplete( pStatus, KErrNotSupported );	
		return;
		}
		
	TRAPD( err, iApaStarter->WaitForApparcToInitialiseL() );
	User::RequestComplete( pStatus, err );
	}



/**
 Make a call to restart the system.
 
 RestartSys currently requires the guard-time KStartSafeRestartTimeout. 
 The Leave() should never eventuate, except in case of a restart problem.
*/
void CStartSafe::InstituteRestartL( const CStartupProperties& aStartupProperties )
	{

	switch( aStartupProperties.RecoveryMethod() )
		{
	case ERestartOS:
		User::LeaveIfError(RestartSys::RestartSystem());
		
		User::After( KStartSafeRestartTimeout );
		User::Leave( KErrAbort );
		break;

	case ERestartOSWithMode:
		User::LeaveIfError( RestartSys::RestartSystem( aStartupProperties.RestartMode() ) );
		
		User::After( KStartSafeRestartTimeout );
		User::Leave( KErrAbort );
		break;
		
	case EIgnoreOnFailure:
	default:
		break;
		}

	}
	


TInt CStartSafe::AwaitRendezvous( RProcess& aProcess, TInt aTimeout )
	{
	__ASSERT_DEBUG( aTimeout >= 0, PanicNow(KPanicStartSafe, ETimerValueInvalid) );

	TInt err = KErrNone;
	TRequestStatus processTrs;
		 	 
	// Initiate the rendezvous	
	aProcess.Rendezvous( processTrs );
		
	// Ensure it hasn't completed immediately, eg KErrNoMemory
	if( processTrs == KRequestPending ) 
		{
		aProcess.Resume();
		
		if ( KStartSafeNoTimeout == aTimeout )
			{
			// Wait unconditionally
			User::WaitForRequest( processTrs );
			ShootBadProcess( aProcess, processTrs );
			}
		else
			{
			RTimer timer;
			err = timer.CreateLocal();
			
			if( KErrNone == err )
				{		
				TRequestStatus timerTrs;
				
				timer.After( timerTrs, TTimeIntervalMicroSeconds32(aTimeout * KStartSafeMilliToMicro) );
				User::WaitForRequest( timerTrs, processTrs );
				
				if( processTrs != KRequestPending )
					{
					// Rendezvous completed within the timeout period.
					timer.Cancel();
					User::WaitForRequest( timerTrs );
					ShootBadProcess( aProcess, processTrs );	
					}
				else
					{
					// Timer completed. Ensure it completed successfully
					if( timerTrs == KErrNone )
						{
						// Rendezvous timed out	
						aProcess.Kill( KErrTimedOut );
						}
					// Wait for the Kill() to be signalled.
					// Or if the timer completed with an error, wait unconditionally 
					User::WaitForRequest( processTrs );							
					}														
				}
			else
				{
				// Creating the timer was unsuccessful so we must wait unconditionally after all.
				User::WaitForRequest( processTrs );	
				ShootBadProcess( aProcess, processTrs );
				}
						
			timer.Close();
			}
					
		}
	else
		{
		// The rendezvous completed before the call to ::Resume(). Perhaps KErrNoMemory
		// The process exit reason should provide the return value KErrGeneral;
		aProcess.Kill( KErrGeneral );
		User::WaitForRequest( processTrs );
		}
	
	
	
	
	// Various clauses make the exit-code derivation non-concise.
	if( KErrNone == err )
		{
		TInt exitType = aProcess.ExitType();
		TInt exitReason = 0;
		
		
		if( EExitPanic == exitType )
			{
			exitReason = aProcess.ExitReason();	
			}

		// Guard against the potential ambiguity of the case where the process has been panicked with a reason of 0
		if( (EExitPanic == exitType) && (0 == exitReason) )
			{
			err = KErrGeneral;	
			}
		else
			{
			// If the process panicked, the trs will have been completed with KErrNone (ie 0), so return the exitReason
			err = ( EExitPanic == exitType ) ? exitReason : processTrs.Int();	
			}
		}

	return err;	
	}



/**
Await completion of the TRS either unconditionally, or allow aTimeout for aProcess to start.
*/
TInt CStartSafe::AwaitProcessStartup( RProcess& aProcess, TInt aTimeout, TRequestStatus& aCommandTrs )
	{		
	__ASSERT_DEBUG( aTimeout >= 0, PanicNow(KPanicStartSafe, ETimerValueInvalid));
					 
	TInt err = KErrNone;
	
	if ( KStartSafeNoTimeout == aTimeout )
		{ 
		 // Wait unconditionally.
		User::WaitForRequest( aCommandTrs );
		ShootBadProcess( aProcess, aCommandTrs );
		}
	else
		{						
		RTimer timer;
		err = timer.CreateLocal();
		
		if( KErrNone == err )
			{
			TRequestStatus timerTrs;
			timer.After( timerTrs, TTimeIntervalMicroSeconds32(aTimeout * KStartSafeMilliToMicro) );
			
		
			User::WaitForRequest( timerTrs, aCommandTrs );
			
			if( aCommandTrs != KRequestPending )
				{
				// App started within the timeout period.
				timer.Cancel();
				User::WaitForRequest( timerTrs );
				ShootBadProcess( aProcess, aCommandTrs );
				}
			else
				{
				// App timed out.
				aProcess.Kill( KErrTimedOut );			 
				User::WaitForRequest( aCommandTrs );				
				}
				
			}
		else
			{
			// Creating the timer was unsuccessful, so we must wait unconditionally after all.
			User::WaitForRequest( aCommandTrs );
			ShootBadProcess( aProcess, aCommandTrs );	
			}
		
		timer.Close();
		}

		
	// Various clauses make the exit-code derivation non-concise. 
	if ( KErrNone == err )
		{
		TInt exitType = aProcess.ExitType();
		TInt exitReason = 0;
		
		
		if( EExitPanic == exitType )
			{
			exitReason = aProcess.ExitReason();	
			}

		// Guard against the potential ambiguity of the case where the process has been panicked with a reason of 0
		if( (EExitPanic == exitType) && (0 == exitReason) )
			{
			err = KErrGeneral;	
			}
		else
			{
			err = ( EExitPanic == exitType ) ? exitReason : aCommandTrs.Int();
			}
			
		}
	  
	  
	return err; 	
	}



void CStartSafe::DoFireAndForgetL( const CStartupProperties& aStartupProperties, RProcess& aProcess )
	{
	
	switch(  aStartupProperties.StartupType() )
		{
		case EStartApp:
			DoStartAppL( aStartupProperties, aProcess );
			break;
			
		case EStartProcess:
			DoStartProcessL( aStartupProperties, aProcess );
			break;
			
		default :
			User::Leave( KErrArgument );
			break;
		}

	}
	
	
	
void CStartSafe::DoWaitForStartL( const CStartupProperties& aStartupProperties, RProcess& aProcess, TInt& aNumRetried )
	{
	
	switch( aStartupProperties.StartupType() )
		{
		case EStartApp:
			User::LeaveIfError( DoStartAppWithRetries(aStartupProperties, aProcess, aNumRetried) );
			break;
			
		case EStartProcess:
			 User::LeaveIfError( DoStartProcessWithRetries(aStartupProperties, aProcess, aNumRetried) );
			break;
			
		default:
			User::Leave( KErrArgument );
			break;
		}

	}



TInt CStartSafe::DoStartAppWithRetries( const CStartupProperties& aStartupProperties, RProcess& aProcess, TInt& aNumRetried )
	{

	// Guard against the case where there is no Apparc.
	if( !iApaStarter )
		{
		return KErrNotSupported;	
		}

	// Returned if NoOfRetries() is < 0.
	TInt err = KErrArgument;
	TRequestStatus commandTrs;
	
	// Note: 1 attempt plus NoOfRetries. So a NoOfRetries of 0 results in one attempt.
	for( aNumRetried = 0; aNumRetried <= aStartupProperties.NoOfRetries(); aNumRetried++ )
		{		
		TThreadId threadId( KStartSafeNullThreadId );
		
		TRAP( err, iApaStarter->StartAppL(aStartupProperties.FileName(), 
				   aStartupProperties.Args(), 
				   aStartupProperties.Viewless(), aStartupProperties.StartInBackground(), threadId, commandTrs) );

		if( KErrNone != err )
			{
			continue;				
			}

		TRAP( err, GetHandleAndWaitForProcessL(aStartupProperties, aProcess, threadId, commandTrs) );
		
		if( KErrNone == err )
			{
			break;
			}
		else if(aNumRetried != aStartupProperties.NoOfRetries())
			{
			// If we are trying to restart the process on failure and this is not the last retry, 
			// then process handle should be closed as it would be leaked otherwise.
			aProcess.Close();
			}
	
				
		}
	
	return err;		
	}
	
	
	
void CStartSafe::GetProcessHandleL( RProcess& aProcess, const TThreadId& aThreadId )
	{
	
	RThread thread;
	CleanupClosePushL( thread );
	
	User::LeaveIfError( thread.Open(aThreadId) );  
	User::LeaveIfError( thread.Process(aProcess) );	
	
	CleanupStack::PopAndDestroy( &thread );	
	}



void CStartSafe::GetHandleAndWaitForProcessL( const CStartupProperties& aStartupProperties, RProcess& aProcess, TThreadId& aThreadId, TRequestStatus& aCommandTrs )
	{

	GetProcessHandleL( aProcess, aThreadId );
	User::LeaveIfError( AwaitProcessStartup(aProcess, aStartupProperties.Timeout(), aCommandTrs) );
	}
	
	

TInt CStartSafe::DoStartProcessWithRetries( const CStartupProperties& aStartupProperties, RProcess& aProcess, TInt& aNumRetried )
	{

	// Returned if NoOfRetries() is < 0.	
	TInt err = KErrArgument;
			
	// Note: 1 attempt plus NoOfRetries. So NoOfRetries of 0 results in one attempt
	for( aNumRetried = 0; aNumRetried <= aStartupProperties.NoOfRetries(); aNumRetried++ )
		{		
		err = aProcess.Create( aStartupProperties.FileName(), aStartupProperties.Args() );
		
		if ( KErrNone == err )
			{
			// AwaitRendezvous() kills the process if unsuccessful. 
			err = AwaitRendezvous( aProcess, aStartupProperties.Timeout() );
			
			if( KErrNone == err )
				{
				break;					
				}
			else if(aNumRetried != aStartupProperties.NoOfRetries())
				{
				// If we are trying to restart the process on failure and this is not the last retry, 
				// then process handle should be closed as it would be leaked otherwise.
				aProcess.Close();
				}
			}
			
		}
		
	
	return err;	
	}



void CStartSafe::DoStartAppL( const CStartupProperties& aStartupProperties, RProcess& aProcess )
	{

	// Guard against the case where there is no Apparc
	if( !iApaStarter )
		{
		User::Leave( KErrNotSupported );	
		}


	TThreadId threadId( KStartSafeNullThreadId );
	
	iApaStarter->StartAppL( aStartupProperties.FileName(), 
							aStartupProperties.Args(), 
							aStartupProperties.Viewless(), 
						 	aStartupProperties.StartInBackground(), 
							threadId );	
					
	// We need the process information only if we want to monitor.				
	if ( aStartupProperties.Monitored() || (aStartupProperties.StartMethod() == EDeferredWaitForStart) ) 
		{
		GetProcessHandleL( aProcess, threadId );	
		}
	
	}
	
	
	
void CStartSafe::DoStartProcessL( const CStartupProperties& aStartupProperties, RProcess& aProcess )
	{
	User::LeaveIfError( aProcess.Create( aStartupProperties.FileName(), aStartupProperties.Args()) );
	aProcess.Resume();	
	}



/**
Load the library. Locate and call the ordinal corresponding to CApStart::NewL().

Note: We do not leave in the case of being unable to load the libray, but assume Apparc is not present.
      iApaStarter is checked for NULL before use passim and appropriate error code supplied if it is. 
      The return code is derived from the loader-server (Base) and not closely specified in RLibrary docs or code.
*/
void CStartSafe::LoadApStartLibL()
	{
	
	 if( KErrNone != iApStartLib.Load(KApStartDLL) )
	 	{
	 	return;	
	 	}
	 
	TFuncNewL apStartNewL = reinterpret_cast<TFuncNewL>( iApStartLib.Lookup(1) );

	iApaStarter = apStartNewL();
	}



/**
 See if the process rendezvoused with an error code and is still alive. If so then Kill() it.
*/
void CStartSafe::ShootBadProcess( RProcess& aProcess, const TRequestStatus& aStatus )
	{
	// Permit positive values of aStatus which may be used for (non-error) information
	if( (aStatus < KErrNone) && (EExitPending == aProcess.ExitType()) )
		{
		aProcess.Terminate( aStatus.Int() );	
		}
		
	}
