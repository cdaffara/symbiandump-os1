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
// @file ctlbsclientstep_tracking.cpp
// This is the class implementation for the Tracking Tests
// 
//
 
#include "ctlbsclientsteptracking.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>


/**
 * Construction.
 */
CT_LbsClientStep_Tracking* CT_LbsClientStep_Tracking::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClientStep_Tracking* testStep = new CT_LbsClientStep_Tracking(aParent);
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


CT_LbsClientStep_Tracking::CT_LbsClientStep_Tracking(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent),
	iVerifyInterval(ETrue), iExpectedErr(KErrNone), iUpdateIndex(0)
	{
	SetTestStepName(KLbsClientStep_Tracking);
	}


void CT_LbsClientStep_Tracking::ConstructL()
	{
	// Setup netsim.
	User::LeaveIfError(OpenNetSim());
	
	// Connect server.
	User::LeaveIfError(iServer.Connect());
	
	// Open positioner.
	User::LeaveIfError(iPositioner.Open(iServer));
	}


/**
 * Destructor
 */
CT_LbsClientStep_Tracking::~CT_LbsClientStep_Tracking()
	{
	iPositioner.Close();
	iServer.Close();	
	CloseNetSim();
	}


const TTimeIntervalMicroSeconds KTimestampMargin = 500000; //+/-0.5s

TBool CT_LbsClientStep_Tracking::VerifyFirstPosTime(const TTime& aTimeRequested, const TPositionInfo& aPosInfo, const TPositionUpdateOptions& aUpdateOpts)
	{
	TTime timeReceived, timeStamp;
	TPosition pos;
					
    TInt64 maxtime;
    maxtime = aUpdateOpts.UpdateTimeOut().Int64();
	if(!maxtime)	
		{
		#pragma message("TO DO: CT_LbsClientStep_Tracking::VerifyFirstPosTime - Update when moduleinfo supported")	
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
TBool CT_LbsClientStep_Tracking::VerifyPosTime(const TTime& aTimeFirstPosStamped, const TPositionUpdateOptions& aUpdateOpts, TPositionInfo& aPosInfo, TInt aPosCount)
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

	if(timeStamp < windowstart && timeReceived < windowstart)
		{
		INFO_PRINTF1(_L("Failed: Position received outside time range"));
		return FALSE;
		}
	if(timeout.Int64() && (timeStamp > windowend || timeReceived > windowend))
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

TInt CT_LbsClientStep_Tracking::SetExpectedError(TInt aExpectedErrVal,TInt aDefaultErr)	
	{
	if(aExpectedErrVal==-1000)
		{
		return aDefaultErr;
		}
	else
		{
		return aExpectedErrVal;
		}
	}

TInt CT_LbsClientStep_Tracking::GetExpectedError(const TDesC& aError)
	{
	TInt expectedErr;
	if (GetIntFromConfig(ConfigSection(), aError, expectedErr))
		{	
		return expectedErr;
		}
	else
		return -1000;
	}
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_Tracking::doTestStepL()
	{
	// Generic test step used to test the LBS SetUpdateOptions API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_Tracking::doTestStepL()"));

	if (TestStepResult()==EPass) 
		{
		// Determine the test case id.
		TInt testCaseId;
				
		// Initialise these errors here - Make this a dynamic array?
		iExpectedErr = GetExpectedError(KExpectedErr);
		iExpectedErr2 = GetExpectedError(KExpectedErr2);
		iExpectedErr3 = GetExpectedError(KExpectedErr3);
		iExpectedErr4 = GetExpectedError(KExpectedErr4);
				
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			// Set the update options.
			T_LbsUtils utils;
			TPtrC configFileName;
			_LIT(KUpdateOptionsFile, "agps_module_update_file");
	
			GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName);

			utils.GetConfigured_ModuleUpdateOptionsL(configFileName, ConfigSection(), iUpdateOpts);
			iPositioner.SetUpdateOptions(iUpdateOpts);
			
			// count the number of updates from the .ini file
			RPointerArray<TLbsModuleUpdateItem> posInfoArray;
			utils.GetConfigured_UpdateArrayL(configFileName, ConfigSection(), posInfoArray);
			TInt numOfPosInfos = posInfoArray.Count();
			posInfoArray.ResetAndDestroy();

			// Perfom test.
			TInt err = KErrNone;
			TPosition pos;
			TTime timeFirstStamped;
			TTime timeRequested;
			iUpdateWindowIndex = 0;
			if(testCaseId == 8)
				{
				iCheckIfSameAsPreviousPosition = ETrue;
				}
			else
				{
				iCheckIfSameAsPreviousPosition = EFalse;				
				}

			
			for (iUpdateIndex = 0 ; iUpdateIndex < numOfPosInfos; iUpdateIndex++)
				{				
				// Get and log current time.
				timeRequested.UniversalTime();
				INFO_PRINTF1(_L("Position requested at: "));	
				INFO_PRINTF4(_L("%d/%d/%d"), timeRequested.DateTime().Day() + 1, timeRequested.DateTime().Month() + 1, timeRequested.DateTime().Year());
				INFO_PRINTF5(_L("at %d :%d :%d :%d"), timeRequested.DateTime().Hour(), timeRequested.DateTime().Minute(), timeRequested.DateTime().Second(), timeRequested.DateTime().MicroSecond());

				// Do request to position update.
				err = DoNotifyUpdateL(iPosInfo);

				iPosInfo.GetPosition(pos);
				
				// First update, ref pos, KErrNone.
				if (iUpdateIndex == 0)
					{
					// Verify time taken for first update.
					timeFirstStamped = pos.Time();	// Need to remember the time we received this update.
					if(!VerifyFirstPosTime(timeRequested, iPosInfo, iUpdateOpts))
						{
						SetTestStepResult(EFail);									
						}

					// Verify reference position.
					TPositionInfo verifyRefPosInfo;
					
					verifyRefPosInfo.SetPosition(iRefPos);
					if (!utils.Compare_PosInfo(verifyRefPosInfo, iPosInfo))
						{
						INFO_PRINTF1(_L("Failed test, reference position incorrect."));
						SetTestStepResult(EFail);
						}
						
					iExpectedErr = KErrNone;
					// Expect to verify interval time, for other updates.
					iVerifyInterval = ETrue;
					iPreviousPos = iPosInfo;
					}

				// Remaining updates.
				else
					{
					// Increment the expected time interval 'window' the update is expected to arrive in
					iUpdateWindowIndex++;
				
					// Determine expected err, and any special test actions.
					switch (iUpdateIndex)
						{
						// Second update, gps accurate, KErrNone, after n * intervals.
						case 1:
							{
							iExpectedErr = KErrNone;
						
							if(testCaseId == 7)
								{
								DoInterval_TestCaseId_07();
								}
							else if(testCaseId == 8)
								{
								DoInterval_TestCaseId_08();
								}
							else
								{
								// Got the first accurate gps update, switch on partial updates.
								iUpdateOpts.SetAcceptPartialUpdates(ETrue);
								iPositioner.SetUpdateOptions(iUpdateOpts);
								}

							break;
							}

						// Remaining updates as per test case.
						default:
							{
							switch (testCaseId)
								{
								// Test case LBS-Tracking-001
								case 1:
									{
									DoInterval_TestCaseId_01();

									break;
									}

								// Test case LBS-Tracking-002
								case 2:
									{
									DoInterval_TestCaseId_02();
									
									break;
									}

								// Test case LBS-Tracking-003
								case 3:
									{
									DoInterval_TestCaseId_03();
								
									break;
									}

								// Test case LBS-Tracking-004									
								case 4:
									{
									DoInterval_TestCaseId_04();
										
									break;
									}
									
								// Test case LBS-Tracking-005
								case 5:
									{
									DoInterval_TestCaseId_05();
									
									break;
									}

								// Test case LBS-Tracking-006
								case 6:
									{
									DoInterval_TestCaseId_06();
									
									break;
									}
									
								// Test case LBS-Tracking-007
								case 7:
									{
									DoInterval_TestCaseId_07();
																	
									break;
									}

								// Test case LBS-Tracking-008
								case 8:
									{
									DoInterval_TestCaseId_08();
																	
									break;
									}
								}
							}
						break;
						}

					// Verify correct interval for update, if required.
					if (iVerifyInterval)
						{
						if(!VerifyPosTime(timeFirstStamped, iUpdateOpts, iPosInfo, iUpdateWindowIndex))
							{
							INFO_PRINTF1(_L("Failed: Position received outside time range"));
							SetTestStepResult(EFail);
							}
						}
					
					if(iCheckIfSameAsPreviousPosition)
						{
						if(iSameAsPreviousPos && !utils.Compare_PosInfo(iPreviousPos, iPosInfo))
							{
							INFO_PRINTF1(_L("Failed test, position different from previous, when it should be same."));
							SetTestStepResult(EFail);
							}
						if(!iSameAsPreviousPos && utils.Compare_PosInfo(iPreviousPos, iPosInfo))
							{
							INFO_PRINTF1(_L("Failed test, position same as previous, when it should be different."));
							SetTestStepResult(EFail);
							}
						}
					iPreviousPos = iPosInfo;
					}
					
				// Verify err.
				if (err != iExpectedErr)
					{
					ERR_PRINTF3(_L("Failed test, error code received %i, expected %i"), err, iExpectedErr);
					SetTestStepResult(EFail);						
					}
				}
		
			}
		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_Tracking::doTestStepL()"));

	return TestStepResult();
	}


// Test 1 - expected updates
//
// Ref pos,				KErrNone,				before interval
// Gps pos (accurate),	KErrNone,				after n * intervals
// Gps pos (accurate),	KErrNone, 				after interval
// Gps pos (partial),	KPositionPartialUpdate,	after interval
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_01()
{



	switch (iUpdateIndex)
		{
		case 2:	// Third update, gps accurate, KErrNone.
			{
			iExpectedErr = SetExpectedError(iExpectedErr2,KErrNone);
			
			break;
			}
	
		case 3:	// Last update, gps inaccurate, KPositionPartialUpdate.	
			{
			iExpectedErr = SetExpectedError(iExpectedErr3,KPositionPartialUpdate);	
			break;
			}
		}
	}


// Test 2 - expected updates
//
// Ref pos,				KErrNone,	before interval
// Gps pos (accurate),	KErrNone,	after n * intervals
// Gps pos (accurate),	KErrNone,	after interval
// Gps pos (accurate),	KErrNone,	after interval
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_02()
{

	switch (iUpdateIndex)
		{
		case 2:	// Third update, gps accurate, KErrNone.
			{
			iExpectedErr = SetExpectedError(iExpectedErr2,KErrNone);	
			break;
			}
		case 3: // Last update, gps accurate, KErrNone.
			{
			iExpectedErr = SetExpectedError(iExpectedErr3,KErrNone);	
			break;
			}
		}
	}


// Test 3 - expected updates
//
// Ref pos,				KErrNone,				before interval
// Gps pos (accurate),	KErrNone,				after n * intervals
// Gps pos (NAN),		KPositionPartialUpdate,	after interval
// Gps pos (NAN),		KPositionPartialUpdate,	after interval
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_03()
{

	switch (iUpdateIndex)
		{
		
		case 2:	// Third update, gps accurate, KPositionPartialUpdate.
			{
			iExpectedErr = SetExpectedError(iExpectedErr2,KPositionPartialUpdate);	
			break;
			}
		case 3: // Last update, gps accurate, KPositionPartialUpdate.
			{
			iExpectedErr = SetExpectedError(iExpectedErr3,KPositionPartialUpdate);	
			break;
			}
		}
	}


// Test 4 - expected updates
//
// Ref pos,					KErrNone,				before interval
// Gps pos (accurate),		KErrNone,				after n * intervals
// Gps pos (inaccurate),	KPositionPartialUpdate,	after interval
// Gps pos (inaccurate),	KPositionPartialUpdate,	after interval
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_04()
	{
	switch (iUpdateIndex)
		{
		
		case 2:	// Third update, gps accurate, KPositionPartialUpdate.
			{
			iExpectedErr = SetExpectedError(iExpectedErr2,KPositionPartialUpdate);	
			break;
			}
		case 3: // Last update, gps accurate, KPositionPartialUpdate.
			{
			iExpectedErr = SetExpectedError(iExpectedErr3,KPositionPartialUpdate);	
			break;
			}
		}
	}


// Test 5 - expected updates
//
// Ref pos,					KErrNone,		before interval
// Gps pos (accurate),		KErrNone,		after n * intervals
// Gps pos (accurate),		KErrNonee,		after interval
// Gps pos (not delivered),	KErrTimedOut,	after interval
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_05()
{

	switch (iUpdateIndex)
		{
		case 2:	// Third update, gps accurate, KErrNone.
			{
			iExpectedErr = SetExpectedError(iExpectedErr2,KErrNone);				

			// Send timeout to the A-GPS hybrid module - we expect further updates to timeout.
			TTimeIntervalMicroSeconds timeOutVal(40000000);
			
			ConfigureModuleTimeOut(timeOutVal);
			
			break;
			}
	
		case 3:	// Last update, gps inaccurate, KPositionPartialUpdate.	
			{
			iExpectedErr = SetExpectedError(iExpectedErr3,KErrTimedOut);	
			iVerifyInterval = EFalse;
			
			// Reset the timeout in the A-GPS module.
			TTimeIntervalMicroSeconds timeOutVal(0);
			ConfigureModuleTimeOut(timeOutVal);
			break;
			}
		}
	}

// Test 6 - expected updates
//
// Ref pos,					KErrNone,				before interval
// Gps pos (accurate),		KErrNone,				after n * intervals
// Gps pos (inaccurate),	KPositionPartialUpdate,	after interval
// Gps pos (NAN),			KPositionQualityLoss,	before interval (because of futile udpate)
// Gps pos (inaccurate),	KPositionPartialUpdate,	after interval
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_06()
{

	switch (iUpdateIndex)
		{
		case 2:	// Third update, gps inaccurate, KPositionPartialUpdate.
			{
			iExpectedErr = SetExpectedError(iExpectedErr2,KPositionPartialUpdate);	
			break;
			}
			
		case 3:	// Fourth update, gps NAN, KPositionQualityLoss.
			{
			iExpectedErr = SetExpectedError(iExpectedErr3,KPositionQualityLoss);	
			// This update should have happened before the next expected 
			// interval window (because it's a futile update from the
			// GPS module), so it will be in the previous 'window'
			iUpdateWindowIndex--;
			break;
			}
	
		case 4:	// Last update, gps inaccurate, KPositionPartialUpdate.	
			{
			iExpectedErr = SetExpectedError(iExpectedErr4,KPositionPartialUpdate);	
			break;
			}
		}
	}

// Test 7 - expected updates
//
// Ref pos,					KErrNone,				before interval
// Gps pos (accurate),		KErrNone,				after n * intervals
// Gps pos (inaccurate),	KPositionQualityLoss,	after timeout
//
void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_07()
	{
	switch (iUpdateIndex)
		{
		case 1:
			{
			iExpectedErr = SetExpectedError(iExpectedErr,KErrNone);	
			break;
			}
		case 2:
			{
			iVerifyInterval = EFalse;
			iExpectedErr = SetExpectedError(iExpectedErr2,KPositionQualityLoss);	
			break;
			}
		}
	}

void CT_LbsClientStep_Tracking::DoInterval_TestCaseId_08()
	{
	iExpectedErr = KErrNone;
	switch (iUpdateIndex)
		{
		case 1:
		case 3:
			{
			iSameAsPreviousPos = EFalse;
			break;
			}
		case 2:
			{
			iSameAsPreviousPos = ETrue;
			break;
			}
		}	
	}

// A-GPS hybrid module configuration function.
void CT_LbsClientStep_Tracking::ConfigureModuleTimeOut(TTimeIntervalMicroSeconds& aTimeOut)
	{
	CT_LbsAGpsHandler* modHandler;
	modHandler = CT_LbsAGpsHandler::NewL(this);

	// Send timeout request.
	modHandler->SendRequestTimeOutMsg(aTimeOut);

	// Wait for response from module.	
	CActiveScheduler::Start();
	
	delete modHandler;
	}


// Response back from A-GPS hybrid module to notify the test it has got the timeout request.
void CT_LbsClientStep_Tracking::ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse)
	{
	if (TT_LbsAGpsResponseMsg::EModuleResponseOk != aResponse)
		{
		// fail test could not config module!
		INFO_PRINTF2(_L("Unable to configure hybrid module, err %d."), aResponse);	
		SetTestStepResult(EFail);						
		}
		
		// Return back to test.
		CActiveScheduler::Stop();	
	}

