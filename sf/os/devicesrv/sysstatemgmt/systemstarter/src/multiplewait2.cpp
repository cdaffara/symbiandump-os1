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

#include "multiplewait2.h"
#include "appstarter2.h"
#include "sysmonclisess.h"
#include "restartsys.h"

#include "SysStartDebug.h"
#include "sysstartpanic.h"


// 
// Standard Symbian factory functions 
// 

CMultipleWait* CMultipleWait::NewL(CommandList& aDeferredList, CStartupUtilProvider& aProvider)
	{
	CMultipleWait* self = NewLC(aDeferredList, aProvider);
	CleanupStack::Pop(self);
	return self;
	}

CMultipleWait* CMultipleWait::NewLC(CommandList& aDeferredList, CStartupUtilProvider& aProvider)
	{
	CMultipleWait* self = new (ELeave) CMultipleWait(aProvider);	
	CleanupStack::PushL(self);	 
	self->ConstructL(aDeferredList);
	return self;
	}

CMultipleWait::CMultipleWait(CStartupUtilProvider& aProvider)
	:iProvider(aProvider)
	{
	}
	
void CMultipleWait::ConstructL(CommandList& aDeferredList)
	{
	TInt size = aDeferredList.Count();

	TInt i=0;
	for (; i<size; i++)
		{
		iDeferredList.AppendL(aDeferredList[i]);
		}
	}

CMultipleWait::~CMultipleWait()
	{ 
  	iDeferredList.Reset();
	}

// 
// Public member functions
//
void CMultipleWait::SetFailOnError(TBool aFail)
	{
	iFailOnError = aFail;
	}

void CMultipleWait::SetTimeout(TInt aTimeout)
	{
	iTimeout = aTimeout;
	}
	
/** The timeout value indicates when to kill a command 
that's taking too long to initialize.
A value of zero means "do not time this command out,
wait indefinitely".
*/
TInt CMultipleWait::Timeout() const
	{
	return iTimeout;
	}

/** Whether startup should fail or continue if a command 
doesn't succeed
*/
TBool CMultipleWait::FailOnError() const
	{
	return iFailOnError;
	}



/** Implementation of MStartupCommand interface.

@see MStartupCommand.
*/
void CMultipleWait::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
 	 	
  	// Start timer going if required. A timer value of 0 means that no timer
  	// will be started and we simply wait until all commands have completed.
  	TRequestStatus timerStatus = KRequestPending; 
  	RTimer timer;
  	timer.CreateLocal();
    if (iTimeout != 0)
  		{
		timer.After(timerStatus, TTimeIntervalMicroSeconds32(iTimeout*1000));
		}

	// The MULTIPLE_WAIT command in the SSC can specify either
	// fail_on_error == EIgnoreCommandFailure(0) - if a command fails just 
	// carry on with the next one.
	// fail_on_error == EPanicOnCommandFailure(1) - Restart system as soon as a command fails
	// or the timer expires.
 
	if (iFailOnError == EPanicOnCommandFailure)
	 	{
	 	CheckCommandsForRestart(timerStatus);
	 	}
	else // iFailOnError == EIgnoreCommandFailure
		{
		CheckCommandsAndIgnore(timerStatus);
		}

  
 	// Cancel the timer if it has been started and is still running. 			
 	if ((iTimeout != 0) && (timerStatus == KRequestPending))
  		{  	
 		timer.Cancel();
		User::WaitForRequest(timerStatus);  
		}
	timer.Close();  
 
	// signal KErrNone to caller because all process fail have be ignored by now or the system restarted
	TRequestStatus* requestStatus = &aStatus;
	User::RequestComplete(requestStatus, KErrNone);
	}


/** This is the helper function to check all the deferred commands when fail_on_error is EIgnoreCommandFailure.
    In this case, if the process is finished before RTimer expire (indicated by aTimerStatus), 
    the process can be added to System Monitor if it was started sucessfully.
    If the process was not started succfully, we ignore the process failure.
    
    @param aTimerStatus This is the TReauestStatus of RTimer that this MULTIPLE_WAIT command is waiting on
*/
void CMultipleWait::CheckCommandsAndIgnore(TRequestStatus& aTimerStatus)
	{
	TBool timerExpired = EFalse;
	
	// Each deferred command will have been invoked and stored in a deferred 
 	// list. Need to process each command object in the list checking if each has
 	// been initialized i.e. rendezvoused.
 	TInt i = 0;
	for (; i < iDeferredList.Count(); i++)
		{
		// Get the next command 
 		CAppStarter* commandPtr = static_cast<CAppStarter*>(iDeferredList [i]);  
  	 		
#ifdef _DEBUG  	 		
 		TParsePtrC parser(commandPtr->AppInfo().FileName());
		TPtrC commandName = parser.Name();
		DEBUGPRINT2(_L("SysStart: checking status of deferred command %S"), &commandName);
#endif			
			
		// Some deferred commands may have failed in creation. So don't wait for
 		// them to rendezvous.
		if (commandPtr->Process().Handle() > 0)
			{
			
 			// Once a timer expires all waiting for commands stops.
 			// The status of any commands not yet processed in the list is checked.
 			// Any that have not yet completed successfully are killed. 
 			if (!timerExpired)
 				{
 				TRequestStatus& commandStatus = commandPtr->CommandStatus();
 				User::WaitForRequest(aTimerStatus, commandStatus);
 				if (aTimerStatus != KRequestPending) 
 					{
 					timerExpired = ETrue;
 					if (commandStatus != KErrNone)
						{	 
						// Process timed out before reaching a rendezvous point. Kill
						// the process and set the timerExpired variable so that no 
						// other commands are waited for.
						DEBUGPRINT3(_L("SysStart: status of deferred command %S, is %d "), &commandName, commandStatus.Int());
						commandPtr->Process().Kill(KErrTimedOut);
						User::WaitForRequest(commandStatus);
						}
					else 
						{
						User::WaitForRequest(commandStatus);
						DEBUGPRINT2(_L("SysStart: successful rendezvous of deferred command %S"), &commandName);		
						TRAPD(err, RegisterMonitoringL(*commandPtr));
						// if monitoring failed, kill the process because start and monitor is expected to succeed or fail as one action
						if (err != KErrNone)
							{
							DEBUGPRINT3(_L("SysStart: %S failed to register for monitor, err is %d "), &commandName, err);
							commandPtr->Process().Kill(err);
							}
						}
 					}
				}
			else // timer has expired, just do clean up.
				{
				// If the command has not already rendezvoused successfully then 
				// kill the process	
				TRequestStatus& commandStatus = commandPtr->CommandStatus();
				if (commandStatus != KErrNone)  
					{	
					DEBUGPRINT3(_L("SysStart: status of deferred command %S, is %d "), &commandName, commandStatus.Int());	
					commandPtr->Process().Kill(KErrTimedOut);
					User::WaitForRequest(commandStatus);
					}
				else
					{
					User::WaitForRequest(commandStatus);
					DEBUGPRINT2(_L("SysStart: successful rendezvous of deferred command %S"), &commandName);		
					TRAPD(err, RegisterMonitoringL(*commandPtr));
					// if monitoring failed, kill the process because start and monitor is expected to succeed or fail as one action
					if (err != KErrNone)
						{
						DEBUGPRINT3(_L("SysStart: %S failed to register for monitor, err is %d "), &commandName, err);
						commandPtr->Process().Kill(err);
						}
					}
				}
			}
		else
			{
			DEBUGPRINT2(_L("SysStart: command %S not created so no rendezvous check"), &commandName);		
			}
 		 }
	}

/** This is the helper function to check all the deferred commands when fail_on_error is EPanicOnCommandFailure.
    In this case, if the process failed for any reason (either before or after RTime expire), the system
    will be restarted base on the process/application starting command.
    Otherwise, the process will be added to System Monitor for monitoring if required.
    
    @param aTimerStatus This is the TReauestStatus of RTimer that this MULTIPLE_WAIT command is waiting on
*/
void CMultipleWait::CheckCommandsForRestart(TRequestStatus& aTimerStatus)
	{
 	// Each deferred command will have been invoked and stored in a deferred 
 	// list. Need to process each command in the list checking if each has
 	// been initialized i.e. rendezvoused.
 	TInt i = 0;
	for (; i < iDeferredList.Count(); i++)
		{
		// Get the next command 
 		CAppStarter* commandPtr = static_cast<CAppStarter*>(iDeferredList[i]); 
 		
#ifdef _DEBUG  	 		
 		TParsePtrC parser(commandPtr->AppInfo().FileName());
		TPtrC commandName = parser.Name();
		DEBUGPRINT2(_L("SysStart: checking status of deferred command %S"), &commandName);
#endif			
				
 		if (commandPtr->Process().Handle() > 0)
 			{			
 			// Wait for either the command to complete the rendezvous or the timer
 			// to expire. (If no timer has been started this code will just wait for the 
 			// command to complete).
 			TRequestStatus& commandStatus = commandPtr->CommandStatus();
 			User::WaitForRequest(aTimerStatus, commandStatus);
 			
 			if ((aTimerStatus != KRequestPending) && (commandStatus != KErrNone))
				{ 
				// Command has timed out. fail_on_error is set to EPanicOnCommandFailure
				// so restart system immediately.
				DEBUGPRINT3(_L("SysStart: status of deferred command %S, is %d "), &commandName, commandStatus.Int());		
  	 			RestartSystem(commandPtr->AppInfo());
				}
			else
				{
			 	// Command has completed. Check that the command has completed successfully. 
				// If there is an error restart system immediately.
				if (commandStatus != KErrNone)
					{
					DEBUGPRINT3(_L("SysStart: status of deferred command %S, is %d "), &commandName, commandStatus.Int());		
	  	 			RestartSystem(commandPtr->AppInfo());
					}
				else
					{
					DEBUGPRINT2(_L("SysStart: successful rendezvous of deferred command %S "), &commandName);
					TRAPD(err, RegisterMonitoringL(*commandPtr));

					// if monitoring failed, restart system because start and monitor is expected to succeed or fail as one action
					if (err != KErrNone)
						{
						DEBUGPRINT3(_L("SysStart: %S failed to register for monitor, err is %d "), &commandName, err);
						RestartSystem(commandPtr->AppInfo());
						}
					}
				}
 			}
 		else
 			{
 			// the command already failed in creation, just restart system
 			DEBUGPRINT2(_L("SysStart: command %S not created so no rendezvous check"), &commandName);
 			RestartSystem(commandPtr->AppInfo());
 			}

		}
	}


/** Helper function to restart system according to the process/application start command properties
	@param aStartupProperties The startup properties for restarting system
*/
void CMultipleWait::RestartSystem(const CStartupProperties& aStartupProperties)
	{
	TInt err = KErrNone;
	if (aStartupProperties.RecoveryMethod() == ERestartOSWithMode)
		{
		err = RestartSys::RestartSystem(aStartupProperties.RestartMode());
		}
	else
		{
		err = RestartSys::RestartSystem();
		}

	if (KErrNone != err)
		{
 		DEBUGPRINT2(_L("SysStart: RestartSystem error = %d"), err);
		PanicNow(KPanicMultipleWait, ERestartSystemCallFailed);
		}

	User::After(5000000); // required by RestartSys API, see comments in RestartSys::RestartSystem()
	}

/** Helper function to register for monitoring after a process has been started successfully
	@param aStartCommand The command that has been started successfully
*/
void CMultipleWait::RegisterMonitoringL(CAppStarter& aStartCommand)
	{
	if (aStartCommand.AppInfo().Monitored())
		{
		// When a deferred wait for start process needs to be monitord, the start method needs to 
		// be changed to EFireAndForget because it cannot be deferred when it die after being monitored
		aStartCommand.AppInfo().SetStartMethod(EFireAndForget);
		
		iProvider.SysMonSessionL().MonitorL(aStartCommand.AppInfo(), aStartCommand.Process());
		}
	}

void CMultipleWait::Release() 
	{
	delete this;
	}
