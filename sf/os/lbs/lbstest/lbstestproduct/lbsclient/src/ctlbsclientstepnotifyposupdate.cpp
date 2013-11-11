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
// @file ctlbsclientstep_notifyposupdate.cpp
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsclientstepnotifyposupdate.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>


// constant definitions
_LIT(KLbsClientStepNotifyPosUpdate, "LbsClientStepNotifyPosUpdate");

//profile_id in ini file
_LIT(KLbsProfileId, "profile_id");

//max_time in ini file
_LIT(KLbsMaxTime, "max_time");


const TTimeIntervalMicroSeconds KModuleTimeout = 6000000;//makes test module time out(which should be more than the Maxtime in lbsprofile.ini)

/**
 *	Sets a Requestor for now - TODO will be removed with new location server
 */
void CT_LbsClientStep_NotifyPosUpdate::TempSetRequestorL()
 	{	
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
												CRequestor::EFormatApplication,
												_L("Tom Tom")));
 	}


/**
 * Destructor
 */
CT_LbsClientStep_NotifyPosUpdate::~CT_LbsClientStep_NotifyPosUpdate()
	{
	delete 	iLbsAdmin;	
	}


/**
 * Constructor
 */
CT_LbsClientStep_NotifyPosUpdate::CT_LbsClientStep_NotifyPosUpdate(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent),iLbsAdmin(NULL),iRequestedAssData(FALSE), iReceivedAssData(FALSE)
	{
	
	SetTestStepName(KLbsClientStep_NotifyPosUpdate);
	}


/**
Static Constructor
*/
CT_LbsClientStep_NotifyPosUpdate* CT_LbsClientStep_NotifyPosUpdate::New(CT_LbsClientServer& aParent)
	{
	
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	
	CT_LbsClientStep_NotifyPosUpdate* testStep = new CT_LbsClientStep_NotifyPosUpdate(aParent);
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


void CT_LbsClientStep_NotifyPosUpdate::ConstructL()
	{
	// create the admin
	iLbsAdmin = CLbsAdmin::NewL();

	}

//from MT_NotifyPosUpdateObserver:
/**
 * 	Callback - called when NotifyPositionUpdate request completes
 */

void CT_LbsClientStep_NotifyPosUpdate::NotifyPositionUpdateCallback(TRequestStatus& aStatus)
	{
	TInt err = aStatus.Int();
	if (KErrCancel != err)	
		{
		SetTestStepResult(EFail); // the request always gets cancelled (see test 0122)
		}
	CActiveScheduler::Stop();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_NotifyPosUpdate::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_NotifyPosUpdate::doTestStepL()"));

	if (TestStepResult()==EPass)
		{
		TInt err = KErrNone;
				
		// Connect to self locate server.
		err = iServer.Connect();
		User::LeaveIfError(err);
		CleanupClosePushL(iServer);
		
		// Carryout unique test actions.
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
				switch (testCaseId)
				{
				// Test case LBS-NotifyPosUpdate-0001
				case 1:
				case 29: // Test case LBS-NotifyPosUpdate-0029
				case 30: // Test case LBS-NotifyPosUpdate-0030
				case 101:
				case 105:
				case 201: // in Lbs "original" mode no ref position!	
					{
					T_LbsUtils utils;

					// Open positioner.
					err = iPositioner.Open(iServer);
					User::LeaveIfError(err);
					CleanupClosePushL(iPositioner);

					// For this test an additional reference position will be returned.
					if ((testCaseId == 101) || (testCaseId == 105))
						{
						
						if (testCaseId == 105)
							{	
							TModuleDataIn modDataIn; // Used to send test information to the test module.
							modDataIn.iRequestType = TModuleDataIn::EModuleRequestTestMode;
							modDataIn.iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn_EarlyPosUpdates;
							// Inform the test module of the required test mode - will block.
							utils.NotifyModuleOfConfigChangeL(modDataIn);
							}
						
						// Setup netsim.
						User::LeaveIfError(OpenNetSim());
						
						// Request notify for the expected reference position.
						TPositionInfo actualRefPosInfo;
						
						err = DoNotifyUpdateL(actualRefPosInfo);
						if (err)
							{
							INFO_PRINTF2(_L("Failed test, reference position request returned err %d."), err);

							SetTestStepResult(EFail);							
							}
							
						// Verify reference position.
						TPositionInfo verifyRefPosInfo;
						
						verifyRefPosInfo.SetPosition(iRefPos);
						if (!utils.Compare_PosInfo(verifyRefPosInfo, actualRefPosInfo))
							{
							if(!iParent.iSharedData->iTestModuleInUse)
								{
								WARN_PRINTF1(_L("WARNING: position received was not reference position"));	
								}
							else
								{	
								INFO_PRINTF1(_L("Failed test, reference position incorrect."));
								SetTestStepResult(EFail);
								}
							}
						}


					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					posInfoArr.Append(posInfo);
	
					// Get the actual position
					err = DoNotifyUpdateL(*posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}

					// Cleanup.
					CleanupStack::PopAndDestroy(&iPositioner);
					if ((testCaseId == 101) || (testCaseId == 105))
						{
						CloseNetSim();
						}
				 
					break;
					}

				
				// Test case LBS-NotifyPosUpdate-0003		
				case 3:
					{
					// Select the Network module because it does not support TPositionSatelliteInfo,
					// then request position update and expect satellite info, which should return
					// in KErrArgument.
					TPositionModuleId	modId;
					
					T_LbsUtils utils;
					modId = utils.GetNetworkModuleIdL(iServer);

					// Select the network module.
					User::LeaveIfError(iPositioner.Open(iServer, modId));
					CleanupClosePushL(iPositioner);

					// Try to get the actual position, using TPositionSatellite which is not
					// supported by the selected module.
					TPositionSatelliteInfo posInfo;
					
					err = DoNotifyUpdateL(posInfo);
					if (KErrArgument == err)
						{
						INFO_PRINTF2(_L("correct err %d returned"), err);
						}
					else
						{
						INFO_PRINTF2(_L("incorrect err %d returned"), err);
						SetTestStepResult(EFail);
						}						
					
					CleanupStack::PopAndDestroy(&iPositioner);
					break;
					}
					
				// Test case LBS-NotifyPosUpdate-0005						
				case 5:	
					{
				
					CLbsAdmin::TSelfLocateService selflocate;
					
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeSelfLocate, CLbsAdmin::ESelfLocateOff));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeSelfLocate, selflocate));
					
					if(selflocate != CLbsAdmin::ESelfLocateOff)
						{
						SetTestStepResult(EFail);
						}
					else
						{
						// Open positioner.
						err = iPositioner.Open(iServer);
						User::LeaveIfError(err);
						CleanupClosePushL(iPositioner);

						TPositionInfo notifyPosInfo;
						err = DoNotifyUpdateL(notifyPosInfo);
						if(KErrAccessDenied != err)
							{
							SetTestStepResult(EFail);
							}
						}
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeSelfLocate, CLbsAdmin::ESelfLocateOn));

				    CleanupStack::PopAndDestroy(&iPositioner);
	
					}
					break;
				
				// Test case LBS-NotifyPosUpdate-0006	
				case 6:	
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					TPositionInfo posInfoNotifyUpd;
					
					TRequestStatus status;
					TBool cancelled;
					
					TempSetRequestorL();
					iPositioner.NotifyPositionUpdate(posInfoNotifyUpd, status);
					
					TInt err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
					
					if(KErrNone == err)
						{
							cancelled = TRUE;
						}
					else
						{
							cancelled = FALSE;
						}
											
					// this should complete with either KErrNone or KErrCancel
					User::WaitForRequest(status);
					
					err = status.Int();
					
					if(cancelled)
						{
						if((KErrCancel != err) && (KErrNone != err))
							{
							SetTestStepResult(EFail); 
							}
						}
					else
						{
						if(KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
						}
											
					CleanupStack::PopAndDestroy(&iPositioner);					

					}
					break;
					
			  	// Test case LBS-NotifyPosUpdate-0007
				case 7:
					{
					// Note: Not connecting to position server.
					TPositionInfo notifyPosInfo;
					TRequestStatus status;
					
					// should panic with EPositionServerBadHandle 
					iPositioner.NotifyPositionUpdate(notifyPosInfo, status);
					}
					break;
					
				// Test case LBS-NotifyPosUpdate-0008
				case 8:	
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
				
					TPositionInfo notifyPosInfo, notifyPosInfo2;
					TRequestStatus status, status2;
					
					TempSetRequestorL();
					// attempt multiple NotifyPositionUpdates on same subsession:
					iPositioner.NotifyPositionUpdate(notifyPosInfo, status);
					
					// expected to panic with EPositionDuplicateRequest:
					iPositioner.NotifyPositionUpdate(notifyPosInfo2, status2);
					
					// need to block here so that loc server can run and recieve both
					// requests - its the second request that causes the panic!
					if(iExpectedApiBehaviour == EApiVariant1)
						{
						User::After(1);
						}
					else
						{
						// takes longer with this api 
						User::After(10);
						}
					// if we get here, we've failed to panic
					SetTestStepResult(EFail);
										
					CleanupStack::PopAndDestroy(&iPositioner);					
					}
					break;						
					
				// Test case LBS-NotifyPosUpdate-0014	INCOMPLETE
				case 14:
					{
					// Set 'autonomous' mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsAutonomous));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					// TODO: figure out how to get notifications of assistance data requests (MLbsNetSimTestObserver?)
					// If we get notification before Notify Update completes, then we fail
					
					if(mode != CLbsAdmin::EGpsAutonomous)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
				
						// Create a posinfos and store in our shared array for later verification.
						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
								
						T_LbsUtils utils;
						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
						
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();

						posInfoArr.Append(posInfoNotifyUpd);
						
						err = DoNotifyUpdateL(*posInfoNotifyUpd);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
							
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					
					
					}
					break;

				// Test case LBS-NotifyPosUpdate-0015	UNSUPPORTED
				case 15:
					{
					// this test should not be run; lbs currently doesn't support AlwaysTerminalBased mode
					__ASSERT_ALWAYS(FALSE, User::Panic(_L("LBS-NotifyPosUpdate-0015 should not be run"), KErrNotSupported));					
					
					// Set 'assisted-only' (EGpsAlwaysTerminalBased) mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsAlwaysTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					// TODO: figure out how to get notifications of assistance data requests (MLbsNetSimTestObserver?)
					// If we do not get notification before Notify Update completes, then we fail
					
					if(mode != CLbsAdmin::EGpsAlwaysTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
				
						// Create a posinfos and store in our shared array for later verification.
						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
								
						T_LbsUtils utils;
						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
						
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();

						posInfoArr.Append(posInfoNotifyUpd);
						
						err = DoNotifyUpdateL(*posInfoNotifyUpd);	// can use this blocking version if netsim uses callbacks for assistance data notifications
						// TODO iRequestedAssData needs to be set in assist data callback
						if (KErrNone != err || !iRequestedAssData || !iReceivedAssData)
							{
							SetTestStepResult(EFail);
							}
							
						// verify that preferred positioning mode was used:
						if(EPositionModeReasonNone != posInfoNotifyUpd->PositionMode())
							{
							SetTestStepResult(EFail);
							}
						
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					}
					break;
					
				// Test case LBS-NotifyPosUpdate-0016	INCOMPLETE
				case 16:
					{
					// Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					#pragma message("TO DO: LBS-NotifyPosUpdate-0016	INCOMPLETE")					
					// figure out how to get notifications of assistance data requests (MLbsNetSimTestObserver?)
					// If we do not get notification before Notify Update completes, then we fail
					
					// do we need to do something to make sure that we require assistancce data (no assistance data already retrieved)?
					
					// Configure NetSim to indciate that network is not available
					
					
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
				
						// Create a posinfos and store in our shared array for later verification.
						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
								
						T_LbsUtils utils;
						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
						
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();

						posInfoArr.Append(posInfoNotifyUpd);
						
						err = DoNotifyUpdateL(*posInfoNotifyUpd);	// can use this blocking version if netsim uses callbacks for assistance data notifications
						if (KErrNone != err || !iRequestedAssData)
							{
							SetTestStepResult(EFail);
							}
							
						// verify that preferred positioning mode was not used:
						if(EPositionModeReasonNetworkError != posInfoNotifyUpd->PositionMode())
							{
							SetTestStepResult(EFail);
							}
						
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					}
					break;
					
				// Test case LBS-NotifyPosUpdate-0017	UNSUPPORTED
				case 17:
					{
					// this test should not be run; lbs currently doesn't support AlwaysTerminalBased mode
					__ASSERT_ALWAYS(FALSE, User::Panic(_L("LBS-NotifyPosUpdate-0017 should not be run"), KErrNotSupported));
										
					// Set 'assisted-only' (EGpsAlwaysTerminalBased) mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsAlwaysTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));					
					
					// Configure NetSim to indciate that network is not available
					
					
					if(mode != CLbsAdmin::EGpsAlwaysTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
										
						TPositionInfo posInfoNotifyUpd;
						
						err = DoNotifyUpdateL(posInfoNotifyUpd);	// can use this blocking version if netsim uses callbacks for assistance data notifications
						if (KErrPositionMethodFailure != err)
							{
							SetTestStepResult(EFail);
							}
						
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					}
					break;
					
				// Test case LBS-NotifyPosUpdate-0018	INCOMPLETE
				case 18:
					{
					#pragma message("TO DO: LBS-NotifyPosUpdate-0018	INCOMPLETE")
					// Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					// TODO: figure out how to get notifications of assistance data requests (MLbsNetSimTestObserver?)
					// If we do not get notification before Notify Update completes, then we fail
					
					// TODO: do we need to do something to make sure that we require assistancce data (no assistance data already retrieved)?
					
					// TODO Configure NetSim to return bad/no assistance data
					// this may involve writing an assistance data plugin to provide bad data!?
					
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
				
						// Create a posinfos and store in our shared array for later verification.
						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
								
						T_LbsUtils utils;
						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
						
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();

						posInfoArr.Append(posInfoNotifyUpd);
						
						err = DoNotifyUpdateL(*posInfoNotifyUpd);	// can use this blocking version if netsim uses callbacks for assistance data notifications
						if (KErrNone != err || !iRequestedAssData)
							{
							SetTestStepResult(EFail);
							}
							
						// verify that preferred positioning mode was not used and appropriate reason given:
						if(EPositionModeReasonDataError != posInfoNotifyUpd->PositionMode())
							{
							SetTestStepResult(EFail);
							}
						
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					}
					break;

				// Test case LBS-NotifyPosUpdate-0019
				case 19:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
			
					// Create a posinfos and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
							
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
					
					TInt numPositions = iParent.iSharedData->iVerifyPosInfoArr.Count();
					while(numPositions--)
						{
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();

						posInfoArr.Append(posInfoNotifyUpd);
						
						User::After(500*1000); 	// on Winscw all the target time requests
												// can get stamped with exactly the same
												// time and thus all three requests may be
												// satified from the one the bus
												// then the test fails! This behaviour is OK but 
												// not what the test is trying to test for . Hence ensure
												// all three target times are different by the delay

						err = DoNotifyUpdateL(*posInfoNotifyUpd);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
						}
	
					// Cleanup.
					CleanupStack::PopAndDestroy(&iPositioner);
					}
					break;									

			    // Test case LBS-NotifyPosUpdate-0022	
			    case 22:
					{
					TInt profileid_ini,maxtime_ini;
			        TBool profileid_exists,maxtime_exists;
			        profileid_exists = GetIntFromConfig(ConfigSection(), KLbsProfileId, profileid_ini);
		            if(!profileid_exists)
						{
						INFO_PRINTF1(_L("Could not get profileid from ini file"));
			    		SetTestStepResult(EFail);
			    		}
					
			    	maxtime_exists = GetIntFromConfig(ConfigSection(), KLbsMaxTime, maxtime_ini);
		            if(!maxtime_exists)
						{
						INFO_PRINTF1(_L("Could not get maxtime from ini file"));
			    		SetTestStepResult(EFail);
			    		}
										
					TLbsQualityProfileId profileId;
					//Set SelfLocate quality profile & valid id(which is exist in lbsprofile.ini)
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, TLbsQualityProfileId(profileid_ini)));
					// Verify that the setting is what we expect
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingQualityProfileSelfLocate, profileId));
					if(profileId != TLbsQualityProfileId(profileid_ini))
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
				
						// Create a posinfos and store in our shared array for later verification.
						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
								
						T_LbsUtils utils;
						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
						
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();
						posInfoArr.Append(posInfoNotifyUpd);
                        
                        TTime timeRequested,timeReceived;
                                               
                        // Position requested at
						timeRequested.UniversalTime();
                                                                      
                        //Do request position update
                        err = DoNotifyUpdateL(*posInfoNotifyUpd);
						if ((KErrNone != err) && (KPositionQualityLoss != err))
							{
							INFO_PRINTF2(_L("NotifyUpdate failed with %d "), err);	
							SetTestStepResult(EFail);
							}
                        //Need to verify MaxTime 
                        else						
					     {
					     // Check we dont have a final network postion
					     if (posInfoNotifyUpd->PositionMode() & TPositionModuleInfo::ETechnologyNetwork)
					     	{
					     	INFO_PRINTF1(_L("Failed: Recieved a final network postion"));
					     	SetTestStepResult(EFail);
					     	}
					     
					     TPosition pos;
						 posInfoNotifyUpd->GetPosition(pos);
						 //Position received at
						 timeReceived = pos.Time();
					     
					     //Timedifference between position actually received and position actually requested
					     TInt64 timediff;
	                     timediff = timeReceived.Int64() - timeRequested.Int64();
	                	 //compare timediff with maxtime 
						 if(timediff > maxtime_ini)
							{
							INFO_PRINTF1(_L("Failed: Position received outside time range"));
							SetTestStepResult(EFail);									
						    }
					     					     
					    }
						
						CleanupStack::PopAndDestroy(&iPositioner);
						}
					}
					break;
					
				// Test case LBS-NotifyPosUpdate-0024						
				case 24: 					
				// Test case LBS-NotifyPosUpdate-0025
				case 25:
					{				
					    TInt profileid_ini;
				        TBool profileid_exists;
				        profileid_exists = GetIntFromConfig(ConfigSection(), KLbsProfileId, profileid_ini);
			            if(!profileid_exists)
							{
							INFO_PRINTF1(_L("Could not get profileid from section"));
				    		SetTestStepResult(EFail);
				    		}
											
						TLbsQualityProfileId profileId;
						//Set SelfLocate quality profile & valid id(which is exist in lbsprofile.ini) 
						User::LeaveIfError(iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, TLbsQualityProfileId(profileid_ini)));
						// Verify that the setting is what we expect
						User::LeaveIfError(iLbsAdmin->Get(KLbsSettingQualityProfileSelfLocate, profileId));
						if(profileId != TLbsQualityProfileId(profileid_ini))
							{
							SetTestStepResult(EFail);
							}
						if(testCaseId==24) 
						   {
							//makes test module timeout
							if (iParent.iSharedData->iTestModuleInUse)
								{
								T_LbsUtils utils;
								TModuleDataIn modDataIn;

								modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
								modDataIn.iTimeOut = KModuleTimeout;
							
								utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
								}
						   }
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
				
						if(iExpectedApiBehaviour == EApiVariant2)	// variant2 version - set the timeout in update options since LocServer doesn't use timeout from quality profile
							{
							TPositionUpdateOptions options;
							
		 					options.SetUpdateInterval(0);
		 					TTimeIntervalMicroSeconds maxFixTime;
		 					if(testCaseId==24) 
		 						{
		 						maxFixTime = 1000000; // 1s (shorter than module delay)
		 						}
		 					else
		 						{
		 						maxFixTime = 80000000; // 80s (longer than module delay)
		 						}
							options.SetUpdateTimeOut(maxFixTime); 					
		 					iPositioner.SetUpdateOptions(options);
									
							}						
						TPositionInfo posInfoNotifyUpd;
						//Do request to position update
                        err = DoNotifyUpdateL(posInfoNotifyUpd);
				        //Position received at
						
						if (testCaseId == 24)
							{
							if (err != KErrTimedOut)
								{
								INFO_PRINTF1(_L("<FONT><B>position update not failed with error -33</B></FONT>"));
								SetTestStepResult(EFail);
								}

							}
						else
							{
							if(iExpectedApiBehaviour == EApiVariant2)
								{
								// on timeout after only partial updates received (and partial not accepted) on s60 client gets 'timedout'
								if (err != KErrTimedOut)
									SetTestStepResult(EFail);								
								}
							else
								{
								if (err != KPositionQualityLoss)
									SetTestStepResult(EFail);								
								}
							}				
					    											
						CleanupStack::PopAndDestroy(&iPositioner);
					}
					break;

				// Test case LBS-NotifyPosUpdate-0027
				case 27:
				// Test case LBS-NotifyPosUpdate-0028
				case 28:
					{
					TInt profileid_ini;
			        TBool profileid_exists;
			        TPositionUpdateOptions options;
					T_LbsUtils utils;

					profileid_exists = GetIntFromConfig(ConfigSection(), KLbsProfileId, profileid_ini);
		            if(!profileid_exists)
						{
						INFO_PRINTF1(_L("Could not get profileid from ini file"));
			    		SetTestStepResult(EFail);
			    		}
					
					//Set SelfLocate quality profile & valid id(which is exist in lbsprofile.ini)
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, TLbsQualityProfileId(profileid_ini)));

					if(testCaseId==28) 
					   {
						//makes test module timeout
						const TTimeIntervalMicroSeconds KModuleTimeout = 60000000; // 1 minute
						if (iParent.iSharedData->iTestModuleInUse)
							{
							T_LbsUtils utils;
							TModuleDataIn modDataIn;

							modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
							modDataIn.iTimeOut = KModuleTimeout;
								
							utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
							}
					   }
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
			
					// Create a posinfos and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					TPositionInfo* posInfoUpd = new(ELeave) TPositionInfo();
					posInfoArr.Append(posInfoUpd);
					
					
 					options.SetUpdateInterval(0);
 					const TTimeIntervalMicroSeconds KMaxFix = 30000000; // 30s
					options.SetUpdateTimeOut(KMaxFix); 					
 					iPositioner.SetUpdateOptions(options);
	
					err = DoNotifyUpdateL(*posInfoUpd);
					
					if (testCaseId == 27)
						{
						if(iExpectedApiBehaviour == EApiVariant1)
							{
							if (err != KPositionQualityLoss)
								SetTestStepResult(EFail);
							}
						else
							{
							if (err != KErrNone)
								SetTestStepResult(EFail);							
							}
						}
					else
						{
						if (err != KErrTimedOut)
							SetTestStepResult(EFail);
						}				
					// Cleanup.
					CleanupStack::PopAndDestroy(&iPositioner);
					}
					break;									
				
				case 26:
					{
					TTimeIntervalMicroSeconds maxFixTime;
					
					if(iExpectedApiBehaviour == EApiVariant1)
						{
						maxFixTime = 5000000;
						}
					else
						{
						maxFixTime = 5000000 + 15000000;	// on S60 the client timeout includes the interval time
						}
					//Set up a 15 second interval between updates, with a 5 second timeout
					TPositionUpdateOptions posUpdateOpts(15000000, maxFixTime, 0, EFalse);

					// Open positioner.											    
					err = iPositioner.Open(iServer);
					User::LeaveIfError(err);
					if(err != KErrNone)
						{
						INFO_PRINTF1(_L("** SubSession Failed to open **"));
						SetTestStepResult(EFail);	
						}
					CleanupClosePushL(iPositioner);

					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
					CleanupStack::PushL(posInfo);

					iPositioner.SetUpdateOptions(posUpdateOpts);	
					User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
																CRequestor::EFormatApplication,
																_L("Tom Tom")));
					TRequestStatus status;		
																			
					//Ask for a position update (should get this one)
					iPositioner.NotifyPositionUpdate(*posInfo,status);

					User::WaitForRequest(status);						 	
					if(status != KErrNone)
						{
						INFO_PRINTF2(_L("Failed test, expected KErrNone, request finished with err %d."), status.Int());
						SetTestStepResult(EFail);
						}			   

const TTimeIntervalMicroSeconds KModuleTimeout = 60000000; // 1 minute
const TTimeIntervalMicroSeconds KModuleNoTimeout = 0; 
					// Set a delay in the test AGPS manager big enough to guarantee 
					// we won't get a response to the next one
					if (iParent.iSharedData->iTestModuleInUse)
						{
						T_LbsUtils utils;
						TModuleDataIn modDataIn;

						modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
						modDataIn.iTimeOut = KModuleTimeout;
						if(iExpectedApiBehaviour == EApiVariant2)
							{
							// turn off timeout after next update returned since by the time we turn off the timeout below it will be too late
							modDataIn.iOneOffTimeOut = ETrue;
							}
						
						utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
						}

					// Try again. Shouldn't get a response this time as it times out.
					iPositioner.NotifyPositionUpdate(*posInfo,status);

					User::WaitForRequest(status);	
					if(status != KErrTimedOut)
						{
						INFO_PRINTF2(_L("Failed test, expected KErrTimedOut, request finished with err %d."), status.Int());
						SetTestStepResult(EFail);
						}			   

					// Get rid of the AGPS manager delay.
					if (iParent.iSharedData->iTestModuleInUse && iExpectedApiBehaviour == EApiVariant1)
						{
						T_LbsUtils utils;
						TModuleDataIn modDataIn;

						modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
						modDataIn.iTimeOut = KModuleNoTimeout;
							
						utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
						}
	
					//Ask for a position update (should get this one again)
					iPositioner.NotifyPositionUpdate(*posInfo,status);

					User::WaitForRequest(status);						 	
					if(status != KErrNone)
						{
						INFO_PRINTF2(_L("Failed test, expected KErrNone, request finished with err %d."), status.Int());
						SetTestStepResult(EFail);
						}			   
						
					iPositioner.Close();

					CleanupStack::PopAndDestroy(posInfo);
					CleanupStack::PopAndDestroy(&iPositioner);
					} // case 26
				break;
				 // Test case LBS-NotifyPosUpdate-0122	
			    case 122:
					{
					const TLbsQualityProfileId KLbsTest0122QualityProfileId = 5;
					TLbsQualityProfileId profileId122(KLbsTest0122QualityProfileId);
					TLbsQualityProfileId oldProfileId(9);
    				TESTL(KErrNone==iLbsAdmin->Get(KLbsSettingQualityProfileSelfLocate, oldProfileId));
    				TESTL(KErrNone==iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, profileId122));
    				TESTL(KErrNone==iLbsAdmin->Get(KLbsSettingQualityProfileSelfLocate, profileId122));
   			
					if (profileId122 != KLbsTest0122QualityProfileId)
						{
						INFO_PRINTF2(_L("Fail test: profile should be 5 but its %d."), profileId122);
						User::Leave(EFail);
						}
					
					// the profile.ini file contains a MaxTime= 2147483647
					CT_LbsClientNotifyPosUpdAO* notifyPosUpdateAO;
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();
							
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					// this gets deleted by the base class destructor:
					posInfoArr.Append(posInfoNotifyUpd);
					
					TModuleDataIn modDataIn;

					modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
					modDataIn.iTimeOut = KModuleTimeout;
						
					utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
			
					// Start NotifyPositionUpdate request:
					notifyPosUpdateAO = CT_LbsClientNotifyPosUpdAO::NewL(*this);
					
					notifyPosUpdateAO->NotifyPosUpdateL(iPositioner, *posInfoNotifyUpd);
			//		User::After(3*1000*1000);
		
					TInt err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
					if(KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					// can start the scheduler now as there are async requests issued
					CActiveScheduler::Start();
						
					CleanupStack::PopAndDestroy(&iPositioner);	
					
	 				iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, oldProfileId);
  
				
					}
					break;
						
 				// Test that the final network position is not passed on to the client if they
 				// switch to autonomous mode mid-way through a session.
 				case 1001:
 					{
 					// Set preferred TB mode
 					// Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
 					CLbsAdmin::TGpsMode mode;
 					
 					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
 					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
 					
 					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
 						{
 						SetTestStepResult(EFail);
 						}
 					else	// Request a location update
 						{
 
 						// Open positioner.
 						User::LeaveIfError(iPositioner.Open(iServer));
 
 						const TTimeIntervalMicroSeconds KMicroSecondsValue(60*1000*1000);
 						TPositionUpdateOptions posOption;
 						posOption.SetUpdateTimeOut(KMicroSecondsValue);
 						User::LeaveIfError(iPositioner.SetUpdateOptions(posOption));
 
 						CleanupClosePushL(iPositioner);
 
 						// Create a posinfos and store in our shared array for later verification.
 						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
 								
 						T_LbsUtils utils;
 						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
 						
 						// Setup netsim.
 						User::LeaveIfError(OpenNetSim());
 						TLbsNetPosRequestQuality quality;
 						quality.SetMinHorizontalAccuracy(200);
 						quality.SetMinVerticalAccuracy(20000);
 						iNetSim.SetQuality(quality);
 						
 						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();
 
 						posInfoArr.Append(posInfoNotifyUpd);
 						
 						// Send the first NPUD & block on response
 						err = DoNotifyUpdateL(*posInfoNotifyUpd);
 						if (KErrNone != err)
 							{
 							SetTestStepResult(EFail);
 							}
 							
 						// Set autonomous mode
 						CLbsAdmin::TGpsMode mode;
 						
 						User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsAutonomous));
 						User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
 						
 						if(mode != CLbsAdmin::EGpsAutonomous)
 							{
 							SetTestStepResult(EFail);
 							}
 						else	// Request a location update
 							{
 							//TPositionModuleInfo::TTechnologyType techType;
 
 							// Send the second NPUD & block on response
 							err = DoNotifyUpdateL(*posInfoNotifyUpd);
 							
 							// This should not give a final network position response
 							//techType = posInfoNotifyUpd->PositionMode();
 							
 							if(err != KPositionQualityLoss)
 								{							
 								SetTestStepResult(EFail);
 								}
 							}
 							
 						CleanupStack::PopAndDestroy(&iPositioner);
 						CloseNetSim();
 						}
 
 					break;
 					}						
								
				// Test that the final network position is not passed on to the client if they
				// switch to autonomous mode mid-way through a session.
				case 1002:
					{
					// Set preferred TB mode
					// Set 'assisted-if-possible' (EGpsPreferTerminalBased) mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{

						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));

						const TTimeIntervalMicroSeconds KMicroSecondsValue(60*1000*1000);
						TPositionUpdateOptions posOption;
						posOption.SetUpdateTimeOut(KMicroSecondsValue);
						User::LeaveIfError(iPositioner.SetUpdateOptions(posOption));

						CleanupClosePushL(iPositioner);

						// Create a posinfos and store in our shared array for later verification.
						RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
								
						T_LbsUtils utils;
						utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
						
						// Setup netsim.
						User::LeaveIfError(OpenNetSim());
						TLbsNetPosRequestQuality quality;
						quality.SetMinHorizontalAccuracy(200);
						quality.SetMinVerticalAccuracy(20000);
						iNetSim.SetQuality(quality);
						
						TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();

						posInfoArr.Append(posInfoNotifyUpd);
						
						// Send the first NPUD & block on response
						err = DoNotifyUpdateL(*posInfoNotifyUpd);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
							
						TPositionModuleInfo::TTechnologyType techType;
						
						// Wait for 10 seconds to allow the FNP to come in then send the next NPUD
						User::After(10*1000*1000);

						// Send the second NPUD & block on response
						err = DoNotifyUpdateL(*posInfoNotifyUpd);
						
						// This should give a final network position response
						techType = posInfoNotifyUpd->PositionMode();
						
						if(techType != (TPositionModuleInfo::ETechnologyNetwork | 
										TPositionModuleInfo::ETechnologyAssisted))
							{							
							INFO_PRINTF1(_L("Failed test, did not get a final network position."));
							SetTestStepResult(EFail);
							}
							
						CleanupStack::PopAndDestroy(&iPositioner);
						CloseNetSim();
						}

					break;
					}
				
				//Tests within SetUpdateOptions test to ensure that the correct Location Position
				// is retured from the cache on a second NPUD (with correct MaxAge set).  This test
				// tests to make sure that if the last position to arrive was a reference position
				// that the reference position is returned
				case 1003:
					{
					//Setup in the TB mode
					CLbsAdmin::TGpsMode mode;
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, mode));
					
					if(mode != CLbsAdmin::EGpsPreferTerminalBased)
						{
						SetTestStepResult(EFail);
						}
					else	// Request a location update
						{
					
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);
						
						//Setup netsim
						User::LeaveIfError(OpenNetSim());
						TLbsNetPosRequestQuality quality;
						quality.SetMinHorizontalAccuracy(200);
						quality.SetMinVerticalAccuracy(20000);
						iNetSim.SetQuality(quality);
						
						//xx
			 			// Set the reference position, which also be used for any required verification.
			 			TPosition refPos;

						#define REFPOS_LAT				1
						#define REFPOS_LONG				1
						#define REFPOS_ALT				1
						#define REFPOS_HORZ_ACCURACY	1
						#define REFPOS_VERT_ACCURACY	1
						
						refPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
						refPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
						refPos.SetCurrentTime();
						iNetSim.SetReferenceLocation(refPos);
						User::After(500*1000);	
						//xx;
						//Do first NPUD, this should get a reference position on the reference cache
						TPositionInfo posInfo;
						TPositionInfo posInfob;
						
						// Send the first NPUD & block on response
						err = DoNotifyUpdateL(posInfo);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
						
						//Change the update options to set the MaxUpdateAge
						TPositionUpdateOptions posOpts;
						posOpts.SetMaxUpdateAge(1*1000000);
							
						iPositioner.SetUpdateOptions(posOpts);
						
						//Do the second NPUD, which should return a position from the cache
						// At this point in time the AGPS module will not yet have produced a position
						err = DoNotifyUpdateL(posInfob);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
						
						//Check to see if the returned position is a reference position
						TPositionModuleInfo::TTechnologyType techType;
						techType = posInfob.PositionMode();
						
						if(techType != TPositionModuleInfo::ETechnologyNetwork)
							{							
							INFO_PRINTF1(_L("Failed test, did not get a Network Reference Position."));
							SetTestStepResult(EFail);
							}
						
						//Since the second position is taken from the cache, check to see the two positions match
						T_LbsUtils utils;
						if(!utils.Compare_PosInfo(posInfo, posInfob))
							{
							INFO_PRINTF1(_L("Failed test, the two positions are not the same!"));
							SetTestStepResult(EFail);
							}
						
						CleanupStack::PopAndDestroy(&iPositioner);
						CloseNetSim();
						}
					
					break;
					}
				case 1004:
					{       
				     	TTimeIntervalMicroSeconds iTimeOut;
			     		TTime iStartTime;
	     				TPositionSatelliteInfo iPos, iPos2;
	     				TBool iEOP;
	     				// Open positioner.
     					User::LeaveIfError(iPositioner.Open(iServer));
	     				CleanupClosePushL(iPositioner);

	     				TPositionUpdateOptions updateOptions;
	     				iTimeOut = 2000000; // Two second timeout.
	     
	     				const TTimeIntervalMicroSeconds KTimeOut(iTimeOut);
	     				updateOptions.SetUpdateTimeOut(KTimeOut);
	     				updateOptions.SetAcceptPartialUpdates(ETrue);
     					iPositioner.SetUpdateOptions(updateOptions);
     
	     				// Store initial time
	    			 	iStartTime.UniversalTime();
         
     					TInt err = DoNotifyUpdateL(iPos);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
     
     					TTimeIntervalMicroSeconds timeLeft,maxUpdate;
	     				TTime cTime;
	     				//TPositionSatelliteInfo pos = iPos;
        
	     				cTime.UniversalTime();
	     				iEOP = EFalse;
     					while (!iEOP)
     						{
	     					cTime.UniversalTime();
	     					// Requesting another update must be the first thing done in RunL
     						timeLeft = (iTimeOut.Int64() - cTime.MicroSecondsFrom(iStartTime).Int64());
	     					maxUpdate = 100000;
    
    	 					if (timeLeft <= 0) 
     							{
	     						iEOP = ETrue;
	     						timeLeft = 1; // Use a very small value for the last update
		     					}
    	 					else
     							{
	     						const TTimeIntervalMicroSeconds KTimeOut(timeLeft);
	     						TPositionUpdateOptions posOpts;
	     						posOpts.SetUpdateTimeOut(KTimeOut);
	     						posOpts.SetMaxUpdateAge(maxUpdate);
	     						posOpts.SetAcceptPartialUpdates(ETrue);
	     						iPositioner.SetUpdateOptions(posOpts);
     
	     						err = DoNotifyUpdateL(iPos);
     							if (KErrNone != err)
     								{
     								SetTestStepResult(EFail);
	     							} 
     							else
		  	   						{	
									// We have a valid position, test passed
		     							SetTestStepResult(EPass);
	     								iEOP = ETrue;
	     								}
	     							}
		     				} // end while
	     				CleanupStack::PopAndDestroy(&iPositioner);
     					break;
					}								

				// Test case LBS-NotifyPosUpdate-0106 for demonstration of INC117582
				case 106:
					{
					
					TPositionModuleId id;
					iServer.GetDefaultModuleId(id);
					
					
					RPositioner positioner1;
					err = positioner1.Open(iServer, id);
					User::LeaveIfError(err);

					RPositioner positioner2;
					err = positioner2.Open(iServer, id);
					User::LeaveIfError(err);

					RPositioner positioner3;
					err = positioner3.Open(iServer, id);
					User::LeaveIfError(err);

					RPositioner positioner4;
					err = positioner4.Open(iServer, id);
					User::LeaveIfError(err);

					RPositioner positioner5;
					err = positioner5.Open(iServer, id);
					User::LeaveIfError(err);

					RPositioner positioner6;
					err = positioner6.Open(iServer, id);
					User::LeaveIfError(err);

					RPositioner positioner7;
					err = positioner7.Open(iServer, id);
					User::LeaveIfError(err);

					
					positioner1.Close();
					positioner2.Close();
					positioner3.Close();
					positioner6.Close();
					positioner7.Close();	// leaving 4 and 5 still open
					
					User::After(2000000);  //allow time for close to work through the locserver
					
				 
					break;
					}
				
				default:
					User::Panic(KLbsClientStepNotifyPosUpdate, KErrUnknown);					
						
    		    }
			}
			
		// All done, clean up.
		CleanupStack::PopAndDestroy(&iServer);		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_NotifyPosUpdate::doTestStepL()"));

	return TestStepResult();
	}

