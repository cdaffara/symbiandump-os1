// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>
#include "monitor.h"
#include "startsafe.h"
#include "restartsys.h"
#include "sysmonserver.h"
#include "sysmoncliserv.h"
#include "timerlist.h"
#include "sysmon_patch.h"
#include <startupproperties.h>

#include "shmadebug.h"
#include "shmapanic.h"
#include <u32hal.h>

#ifdef _DEBUG
#include <e32property.h>
#endif // _DEBUG

const TInt CMonitor::iOffset = _FOFF(CMonitor, iSglQueLink);
const TInt KDelayRequiredForRestartSys = 5000000;	 // required by RestartSys API, see comments in RestartSys::RestartSystem()

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
_LIT(KRestartSysProxyDLL, "restartsys.dll");
_LIT(KCustomisedRestartSysProxyDLL, "custrestartsys.dll");
typedef TInt (*TFuncRestartL)(void);
typedef TInt (*TFuncRestartWithModeL)(TInt);
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN

TProcessId CMonitor::ProcessId() const
	{
	return iProcessId;
	}	


CMonitor* CMonitor::NewL(CSysMonServer& aServer, const TProcessId& aId, CStartupProperties* aStartupProperties, TBool aExecuteRecoveryMethodOnFailure)
	{
	ASSERT(aStartupProperties);
	CMonitor *monitor = new(ELeave) CMonitor(aServer, aId);
	CleanupStack::PushL(monitor);
	monitor->ConstructL(aStartupProperties, aExecuteRecoveryMethodOnFailure);
	CleanupStack::Pop(monitor);
	return monitor;
	}


CMonitor::CMonitor(CSysMonServer& aServer, const TProcessId& aId)
	:CActive(EPriorityHigh),
	iSysMonServer(aServer),
	iProcessId(aId),
	iProcess(aId),
	iLoadTime()
	{
	CActiveScheduler::Add(this);
	}


void CMonitor::ConstructL(CStartupProperties* aStartupProperties, TBool aExecuteRecoveryMethodOnFailure)
	{
	iLogonBackoffTimer = CLogonBackoffTimer::NewL( *this );
	
	User::LeaveIfError(iProcess.Open(iProcessId));
	
	// Don't support monitoring of system critical components as they will restart the device on failure anyway
	User::TCritical critical = User::ProcessCritical(iProcess);
	if (critical == User::ESystemCritical || critical == User::ESystemPermanent)
		{
		User::Leave(KErrNotSupported);
		}
	
	//Make sure the process is still running
	if((EExitPending == iProcess.ExitType()))
		{
		//Ensure the the filename in aStartupProperties is the same as in iProcess
		//(not possible to read filename from a dead process).
		TParse nameInProc, nameInProp;
		nameInProc.SetNoWild(iProcess.FileName(),NULL,NULL);
		nameInProp.SetNoWild(aStartupProperties->FileName(),NULL,NULL);
		
		if( nameInProc.Name().CompareF(nameInProp.Name()) )
			{
			User::Leave(KErrArgument);
			}
		}
	else
		{
		//The process is already dead, either leave now or let this monitor recover the process
		if(!aExecuteRecoveryMethodOnFailure)
			User::Leave(KErrDied);
		}	
	
	iReLaunchAttempts = 0;	// No. of re-launch attempts at the start of monitering would be zero.
	iReLaunchIntervalTimer = CRelaunchIntervalTimer::NewL(*this);

	// Can't leave after taking ownership of aStartupProperties 
	iStartupProperties = aStartupProperties;
	iLoadTime.UniversalTime();		
	}


CMonitor::~CMonitor()
	{
	delete iReLaunchIntervalTimer;
	delete iLogonBackoffTimer;
	
	Cancel();
	delete iStartupProperties;
	iProcess.Close();
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	iRestartSysLib.Close();
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	}


void CMonitor::Start()
	{
	iLogonBackoffTimer->ProcessLogon();
	}



void CMonitor::DoCancel()
	{
#ifdef _DEBUG
	TPtrC fileName = iStartupProperties->FileName();
	DEBUGPRINT2(_L("SysMonMonitor: Monitor Cancelled for %S"), &(fileName));
#endif

	iProcess.LogonCancel(iStatus); // cancels monitoring
	}


void CMonitor::RestartProcessL()
	{
#ifdef _DEBUG
	TPtrC fileName = iStartupProperties->FileName();
#endif
	DEBUGPRINT3(_L("SysMonMonitor: Going to restart %S, old process id=%u"), &fileName, iProcessId.Id());  
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	// As the retries are now counted(after PREQ1871) over a period of time it does make sense for fire and forget processes to have a non-zero retries count.
	__ASSERT_DEBUG((iStartupProperties->StartMethod() == EWaitForStart || iStartupProperties->StartMethod() == EFireAndForget), PanicNow(KPanicMonitor, EInvalidStartMethod));
	
	TInt propRetries = iStartupProperties->NoOfRetries();
	TBool restoreRetries = EFalse;
	
	if(propRetries > 0)
		{
		// In the restart scenario we want StartSafe to make 'NoOfRetries' attempts
		// rather than '1 + NoOfRetries' which it will otherwise do.
		restoreRetries = ETrue;
		iStartupProperties->SetNoOfRetries(--propRetries);	
		}
	
	TInt retried = 0;
	
	// Attempt restart/s. Do not allow a leave until NoOfRetries has been restored.
	TRAPD( err, startSafe->StartL(*iStartupProperties, iProcess, retried) );
	
	if(restoreRetries)
		{
		iStartupProperties->SetNoOfRetries(++propRetries);	
		}
		
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(startSafe);	
	DEBUGPRINT3(_L("SysMonMonitor: %S restarted, new iProcessId=%u. Logon to monitor again"), &fileName, iProcess.Id().Id());
	
#ifdef _DEBUG
    TInt restartExeCount = 0;
    // The argument passed to the process is converted into an integer and is used as the key to set the RProperty.
    // This way each process' restart count is stored in a unique key.
    TPtrC processArgs = iStartupProperties->Args();
    TLex processArgsToInt(processArgs);
    err = processArgsToInt.Val(restartExeCount);
    if( KErrNone == err )
        {
        err = RProperty::Set(RProcess().SecureId(), restartExeCount, ++iRestartCount);
        DEBUGPRINT4(_L("SysMonMonitor: Setting Test Property with key %d to %d completed with error %d"), restartExeCount, iRestartCount, err);
        }
#endif // _DEBUG
    
	iProcessId = iProcess.Id();
	iReLaunchAttempts++;	// Increment after each re-launch attempt.
	if (!iReLaunchIntervalTimer->IsActive())
		{
		DEBUGPRINT3(_L("SysMonMonitor: ReLaunch Interval Timer is started for %S with processId %u"), &fileName, iProcessId.Id());
		iReLaunchIntervalTimer->Start();
		}
	DEBUGPRINT3(_L("SysMonMonitor: ReLaunch Attempts for %S is %d"), &fileName, iReLaunchAttempts);
	iLogonBackoffTimer->ProcessLogon();
	}


/**
 This function is inherited from MLogonCallback and is called from CLogonBackoffTimer
*/
TInt CMonitor::DoProcessLogon()
	{

	iProcess.Logon( iStatus );
	return ( (iStatus == KRequestPending) ? KErrNone : iStatus.Int() );
	}



/**
 This function is inherited from MLogonCallback and is called from CLogonBackoffTimer
*/
void CMonitor::ActivateSelf()
	{
	
	iLoadTime.UniversalTime();
	SetActive();		
	}
	
TInt CMonitor::Callback(TAny* aParent)
	{		
	CMonitor* monitor = reinterpret_cast<CMonitor*> (aParent);
	DEBUGPRINT2(_L("SysMonMonitor: Finished waiting for throttle time, try to restart failed processId=%u"), monitor->iProcessId.Id());

 	 if (!monitor->HasExceededRateOfFailurePolicy())
		{
		TRAPD(err, monitor->RestartProcessL());
		if (err != KErrNone)
			{
			// process failed to be started, cancel monitoring of this process
			DEBUGPRINT2(_L("SysMonMonitor::RestartProcessL failed with err=%d, cancelling"), err);
			monitor->CancelMonitor();
			}
		}
	return KErrNone;
	}
	
	
/*
CMonitor::RunL() gets called when a monitor process terminates.
@panic ERestartSystemCallFailed if the RestartSystem call fails
@panic ERestartSystemCallWithMode if the RestartSystem_with_mode call fails
@panic EInvalidStartMethod if the Start mode is invalid
*/
void CMonitor::RunL()
	{
	DEBUGPRINT1(_L("SysMonMonitor: CMonitor::RunL() called"));
	
	iProcess.Close();	// closing the current handle
	
	TRecoveryMethod recoveryMethod = iStartupProperties->RecoveryMethod();
	if (recoveryMethod == ECriticalNoRetries)
		{
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		// If the retry_failure_recovery_method is set to ECriticalNoRetries, then SysMon will restart
		// the OS. It will not try to restart the process even if iStartupProperties->NoOfRetries() 
		// is greater than zero.
		RestartSysDll(EFalse);
#else
		TInt err = RestartSys::RestartSystem() ;	// restart the system

		if (KErrNone != err)
			{
			DEBUGPRINT2(_L("Shma: RestartSystem error %d"), err);
			PanicNow(KPanicMonitor, ERestartSystemCallFailed);
			}
		User::After(KDelayRequiredForRestartSys);
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		}
	else
		{
		if (iStartupProperties->NoOfRetries() == 0)
			{
			// If NoOfRetries() == 0, execute recovery method immediately.
			FailureRecoveryPolicy();
			}
		else
			{
			TTime curTime;
			curTime.UniversalTime(); // current time, can be considered as the time of termination for the process
			
			TTime thresholdTime = (iLoadTime + TTimeIntervalMicroSeconds32(KWaitTime)); // time, till when no restart should take place
			
			if (curTime < thresholdTime)
				{	
				// Implies process terminated less than KWaitTime since the last launch of the process
				// So to reduce Denial of Service we wait the remaining time of KWaitTime.
	#ifdef _DEBUG
				TPtrC fileName = iStartupProperties->FileName();
				DEBUGPRINT2(_L("SysMonMonitor: Wait for throttle time before restarting process %S"), &(fileName));
	#endif
				iSysMonServer.TimerListL().AddL(thresholdTime, TCallBack(Callback, this));
				}
			else
				{
	  			 if (!HasExceededRateOfFailurePolicy())
					RestartProcessL();
				}
			}
		}
	}


void CMonitor::FailureRecoveryPolicy()
	{
	TRecoveryMethod recoveryMethod = iStartupProperties->RecoveryMethod();
	DEBUGPRINT2(_L("SysMonMonitor: Process failed RecoveryMethod=%d"), recoveryMethod);
	if (recoveryMethod == ERestartOS)
		{
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		RestartSysDll(EFalse);
#else
		TInt err = RestartSys::RestartSystem() ;	// restart the system

		if (KErrNone != err)
			{
			DEBUGPRINT2(_L("Shma: RestartSystem error %d"), err);
			PanicNow(KPanicMonitor, ERestartSystemCallFailed);
			}

		User::After(KDelayRequiredForRestartSys);
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		}			
	else if (recoveryMethod == ERestartOSWithMode)
		{
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		RestartSysDll(ETrue, iStartupProperties->RestartMode());	// restart system in a mode
#else
		TInt err = RestartSys::RestartSystem(iStartupProperties->RestartMode()) ;	// restart system in a mode

		if (KErrNone != err)
			{
			DEBUGPRINT2(_L("Shma: RestartSystem with mode error %d"), err);
			PanicNow(KPanicMonitor, ERestartSystemCallWithMode);
			}

		User::After(KDelayRequiredForRestartSys);
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		}
	else
		{
		// ignore on failure, cancel monitoring of this process
		CancelMonitor();
		}
	}

TBool CMonitor::HasExceededRateOfFailurePolicy()
	{
	if (iReLaunchAttempts > iStartupProperties->NoOfRetries())
		{
		// If the component's failure rate exceeds that set for the component, no new re-launch attempts will be 
		// performed on the component and failure recovery policy will be enacted.
		DEBUGPRINT1(_L("SysMonMonitor: Failure Recovery Policy is enacted as component's failure rate exceeded that set for the component"));
		FailureRecoveryPolicy();
		return ETrue;
		}
	else
		return EFalse;
	}

TInt CMonitor::RunError(TInt aError)
	{
	DEBUGPRINT2(_L("SysMonMonitor: RunError called with error=%d, cancelling"), aError);
	// process failed to be started, cancel monitoring of this process
	CancelMonitor();
	
	// we are returning KErrNone, as the error returned by RunL() has been handled by cancelling the monitor.
	aError = KErrNone;
	return aError;
	}

void CMonitor::CancelMonitor()
	{
	DEBUGPRINT2(_L("SysMonMonitor: CMonitor cancelling monitor with iProcessId=%u"), iProcessId.Id());
	iSysMonServer.CancelMonitor(iProcessId);
	}

void CMonitor::DecrementRelaunchAttempts()
	{
	 // Decrement the re-launch attempts at regular interval(interval defined by KIntervalForReLaunchRateOfFailure)
	TInt relaunchAttempts = (iReLaunchAttempts) ? (--iReLaunchAttempts): 0;
	TPtrC fileName = iStartupProperties->FileName();
	DEBUGPRINT3(_L("SysMonMonitor: Decremented ReLaunch Attempts for %S to %d"), &fileName, relaunchAttempts);

	if (!relaunchAttempts)
		{
		DEBUGPRINT2(_L("SysMonMonitor: Cancelled ReLaunch Interval Timer with processId=%u"), iProcessId.Id());
		iReLaunchIntervalTimer->Cancel();
		}
	else
		{
		// Start the relaunch interval timer again if the relaunch attempts is greater than 0.
		iReLaunchIntervalTimer->Start();
		}
	}

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
void CMonitor::RestartSysDll(TBool aUseStartupMode, TInt aStartupMode)
	{
	TInt err = KErrNone;
	if (!iRestartSysLoaded)
		{
		err = iRestartSysLib.Load(KCustomisedRestartSysProxyDLL);
		// if customised restartsys dll is not provided, we try to load the default
		// restartsys dll
		if(KErrNotFound == err)
			{
			DEBUGPRINT1(_L("SysMonMonitor: Loading default restartsys.dll as custrestartsys.dll is not found"));
			err = iRestartSysLib.Load(KRestartSysProxyDLL);
			}
		if(KErrNone != err)
	 		{
			// restartsys dll (customised or defaut) should be present for ERestartOS and ERestartOSWithMode 
			// recovery policies we panic here if it is not present
			DEBUGPRINT2(_L("CustRestartSys/ RestartSys failed to load - Error Code= %d"), err);
	 		PanicNow(KPanicMonitor, ERestartSysLibNotPresent);
	 		}
		iRestartSysLoaded = ETrue;
		}

	if (aUseStartupMode)
		{
		TFuncRestartWithModeL restartSys = reinterpret_cast<TFuncRestartWithModeL>(iRestartSysLib.Lookup(1));
		if (restartSys == NULL)
			{
			DEBUGPRINT1(_L("CStartSafe: Ordinal Lookup Error\n Expected Function prototype: RestartSystem(TInt aStartupMode)"));
			User::Leave(KErrBadLibraryEntryPoint);
			}
		err = restartSys(aStartupMode);	// restart the system with the given startup mode
		}
	else
		{
		TFuncRestartL restartSys = reinterpret_cast<TFuncRestartL>(iRestartSysLib.Lookup(2));
		if (restartSys == NULL)
			{
			DEBUGPRINT1(_L("CStartSafe: Ordinal Lookup Error\n Expected Function prototype: RestartSystem()"));
			User::Leave(KErrBadLibraryEntryPoint);
			}
		err = restartSys();	// restart the system
		}

	if (KErrNone != err)
		{
		DEBUGPRINT2(_L("Shma: RestartSystem error %d"), err);
		if (aStartupMode == 0)
			{
			PanicNow(KPanicMonitor, ERestartSystemCallFailed);
			}
		else
			{
			PanicNow(KPanicMonitor, ERestartSystemCallWithMode);
			}
		}

	User::After(KDelayRequiredForRestartSys);
	}
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN

CRelaunchIntervalTimer* CRelaunchIntervalTimer::NewL(CMonitor& aMonitor)
	{
	CRelaunchIntervalTimer* self=new (ELeave) CRelaunchIntervalTimer(aMonitor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRelaunchIntervalTimer::~CRelaunchIntervalTimer()
	{
	Cancel();
	}

CRelaunchIntervalTimer::CRelaunchIntervalTimer(CMonitor& aMonitor)
		: CTimer(EPriorityStandard), iMonitor(&aMonitor)
	{
	}

void CRelaunchIntervalTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CRelaunchIntervalTimer::Start()
	{
	DEBUGPRINT1(_L("SysMonMonitor: Start the relaunch interval timer"));
	TTime curTime;
	curTime.UniversalTime();
	
	TInt intervalForReLaunchRateOfFailure = KIntervalForReLaunchRateOfFailure;
#ifdef __WINS__
// KIntervalForReLaunchRateOfFailure is a Rom patchable constant, so need an emulator equivalent
// if WINS then read value from epoc.ini requires licencees to set property in epoc.ini. This value is taking in secs.
// Usage: In epoc.ini patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25

	TInt valueOfKIntervalForReLaunchRateOfFailure = 0;
	if (UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure",&valueOfKIntervalForReLaunchRateOfFailure) == KErrNone)
		{
		intervalForReLaunchRateOfFailure = valueOfKIntervalForReLaunchRateOfFailure;
		}
#endif
	
	const TTime reLaunchIntervalTime = (curTime + TTimeIntervalSeconds(intervalForReLaunchRateOfFailure));
	AtUTC(reLaunchIntervalTime);
	}

void CRelaunchIntervalTimer::RunL()
	{
	iMonitor->DecrementRelaunchAttempts();
	}

