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
// @file ctlbsclientstep_lastknownpos.cpp
// This is the class implementation for the Last Known Position Tests
// 
//
 
#include "ctlbsclientsteplastknownpos.h"

#include <lbs.h>
#include <lbs/lbsadmin.h>
#include "lbssatellite.h"
#include <lbs/EPos_CPosModules.h>
#include <lbs/EPos_CPosModuleUpdate.h>
#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>
#include "netpsy.hrh"

/**
 * Destructor
 */
CT_LbsClientStep_LastKnownPos::~CT_LbsClientStep_LastKnownPos()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_LastKnownPos::CT_LbsClientStep_LastKnownPos(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_LastKnownPos);
	}


/**
Static Constructor
*/
CT_LbsClientStep_LastKnownPos* CT_LbsClientStep_LastKnownPos::New(CT_LbsClientServer& aParent)
	{
    // Note the lack of ELeave.
    // This means that having insufficient memory will return NULL;
	CT_LbsClientStep_LastKnownPos* testStep =  new CT_LbsClientStep_LastKnownPos(aParent);

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

void CT_LbsClientStep_LastKnownPos::ConstructL()
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

void CT_LbsClientStep_LastKnownPos::SwitchOnselfLocateAPIL()
	{
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	adminApi->Set(KLbsSettingHomeSelfLocate, CLbsAdmin::ESelfLocateOn);	
	CleanupStack::PopAndDestroy(adminApi);
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_LastKnownPos::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_LastKnownPos::doTestStepL()"));

    if (TestStepResult() == EPass)
		{
		TInt err = KErrNone;
		
		// Wait for server to cease to be - thus ensuring cache is empty
		//User::After(10000000);   //Not needed
		
		// Carryout unique test actions.
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			if(testCaseId == 2)
				{
				//The delay below is to make sure that there is no request to server for at least 200 seconds
				//so that it will shutdown and clear cache.

				User::After(3*1000*1000);
				}

			// Connect to self locate server.
			User::LeaveIfError(iServer.Connect());
			CleanupClosePushL(iServer);
				
			switch (testCaseId)
				{
				// Test case LBS-LastKnownPos-0001
				case 1:
				// Test case LBS-LastKnownPos-0101
				case 101:
				    {
					T_LbsUtils utils;
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
                    if(testCaseId==101)
	                    {
	                    // For this test an additional reference position will be returned.
					
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
							INFO_PRINTF1(_L("Failed test, reference position incorrect."));
							SetTestStepResult(EFail);
							}	
	                    }
					
					
				    TPosition pos;		// original position
					TTime timeStamp;	// timestamp inside original received position
					
				    // Carry out a notify update to ensure last known cache is filled.
					TPositionInfo notifyPosInfo;
					User::LeaveIfError(DoNotifyUpdateL(notifyPosInfo));

					// get the position data:
					notifyPosInfo.GetPosition(pos);
					
					timeStamp = pos.Time();
					INFO_PRINTF1(_L("Original position timeStamp: "));
					INFO_PRINTF4(_L("%d/%d/%d"), timeStamp.DateTime().Day() + 1, timeStamp.DateTime().Month() + 1, timeStamp.DateTime().Year());
					INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeStamp.DateTime().Hour(), timeStamp.DateTime().Minute(), timeStamp.DateTime().Second(), timeStamp.DateTime().Second());
						
					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					posInfoArr.Append(posInfo);

					// Get the last known position
					err = DoLastKnownPosL(*posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					else
						{								
						TTime timeStamp_cached;		
						posInfo->GetPosition(pos);	
						timeStamp_cached = pos.Time();
						
						timeStamp = pos.Time();
						INFO_PRINTF1(_L("Cached position timeStamp: "));
						INFO_PRINTF4(_L("%d/%d/%d"), timeStamp_cached.DateTime().Day() + 1, timeStamp_cached.DateTime().Month() + 1, timeStamp_cached.DateTime().Year());
						INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeStamp_cached.DateTime().Hour(), timeStamp_cached.DateTime().Minute(), timeStamp_cached.DateTime().Second(), timeStamp_cached.DateTime().Second());
													
						// verify that the timestamp matches the original one:
						if(timeStamp_cached != timeStamp)
							{
							SetTestStepResult(EFail);
							}
						
						}
					CleanupStack::PopAndDestroy(&iPositioner);
					if (testCaseId == 101)
						{
						CloseNetSim();
						}
					break;
					}
				
				// NEGATIVE TEST
				// Test case LBS-LastKnownPos-0002
				case 2:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					// DONT carry out a notify update to ensure last known cache is filled.

					// Create a posinfo to store position info from LastKnownPosition method.
					TPositionInfo posInfo;

					// Get the last known position
					err = DoLastKnownPosL(posInfo);
					if (KErrUnknown != err)
						{
						INFO_PRINTF2(_L("Failed with error %d when expecting error KErrUnknown, indicating no position in cache"), err);
						SetTestStepResult(EFail);
						}
						
					// Wait for a time period and verify that location info is not published.
					// 10seconds
					User::After(10000000);
					
					// Get the last known position
					err = DoLastKnownPosL(posInfo);
					if (KErrUnknown != err)
						{
						INFO_PRINTF2(_L("Failed with error %d when expecting error KErrUnknown, indicating no position in cache"), err);
						SetTestStepResult(EFail);
						}
						
					CleanupStack::PopAndDestroy(&iPositioner);
					break;
					}
					

				// NEGATIVE TEST	
				// Test case LBS-LastKnownPos-0004
				case 4:
					{
					T_LbsUtils utils;
					TPositionModuleId networkModuleId = utils.GetNetworkModuleIdL(iServer);
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer, networkModuleId));
					CleanupClosePushL(iPositioner);

					// Setup netsim.
					User::LeaveIfError(OpenNetSim());
						
					// Carry out a notify update to ensure last known cache is filled.
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
						INFO_PRINTF1(_L("Failed test, reference position incorrect."));
						SetTestStepResult(EFail);
						}
				    
				    // ** Create a position info which is not supported by the network module **
					TPositionSatelliteInfo positionSatelliteInfo;

					// Get the last known position
					err = DoLastKnownPosL(positionSatelliteInfo);
					if (KErrArgument != err)
						{
						SetTestStepResult(EFail);
						}
												
					CleanupStack::PopAndDestroy(&iPositioner);
					// Close netsim.
					CloseNetSim();
					break;
					}
									
				// Test case LBS-LastKnownPos-0006
				case 6:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
											
					CLbsAdmin* adminApi = CLbsAdmin::NewL();
					CleanupStack::PushL(adminApi);
					
					adminApi->Set(KLbsSettingHomeSelfLocate, CLbsAdmin::ESelfLocateOff);
					
					TPositionInfo posInfo;

					// Get the last known position
					err = DoLastKnownPosL(posInfo);
					if (KErrAccessDenied != err)
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(adminApi);
					CleanupStack::PopAndDestroy(&iPositioner);

					SwitchOnselfLocateAPIL();

					break;		
					}

				// Test case LBS-LastKnownPos-0007
				case 7:
				// Test case LBS-LastKnownPos-0102
				case 102:
					{
					T_LbsUtils utils;
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
                    if(testCaseId==102)
	                    {
	                    // For this test an additional reference position will be returned.
					
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
							INFO_PRINTF1(_L("Failed test, reference position incorrect."));
							SetTestStepResult(EFail);
							}	
	                    }
														
					// Carry out a notify update to ensure last known cache is filled.
					TPositionInfo notifyPosInfo;
					TInt err = DoNotifyUpdateL(notifyPosInfo);
					User::LeaveIfError(err);

					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();

				
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					posInfoArr.Append(posInfo);

					// Get the last known position
					CT_LbsAsyncWaiter*	waiter = CT_LbsAsyncWaiter::NewL();
					CleanupStack::PushL(waiter);

					User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
																	CRequestor::EFormatApplication,
																	_L("Tom Tom")));
																	
					iPositioner.GetLastKnownPosition(*posInfo, waiter->iStatus);
									
					TInt errCancel = iPositioner.CancelRequest(EPositionerGetLastKnownPosition);
						
					waiter->StartAndWait();

					TInt errLkp = waiter->Result();

					CleanupStack::PopAndDestroy(waiter);
					
					// Sync Cancel() call returns either KErrNone or KErrNotFound. 
					if ( errCancel != KErrNone && errCancel != KErrNotFound )
						{
						INFO_PRINTF1(_L("<FONT><B>Cancel must return KErrNone or KErrNotFound</B></FONT>"));
						SetTestStepResult(EFail);									
						}
					// If Cancel() returns KErrNone, then the original request may complete with KErrCancel.
					if (errCancel != KErrNone &&  (errLkp != KErrCancel && errLkp != KErrNone )  )
						{
						INFO_PRINTF1(_L("<FONT><B>Cancel must return KErrCancel or KErrNone</B></FONT>"));
						SetTestStepResult(EFail);									
						}
						
						
					CleanupStack::PopAndDestroy(&iPositioner);
					if (testCaseId == 102)
						{
						CloseNetSim();
						}			
					break;		
					}		
					
				// Test case LBS-LastKnownPos-0008
				case 8:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					// DONT get the last known position, but DO cancel
					err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
					
					if (KErrNotFound != err)
						{
						//Didnt cancel or complete successfully.
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&iPositioner);
									
					break;		
					}	
					

				// Test case LBS-LastKnownPos-0009
				case 9:
					{
					//DONT open connection to Position Server

					TPositionInfo posInfo;

					// Get the last known position, THIS SHOULD PANIC WITH EPositionServerBadHandle
					TRequestStatus status;
					iPositioner.GetLastKnownPosition(posInfo, status);
					err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);

					if (KErrNone == err)
						{
						//Completed Successfully.
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&iPositioner);
					
					SwitchOnselfLocateAPIL();
					
					break;		
					}	
					

				// Test case LBS-LastKnownPos-0012
				case 12:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					// Carry out a notify update to ensure last known cache is filled.
					TPositionInfo notifyPosInfo;
					User::LeaveIfError(DoNotifyUpdateL(notifyPosInfo));

					TPositionInfo posInfo;

					// Get the last known position while self Locate Server is busy
					err = DoLastKnownPosL(posInfo);

					if (KErrServerBusy != err)
						{
						// Completed successfully.
						INFO_PRINTF1(_L("<FONT><B>**Test should pass if GetLastKnownPosition() fails with KErrServerBusy **</B></FONT>"));
						SetTestStepResult(EFail);
						}
					CleanupStack::PopAndDestroy(&iPositioner);
					break;	
					}
			
			
				// Test case LBS-LastKnownPos-0014
				case 14:
					{
					// Testcase placeholder
					// Request multiples LKPs from different clients concurrently 
					SetTestStepResult(EFail);						
					break;
					}
			
			
				// Test case LBS-LastKnownPos-0015
				case 15:
				// Test case LBS-LastKnownPos-0103
				case 103:
					{
					T_LbsUtils utils;
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);		
					if(testCaseId==103)
	                    {
	                    // For this test an additional reference position will be returned.
					
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
							INFO_PRINTF1(_L("Failed test, reference position incorrect."));
							SetTestStepResult(EFail);
							}	
	                    }
																				
					// Carry out a notify update to ensure last known cache is filled.
					TPositionInfo notifyPosInfo;
					TInt err = DoNotifyUpdateL(notifyPosInfo);
					User::LeaveIfError(err);
					
					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					
					
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
					
					CT_LbsAsyncWaiter*	waiter = CT_LbsAsyncWaiter::NewL();
					CleanupStack::PushL(waiter);
					
					TInt numpositions = iParent.iSharedData->iVerifyPosInfoArr.Count();
					while(numpositions--)
						{
						RPositioner positioner;
						// Open positioner.
						User::LeaveIfError(positioner.Open(iServer));
						CleanupClosePushL(positioner);		
												
						TPositionInfo* posInfo = new(ELeave) TPositionInfo();
						posInfoArr.Append(posInfo);

						// LastKnownPosition from multiple sub sessions

						User::LeaveIfError(positioner.SetRequestor(	CRequestor::ERequestorService,
																		CRequestor::EFormatApplication,
																		_L("Tom Tom")));
																		
						positioner.GetLastKnownPosition(*posInfo, waiter->iStatus);
						waiter->StartAndWait();
						TInt res = waiter->Result();
						
						if(KErrNone != res)
							{
							//One of the LKPs failed, so test fails.
							SetTestStepResult(EFail);
							}
						CleanupStack::PopAndDestroy(&positioner);
						}

						CleanupStack::PopAndDestroy(waiter);
						CleanupStack::PopAndDestroy(&iPositioner);
					    if (testCaseId == 103)
							{
							CloseNetSim();
							}
					break;	
					}
				// Test case LBS-LastKnownPos-0016
				case 16:
				// Test case LBS-LastKnownPos-0104
				case 104:
					{
					T_LbsUtils utils;
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
                    if(testCaseId==104)
	                    {
	                    // For this test an additional reference position will be returned.
					
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
							INFO_PRINTF1(_L("Failed test, reference position incorrect."));
							SetTestStepResult(EFail);
							}	
	                    }
		
					// Carry out a notify update to ensure last known cache is filled.
					TPositionInfo notifyPosInfo;
					User::LeaveIfError(DoNotifyUpdateL(notifyPosInfo));

					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;

				
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					TInt numPositions = iParent.iSharedData->iVerifyPosInfoArr.Count();
					while(numPositions--)
						{
						TPositionInfo* posInfo = new(ELeave) TPositionInfo();
						posInfoArr.Append(posInfo);
						
						err = DoLastKnownPosL(*posInfo);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
						}
					
					CleanupStack::PopAndDestroy(&iPositioner);
					if (testCaseId == 104)
						{
						CloseNetSim();
						}
					break;	
					}
				// Test case LBS-LastKnownPos-0105	
				case 105:
			    	{
			    	T_LbsUtils utils;
			    	TPositionModuleId networkModuleId = utils.GetNetworkModuleIdL(iServer);
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer, networkModuleId));
					CleanupClosePushL(iPositioner);

					// Setup netsim.
					User::LeaveIfError(OpenNetSim());
						
					TPosition pos;		// original position
					TTime timeStamp;	// timestamp inside original received position
					
					// Carry out a notify update to ensure last known cache is filled.
					TPositionInfo actualRefPosInfo;
						
					err = DoNotifyUpdateL(actualRefPosInfo);
										
					if (err)
						{
						INFO_PRINTF2(_L("Failed test, reference position request returned err %d."), err);
						SetTestStepResult(EFail);							
						}
												
					// get the position data:
					actualRefPosInfo.GetPosition(pos);
					
					timeStamp = pos.Time();
					INFO_PRINTF1(_L("Original position timeStamp: "));
					INFO_PRINTF4(_L("%d/%d/%d"), timeStamp.DateTime().Day() + 1, timeStamp.DateTime().Month() + 1, timeStamp.DateTime().Year());
					INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeStamp.DateTime().Hour(), timeStamp.DateTime().Minute(), timeStamp.DateTime().Second(), timeStamp.DateTime().Second());
						
					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					posInfoArr.Append(posInfo);

					// Get the last known position
					err = DoLastKnownPosL(*posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					else
						{								
						TTime timeStamp_cached;		
						posInfo->GetPosition(pos);	
						timeStamp_cached = pos.Time();
						
						timeStamp = pos.Time();
						INFO_PRINTF1(_L("Cached position timeStamp: "));
						INFO_PRINTF4(_L("%d/%d/%d"), timeStamp_cached.DateTime().Day() + 1, timeStamp_cached.DateTime().Month() + 1, timeStamp_cached.DateTime().Year());
						INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeStamp_cached.DateTime().Hour(), timeStamp_cached.DateTime().Minute(), timeStamp_cached.DateTime().Second(), timeStamp_cached.DateTime().Second());
													
						// verify that the timestamp matches the original one:
						if(timeStamp_cached != timeStamp)
							{
							SetTestStepResult(EFail);
							}
						
						}
														
					CleanupStack::PopAndDestroy(&iPositioner);
					// Close netsim.
					CloseNetSim();
					break;
					}
				
				default:
					{
					User::Leave(KErrArgument);
					}
				}
			}

		// All done, clean up.

		CleanupStack::PopAndDestroy(&iServer);		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_LastKnownPos::doTestStepL()"));

	return TestStepResult();
	}

