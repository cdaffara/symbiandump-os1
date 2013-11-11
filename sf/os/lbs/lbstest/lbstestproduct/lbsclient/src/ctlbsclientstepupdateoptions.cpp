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
// @file ctlbsclientstep_UpdateOptions.cpp
// This is the class implementation for the UpdateOptions Tests
// 
//
 
#include "ctlbsclientstepupdateoptions.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

#define EXPECTED_GPS_ACCURACY_HORIZONTAL 100
#define EXPECTED_GPS_ACCURACY_VERTICAL 100

const TTimeIntervalMicroSeconds KTimestampMargin = 500000; //+/-0.5s

/**
 
 */
TBool CT_LbsClientStep_UpdateOptions::VerifyFirstPosTime(const TTime& aTimeRequested, const TPositionInfo& aPosInfo, const TPositionUpdateOptions& aUpdateOpts)
	{
	TTime timeReceived, timeStamp;
	TPosition pos;
					
    TInt64 maxtime;
    maxtime = aUpdateOpts.UpdateTimeOut().Int64();
	if(!maxtime)	
		{
		#pragma message("TO DO: CT_LbsClientStep_UpdateOptions::VerifyFirstPosTime - Update when moduleinfo supported")	
		const TTimeIntervalMicroSeconds ttff = 90*1000000;	// TO DO - this should be retrieved from the module info (currently hardcoded in locserver too)
		maxtime = ttff.Int64();
		}
										
	// get current time
	timeReceived.UniversalTime();
	
	// get the position data:
	aPosInfo.GetPosition(pos);
	
	INFO_PRINTF1(_L("First position received at: "));	
	INFO_PRINTF4(_L("%d/%d/%d"), timeReceived.DateTime().Day() + 1, timeReceived.DateTime().Month() + 1, timeReceived.DateTime().Year());
	INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeReceived.DateTime().Hour(), timeReceived.DateTime().Minute(), timeReceived.DateTime().Second(), timeReceived.DateTime().MicroSecond());
	
	timeStamp = pos.Time();
	INFO_PRINTF1(_L("First position timeStamp: "));
	INFO_PRINTF4(_L("%d/%d/%d"), timeStamp.DateTime().Day() + 1, timeStamp.DateTime().Month() + 1, timeStamp.DateTime().Year());
	INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeStamp.DateTime().Hour(), timeStamp.DateTime().Minute(), timeStamp.DateTime().Second(), timeStamp.DateTime().MicroSecond());	
	
	TInt64 timediff;
	timediff = timeReceived.Int64() - aTimeRequested.Int64();
	
	// *first* position received can arrive 'immediately' and up to timeout, or maxtimetofirstfix if timeout not specified
	if(timediff > maxtime)
		{
		INFO_PRINTF1(_L("Failed: First Position received outside time range"));
		return FALSE;									
		}
	else
		{
		return TRUE;
		}
	
	}
	
	
/**
 aPosCount - which position in a set of periodic positions this is (first position = 0)
 */
TBool CT_LbsClientStep_UpdateOptions::VerifyPosTime(const TTime& aTimeFirstPosStamped, const TPositionUpdateOptions& aUpdateOpts, TPositionInfo& aPosInfo, TInt aPosCount)
	{
	TTime timeReceived, timeStamp;
	TPosition pos;
    TInt64 interval = aUpdateOpts.UpdateInterval().Int64();	
    TTimeIntervalMicroSeconds timeout;
    timeout = aUpdateOpts.UpdateTimeOut().Int64();
    	
    // should call different function for first position:		
    __ASSERT_ALWAYS(aPosCount > 0, User::Panic(_L("Update options test"), KErrArgument));				
    
	// get current time
	timeReceived.UniversalTime();
	
	// get the position data:
	aPosInfo.GetPosition(pos);
	
	INFO_PRINTF1(_L("Position received at: "));	
	INFO_PRINTF4(_L("%d/%d/%d"), timeReceived.DateTime().Day() + 1, timeReceived.DateTime().Month() + 1, timeReceived.DateTime().Year());
	INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeReceived.DateTime().Hour(), timeReceived.DateTime().Minute(), timeReceived.DateTime().Second(), timeReceived.DateTime().MicroSecond());
	
	timeStamp = pos.Time();
	INFO_PRINTF1(_L("Position timeStamp: "));
	INFO_PRINTF4(_L("%d/%d/%d"), timeStamp.DateTime().Day() + 1, timeStamp.DateTime().Month() + 1, timeStamp.DateTime().Year());
	INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeStamp.DateTime().Hour(), timeStamp.DateTime().Minute(), timeStamp.DateTime().Second(), timeStamp.DateTime().MicroSecond());

	TTime windowstart = aTimeFirstPosStamped + static_cast<TTimeIntervalMicroSeconds>(aPosCount * interval) - KTimestampMargin;
	TTime windowend = windowstart + timeout + KTimestampMargin;

	INFO_PRINTF1(_L("valid window start: "));	
	INFO_PRINTF4(_L("%d/%d/%d"), windowstart.DateTime().Day() + 1, windowstart.DateTime().Month() + 1, windowstart.DateTime().Year());
	INFO_PRINTF5(_L("at %d :%d :%d :%d"), windowstart.DateTime().Hour(), windowstart.DateTime().Minute(), windowstart.DateTime().Second(), windowstart.DateTime().MicroSecond());

	if(timeStamp < windowstart)
		{
		INFO_PRINTF1(_L("Failed: Position received outside time range"));
		return FALSE;
		}
	if(timeout.Int64() && (timeStamp > windowend))
		{
		INFO_PRINTF1(_L("valid window end: "));	
		INFO_PRINTF4(_L("%d/%d/%d"), windowend.DateTime().Day() + 1, windowend.DateTime().Month() + 1, windowend.DateTime().Year());
		INFO_PRINTF5(_L("at %d :%d :%d :%d"), windowend.DateTime().Hour(), windowend.DateTime().Minute(), windowend.DateTime().Second(), windowend.DateTime().MicroSecond());
		
		return FALSE;
		}
	else
		{
		return TRUE;
		}
	}
	



// constant definitions
_LIT(KLbsClientStepUpdateOptions, "LbsClientStepUpdateOptions");

/**
 *	Sets a Requestor for now - TO DO will be removed with new location server?
 */
void CT_LbsClientStep_UpdateOptions::TempSetRequestorL()
 	{	
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
												CRequestor::EFormatApplication,
												_L("Tom Tom")));
 	}


/**
 * Destructor
 */
CT_LbsClientStep_UpdateOptions::~CT_LbsClientStep_UpdateOptions()
	{
	delete 	iLbsAdmin;	
	}


/**
 * Constructor
 */
CT_LbsClientStep_UpdateOptions::CT_LbsClientStep_UpdateOptions(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent), iLbsAdmin(NULL), iRequestedAssData(FALSE), iReceivedAssData(FALSE)
	{
	SetTestStepName(KLbsClientStep_UpdateOptions);
	}


/**
Static Constructor
*/
CT_LbsClientStep_UpdateOptions* CT_LbsClientStep_UpdateOptions::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_UpdateOptions(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}	
	

/**
Compare UpdateOptions
*/
TBool CT_LbsClientStep_UpdateOptions::CompareUpdateOptions(TPositionUpdateOptions aPosA, TPositionUpdateOptions aPosB)
	{
	TBool isEqual = ETrue;

	if(aPosA.AcceptPartialUpdates() != aPosB.AcceptPartialUpdates()) 
		{
			return EFalse;
		}
		
	if(aPosA.MaxUpdateAge() != aPosB.MaxUpdateAge())
		{
			return EFalse;
		}
		
	if(aPosA.PositionClassSize() != aPosB.PositionClassSize())
		{
			return EFalse;
		}
		
	if(aPosA.PositionClassType() != aPosB.PositionClassType())
		{
			return EFalse;
		}
		
	if(aPosA.UpdateInterval() != aPosB.UpdateInterval())
		{
			return EFalse;
		}
		
	if(aPosA.UpdateTimeOut() != aPosB.UpdateTimeOut())
		{
			return EFalse;
		}
		
	return isEqual;
	
	}
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_UpdateOptions::doTestStepL()
	{
	// Generic test step used to test the LBS SetUpdateOptions API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_UpdateOptions::doTestStepL()"));

	if (TestStepResult()==EPass) 
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
				// Test case LBS-UpdateOptions-001
				case 1:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					TPositionUpdateOptions optsA;
					
					TPositionUpdateOptions optsB;
					iPositioner.SetUpdateOptions(optsA);
					iPositioner.GetUpdateOptions(optsB);
					if (!CompareUpdateOptions(optsA, optsB))
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&iPositioner);
					
					break;
					}
				
				// Test case LBS-UpdateOptions-002
				case 2:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					//(aInterval, aTimeOut, aMaxAge, TBool aAcceptPartialUpdates=EFalse)
					TPositionUpdateOptions optsA(-1, 0, 0, EFalse);
					
					CleanupStack::PopAndDestroy(&iPositioner);
					
					break;
					}
					
				// Test case LBS-UpdateOptions-003
				case 3:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					TPositionUpdateOptions optsA(0, -1, 0, EFalse);
					
					CleanupStack::PopAndDestroy(&iPositioner);
					
					break;
					}	
					
				// Test case LBS-UpdateOptions-004
				case 4:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					TPositionUpdateOptions optsA(0, 0, -1, EFalse);
					
					CleanupStack::PopAndDestroy(&iPositioner);
					
					break;
					}	
					
					
				// Test case LBS-UpdateOptions-005
				case 5:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					
					TPositionUpdateOptions& updateOpts = iParent.iSharedData->iVerifyUpdateOpts;
					iPositioner.SetUpdateOptions(updateOpts);
					
					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					posInfoArr.Append(posInfo);
	
					TTime timeRequested;
					timeRequested.UniversalTime();
					INFO_PRINTF1(_L("Position requested at: "));	
					INFO_PRINTF4(_L("%d/%d/%d"), timeRequested.DateTime().Day() + 1, timeRequested.DateTime().Month() + 1, timeRequested.DateTime().Year());
					INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeRequested.DateTime().Hour(), timeRequested.DateTime().Minute(), timeRequested.DateTime().Second(), timeRequested.DateTime().MicroSecond());
					

					// Get the actual position
					err = DoNotifyUpdateL(*posInfo);
					if (KErrNone != err || !VerifyFirstPosTime(timeRequested, *posInfo, updateOpts))
						{
						SetTestStepResult(EFail);
						}
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
				// Test case LBS-UpdateOptions-006
				case 6:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					// Create a posinfo and store in our shared array for later verification.
					TPositionInfo* posInfo = new(ELeave) TPositionInfo();
					posInfoArr.Append(posInfo);					
					
					TRequestStatus status;
										
					TPositionUpdateOptions tempOpts;
					// set a fairly long interval (1 minute) to ensure the 2nd request doesn't complete too quicky:
					tempOpts.SetUpdateInterval(60*1000000);
					tempOpts.SetUpdateTimeOut(300*1000000);
					iPositioner.SetUpdateOptions(tempOpts);
					iPositioner.GetUpdateOptions(tempOpts);			
					
					TempSetRequestorL();
					
					TTime timeRequested;					
					TTime timeFirstStamped;							
					
					INFO_PRINTF2(_L("Requesting position (current interval set to %d): "),tempOpts.UpdateInterval().Int64() );
					timeRequested.UniversalTime();
					err = DoNotifyUpdateL(*posInfo);
					if (KErrNone != err)
						{
						INFO_PRINTF2(_L("Failed: Position Update failed with %d"), err);
						SetTestStepResult(EFail);
						}
					else
						{
						TPosition pos;
						posInfo->GetPosition(pos);
						
						// need to remember the time we received this one:		
						timeFirstStamped = pos.Time();		
						if(!VerifyFirstPosTime(timeRequested, *posInfo, tempOpts))
							{
							SetTestStepResult(EFail);									
							}
						}
						
					// Create another posinfo and store in our shared array for later verification.						
					posInfo = new(ELeave) TPositionInfo();
					posInfoArr.Append(posInfo);					
											
					INFO_PRINTF2(_L("Requesting position (current interval set to %d): "), tempOpts.UpdateInterval().Int64());
					
					// this should complete after original interval (1 min), despite setting a new interval while it's outstanding:
					iPositioner.NotifyPositionUpdate(*posInfo, status);	

					// set configured update options while update still outstanding:
					TPositionUpdateOptions& updateOpts = iParent.iSharedData->iVerifyUpdateOpts;					
					iPositioner.SetUpdateOptions(updateOpts);
					iPositioner.GetUpdateOptions(updateOpts);
					
					INFO_PRINTF2(_L("Interval now set to %d microseconds"), updateOpts.UpdateInterval().Int64());
					
					User::WaitForRequest(status);
					
					// verify time for 'outstanding when interval changed' position										
					if(!VerifyPosTime(timeFirstStamped, tempOpts, *posInfo, 1))
						{
						INFO_PRINTF1(_L("CT_LbsClientStep_UpdateOptions::doTestStepL() Failed: Time received outside valid range"));
						SetTestStepResult(EFail);									
						}
					else
						{
						TInt numOfPosInfos = iParent.iSharedData->iVerifyPosInfoArr.Count() - 2;	// -2 because first 2 positions used above
						for(TInt i = 0 ; i < numOfPosInfos ; i++)
							{
							// get current time
							timeRequested.UniversalTime();
							INFO_PRINTF1(_L("Position requested at: "));	
							INFO_PRINTF4(_L("%d/%d/%d"), timeRequested.DateTime().Day() + 1, timeRequested.DateTime().Month() + 1, timeRequested.DateTime().Year());
							INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeRequested.DateTime().Hour(), timeRequested.DateTime().Minute(), timeRequested.DateTime().Second(), timeRequested.DateTime().MicroSecond());
							
							//Do request to position update
							posInfo = new(ELeave) TPositionInfo();
							posInfoArr.Append(posInfo);					
							
							err = DoNotifyUpdateL(*posInfo);
							if (KErrNone != err)
								{
								INFO_PRINTF2(_L("Failed: Position Update failed with %d"), err);
								SetTestStepResult(EFail);
								}
							else
								{	
								if(i > 0)
									{							
									if(!VerifyPosTime(timeFirstStamped, updateOpts, *posInfo, i))
										{
										INFO_PRINTF1(_L("Failed: Position received outside time range"));
										SetTestStepResult(EFail);
										}
									}
								else	// first position
									{
									TPosition pos;
									
									posInfo->GetPosition(pos);
									
									// need to remember the time we received this one:		
									timeFirstStamped = pos.Time();		
									if(!VerifyFirstPosTime(timeRequested, *posInfo, updateOpts))
										{
										SetTestStepResult(EFail);									
										}
									}		
								}	
							}
						}

					CleanupStack::PopAndDestroy(&iPositioner);
					
					break;
					}
					
					
				// Test case LBS-UpdateOptions-007
				case 7:
				case 107:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);


					// For this test an additional reference position will be returned.
					if (testCaseId == 107)
						{
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
						T_LbsUtils utils;
						TPositionInfo verifyRefPosInfo;
						
						verifyRefPosInfo.SetPosition(iRefPos);
						if (!utils.Compare_PosInfo(verifyRefPosInfo, actualRefPosInfo))
							{
							INFO_PRINTF1(_L("Failed test, reference position incorrect."));
							SetTestStepResult(EFail);
							}
						}
					
					
					TPositionUpdateOptions& updateOpts = iParent.iSharedData->iVerifyUpdateOpts;
															
					iPositioner.SetUpdateOptions(updateOpts);
					
					// verify that the set worked:
					ASSERT(updateOpts.UpdateInterval() == iParent.iSharedData->iVerifyUpdateOpts.UpdateInterval());
					
					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					TPositionInfo* posInfo;
					TTime timeRequested;
					
					iPositioner.GetUpdateOptions(updateOpts);
										
					TInt  numOfPosInfos = iParent.iSharedData->iVerifyPosInfoArr.Count();
					TTime timeFirstStamped;
					for(TInt i = 0 ; i < numOfPosInfos ; i++)
						{
						// get current time
						timeRequested.UniversalTime();
						INFO_PRINTF1(_L("Position requested at: "));	
						INFO_PRINTF4(_L("%d/%d/%d"), timeRequested.DateTime().Day() + 1, timeRequested.DateTime().Month() + 1, timeRequested.DateTime().Year());
						INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeRequested.DateTime().Hour(), timeRequested.DateTime().Minute(), timeRequested.DateTime().Second(), timeRequested.DateTime().MicroSecond());

						//Do request to position update
						posInfo = new(ELeave) TPositionInfo();
						posInfoArr.Append(posInfo);
																		
						err = DoNotifyUpdateL(*posInfo);
						if (KErrNone != err)
							{
							INFO_PRINTF2(_L("NotifyUpdate failed with %d "), err);	
							SetTestStepResult(EFail);
							}
						else
							{
							if(i > 0)
								{							
								if(!VerifyPosTime(timeFirstStamped, updateOpts, *posInfo, i))
									{
									INFO_PRINTF1(_L("Failed: Position received outside time range"));
									SetTestStepResult(EFail);
									}
								}
							else	// first position
								{
								TPosition pos;
								
								posInfo->GetPosition(pos);
								
								// need to remember the time we received this one:		
								timeFirstStamped = pos.Time();		
								if(!VerifyFirstPosTime(timeRequested, *posInfo, updateOpts))
									{
									SetTestStepResult(EFail);									
									}
								}				
							}
						}

					// Clean up.						
					CleanupStack::PopAndDestroy(&iPositioner);
					if (testCaseId == 107)
						{
						CloseNetSim();
						}

					break;
					}
					
				// Test case LBS-UpdateOptions-008
				case 8:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					
					//2 second interval
					TPositionUpdateOptions optsA(2000000, 0, 0, EFalse);
					optsA.SetUpdateInterval(-1);
					
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
				// Test case LBS-UpdateOptions-009
				case 9:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					

					//10 second interval
					TPositionUpdateOptions posUpdateOptsA(10000000, 0, 0, EFalse);
					TPositionUpdateOptions posUpdateOptsB; 
					iPositioner.SetUpdateOptions(posUpdateOptsA);
					iPositioner.GetUpdateOptions(posUpdateOptsB);
					
					if(!CompareUpdateOptions(posUpdateOptsA, posUpdateOptsB))
						{
						INFO_PRINTF1(_L("SetUpdate and GetUpdate comparison didn't match"));
						SetTestStepResult(EFail);
						}	
												
					//5 second timeout
					posUpdateOptsA.SetUpdateTimeOut(5000000);
					iPositioner.SetUpdateOptions(posUpdateOptsA);
					iPositioner.GetUpdateOptions(posUpdateOptsB);
					
					if(!CompareUpdateOptions(posUpdateOptsA, posUpdateOptsB))
						{
						INFO_PRINTF1(_L("SetUpdate and GetUpdate comparison didn't match"));
						SetTestStepResult(EFail);
						}
						
					TPositionInfo posInfo;
					
					// TO DO - is this sufficient, or should we really be testing how long it takes to timeout?
	
					// Get the actual position, should time out on AGPS side
					// ** Configure test gps module to cause a timeout
					err = DoNotifyUpdateL(posInfo);
					if (KErrTimedOut != err)
						{
						INFO_PRINTF1(_L("<FONT><B>AGPS module didn't timeout as expected.</B></FONT>"));
						SetTestStepResult(EFail);
						}						
						
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
				// Test case LBS-UpdateOptions-010
				case 10:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					
					TPositionUpdateOptions optsA;
					optsA.SetUpdateTimeOut(-1);
						
					CleanupStack::PopAndDestroy(&iPositioner);
					break;
					}
	
					
				// Test case LBS-UpdateOptions-011
				case 11:
					{
					// Open positioner with
					// configured required quality
					TPositionCriteria	criteria;
					TPositionQuality    quality;
					
					// set up the required accuracy
					quality.SetHorizontalAccuracy(EXPECTED_GPS_ACCURACY_HORIZONTAL);
					quality.SetVerticalAccuracy(EXPECTED_GPS_ACCURACY_VERTICAL);
					
					criteria.SetRequiredQuality(quality);
					User::LeaveIfError(iPositioner.Open(iServer, criteria));
					CleanupClosePushL(iPositioner);
					
					//Configure partial Updates
					TPositionUpdateOptions optsA;
					optsA.SetAcceptPartialUpdates(EFalse);
					if(EFalse != optsA.AcceptPartialUpdates())
						{
						INFO_PRINTF1(_L("Partial Updates not set."));
						SetTestStepResult(EFail);
						}	
					
					iPositioner.SetUpdateOptions(optsA);
					
					
					// NOTE:
					// -Expect to call NPU once, and have module return 2 partial updates
					//  before finally returning the complete update.
					// -We need a way to inform the module that it nedes to return partial updates
					//  When we have AcceptPartialUpdates SwitchedOff.
										
					// ** Test AGPS manager must already be configured to return partial location info
					// ** We should only see the full location info to required accuracy etc
					TPositionInfo positionInfo;
					TPositionInfo positionInfoTemp = reinterpret_cast<TPositionInfo&>(iParent.iSharedData->iVerifyPosInfoArr[2]);
	
					// Get the actual position
					err = DoNotifyUpdateL(positionInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					else
						{
						//Compare posInfo with Moduledata 3
						T_LbsUtils utils;
						if(!utils.Compare_PosInfo(positionInfo, positionInfoTemp))
							{
							INFO_PRINTF1(_L("<FONT><B>We didn't recieve complete update.</B></FONT>"));
							SetTestStepResult(EFail);
							}
						}
																
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
	
				// Test case LBS-UpdateOptions-012
				case 12:
					{
					// Open positioner with
					// configured required quality
					TPositionCriteria	criteria;
					TPositionQuality    quality;
					
					// set up the required accuracy
					quality.SetHorizontalAccuracy(EXPECTED_GPS_ACCURACY_HORIZONTAL);
					quality.SetVerticalAccuracy(EXPECTED_GPS_ACCURACY_VERTICAL);
					
					criteria.SetRequiredQuality(quality);
					err = iPositioner.Open(iServer, criteria);
					User::LeaveIfError(err);
					CleanupClosePushL(iPositioner);
					
					//Configure partial Updates
					TPositionUpdateOptions optsA;
					optsA.SetAcceptPartialUpdates(ETrue);
					if(!optsA.AcceptPartialUpdates())
						{
						INFO_PRINTF1(_L("Partial Updates not set."));
						SetTestStepResult(EFail);
						}	
					
					err = iPositioner.SetUpdateOptions(optsA);
					User::LeaveIfError(err);
					
					// NOTE:
					// -Expect to call NPU repeatedly to recieve each partial update
					// -We need a way to inform the module that it nedes to return partial updates
					//  When we have AcceptPartialUpdates SwitchedOff.
										
					// ** Test AGPS manager must already be configured to return partial location info
					// ** We should only see the full location info to required accuracy etc

					// Create a posinfo and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					TInt loopCount = iParent.iSharedData->iCurrentPosInfoArr.Count()-1;
					// Get the actual position
					for(int i = 0 ; i < loopCount ; i++ )
						{
						TPositionInfo* positionInfo = new(ELeave) TPositionInfo();
						posInfoArr.Append(positionInfo);
						
						err = DoNotifyUpdateL(*positionInfo);
						if (KErrNone != err)
							{
							SetTestStepResult(EFail);
							}	
						}
									
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
				
				// Test case LBS-UpdateOptions-013
				case 13:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);					
					
					TPositionInfo posInfo;
					TPositionInfo posInfob;
	
					// This call to NPU puts a location in cache
					err = DoNotifyUpdateL(posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					// TO DO - read these from config?
					TPositionUpdateOptions posOpts;
					posOpts.SetMaxUpdateAge(1*1000000);
					posOpts.SetUpdateInterval(5*1000000);
					
					if((posOpts.MaxUpdateAge() != 1000000) || (posOpts.UpdateInterval() != 5000000))
						{
						SetTestStepResult(EFail);
						}
						
					iPositioner.SetUpdateOptions(posOpts);
											
					//Retrieve cached location with second call to NPU
					err = DoNotifyUpdateL(posInfob);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					
					//INTERNAL COMPARISON
					//Make sure we've retrieved the cached location						
					T_LbsUtils utils;
					if(!utils.Compare_PosInfo(posInfo, posInfob))
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}	
					
			
					
					
					
				// Test case LBS-UpdateOptions-014
				case 14:
					{
					T_LbsUtils utils;
					TPositionModuleId networkModId = utils.GetNetworkModuleIdL(iServer);
									
					// Open positioners.
					User::LeaveIfError(iPositioner.Open(iServer, networkModId));
					CleanupClosePushL(iPositioner);
					
					RPositionServer server2;
					RPositioner positioner2;
					
					// Connect to self locate servers.
					User::LeaveIfError(server2.Connect());
					CleanupClosePushL(server2);
					
					TPositionModuleId aGpsModId = utils.GetAGpsModuleIdL(server2);			
					
					User::LeaveIfError(positioner2.Open(server2, aGpsModId));
					CleanupClosePushL(positioner2);
					
					
					TPositionUpdateOptions optsA; 
					TPositionUpdateOptions optsB;
					
					optsA.SetMaxUpdateAge(10000000);
					optsA.SetUpdateInterval(5000000);
					
					optsB.SetMaxUpdateAge(0);
					optsB.SetUpdateInterval(2000000);
					
					TPositionInfo posInfo;
					TPositionInfo posInfob;
	
	
					// Get the actual position
					err = DoNotifyUpdateL(posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					err = DoNotifyUpdateL(posInfob);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&positioner2);
					CleanupStack::PopAndDestroy(&server2);
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
				
				
				// Test case LBS-UpdateOptions-015
				case 15:
					{
					T_LbsUtils utils;
					TPositionModuleId networkModId = utils.GetNetworkModuleIdL(iServer);
									
					// Open positioners.
					User::LeaveIfError(iPositioner.Open(iServer, networkModId));
					CleanupClosePushL(iPositioner);
					
					RPositionServer server2;
					RPositioner positioner2;
					
					// Connect to self locate servers.
					User::LeaveIfError(server2.Connect());
					CleanupClosePushL(server2);
					
					User::LeaveIfError(positioner2.Open(server2, networkModId));
					CleanupClosePushL(positioner2);
					
					
					TPositionUpdateOptions optsA; 
					TPositionUpdateOptions optsB;
					
					optsA.SetMaxUpdateAge(10000000);
					optsA.SetUpdateInterval(5000000);
					
					optsB.SetMaxUpdateAge(0);
					optsB.SetUpdateInterval(2000000);
					
					TPositionInfo posInfo;
					TPositionInfo posInfob;
	
	
					// Get the actual position
					err = DoNotifyUpdateL(posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					// Loop? Keep calling until full info recieved.
					err = DoNotifyUpdateL(posInfob);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&positioner2);
					CleanupStack::PopAndDestroy(&server2);
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
					
					
					
				// Test case LBS-UpdateOptions-016
				case 16:
					{
					T_LbsUtils utils;
					TPositionModuleId networkModId = utils.GetNetworkModuleIdL(iServer);
									
					// Open positioners.
					User::LeaveIfError(iPositioner.Open(iServer, networkModId));
					CleanupClosePushL(iPositioner);
					
					RPositionServer server2;
					RPositioner positioner2;
					
					//Connect to self locate server
					TPositionModuleId aGpsModId = utils.GetAGpsModuleIdL(server2);	
					
					User::LeaveIfError(positioner2.Open(iServer, aGpsModId));
					CleanupClosePushL(positioner2);
					
					TPositionUpdateOptions optsA; 
					TPositionUpdateOptions optsB;
					
					optsA.SetMaxUpdateAge(10000000);
					optsA.SetUpdateInterval(5000000);
					
					optsB.SetMaxUpdateAge(0);
					optsB.SetUpdateInterval(2000000);
					
					TPositionInfo posInfo;
					TPositionInfo posInfob;
	
					// Get the actual position
					err = DoNotifyUpdateL(posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					// Loop? Keep calling until full info recieved.
					err = DoNotifyUpdateL(posInfob);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&positioner2);
					CleanupStack::PopAndDestroy(&server2);
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
					
				// Test case LBS-UpdateOptions-017
				case 17:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					
					
					TPositionUpdateOptions optsA;
					optsA.SetUpdateInterval(1000000);
					optsA.SetMaxUpdateAge(5000000);
					
					if((optsA.MaxUpdateAge() != 5000000) || (optsA.UpdateInterval() != 1000000))
						{
						SetTestStepResult(EFail);
						}
						
					iPositioner.SetUpdateOptions(optsA);
					// Test AGPS manager must already be configured to return partial location info
		
					TPositionInfo posInfo;
	
					// This call to NPU puts a location in cache
					err = DoNotifyUpdateL(posInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					/*
						MaxUpdateAge () returns the current age limit for 
						information returned by NotifyPositionUpdate(). 
						Zero is returned when there is no acceptable age limit.
					*/
					
					if(optsA.MaxUpdateAge() != 0)
						{
						SetTestStepResult(EFail);
						}

					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
				// Test case LBS-UpdateOptions-018
				case 18:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
										
					TPositionUpdateOptions optsA;
					optsA.SetMaxUpdateAge(-1);
					
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
				// Test case LBS-UpdateOptions-019
				case 19:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
					
					//MaxUpdateAge comparision replacede by verify data step
					TPositionUpdateOptions& updateOpts = iParent.iSharedData->iVerifyUpdateOpts;
					
					TPositionInfo posInfoA, posInfoB;
					
					//Get fresh location and have it in cache
					err = DoNotifyUpdateL(posInfoA);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					
					//Second call to NPU should have maxupdateage set.
					iPositioner.SetUpdateOptions(updateOpts);
					
					err = DoNotifyUpdateL(posInfoB);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					
					T_LbsUtils utils;
						
					if(!utils.Compare_PosInfo(posInfoA, posInfoB))
							{
							//Cached copy doesnt match retrieved copy.
							SetTestStepResult(EFail);
							}
						
					CleanupStack::PopAndDestroy(&iPositioner);
					
					break;
					}

				// Test case LBS-UpdateOptions-020
				case 20:
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);
 					
					TPositionInfo cachedInfo, firstposInfo, secondposInfo;
 
  					TPositionUpdateOptions& updateOpts = iParent.iSharedData->iVerifyUpdateOpts;
					updateOpts.SetUpdateInterval(0);
					updateOpts.SetMaxUpdateAge(0);
					updateOpts.SetUpdateTimeOut(4000000);
					//Second call to NPU should have maxupdateage set.
					iPositioner.SetUpdateOptions(updateOpts);
   					
					// Get fresh location and have it in cache
					err = DoNotifyUpdateL(cachedInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					
					// Change the max fix time
					updateOpts.SetMaxUpdateAge(5000000);
					iPositioner.SetUpdateOptions(updateOpts);
			
					// Wait 4 seconds
					User::After(4000000);
				
					// Get a position should be the same as in the cache
					err = DoNotifyUpdateL(firstposInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
						
					// Wait 2 seconds
					User::After(2000000);
					// Get a position should not be the same as the cache
					err = DoNotifyUpdateL(secondposInfo);
					if (KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
				
					T_LbsUtils utils;
			
					TBool a = utils.Compare_PosInfo(cachedInfo, firstposInfo);
					TBool b = utils.Compare_PosInfo(firstposInfo, secondposInfo);
						
					if(!a || b)
						{
						//Cached copy doesnt match retrieved copy.
						SetTestStepResult(EFail);
						}
						
					CleanupStack::PopAndDestroy(&iPositioner);

					break;
					}
					
					
					// Test case LBS-UpdateOptions-101 (for DEF118183)
					case 101:
						{
						// Open positioner.
						User::LeaveIfError(iPositioner.Open(iServer));
						CleanupClosePushL(iPositioner);					
						
						TPositionInfo posInfoa;
						TPositionInfo posInfob;
						TPositionInfo posInfoc;
		
						// This call to NPU puts a location in cache
						err = DoNotifyUpdateL(posInfoa);
						if (KErrNone != err)
							{
							INFO_PRINTF2(_L("Failed: During first call to NPU err is %d"), err);
							SetTestStepResult(EFail);
							}

						TPositionUpdateOptions posOpts;
						posOpts.SetMaxUpdateAge(1*1000000);
						posOpts.SetUpdateInterval(5*1000000);
						
						if((posOpts.MaxUpdateAge() != 1000000) || (posOpts.UpdateInterval() != 5000000))
							{
							INFO_PRINTF1(_L("Failed: During check of options values"));
							SetTestStepResult(EFail);
							}
							
						iPositioner.SetUpdateOptions(posOpts);
												
						//Retrieve cached location with second call to NPU
						err = DoNotifyUpdateL(posInfob);
						if (KErrNone != err)
							{
							INFO_PRINTF2(_L("Failed: During NPU 2 value is %d"), err);
							SetTestStepResult(EFail);
							}
						
						//INTERNAL COMPARISON
						//Make sure we've retrieved the cached location						
						T_LbsUtils utils;
						if(!utils.Compare_PosInfo(posInfoa, posInfob))
							{
							INFO_PRINTF1(_L("Failed: During comparison of pos info values"));
							SetTestStepResult(EFail);
							}
						
						// Make 3rd call to NPU - this one to be cancelled
						TRequestStatus status;
						iPositioner.NotifyPositionUpdate(posInfoc, status);	

						// Wait 2 seconds and then cancel
						User::After(2000000);
						err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
						if (KErrNone != err)
							{
							// Pre-DefectFix code would return -1 at this point since there would have been no request to cancel
							INFO_PRINTF2(_L("Failed: Cancel request was rejected with error %d"), err);
							SetTestStepResult(EFail);						
							}
						
						User::WaitForRequest(status);
						if (KErrCancel != status.Int())
							{
							// Pre-DefectFix code would return 0 at this point to indicate a successfully returned position
							INFO_PRINTF2(_L("Failed: Expected NPU to complete with KErrCancel but actual value is %d"), status.Int());
							SetTestStepResult(EFail);
							}

						CleanupStack::PopAndDestroy(&iPositioner);
						
						break;
						}
						
						
					
				default:
					User::Panic(KLbsClientStepUpdateOptions, KErrUnknown);					
						
					}
				
				}
		
			
		// All done, clean up.
		CleanupStack::PopAndDestroy(&iServer);		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_UpdateOptions::doTestStepL()"));

	return TestStepResult();
	}

