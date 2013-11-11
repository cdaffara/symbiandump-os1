// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
			  
#include "StartupState.h"
#include <domainmanager.h>
#include <startup.hrh>
#include "StartupStateInfo.h"
#include "StartupCommand.h"
#include "startupdomaindefs.h"

#include "SysStartDebug.h"

#include "sysstartpanic.h"
#include "restartsys.h"

//
// Standard Symbian factory functions
//

CStartupState* CStartupState::NewL(const MStartupStateInfo& aInfo)
	{
	CStartupState* self = CStartupState::NewLC(aInfo);
	CleanupStack::Pop(self);
	return self;
	}

CStartupState* CStartupState::NewLC(const MStartupStateInfo& aInfo)
	{
	CStartupState* self = new (ELeave) CStartupState(aInfo);
	CleanupStack::PushL(self);
	return self;
	}

//
// Public functions
//

void CStartupState::Start()
	{
	TPtrC stateName = iStateInfo.Name();

 	// Connect to the domain manager in order to notify state transition
	RDmDomainManager notifier;
	TInt r = notifier.Connect(KDmHierarchyIdStartup);
	if (r == KErrNone)
		{
		//TRequestStatus is used in the while-loop and the loop should be run for the first time, 
		//so its intial value is set to KErrGeneral.. 
		TRequestStatus stateNotifyStatus = KErrGeneral;
 	
 		// Find out from the value provisioned in the startup configuration file (SSC)
 		// how many times to re-attempt state change following a state transition failure.	
 		TInt attemptsRequired = iStateInfo.NoOfRetries() + 1;  
 	
   		while ((stateNotifyStatus != KErrNone) && (attemptsRequired-- >0))
   			{	 	
 			// Request a state change from the domain manager
 			DEBUGPRINT2(_L("SysStart: Send state transition request (move to %S) to domain manager"), &stateName);
			notifier.RequestSystemTransition(iStateInfo.StateId(),
										 ETraverseParentsFirst, stateNotifyStatus);
 	 
 			// Wait for result of the state change
			User::WaitForRequest(stateNotifyStatus);
			DEBUGPRINT2(_L("SysStart: Result of state transition request is %d"), stateNotifyStatus.Int() );	 
   			}  
	  
 		// If state transition has failed after the specified no of retries,
 		// take action as specified in the SSC
		if (stateNotifyStatus != KErrNone)
			{
			// Value in configuration file can be set to panic or ignore.
			if (iStateInfo.ActionOnStateTransitionFailure() == EPanicOnFailure)
				{				
				DEBUGPRINT2(_L("SysStart: State transition to %S failed"), &stateName);	 	

				// RestartSys will be used to trigger system restart.
				TInt ret = RestartSys::RestartSystem();
				if (ret != KErrNone)
					{
					DEBUGPRINT2(_L("SysStart: RestartSystem failed with error %d"), ret);
					PanicNow(KPanicStartupState, ERestartSystemCallFailed);
					}
				
				User::After(5000000); // required by RestartSys API, see comments in RestartSys::RestartSystem()
				}
			else 
				{
				DEBUGPRINT2(_L("SysStart: State transition to %S failed. Ignoring as specified in the SSC"), &stateName);
				}	 
			}
	 	 
		notifier.Close();
		}
	else // Domain manager connection failed.
		{
		DEBUGPRINT2(_L("SysStart: Domain manager connection failed with error %d"), r);
		PanicNow(KPanicStartupState, EDomainManagerConnectionFailure);
		}

	// Execute the commands relevant to this state
	for (TInt i = 0; i < iStateInfo.Count(); ++i)
		{
		DoCommand(*(iStateInfo.GetCommand(i)));
		}
	}

//
// Private functions
//

CStartupState::CStartupState(const MStartupStateInfo& aInfo) :
	iStateInfo(aInfo)
	{
	}

void CStartupState::DoCommand(MStartupCommand& aCommand)
	{
	TRequestStatus status;
	aCommand.Execute(status);
	User::WaitForRequest(status);
	
	if (status!=KErrNone)
		{
		DEBUGPRINT2(_L("SSC command failed with error %d"), status.Int());
		}			
	}
