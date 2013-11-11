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


#include <ssm/ssmstartsafe.h>

#include <ssm/ssmstartupproperties.h>
#include "startandretry.h"
#include "rvobserver.h"
#include "fireandforget.h"
#include "activewaiter.h"
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmloadsysmon.h>
#include <apastarter.h>

_LIT(KApStartDLL, "apstart.dll");
_LIT( KSysMonProxyDLL, "loadsysmon.dll" );
typedef CApaStarter* (*TFuncNewL)( void );
typedef MSsmLoadSysMon* (*TFuncCreateL)( void );



/**
Used to start a process or application and initiate monitoring. 
Use this for ESsmWaitForSignal or ESsmDeferredWaitForSignal. 
The number-of-retries parameter in aStartupProperties applies to the startup procedure.
If monitoring is not successfully setup, the started process is killed and this function
leaves with the error code supplied by the system-monitor.

@param aStartupProperties Startup properties provided by the caller.
@param aProcess Handle on the process to be created.
@leave KErrArgument if the monitor flag is not set in aStartupProperties, or if the execution behaviour is ESsmFireandForget.
@leave KErrNotSupported if the system monitor sub-system is not installed. 
@leave Another of the system-wide error codes @see StartL

@internalTechnology
@released
*/
EXPORT_C void CSsmStartSafe::StartAndMonitorL( const CSsmStartupProperties& aStartupProperties, RProcess& aProcess )
	{
	
	if( !aStartupProperties.IsMonitoringRequired() )
		{
		User::Leave( KErrArgument );	
		}
	
	if( !iSysMonCli )
		{
		User::Leave( KErrNotSupported );
		}
	
	StartL( aStartupProperties, aProcess );
	
	// Monitor the process only if the behavior is not EFireAndForget
	// The monitor for process/app with EFireAndForget behavior is started only after the process/app is started
	// Hence the monitor is started in the CFireAndForget object
	if (ESsmFireAndForget != aStartupProperties.ExecutionBehaviour())
		{
		iSysMonCli->OpenL();
		TRAPD( err, iSysMonCli->MonitorL( aStartupProperties, aProcess ) );
		iSysMonCli->Close();
		if( KErrNone != err )
			{
			aProcess.Terminate( err );
			User::Leave( err );	
			}
		}

	}



/**
 Synchronous function to start a process or application.
 Note: This API should be used only if one synchronous call to StartL at any one time can be assured.
 
@param aStartupProperties Startup properties provided by the caller.
@param aProcess Handle on the process to be created.
@leave KErrNoMemory if no memory. 
@leave KErrArgument if the TStartMethod enumeration in aStartupProperties is out of range. 
@leave KErrTimedOut if the Application or process failed to rendezvous within the interval specified in aStartupProperties. 
@leave Or one of the system-wide error codes
@internalTechnology
@released
*/
EXPORT_C void CSsmStartSafe::StartL( const CSsmStartupProperties &aStartupProperties, RProcess& aProcess )	
	{
	// The index is not required as a parameter because this function is synchronous and must only be used consecutively.
	TInt callerIndex = 0;
	CActiveWaiter* waiter = new(ELeave) CActiveWaiter;
	CleanupStack::PushL( waiter );
	
	Start( aStartupProperties, aProcess, waiter->iStatus, callerIndex );
	waiter->WaitActive();
	
	User::LeaveIfError(waiter->iStatus.Int());
	
	CleanupStack::PopAndDestroy( waiter );
	}
	
	

/**
 Asynchronous function to start a process or application.

@param aStartupProperties The startup-properties defining the app or process to be started. 
@param aProcess Handle on the process to be created.
@param aCommandTrs The TRequestStatus which is completed as a result of the process rendezvous or any error arising from the attempt.
@param aCallerIndex The index integer supplied by this call. This value should be perserved and passed in the case of a call to StartCancel()
@internalTechnology
@released 
*/
EXPORT_C void CSsmStartSafe::Start( const CSsmStartupProperties& aStartupProperties, RProcess& aProcess, TRequestStatus& aCommandTrs, TInt& aCallerIndex )
	{	
	
	aCommandTrs = KRequestPending;
	
	if (ESsmFireAndForget == aStartupProperties.ExecutionBehaviour())
		{
		TRequestStatus* status = &aCommandTrs;
	
		CFireAndForget* fireAndForget = NULL;
		// F&F destroys itself.
		TRAPD(err,fireAndForget = CFireAndForget::NewL(aStartupProperties));
		
		if(KErrNone == err)
			{
			fireAndForget->FireD();
			}
		User::RequestComplete(status, err);
		}
	else
		{
		TInt err = RegisterCall( aCallerIndex, aCommandTrs );

		if (err != KErrNone)
			{
			aCallerIndex = KErrNotFound;
			TRequestStatus* status = &aCommandTrs;
			User::RequestComplete( status, err );
			return;
			}

		TRAP( err, iStartSafeEntryArray[ aCallerIndex ].iStartAndRetry = CStartAndRetry::NewL( *this, aStartupProperties, aProcess, aCallerIndex, iApaStarter ));
		if (err != KErrNone)
			{
			DeRegisterCall( aCallerIndex );	//should complete the request with KErrCancel
			return;
			}
		iStartSafeEntryArray[ aCallerIndex ].iStartAndRetry->Actuate();
		}
	}

/**
Waits for AppArc Server to initialise.
@return KErrNotSupported if AppArc is not installed
@return TInt system error code
@internalTechnology
@released 
*/			
EXPORT_C TInt CSsmStartSafe::InitAppArcServer()
	{
	
	// Guard against the case of no Apparc.
	if( !iApaStarter )
		{
		return KErrNotSupported;	
		}
		
	TRAPD( err, iApaStarter->WaitForApparcToInitialiseL() );
	
	return err;
	}

/**
Async call to wait for Apparc Server to initialise.
@param aCommandTrs The TRequestStatus which is completed when applist is populated.
@internalTechnology
@released 
*/			
EXPORT_C void CSsmStartSafe::InitApparcServer(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	// Guard against the case of no Apparc.
	if( !iApaStarter )
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		return;
		}

	iApaStarter->InitApparcServer(aStatus);
	}

/**
Cancel Apparc Server initialization.
@internalTechnology
@released 
*/			
EXPORT_C void CSsmStartSafe::InitApparcServerCancel()
	{
	if (iApaStarter)
		{
		iApaStarter->InitApparcServerCancel();
		}
	}

/**
 Calling cancel precipitates the destruction of the rendezvous-observer/timeoutwaiter.
 These are AOs and so cancellation of any outstanding rendezvous is implied.
 @param aCallerIndex the index value supplied by the async CSsmStartSafe::StartL(). This is set to a large negative value upon completion
 of the operation to guard against multiple calls, or KErrArgument if the index is out of range.
*/
EXPORT_C void CSsmStartSafe::StartCancel( TInt& aCallerIndex )
	{
	
	const TInt KSsNegValue = -9999;
	
	if( KErrNone != DeRegisterCall(aCallerIndex) )
		{
		aCallerIndex = KErrArgument;
		}
	else
		{
		aCallerIndex = KSsNegValue;
		}

	}

/**
 Inherited from MStartAndRetryNotifications
*/
void CSsmStartSafe::RendezvousComplete( TInt aComplete, TInt aCallerIndex )
	{
	
	User::RequestComplete( iStartSafeEntryArray[ aCallerIndex ].iCallerTrs, aComplete );

	DeRegisterCall( aCallerIndex );
	}



void CSsmStartSafe::CancellationComplete( TInt aComplete, TInt aCallerIndex )
	{
	User::RequestComplete( iStartSafeEntryArray[ aCallerIndex ].iCallerTrs, aComplete );
	}



/**
 Add the caller to the array of callers. Supply an index which identifies the caller. 
 In the case a call to one of the cancel functions, the caller identifies themselves 
 by this index.
*/
TInt CSsmStartSafe::RegisterCall( TInt& aCallerIndex, TRequestStatus& aStatus )
	{
	const TInt count = iStartSafeEntryArray.Count();
	TBool slotFound = EFalse;	
	for(TInt i = 0; i < count; i++ )
		{
		if( !iStartSafeEntryArray[ i ].iInUse )
			{
			iStartSafeEntryArray[ i ].iInUse = ETrue;
			iStartSafeEntryArray[ i ].iStartAndRetry = NULL;
			iStartSafeEntryArray[ i ].iCallerTrs = &aStatus;
			iStartSafeEntryArray[ i ].iCallerIndex = aCallerIndex;
			aCallerIndex = i;
			slotFound = ETrue;
			iCallerCount++;
			break;
			}		
		}
	TInt err = KErrNone;
	if( !slotFound )
		{
		TSsmStartSafeEntry entry;
		entry.iStartAndRetry = NULL;
		entry.iCallerTrs = &aStatus;
		entry.iInUse = ETrue;
		err = iStartSafeEntryArray.Append( entry );
		if( KErrNone == err )
			{
			aCallerIndex = iCallerCount++;
			}
		}
	return err;
	}



TInt CSsmStartSafe::DeRegisterCall(TInt aCallerIndex )
	{	
	if( !__IN_RANGE(aCallerIndex, iStartSafeEntryArray.Count()) )
		{
		return KErrArgument;
		}
	
	if( iStartSafeEntryArray[ aCallerIndex ].iInUse )
		{
		delete iStartSafeEntryArray[ aCallerIndex ].iStartAndRetry;
		iStartSafeEntryArray[ aCallerIndex ].iStartAndRetry = NULL;	
		iStartSafeEntryArray[ aCallerIndex ].iInUse = EFalse;
		
		// In case of call from destructor. Actually, the other classes look after themselves
		// so this conditional shouldn't evaluate to true.
		TRequestStatus* trs = iStartSafeEntryArray[ aCallerIndex ].iCallerTrs;
		
		if( trs && (*trs == KRequestPending) )
			{
			User::RequestComplete( iStartSafeEntryArray[ aCallerIndex ].iCallerTrs, KErrCancel );
			}
		
		iCallerCount--;

	   // Remove unused elements at the end of the array, so iStartSafeEntryArray can be (granular)shrunk
		const TInt count = iStartSafeEntryArray.Count();
		TBool slotRemoved = EFalse;
		for(TInt i = count-1; i >= 0 ; i-- )
			{
			if( !iStartSafeEntryArray[ i ].iInUse )
				{
	      		iStartSafeEntryArray.Remove(i);
	      		slotRemoved = ETrue;
				}
			else
				{
				break;
				}
			}
		if (slotRemoved)
			{
			iStartSafeEntryArray.GranularCompress();
			}
		}
	
	return KErrNone;
	}



/**
Load the library. Locate and call the ordinal corresponding to CApStart::NewL().

Note: We do not leave in the case of being unable to load the libray, but assume Apparc is not present.
      iApaStarter is checked for NULL before use passim and appropriate error code supplied if it is. 
      The return code is derived from the loader-server (Base) and not closely specified in RLibrary docs or code.
*/
void CSsmStartSafe::LoadApStartLibL()
	{	
	 if( KErrNone != iApStartLib.Load(KApStartDLL) )
	 	{
	 	return;	
	 	}
	 
	TFuncNewL apStartNewL = reinterpret_cast<TFuncNewL>( iApStartLib.Lookup(1) );
	iApaStarter = apStartNewL();
	}



/**
 Load the dll interfacing between us and the System Monitor component, if installed.
*/
void CSsmStartSafe::LoadMonitorProxyLibL()
	{
	if( KErrNone != iSysMonProxyLib.Load(KSysMonProxyDLL) )
	 	{
	 	return;	
	 	}

	TFuncCreateL sysMonProxyCreateL = reinterpret_cast<TFuncCreateL>( iSysMonProxyLib.Lookup(1) );
	iSysMonCli = sysMonProxyCreateL();
	}



/**
Used to create an instance of CSsmStartSafe class
@return A pointer to an instance of CSsmStartSafe
@internalTechnology
@released
*/
EXPORT_C CSsmStartSafe* CSsmStartSafe::NewL()
	{
	CSsmStartSafe* self = NewLC();
	CleanupStack::Pop( self );
	
	return self;	
	}



EXPORT_C CSsmStartSafe* CSsmStartSafe::NewLC()
	{
	CSsmStartSafe* self = new(ELeave) CSsmStartSafe();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}



CSsmStartSafe::CSsmStartSafe()
	{
	}

 

void CSsmStartSafe::ConstructL()
	{
	LoadApStartLibL();		
	LoadMonitorProxyLibL();
	}
	


CSsmStartSafe::~CSsmStartSafe()
	{
	const TInt count = iStartSafeEntryArray.Count();	
	for(TInt i = 0; i < count; i++ )
		{
		DeRegisterCall( i );
		}
	
	delete iApaStarter;
	delete iSysMonCli;
	iStartSafeEntryArray.Close();
	iApStartLib.Close();
	iSysMonProxyLib.Close();
	}

