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
// This is the class implementation for the Module Information Tests
// EPOC includes.sue
// 
//

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <e32math.h>
#include <e32property.h>
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include "LbsExtendModuleInfo.h"
#include "lbsdevloggermacros.h"

// LBS test includes.
#include "ctlbsmolrtracking.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

const TInt KSmallDelta = 800 * 1000; // 800 ms

/**
Static Constructor
*/
CT_LbsMolrTracking* CT_LbsMolrTracking::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMolrTracking* testStep = new CT_LbsMolrTracking(aParent);
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


/**
 * Constructor
 */
CT_LbsMolrTracking::CT_LbsMolrTracking(CT_LbsHybridMOLRServer& aParent) 
	: CT_LbsHybridMOLRStep(aParent),  
		iAdminGpsMode(CLbsAdmin::EGpsModeUnknown),  
		iGPSModeNotSupported(EFalse),
		iSpecialTestMode(0)
	{
	SetTestStepName(KLbsMolrTracking);
	}


void CT_LbsMolrTracking::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsMolrTracking::~CT_LbsMolrTracking()
	{
	TRAP_IGNORE(doTestStepPostambleL());
	}

/**
 Things read from the ini file:
 - Network Method (1 - TB, 2 - TBTA, 3 - TA, 4 - TATB)
 - Planned Positions Originator (used in hybrid to decide where the position should 
 			come from. Same for all positions returned, 1 - Module, 2 - Network)
 - Could have checks for the module modes if wanted (optional, doesn't affect the test) 
 			-first argument one of: 1 - PTB, 2 - ATB, 3 - Autonomous, 4 - PTA, 5 - ATA
 			-second and third argument can be 1 - Positions, 2 - Measurments
 - For each client the following information can be provided:
 	- the starting delay, represented by two numbers:
 	 	* the first one specifies after which network session to start - default is 0
 	 	* The second one specifies after which step in the network to start - default is 0;
 	 		For a list of the network steps see ctlbsnetprotocol.h
 	- the number of NPUDs, default is one NPUD. One NPUD means not tracking, more than 
 	 	that means the client is tracking
 	- the positioner options: interval, max fix time, max age, partial accepted.
 			Default all zero and partial not accepted
 	- whether the client's NPUD request should be cancelled and at what stage
 			Default to not cancelled
 The number of clients is determined from how many entries are that contain 
 client details. If there are no entries one default client is used. The information 
 missing from an entry will be assumed to be default. If either the tracking interval 
 or the number of NPUDs is set to indicate no tracking then this client will not 
 do any tracking.
 
 NOTE: The only way a client can do more NPUDs is if it's doing tracking
 NOTE: The Planned Position Originator has to be provided for all tests. Logic to 
 		work it out when not running in hybrid hasn't been provided in the test yet
 
 TODO: Add some position checking and make the network position change
 TODO: Add logic to deal with the max fix time bigger than the interval and positions coming in 
 			overlapping windows
 TODO: Sort out the PM returning a reference position in the right modes
 TODO: Checking of status changes for PM none <-> tracking 
 
 Example of ini file entry:
 	[TEST]
	agps_module_update_file = c:\testdata\configs\test.ini
	$update,3,0,NAN,NAN,NAN,NAN,NAN*
	$update,1,0,49.2,3.5,50,20,20*
	$update,1,0,NAN,NAN,NAN,NAN,NAN*
	$update,1,0,49.3,3.5,50,20,20*
	$update,1,0,NAN,NAN,NAN,NAN,NAN*
	$update,1,0,49.4,3.5,50,20,20*
	$update,1,0,NAN,NAN,NAN,NAN,NAN*
	$update,1,0,49.4,3.4,50,20,20*
	$update,1,0,NAN,NAN,NAN,NAN,NAN*
	$update,1,0,49.4,3.3,50,20,20*
	$update,1,0,NAN,NAN,NAN,NAN,NAN*
	$update,1,0,49.5,3.3,50,20,20*
	// The network will request TB
	NetworkMethod = 1
	// It is expected that the module will return the position
	PositionOriginator = 1
	// The module should be set to PTB and then not set again (this is optional - for additional checking)
	// PTB
	$modulemodes,1*
	// The client information
	// first client: session ID delay = 0, session step delay = 0, NPUDs = 4, interval = 10s, time out = 5s, max age = 0s, accept partial = false
	$clientdetails,0,0,4,10000000,5000000,0,false*
	// second client: session ID delay = 1, session step delay = 4, NPUDs = 3, interval = 8s, time out = 3s, max age = 0s, accept partial = false
	$clientdetails,1,4,3,8000000,3000000,0,false*
	// third client: session ID delay = 1, session step delay = 0, NPUDs = 1, interval = 0s, time out = 5s, max age = 0s, accept partial = false
	$clientdetails,1,0,1,0,5000000,0,false*
	// fourth client: session ID delay = 4, session step delay = 3, NPUDs = 1 (so no tracking)
	$clientdetails,4,3,1*
	// fifth client: session ID delay = 4, session step delay = 3, no tracking
	$clientdetails,4,3*
	// sixth client: no delay, no tracking (default client)
	$clientdetails*
	// seventh client: session ID delay = 0, session step delay = 0, NPUDs = 2, interval = 5s, time out = 5s, max age = 0s, accept partial = false, cancel = true, cancel session ID = 1, cancel session step = 3
	$clientdetails,0,0,2,5000000,5000000,0,false,cancel,1,3*

 */
TVerdict CT_LbsMolrTracking::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrTracking::doTestStepPreambleL()"));
	CT_LbsHybridMOLRStep::doTestStepPreambleL();

	// Get the GPS mode set in the Admin
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);

	TESTL(KErrNone == admin->Get(KLbsSettingRoamingGpsMode, iAdminGpsMode));
	CleanupStack::PopAndDestroy(admin);
	
	// Set the test up so it expects that the GPS mode won't be supported if there is an incompatibility
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities deviceCapabilities;
	TInt err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, deviceCapabilities);
	
	if(!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB) && !(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted))
		{
		if(iAdminGpsMode == CLbsAdmin::EGpsPreferTerminalAssisted || 
				iAdminGpsMode == CLbsAdmin::EGpsAlwaysTerminalAssisted)
			{
			iGPSModeNotSupported = ETrue;
			}	
		}
	if(!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB) && !(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased))
		{
		if(iAdminGpsMode == CLbsAdmin::EGpsPreferTerminalBased || 
				iAdminGpsMode == CLbsAdmin::EGpsAlwaysTerminalBased ||
				iAdminGpsMode == CLbsAdmin::EGpsAutonomous)
			{
			iGPSModeNotSupported = ETrue;
			}	
		}	

	SetTestStepResult(EPass);
	T_LbsUtils utils;

	iNetworkProtocol = CT_LbsNetProtocol::NewL(*this, *this);
	// Get the Network Method
	_LIT(KNetworkMethod, "NetworkMethod");
	TInt networkMethodInt;
	if(GetIntFromConfig(ConfigSection(), KNetworkMethod, networkMethodInt))
		{
		iNetworkProtocol->SetNetworkMethod(networkMethodInt);
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMolrTracking::doTestStepPreambleL() - FAILED: no network method configured"));
		TEST(EFalse);
		}
	
	// Get the position originator
	_LIT(KPositionOriginator, "PositionOriginator");
	TInt positionOriginatorInt;
	if(GetIntFromConfig(ConfigSection(), KPositionOriginator, positionOriginatorInt))
		{
		iNetworkProtocol->SetPlannedPositionOriginator(positionOriginatorInt);
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMolrTracking::doTestStepPreambleL() - FAILED: no position originator configured"));
		TEST(EFalse);
		}

	// Get the SpecialTestModemode setting ... 0 means not in a LastKnownPosition mode
	_LIT(KLastKnownPosition, "SpecialTestMode");
	TInt specialTestModeInt;
	if(GetIntFromConfig(ConfigSection(), KLastKnownPosition, specialTestModeInt))
		{
		iSpecialTestMode = specialTestModeInt;
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMolrTracking::doTestStepPreambleL() - no SpecialTestMode in .ini section - setting mode to disabled"));
		iSpecialTestMode =0;
		}

	TPtrC configFileName;
	_LIT(KUpdateOptionsFile, "agps_module_update_file");
	TEST(GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName));
	RArray<TCTClientDetails> clientDetailsArray;
	CleanupClosePushL(clientDetailsArray);
	utils.GetConfigured_ClientDetailsL(configFileName, ConfigSection(), clientDetailsArray);

	
	for(TInt index = 1; index <= clientDetailsArray.Count(); ++index)
		{
		CT_LbsTestActiveManager* clientTestManager = CT_LbsTestActiveManager::NewL(index, *this, CActive::EPriorityStandard, *this);
		iClientTestManagers.AppendL(clientTestManager);
		}
	
	for(TInt index = 0; index < clientDetailsArray.Count(); ++index)
		{
		CT_ClientData* client = new(ELeave) CT_ClientData(clientDetailsArray[index].iSessionIdDelay, clientDetailsArray[index].iSessionStepDelay, clientDetailsArray[index].iCancelRequest, clientDetailsArray[index].iSessionIdCancel, clientDetailsArray[index].iSessionStepCancel, clientDetailsArray[index].iNumberOfNPUDs, 
							clientDetailsArray[index].iUpdateOptions, clientDetailsArray[index].iPsyId, clientDetailsArray[index].iExpectedError);
		iClients.AppendL(client);
		}
	CleanupStack::PopAndDestroy(&clientDetailsArray);

	return TestStepResult();
	}

TVerdict CT_LbsMolrTracking::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrTracking::doTestStepPostambleL()"));
	iClients.ResetAndDestroy();
	iClientTestManagers.ResetAndDestroy();

	if(iNetworkProtocol)
		{
		delete iNetworkProtocol;
		iNetworkProtocol = NULL;
		}
	
	iAdminGpsMode = CLbsAdmin::EGpsModeUnknown;
	CT_LbsHybridMOLRStep::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CT_LbsMolrTracking::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsMolrTracking::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);
	
	iNetworkProtocol->StartNetworkL();

	CActiveScheduler::Start();
	return TestStepResult();
	}

void CT_LbsMolrTracking::OnReadyL(TInt aObjectId)
	{
	if(aObjectId > 0 && aObjectId <= iClients.Count())
		{
		DoClientStepL(aObjectId - 1);
		return;
		}
	
	User::Leave(KErrCorrupt);
	}

void CT_LbsMolrTracking::OnTimedOutL(TInt aObjectId)
	{
	INFO_PRINTF2(_L("Client number %d timed out waiting for a position"), aObjectId);
	User::Leave(KErrTimedOut);
	}

void CT_LbsMolrTracking::OnGetLastKnownPositionL(TInt aObjectId, TInt32 aErr, const TPositionInfoBase&  /*aPosInfo*/)
	{
	INFO_PRINTF2(_L("CT_LbsMolrTracking::OnGetLastKnownPositionL()	>> LKNP() complete with %d"), aErr);

	CT_ClientData& client = *(iClients[aObjectId-1]);
	
	TInt numberOfNPUDs = client.iNumberOfNPUDs;
	TInt positionsReceived = client.iPositionsReceived;

	if (iSpecialTestMode==1)
		{
		INFO_PRINTF1(_L("CT_LbsMolrTracking::OnGetLastKnownPosition() - Special Test for FNP delivery"));
		INFO_PRINTF2(_L("Client number %d received a last known position"), aObjectId);
		INFO_PRINTF3(_L("Client now has %d of %d positions"), positionsReceived+1, numberOfNPUDs);

		INFO_PRINTF1(_L("CT_LbsMolrTracking::OnGetLastKnownPosition() - a network calculated FNP was correctly delivered"));

		}
	else
	{
	INFO_PRINTF1(_L("CT_LbsMolrTracking::OnGetLastKnownPosition() - FAILED: not expecting this call"));
	TESTL(EFalse); // Shouldn't see this...
	}

	++client.iPositionsReceived;

	if(client.iPositionsReceived == client.iNumberOfNPUDs)
		{ // stop this client tracking and close connection to the server also check that all the other clients have received positions as expected
		iClientTestManagers[aObjectId-1]->Deactivate();

		delete client.iWatcher;
		client.iWatcher = NULL;
		client.iPositioner.Close();
		client.iServer.Close();
			
		}
		else // re-issue the request
		{
		iClientTestManagers[aObjectId-1]->StartL(0);
		}
		
	}

void CT_LbsMolrTracking::OnNotifyPositionUpdateL(TInt aObjectId, TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	RDebug::Printf("LBSLog: LocSrv -> Client%d: Deliver Position, error = %d", aObjectId, aErr);
	INFO_PRINTF2(_L("CT_LbsMolrTracking::OnNotifyPositionUpdate() >> NPUD() complete with %d"), aErr);
	if(iGPSModeNotSupported)
		{
		TESTL(aErr == KErrNotSupported);
		}
	else
		{
		CT_ClientData& client = *(iClients[aObjectId-1]);		
		TTime now;
		now.HomeTime();
		
		if(client.iRequestCancelled)
			{
			TESTL(aErr == KErrCancel);
			}
		else if(client.iExpectedError)
			{
			TESTL(aErr == client.iExpectedError);
			}
		else
			{
			TInt numberOfNPUDs = client.iNumberOfNPUDs;
			TInt positionsReceived = client.iPositionsReceived;
			
			INFO_PRINTF2(_L("Client number %d received a position"), aObjectId);
			INFO_PRINTF3(_L("Client now has %d of %d positions"), positionsReceived+1, numberOfNPUDs);
		
			if (iSpecialTestMode == 2)
				{
				
				INFO_PRINTF1(_L("testing MaxAge feature"));
				const TPositionInfo* lastPosReturned = static_cast<const TPositionInfo*>(&aPosInfo);
				TPosition pos;
				lastPosReturned->GetPosition(pos);
	 
				if (client.iPositionsReceived == 0)
					{ // if its the first one 
					client.iTimeOfLastPosition = pos.Time();
					}
				else
					{
					if (client.iTimeOfLastPosition != pos.Time())
						{
						INFO_PRINTF1(_L("testing MaxAge feature - FAIL! position time was not same as previous"));
						TESTL(EFalse);	
						}
					else
						{
						INFO_PRINTF1(_L("testing MaxAge feature - PASS! position time was same as previous"));
						}
					client.iTimeOfLastPosition = pos.Time();
					}
				}
			else if (iSpecialTestMode==3) // testing early complete behaviour when have multiple client requests 
				{
					if ( (aErr == KPositionEarlyComplete) || (aErr == KErrNone))
					{
					INFO_PRINTF2(_L("testing early complete behaviour got correct error%d"), aErr);
					}
					else
					{
					INFO_PRINTF2(_L("testing early complete behaviour got error %d"), aErr);
					TESTL(EFalse);	
					}
				}
			else
				{
				// Normally it should be KErrNone
				TESTL(aErr == KErrNone);
				}
		
			// Verify position.
			//CT_ClientData& client = *(iClients[aObjectId-1]);
		
			TESTL(aPosInfo.PositionClassType() == EPositionInfoClass);
			if(iNetworkProtocol->PlannedPositionOriginator() == CT_LbsNetProtocol::EPositionOriginatorModule)
				{
				if(client.iAutonomousPsy)
					{
					INFO_PRINTF2(_L("Autonomous GPS client got position with techtype = %d"), aPosInfo.PositionMode());
					TESTL( (aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal)) || (aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted)));
					}
				else
					{
					TESTL(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
					}
				}
			if(iNetworkProtocol->PlannedPositionOriginator() == CT_LbsNetProtocol::EPositionOriginatorNetwork)
				{
				if (iSpecialTestMode==3)
					{
					INFO_PRINTF1(_L("testing early complete behaviour - don't check tech type"));
					}
				else
					{
					TESTL(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
					}
				}
			
			if(iExpectedApiBehaviour == EApiVariant1)
				{
				if(client.iPositionsReceived == 0)
					{ // record the time when a first position is received by the client
					client.iTimeOfInitialPosition.HomeTime();
					}
				else
					{ // make sure that the position was received in the right window
					TTimeIntervalMicroSeconds interval = now.MicroSecondsFrom(client.iTimeOfInitialPosition);
					
					if (iSpecialTestMode==3) // testing early complete behaviour when have multiple client requests 
						{
						INFO_PRINTF1(_L("testing early complete behaviour - don't check arrival time"));
		
						}
					else
						{
						if (client.iUpdateOptions.UpdateInterval().Int64())
							{ // if tracking
							if(interval.Int64() < client.iUpdateOptions.UpdateInterval().Int64() * client.iPositionsReceived && 
									interval.Int64()+KSmallDelta >= client.iUpdateOptions.UpdateInterval().Int64() * client.iPositionsReceived)
								{ // if the time is slightly before the window then allow for possible delays in the test framework
								interval = interval.Int64() + KSmallDelta;
								}
							if(interval.Int64() % client.iUpdateOptions.UpdateInterval().Int64() > client.iUpdateOptions.UpdateTimeOut().Int64() && 
									(interval.Int64()-KSmallDelta) % client.iUpdateOptions.UpdateInterval().Int64() <= client.iUpdateOptions.UpdateTimeOut().Int64())
								{ // if the time is slightly after the window then allow for possible delays in the test framework
								interval = interval.Int64() - KSmallDelta;
								}
							TESTL(interval.Int64() / client.iUpdateOptions.UpdateInterval().Int64() == client.iPositionsReceived);
							TESTL(interval.Int64() % client.iUpdateOptions.UpdateInterval().Int64() <= client.iUpdateOptions.UpdateTimeOut().Int64());
							}
						}
					}
				}
			else
				{ // variant2 api behaviour
				if(client.iPositionsReceived > 0) // make sure that the position was received in the right window
					{
					TTimeIntervalMicroSeconds interval = now.MicroSecondsFrom(client.iTimeOfPreviousPosition);
					if (iSpecialTestMode==3) // testing early complete behaviour when have multiple client requests 
						{
						INFO_PRINTF1(_L("testing early complete behaviour - don't check arrival time"));
						}
					else
						{
						if (client.iUpdateOptions.UpdateInterval().Int64())
							{ // if tracking
							if(interval.Int64() < client.iUpdateOptions.UpdateInterval().Int64() && 
									interval.Int64()+KSmallDelta >= client.iUpdateOptions.UpdateInterval().Int64())
								{ // if the time is slightly before the window then allow for possible delays in the test framework
								interval = interval.Int64() + KSmallDelta;
								}
							if(interval.Int64() - client.iUpdateOptions.UpdateInterval().Int64() > client.iUpdateOptions.UpdateTimeOut().Int64() && 
									(interval.Int64()-KSmallDelta) - client.iUpdateOptions.UpdateInterval().Int64() <= client.iUpdateOptions.UpdateTimeOut().Int64())
								{ // if the time is slightly after the window then allow for possible delays in the test framework
								interval = interval.Int64() - KSmallDelta;
								}
							TESTL(interval.Int64() >= client.iUpdateOptions.UpdateInterval().Int64());
							TESTL(interval.Int64() - client.iUpdateOptions.UpdateInterval().Int64() <= client.iUpdateOptions.UpdateTimeOut().Int64());
							}
						}
					}
				client.iTimeOfPreviousPosition.HomeTime();
				}
			}
			
		++client.iPositionsReceived;
		
		if(client.iPositionsReceived == client.iNumberOfNPUDs)
			{ // stop this client tracking and close connection to the server also check that all the other clients have received positions as expected
			iClientTestManagers[aObjectId-1]->Deactivate();

			delete client.iWatcher;
			client.iWatcher = NULL;
			client.iPositioner.Close();
			client.iServer.Close();

			TBool moreActiveClients(EFalse);
			
			if(iSpecialTestMode==3) // testing early complete behaviour when have multiple client requests 
				{
				INFO_PRINTF1(_L("testing early complete behaviour - don't check arrival times are in tracking windows"));
				}
			else
				{
				for(TUint index = 0; index < iClients.Count(); ++index)
					{ // for each client check that the positions were received in the right window until now
					if(!iClientTestManagers[index]->IsDeactivated())
						{
						moreActiveClients = ETrue;
						CT_ClientData& clientToCheck = *(iClients[index]);
						if((!(clientToCheck.iRequestCancelled)) && (clientToCheck.iPositionsReceived > 0) &&  !(clientToCheck.iExpectedError))
							{
							if(iExpectedApiBehaviour == EApiVariant1)
								{
								TTimeIntervalMicroSeconds interval = now.MicroSecondsFrom(clientToCheck.iTimeOfInitialPosition);
								if (client.iUpdateOptions.UpdateInterval().Int64())
									{
						
									TESTL(interval.Int64() / clientToCheck.iUpdateOptions.UpdateInterval().Int64() == clientToCheck.iPositionsReceived-1 || // either the client received all the positions already 
									(interval.Int64() / clientToCheck.iUpdateOptions.UpdateInterval().Int64() == clientToCheck.iPositionsReceived &&  // or the window to receive a position is now and it's RunL didn't get called yet
									 interval.Int64() % clientToCheck.iUpdateOptions.UpdateInterval().Int64() <= clientToCheck.iUpdateOptions.UpdateTimeOut().Int64()));
									}
								}
							else
								{
								TTimeIntervalMicroSeconds interval = now.MicroSecondsFrom(clientToCheck.iTimeOfPreviousPosition);
								if (client.iUpdateOptions.UpdateInterval().Int64())
									{
									TESTL(interval.Int64() - clientToCheck.iUpdateOptions.UpdateInterval().Int64() - KSmallDelta <= 
												clientToCheck.iUpdateOptions.UpdateTimeOut().Int64());
									}
								}
							}
						}
					}
				}
			
			if(!moreActiveClients)
				{ // if all the clients finished then tell the network to close the session
				iNetworkProtocol->SignalClientsFinished();
				}
			}
		else
			{
			// re-issue the request
			iClientTestManagers[aObjectId-1]->StartL(0);
			}
		}
	}

void CT_LbsMolrTracking::StopTest()
	{
	CActiveScheduler::Stop();
	}

void CT_LbsMolrTracking::OnSignalNetworkStep(TInt aSessionId, TInt aSessionStep)
	{
	for(TInt index = 0; index < iClientTestManagers.Count(); ++index)
		{
		CT_ClientData& client = *iClients[index];
		if(client.iInitialStep)
			{
			if(aSessionId > client.iSessionIdDelay || 
					(aSessionId == client.iSessionIdDelay && aSessionStep >= client.iSessionStepDelay))
				{
				iClientTestManagers[index]->StartL(0);
				}
			}
		if(client.iCancelRequest && !(client.iRequestCancelled))
			{
			INFO_PRINTF3(_L("Waiting to cancel. At network session %d, step %d"), aSessionId, aSessionStep);			
			if(aSessionId > client.iSessionIdCancel || 
					(aSessionId == client.iSessionIdCancel && aSessionStep >= client.iSessionStepCancel))
				{
				// cancel oustanding NPUD
				INFO_PRINTF3(_L("Cancelling Request for Client %d's NPUD %d"),index+1, client.iPositionsReceived+1);
				__ASSERT_DEBUG((client.iNumberOfNPUDs == client.iPositionsReceived + 1), User::Invariant());
				RDebug::Printf("LBSLog: Client%d -> LocSrv : CancelRequest", index+1);
				client.iWatcher->CancelRequest();
				client.iRequestCancelled = ETrue;
				}
			}		
		}	
	}

void CT_LbsMolrTracking::DoClientStepL(TInt aIndex)
	{
	CT_ClientData& client = *(iClients[aIndex]);
	const TInt KPosTimeOutDelay = 6 * 1000 * 1000; // this is so it leaves some space for delays due to other things in the test running
	TInt posTimeOut(KPosTimeOutDelay);
	
	TInt positionsReceived = client.iPositionsReceived;

	if(client.iInitialStep)
		{
		TESTL(KErrNone == client.iServer.Connect());
		TRequestStatus dbclear;
		client.iServer.EmptyLastKnownPositionStore(dbclear);
		User::WaitForRequest(dbclear);
		
		if(client.iPsyUid)	// client wants a particular psy
			{
			EnablePsy(client.iPsyUid);
			
			TPositionModuleInfo moduleInfo;
			
			client.iServer.GetModuleInfoById(TUid::Uid(client.iPsyUid), moduleInfo);
			
			if(moduleInfo.TechnologyType() == TPositionModuleInfo::ETechnologyTerminal)
				{
				client.iAutonomousPsy = ETrue;
				}
			TESTL(KErrNone == client.iPositioner.Open(client.iServer, TUid::Uid(client.iPsyUid)));
			}
		else
			{
			TESTL(KErrNone == client.iPositioner.Open(client.iServer));
			}
		TPositionUpdateOptions updateOptions(client.iUpdateOptions);
		if(iExpectedApiBehaviour == EApiVariant2)
			{ // for variant 2 of the API behaviour the timeout is considered from the time of the NPUD, rather than from the target time
			updateOptions.SetUpdateTimeOut(updateOptions.UpdateTimeOut().Int64() + updateOptions.UpdateInterval().Int64());
			}
		client.iPositioner.SetUpdateOptions(updateOptions);
		
		client.iWatcher = CPosServerWatcher::NewLC(client.iPositioner, *(iClientTestManagers[aIndex]));
		CleanupStack::Pop(client.iWatcher);
		client.iInitialStep = EFalse;
		posTimeOut += client.iUpdateOptions.UpdateTimeOut().Int64();; 
		}
	else
		{
		posTimeOut += client.iUpdateOptions.UpdateInterval().Int64() + client.iUpdateOptions.UpdateTimeOut().Int64(); 		
		}
	

	if ((iSpecialTestMode == 1) && (aIndex == 0) && (positionsReceived == 1)) 
		{
		INFO_PRINTF2(_L("Client number %d issuing LKNP request"), aIndex+1);
		client.iWatcher->IssueGetLastKnownPosition();
		}
	else
		{
		INFO_PRINTF2(_L("Client number %d issuing NPUD request"), aIndex+1);
		RDebug::Printf("LBSLog: Client%d -> LocSrv : NotifyPositionUpdate", aIndex+1);
		client.iWatcher->IssueNotifyPositionUpdate();
		}

	
	if (iSpecialTestMode==3)
		{
		if ((aIndex == 0) && (positionsReceived == 0)) 	// in this mode client 1 will immediately  CompleteRequest the 
														// NPUD. The  test has contrived to have an ongoing MoLr
														// because client 2 has an outstanding NPUD that should not 
														// be interfered with 	
			{
			INFO_PRINTF1(_L("CompleteRequest!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
			client.iWatcher->CompleteRequest() ;
			}
		}
	iClientTestManagers[aIndex]->WaitForPosition(posTimeOut);
	}



void CT_LbsMolrTracking::EnablePsy(TInt aPsyUid)
	{
	CPosModules* db = CPosModules::OpenL();
	CleanupStack::PushL(db);
	CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
	
	CPosModuleIdList* prioList = db->ModuleIdListLC();
	
	// Enable the PSY that came as an in parameter
	moduleUpdate->SetUpdateAvailability(ETrue);
	db->UpdateModuleL(TUid::Uid(aPsyUid), *moduleUpdate);
	
	CleanupStack::PopAndDestroy(prioList);
	CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(db);
}
