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

#include "monitor.h"
#include "startsafe.h"
#include "restartsys.h"
#include "sysmonserver.h"
#include "sysmoncliserv.h"
#include "timerlist.h"
#include "sysmon_patch.h"
#include <startupproperties.h>

#include "SysStartDebug.h"
#include "sysstartpanic.h"

const TInt CMonitor::iOffset = _FOFF(CMonitor, iSglQueLink);

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
	
	// Can't leave after taking ownership of aStartupProperties 
	iStartupProperties = aStartupProperties;
	iLoadTime.UniversalTime();		
	}


CMonitor::~CMonitor()
	{
	delete iLogonBackoffTimer;
	
	Cancel();
	delete iStartupProperties;
	iProcess.Close();	
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
	DEBUGPRINT3(_L("SysMonMonitor: Going to restart %S, old process id=%d"), &fileName, iProcessId.Id());
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	__ASSERT_DEBUG(iStartupProperties->StartMethod() == EWaitForStart, PanicNow(KPanicMonitor, EInvalidStartMethod));
	iStartupProperties->SetStartMethod(EWaitForStart);
	
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
	DEBUGPRINT3(_L("SysMonMonitor: %S restarted, new iProcessId=%d. Logon to monitor again"), &fileName, iProcess.Id().Id());
	
	iProcessId = iProcess.Id();
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
	DEBUGPRINT2(_L("SysMonMonitor: Finished waiting for throttle time, try to restart failed processId=%d"), monitor->iProcessId.Id());

	TRAPD(err, monitor->RestartProcessL());

	if (err != KErrNone)
		{
		// process failed to be started, cancel monitoring of this process
		DEBUGPRINT2(_L("SysMonMonitor::RestartProcessL failed with err=%d, cancelling"), err);
		monitor->CancelMonitor();
		}

	return KErrNone;
	}
	
	
/*
CMonitor::RunL() gets called when a monitor process terminates.
*/
void CMonitor::RunL()
	{
	DEBUGPRINT1(_L("SysMonMonitor: CMonitor::RunL() called"));
	
	iProcess.Close();	// closing the current handle
	
	if (iStartupProperties->NoOfRetries() == 0 ||
		iStartupProperties->StartMethod() == EFireAndForget)
		{
		// 1. If NoOfRetries() == 0, execute recovery method immediately.
		// 2. If StartMethod() == EFireAndForget, the retry value is ignored during monitoring because 
		//	  restarting the process can result in a forever loop that make SysMon keep restarting the 
		//	  process. This is because in EFireAndForget, we don't need to check the successful start 
		//	  of the process, so as soon as the process fail again, this monitoring function would get 
		//	  called again. So for EFireAndForget we execute the recovery action if a process failed.
		TRecoveryMethod recoveryMethod = iStartupProperties->RecoveryMethod();
		TInt err = KErrNone;
		DEBUGPRINT2(_L("SysMonMonitor: Process failed RecoveryMethod=%d"), recoveryMethod);
		if (recoveryMethod == ERestartOS)
			{
			err = RestartSys::RestartSystem() ;	// restart the system

			if (KErrNone != err)
				{
				DEBUGPRINT2(_L("Sysstart: RestartSystem error %d"), err);
				PanicNow(KPanicMonitor, ERestartSystemCallFailed);
				}

			User::After(5000000); // required by RestartSys API, see comments in RestartSys::RestartSystem()
			}			
		else if (recoveryMethod == ERestartOSWithMode)
			{
			err = RestartSys::RestartSystem(iStartupProperties->RestartMode()) ;	// restart system in a mode

			if (KErrNone != err)
				{
				DEBUGPRINT2(_L("Sysstart: RestartSystem with mode error %d"), err);
				PanicNow(KPanicMonitor, ERestartSystemCallWithMode);
				}

			User::After(5000000); // required by RestartSys API, see comments in RestartSys::RestartSystem()
			}
		else
			{
			// ignore on failure, cancel monitoring of this process
			CancelMonitor();
			}
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
			RestartProcessL();
			}
		}
	}


#ifdef _DEBUG
TInt CMonitor::RunError(TInt aError)
#else
TInt CMonitor::RunError(TInt /*aError*/)
#endif
	{
	DEBUGPRINT2(_L("SysMonMonitor: RunError called with error=%d, cancelling"), aError);
	// process failed to be started, cancel monitoring of this process
	CancelMonitor();
	
	return KErrNone;
	}


void CMonitor::CancelMonitor()
	{
	DEBUGPRINT2(_L("SysMonMonitor: CMonitor cancelling monitor with iProcessId=%d"), iProcessId.Id());
	iSysMonServer.CancelMonitor(iProcessId);
	}
