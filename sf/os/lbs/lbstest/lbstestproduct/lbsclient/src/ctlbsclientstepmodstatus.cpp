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
// @file ctlbsclientstepmodstatus.cpp
// This is the class implementation for the Module Information Tests
// 
//
 
#include "ctlbsclientstepmodstatus.h"
#include "ctlbsclientstep.h"

#include <e32property.h>
#include <lbs.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

#include <lbs/EPos_CPosModules.h>
#include <lbs/EPos_CPosModuleUpdate.h>
#include "netpsy.hrh"

/**
 * Destructor
 */
CT_LbsClientStep_ModStat::~CT_LbsClientStep_ModStat()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_ModStat::CT_LbsClientStep_ModStat(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_ModStatus);
	}


/**
Static Constructor
*/
CT_LbsClientStep_ModStat* CT_LbsClientStep_ModStat::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
    CT_LbsClientStep_ModStat* testStep =  new CT_LbsClientStep_ModStat(aParent);
    
    if (testStep)
        {
        TInt err = KErrNone;
    
        TRAP(err, testStep->ConstructL());
        if (err)
            {
            delete testStep;
            testStep = NULL;
            }
        }
    return testStep;
	}

void CT_LbsClientStep_ModStat::ConstructL()
    {
    // Re-enable the network PSY
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    
    moduleUpdate->SetUpdateAvailability(ETrue);
    moduleUpdate->SetUpdateVisibility(ETrue);
    TUid uid = TUid::Uid(KPosNETPSYImplUid);
    db->UpdateModuleL(uid, *moduleUpdate);
    
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    }

/**
 *	Sets requested event mask as specified for supplied module. 
 *  Kicks test module to cause events requested then waits for event(s)
 *  Updates the supplied module status 
 *  returns TRUE if everything went as expected, FALSE otherwise
 */
 /**
 *	Sets requested event mask as specified for supplied module. 
 *  Kicks test module to cause events requested then waits for event(s)
 *  Updates the supplied module status 
 *  returns TRUE if everything went as expected, FALSE otherwise
 */
 TBool CT_LbsClientStep_ModStat::DoRequestAndVerifyModStatusEventsL(TPositionModuleStatusEvent::TModuleEvent aReqEventsMask, TPositionModuleStatus& aModStatus, TPositionModuleId aModId = KPositionNullModuleId )
	{
	TBool result = TRUE;
	TPositionModuleStatusEvent statusEvent;
	TPositionModuleId agpsmodId;
								
	// we're interested in the agps module id
	T_LbsUtils utils;
	agpsmodId = utils.GetAGpsModuleIdL(iServer);
	
	//  Set event mask to select requested status events								
	statusEvent.SetRequestedEvents(aReqEventsMask);
	
	// verify the set worked
	if(statusEvent.RequestedEvents() != aReqEventsMask)
		{
		INFO_PRINTF2(_L("Requested Events don't match. Got0x%x"), statusEvent.RequestedEvents());
		return FALSE;
		}
	
	CT_LbsAsyncWaiter* waiter = CT_LbsAsyncWaiter::NewL();
	
	// Request module status event using supplied module id
	iServer.NotifyModuleStatusEvent(statusEvent, waiter->iStatus, aModId);
	
	TPositionModuleStatusEvent::TModuleEvent eventsExpected = 0;
	
	// assume we are interested in the agps module since we can't control events from network module
	ASSERT(aModId == agpsmodId || aModId == KPositionNullModuleId);
	
	// kick the test agps module to cause the requested status event(s)
	TPckgBuf<TModuleDataIn> modDataInBuf;
	TModuleDataIn& modDataIn = modDataInBuf();
	
	modDataIn.iRequestType = TModuleDataIn::EModuleRequestEventKickOff; 
		modDataIn.iKickOffEventType = 0;
	if(aReqEventsMask & TPositionModuleStatusEvent::EEventDeviceStatus)
		{
		modDataIn.iKickOffEventType |= TModuleDataIn::EKickOffEventTypeDevice;
		eventsExpected |= TPositionModuleStatusEvent::EEventDeviceStatus;
		}

	if(aReqEventsMask & TPositionModuleStatusEvent::EEventDataQualityStatus)
		{
		modDataIn.iKickOffEventType |= TModuleDataIn::EKickOffEventTypeDataQuality;
		eventsExpected |= TPositionModuleStatusEvent::EEventDataQualityStatus;
		}
	
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleIn, modDataInBuf));


	// Wait for event(s)
	while(result && eventsExpected)
		{
		waiter->StartAndWait();
		User::LeaveIfError(waiter->Result());


		TInt result = waiter->Result();
		
		TPositionModuleStatusEvent::TModuleEvent eventsOccurred = statusEvent.OccurredEvents();
		
		ASSERT(eventsOccurred);
		
		if(result)
			{
			INFO_PRINTF2(_L("Unexpected error returned %d"), result);
			result = FALSE;
			}
		// Verify correct event(s) happenned			
		else 
			{
				
			if(eventsOccurred & ~(eventsExpected))
				{
				INFO_PRINTF2(_L("Unexpected event received 0x%x"), statusEvent.OccurredEvents());
				result = FALSE;						
				}
			
			if(statusEvent.ModuleId() == agpsmodId)
				{
				eventsExpected &= ~eventsOccurred;	
				}
				
			// Request next module status event
			if (eventsExpected)
				{
				iServer.NotifyModuleStatusEvent(statusEvent, waiter->iStatus/*request*/, aModId);
				}
			}	
		}
		
	// TODO - appropriate to do this here?
	// device status event happened. Read in new status (this will be verified in future test step):
	statusEvent.GetModuleStatus(aModStatus);
	
	delete waiter;
	
	return result;
	}

/**
 *	Sets requested event mask as specified for agps module. 
 *  Kicks test module to cause events requested then waits for event(s)
 *  Updates the supplied module status 
 *  returns TRUE if everything went as expected, FALSE otherwise
 */
 TBool CT_LbsClientStep_ModStat::DoRequestAndVerifyAGPSModStatusEventsL(TPositionModuleStatusEvent::TModuleEvent aReqEventsMask, TPositionModuleStatus& aModStatus)
 	{
	TPositionModuleId modId;
								
	// we're interested in the agps module id
	T_LbsUtils utils;
	modId = utils.GetAGpsModuleIdL(iServer);
					
	return DoRequestAndVerifyModStatusEventsL(aReqEventsMask, aModStatus, modId);
 	}
 	
 	
/**
 * Sets requested event mask as specified for supplied module and asks to be notified of events
 * (note does not kick test module to cause events; use only if expect error to be returned)
 * Waits for event and returns error
 */
TInt CT_LbsClientStep_ModStat::DoRequestModStatusEventsWithError(TPositionModuleStatusEvent::TModuleEvent aReqEventsMask, TPositionModuleId aModId)
	{
	//TRequestStatus request;					
	TPositionModuleStatusEvent statusEvent;
	
	statusEvent.SetRequestedEvents(aReqEventsMask);
					
	CT_LbsAsyncWaiter* waiter = CT_LbsAsyncWaiter::NewL();
		
	iServer.NotifyModuleStatusEvent(statusEvent, waiter->iStatus, aModId);
	
	waiter->StartAndWait();

	TInt result = waiter->Result();

	//User::WaitForRequest(request);
	delete waiter;
	
	return result;
	
	}

TBool CT_LbsClientStep_ModStat::DoVerifyModStatus(TPositionModuleStatusEvent::TModuleEvent aExpectedModStatus,TPositionModuleId aModId)
	{
	// Initialise current module status.
	TPositionModuleStatus& currentModStatus = iParent.iSharedData->iCurrentModuleStatus;
	iServer.GetModuleStatus(currentModStatus,aModId);

	if(currentModStatus.DeviceStatus()!=aExpectedModStatus)
		{
		INFO_PRINTF3(_L("Module in Unexpected State - %d and not %d"),currentModStatus.DeviceStatus(),aExpectedModStatus);
		SetTestStepResult(EFail);	
		return EFalse;
		}
	else
		{
		INFO_PRINTF2(_L("Module in Expected State %d"),aExpectedModStatus);
		return ETrue;
		}
	}
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_ModStat::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_ModStat::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TInt err = KErrNone;
			
		// Connect to self locate server.
		User::LeaveIfError(iServer.Connect());
		CleanupClosePushL(iServer);
		
		// Carryout unique test actions.
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				// Test case LBS-ModStatus-0001 to 0004
				case 01:
				case 02:
				case 03:				
				case 04:
					{
					TPositionModuleId testmodId;
					
					T_LbsUtils utils;
					testmodId = utils.GetAGpsModuleIdL(iServer);
					
					// Get the module device status.
					TPositionModuleStatus& modStatus = iParent.iSharedData->iCurrentModuleStatus;					
					err = iServer.GetModuleStatus(modStatus, testmodId);
					if(KErrNone != err)
						{
						INFO_PRINTF2(_L("Failed with err %d"), err);
						SetTestStepResult(EFail);	
						}
					}
					break;
					
				// Test case LBS-ModStatus-0005
				case 05:
					{
					TPositionModuleId badmodId;
					T_LbsUtils utils;
					badmodId = utils.GetBadModuleId();
					
					// Get the module device status.
					TPositionModuleStatus moduleStatus;
					
					err = iServer.GetModuleStatus(moduleStatus, badmodId);
					if(KErrNotFound != err)
						{
						INFO_PRINTF2(_L("Failed with err %d. Should have been KErrNotFound(-1)"), err);
						SetTestStepResult(EFail);	
						}
					}
					break;					
										
				// LBS-Mod-Status-Event-0002
				
				case 22:
					{
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						T_LbsUtils utils;
						TPositionModuleId modId;
						modId = utils.GetAGpsModuleIdL(iServer);
						User::LeaveIfError(iPositioner.Open(iServer, modId));
						CleanupClosePushL(iPositioner);						
						}
					
					TPositionModuleStatus& testmodStatus = iParent.iSharedData->iCurrentModuleStatus;
												
					// Get device status events for ANY module:										
					if(!DoRequestAndVerifyModStatusEventsL(TPositionModuleStatusEvent::EEventDeviceStatus, testmodStatus))
						{
						SetTestStepResult(EFail);
						}
					
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					}
					break;
					
				// LBS-Mod-Status-Event-0003
				case 23:
					{
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						T_LbsUtils utils;
						TPositionModuleId modId;
						modId = utils.GetAGpsModuleIdL(iServer);
						User::LeaveIfError(iPositioner.Open(iServer, modId));
						CleanupClosePushL(iPositioner);						
						}
					
					TPositionModuleStatus& testmodStatus = iParent.iSharedData->iCurrentModuleStatus;
					
					// Get device status events	for (test) agps module		
					if(!DoRequestAndVerifyAGPSModStatusEventsL(TPositionModuleStatusEvent::EEventDeviceStatus, testmodStatus))
						{
						SetTestStepResult(EFail);
						}
					
					if(iExpectedApiBehaviour == EApiVariant1)// on variant2 we don't see any other status updates (expected)
						{
						// Get device status events for ALL modules				
						if(!DoRequestAndVerifyModStatusEventsL(TPositionModuleStatusEvent::EEventDeviceStatus, testmodStatus))
							{
							SetTestStepResult(EFail);
							}
						}
					else
						{
						CleanupStack::PopAndDestroy(&iPositioner);
						}					
					}
					break;
				
				// LBS-Mod-Status-Event-0004
				case 24:
					{
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						T_LbsUtils utils;
						TPositionModuleId modId;
						modId = utils.GetAGpsModuleIdL(iServer);
						User::LeaveIfError(iPositioner.Open(iServer, modId));
						CleanupClosePushL(iPositioner);						
						}						
					TPositionModuleStatus& testmodStatus = iParent.iSharedData->iCurrentModuleStatus;
						
					// Get AGPS device status events				
					if(!DoRequestAndVerifyAGPSModStatusEventsL(TPositionModuleStatusEvent::EEventDeviceStatus, testmodStatus))
						{
						SetTestStepResult(EFail);
						}
					}
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						CleanupStack::PopAndDestroy(&iPositioner);
						}						
					break;
					
				// LBS-Mod-Status-Event-0005
				case 25:
					{
					if(iExpectedApiBehaviour == EApiVariant2)
							{
							T_LbsUtils utils;
							TPositionModuleId modId;
							modId = utils.GetAGpsModuleIdL(iServer);
							User::LeaveIfError(iPositioner.Open(iServer, modId));
							CleanupClosePushL(iPositioner);						
							}					
					TPositionModuleStatus& testmodStatus = iParent.iSharedData->iCurrentModuleStatus;
					// Get device status events				
					if(!DoRequestAndVerifyAGPSModStatusEventsL(TPositionModuleStatusEvent::EEventDataQualityStatus, testmodStatus))
						{
						SetTestStepResult(EFail);
						}	
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						CleanupStack::PopAndDestroy(&iPositioner);
						}						
					}
					break;
										
				// LBS-Mod-Status-Event-0009
				case 29:
					{
					if(iExpectedApiBehaviour == EApiVariant2)
							{
							T_LbsUtils utils;
							TPositionModuleId modId;
							modId = utils.GetAGpsModuleIdL(iServer);
							User::LeaveIfError(iPositioner.Open(iServer, modId));
							CleanupClosePushL(iPositioner);						
							}						
					TPositionModuleStatus& testmodStatus = iParent.iSharedData->iCurrentModuleStatus;
					
					//In feature when system events support, change TPositionModuleStatusEvent::EEventAll 
					// Get device status events				
					if(!DoRequestAndVerifyAGPSModStatusEventsL(((TPositionModuleStatusEvent::EEventDeviceStatus)|(TPositionModuleStatusEvent::EEventDataQualityStatus)), testmodStatus))
						{
						SetTestStepResult(EFail);
						}	
					if(iExpectedApiBehaviour == EApiVariant2)
						{
						CleanupStack::PopAndDestroy(&iPositioner);
						}						
					}
					break;
															
				// LBS-Mod-Status-Event-0010
				case 30:
					{
					TPositionModuleId modId;
					TInt err;
						
					// use bad module id
					T_LbsUtils utils;
					modId = utils.GetBadModuleId();
								
					err = DoRequestModStatusEventsWithError(TPositionModuleStatusEvent::EEventDataQualityStatus, modId);								
					if(KErrNotFound != err)
						{
						SetTestStepResult(EFail);
						}
					}
					break;
															
				// LBS-Mod-Status-Event-0011
				case 31:
					{
					TPositionModuleId modId;
					TInt err;
						
					// use agps module id
					T_LbsUtils utils;
					modId = utils.GetAGpsModuleIdL(iServer);
								
					err = DoRequestModStatusEventsWithError(0, modId);								
					if(KErrArgument != err)
						{
						SetTestStepResult(EFail);
						}
					}
					break;

				// LBS-Mod-Status-Event-0012
				case 32:
					{
					TPositionModuleId modId;
					TRequestStatus request;					
					TPositionModuleStatusEvent statusEvent;
						
					// use agps module id
					T_LbsUtils utils;
					modId = utils.GetAGpsModuleIdL(iServer);
					
					statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus);
										
					iServer.NotifyModuleStatusEvent(statusEvent, request, modId);
					
					TInt err = iServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
					if(KErrNone == err)
						{
						User::WaitForRequest(request);
						if(KErrCancel != request.Int())
							{
							INFO_PRINTF2(_L("Failed: KErrCancel not returned, Got %d"), request.Int());
							SetTestStepResult(EFail);
							}
						}
					else
						{
						INFO_PRINTF2(_L("Failed: CancelRequest returned error %d"), err);
						SetTestStepResult(EFail);
						}
					}
					break;
				
				// LBS-Mod-Status-Event-0013
				case 33:
					{
					TInt err = iServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
					if(KErrNotFound != err)
						{
						INFO_PRINTF2(_L("Failed: CancelRequest returned error %d, expecting KErrNotFound"), err);
						SetTestStepResult(EFail);						
						}
					}
					break;

				// LBS-Mod-Status-Event-0014
				case 34:
					{
					// close server since we don't want to be connected for this test
					iServer.Close();
					
					TInt err = iServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
					
					// if we get here, something went wrong, so panic
					SetTestStepResult(EFail);
					
					User::Panic(KLbsClientStep_ModStatus, KErrGeneral);
					}
					break;
					
				// LBS-Mod-Status-Event-0015
				case 35:
					{
					TPositionModuleId modId;
					TRequestStatus request;					
					TPositionModuleStatusEvent statusEvent;
						
					// use agps module id
					T_LbsUtils utils;
					modId = utils.GetAGpsModuleIdL(iServer);
					
					// set requested events mask
					statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus);
					
					// ask to be notified of requested events					
					iServer.NotifyModuleStatusEvent(statusEvent, request, modId);
					
					// ask to be notified of requested events a second time (panics)					
					iServer.NotifyModuleStatusEvent(statusEvent, request, modId);
					
					// if we get here, something went wrong, so panic
					SetTestStepResult(EFail);
					
					User::Panic(KLbsClientStep_ModStatus, KErrGeneral);
					}
					break;
				case 101:
					{
					// This test requires net sim to be open.
					OpenNetSim();
					
					// Select the network module.
					T_LbsUtils utils;
					TPositionModuleId modId;
					modId = utils.GetNetworkModuleIdL(iServer);
					User::LeaveIfError(iPositioner.Open(iServer, modId));
					CleanupClosePushL(iPositioner);
					
					// Initialise the position info variable.
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
					CleanupStack::PushL(posInfo);
						
					// set requested events mask so we get notified on EEventDeviceStatus (any status change)
					TPositionModuleStatusEvent statusEvent;
					statusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventDeviceStatus);
					CT_LbsAsyncWaiter* waiter = CT_LbsAsyncWaiter::NewL();
					iServer.NotifyModuleStatusEvent(statusEvent, waiter->iStatus, modId);
					
					if(iExpectedApiBehaviour == EApiVariant1)
						{
						// Verify that the module status is ready before we make any requests.
						DoVerifyModStatus(TPositionModuleStatus::EDeviceReady,modId);
						}
					else // on S60 the device starts off in 'inactive' state
						{
						// Verify that the module status is ready before we make any requests.
						DoVerifyModStatus(TPositionModuleStatus::EDeviceInactive,modId);						
						}
					// Request a position update, we should then see a state change to EDeviceActive
					TRequestStatus status;		
					INFO_PRINTF1(_L("Performing NotifyPositionUpdate()"));
					iPositioner.NotifyPositionUpdate(*posInfo,status);
					// Use the waiter object so that we can get the device status change.
					waiter->StartAndWait();
					waiter->Result();
					
					TPositionModuleStatusEvent::TModuleEvent eventsExpected;
					
					if(iExpectedApiBehaviour == EApiVariant1)
						{
						// Check that the change that happened was the correct thing.
						eventsExpected = TPositionModuleStatusEvent::EEventDeviceStatus;
						}
					else	// on s60 we also get a data quality status because this doesn't get reported until the PSY is loaded
						{
						eventsExpected = TPositionModuleStatusEvent::EEventDeviceStatus | TPositionModuleStatusEvent::EEventDataQualityStatus;
						}
					
					TPositionModuleStatusEvent::TModuleEvent eventsOccurred = statusEvent.OccurredEvents();
					
					if(eventsOccurred!=eventsExpected)
						{
						INFO_PRINTF1(_L("No Module Status Event have Occured"));
						SetTestStepResult(EFail);
						}
					
					
					// Verify the device is now active
					DoVerifyModStatus(TPositionModuleStatus::EDeviceActive,modId);	
					
					INFO_PRINTF1(_L("Position Received"));
					User::WaitForRequest(status);

					//Verify that the position is EDeviceReady again
					DoVerifyModStatus(TPositionModuleStatus::EDeviceReady,modId);
				
					// Tidy up everything
					CloseNetSim();					
					delete waiter;
					CleanupStack::PopAndDestroy(posInfo);	
					CleanupStack::PopAndDestroy(&iPositioner);	
					}
					break;					
				default:
					{
					User::Panic(KLbsClientStep_ModStatus, KErrUnknown);
					}					
				}
				

			}
			
		// All done, clean up.
		CleanupStack::PopAndDestroy(&iServer);		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_ModStat::doTestStepL()"));

	return TestStepResult();
	}

