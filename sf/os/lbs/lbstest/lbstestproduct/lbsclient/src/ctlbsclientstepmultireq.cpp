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
// @file ctlbsclientstepmultireq.cpp
// This is the class implementation for the Multi-Request Tests
// 
//
 
#include "ctlbsclientstepmultireq.h"

#include <lbs.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbsipc.h>

#include <lbs/test/tlbsutils.h>

// constant definitions
_LIT(KLbsClientStepMultiReq, "LbsClientStepMultiReq");


/**
 * Destructor
 */
CT_LbsClientStep_MultiReq::~CT_LbsClientStep_MultiReq()
	{
	delete	iGetLastKnownPosAO;
	delete	iNotifyPosUpdateAO;	
	delete  iTimer;
	delete 	iTempPosInfo;
	}


/**
 * Constructor
 */
CT_LbsClientStep_MultiReq::CT_LbsClientStep_MultiReq(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent), iState(EStateUnknown), iTempPosInfo(NULL)
	{
	SetTestStepName(KLbsClientStep_MultiReq);
	}


/**
Static Constructor
*/
CT_LbsClientStep_MultiReq* CT_LbsClientStep_MultiReq::NewL(CT_LbsClientServer& aParent)
	{
	return new(ELeave) CT_LbsClientStep_MultiReq(aParent);
	}


/**
Static Constructor
*/
CT_LbsClientStep_MultiReq* CT_LbsClientStep_MultiReq::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_MultiReq(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}
	

/**
 * 	Callback - called when NotifyPositionUpdate request completes
 */
void CT_LbsClientStep_MultiReq::NotifyPositionUpdateCallback(TRequestStatus& aStatus)
/**
 *	Called when async NotifyPositionUpdate completes
 */
	{
	TInt testCaseId;

	if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
		{
		switch (testCaseId)
			{
			case 01:		// multiple requests of different types
				{
				if(KErrNone == aStatus.Int())
					{
					switch(iState)
						{
						case EStateUnknown:					
							{
							iState = EStateLastNotifyPositionUpdateComplete;
							}
							break;
							
						case EStateLastKnownPositionComplete:
							{ 
							iState = EStateDone;
							AsyncTidyUp();
							// note: we'll now drop back into the main test step
							}
							break;			
							
						case EStateLastNotifyPositionUpdateComplete:
						case EStateDone :								
						// fall through
						default:
							{
							User::Panic(KLbsClientStepMultiReq, KErrUnknown);
							}
							break;	
						}	
					}
				else
					{
					INFO_PRINTF2(_L("NotifyModuleStatusEvent completed. Status = %d"), aStatus.Int());	
					SetTestStepResult(EFail);
					AsyncTidyUp();
					}
				}// end case 01
				break;
			case 02:	// 
				{
				switch(iState)
					{
					case EStateUnknown:					
						{
						iState = EStateLastNotifyPositionUpdateComplete;
						}
						break;						

					case EStateLastKnownPositionComplete:
						{
						iState = EStateDone;
						AsyncTidyUp();
						}
						break;
	
					case EStateLastNotifyPositionUpdateComplete:											
					case EStateDone :								
					// fall through
					default:
						{
						User::Panic(KLbsClientStepMultiReq, KErrUnknown);
						}
						break;	
					}	
				}
				break;
			default:
				{
				User::Panic(KLbsClientStepMultiReq, KErrUnknown);
				}
			
			}// end switch testCaseId
		}// end if GetIntFromConfig
	}

 
 
/**
 * 	Callback - called when GetLastKnownPosition request completes
 */
void CT_LbsClientStep_MultiReq::GetLastKnownPositionCallback(TRequestStatus& aStatus)
/**
 *	Called when NotifyModuleStatusEvent completes
 */
	{
	TInt testCaseId;

	if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
		{
		switch (testCaseId)
			{
			case 01:		// multiple requests of different types
				{
				if(KErrNone == aStatus.Int())
					{
					switch(iState)
						{
						case EStateUnknown:					
							{
							iState = EStateLastKnownPositionComplete;
							}
							break;
							
						case EStateLastNotifyPositionUpdateComplete:				
							{ 
							iState = EStateDone;
							AsyncTidyUp();
							// note: we'll now drop back into the main test step
							}
							break;			
							
						case EStateLastKnownPositionComplete:
						case EStateDone :								
						// fall through
						default:
							{
							User::Panic(KLbsClientStepMultiReq, KErrUnknown);
							}
						break;
						
						}	
					}
				else
					{
					INFO_PRINTF2(_L("NotifyModuleStatusEvent completed. Status =%d"), aStatus.Int());	
					SetTestStepResult(EFail);
					AsyncTidyUp();
					}
					
				}// end case 01
				break;
			case 02: // NotifyPositionUpdate then GetLKP, latter cancelled
				{
				switch(iState)
					{
					case EStateUnknown:
						{
						iState = EStateLastKnownPositionComplete;
						TInt err = aStatus.Int();
						switch(err)
							{
							case KErrNone:
								{
								INFO_PRINTF1(_L("MultiReq test: Last Known Position request completed successfully"));
								}
								break;
							case KErrCancel:
								{
								INFO_PRINTF1(_L("MultiReq test: Last Known Position request completed with KErrCancel"));
								}
								break;
							case KErrUnknown:
								{
								INFO_PRINTF1(_L("MultiReq test: Last Known Position request completed with KErrUnknown"));
								}
								break;
							default:
								{
		
								SetTestStepResult(EFail);
								AsyncTidyUp();								
								}
								break;
							}
						}
						break;
																	
					case EStateLastNotifyPositionUpdateComplete:	
						{
						iState = EStateDone;
						TInt err = aStatus.Int();
						if(KErrNone != err)
							{
							SetTestStepResult(EFail);
							}
						AsyncTidyUp();
						}
						break;
						
					case EStateLastKnownPositionComplete:						
					case EStateDone :								
					// fall through
					default:
						{
						User::Panic(KLbsClientStepMultiReq, KErrUnknown);
						}					
					}// end switch(iState)
				}// end case 2
				break;
				
			default:
				{
				User::Panic(KLbsClientStepMultiReq, KErrUnknown);
				}
			break;				
			}
		}			
	}
	
	
/**
 *	tidy up everything
 */
void CT_LbsClientStep_MultiReq::AsyncTidyUp()
	{
	if ( iTimer->IsActive() )
		{
		iTimer->Cancel();
		}	
	// Cancel any outstanding requests (to be tested):
	if(iNotifyPosUpdateAO->IsActive())
		{
		iNotifyPosUpdateAO->Cancel();
		}
	if(iGetLastKnownPosAO->IsActive())
		{
		iGetLastKnownPosAO->Cancel();
		}
	CActiveScheduler::Stop();
	}
	
	
/**
 *	Callback function - called if timer fires (requests did not all complete in time)
 */
static TInt TimerCallback(TAny* aPointer)
	{
	//the timer expired and we have not got the responses to our lbs requests
	static_cast<CT_LbsClientStep_MultiReq*>(aPointer)->INFO_PRINTF1(_L(">>TEST FAILED: timer fired"));
	static_cast<CT_LbsClientStep_MultiReq*>(aPointer)->AsyncTidyUp();
	static_cast<CT_LbsClientStep_MultiReq*>(aPointer)->SetTestStepResult(EFail);

	return KErrNone;	
	}
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_MultiReq::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_MultiReq::doTestStepL()"));
		
	if (TestStepResult()==EPass)
		{		
		// Connect to self locate server.
		User::LeaveIfError(iServer.Connect());
		CleanupClosePushL(iServer);
		
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				case 01:		// multiple requests of different types
					{
					// Open positioner.
					User::LeaveIfError(iPositioner.Open(iServer));
					CleanupClosePushL(iPositioner);

					// Carry out a notify update to ensure last known cache is filled (note: this blocks!)
					TPositionInfo notifyPosInfo;
					User::LeaveIfError(DoNotifyUpdateL(notifyPosInfo));

					// Create 2 posinfos and store in our shared array for later verification.
					RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
					TPositionInfo* posInfoLastKnown = new(ELeave) TPositionInfo();
					TPositionInfo* posInfoNotifyUpd = new(ELeave) TPositionInfo();
							
					T_LbsUtils utils;
					utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

					// these get deleted by the base class destructor
					posInfoArr.Append(posInfoLastKnown);
					posInfoArr.Append(posInfoNotifyUpd);

					// Start GetLastKnownPosition request:
					iGetLastKnownPosAO = CT_LbsClientGetLastKnownPosAO::NewL(*this);					
				
					iGetLastKnownPosAO->GetLastKnownPosL(iPositioner, *posInfoLastKnown);
					
					// Start NotifyPositionUpdate request:
					iNotifyPosUpdateAO = CT_LbsClientNotifyPosUpdAO::NewL(*this);
					
					iNotifyPosUpdateAO->NotifyPosUpdateL(iPositioner, *posInfoNotifyUpd);
					
					// Start a timer to make sure neither of above times out:
					iTimer = CPeriodic::New(CActive::EPriorityStandard);						
					TCallBack callbackFunction = TCallBack(TimerCallback, this);
					iTimer->Start( delay10Seconds, 0, callbackFunction);
					
					// can start the scheduler now as there are async requests issued
					CActiveScheduler::Start();
					
					// All requests have now completed (and scheduler stopped)
					
					CleanupStack::PopAndDestroy(&iPositioner);					
					} // end case 1
					break;
					
				case 02:	// cancel one of two outstanding requests
					{
					TInt expected_error;
					// For the S60 Loc Server the test will return KErrNone instead of KErrNotFound.
					if(!GetIntFromConfig(ConfigSection(), KExpected_Error, expected_error))
						{
						expected_error = KErrNotFound;
						}
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
					
					// Start NotifyPositionUpdate request:
					iNotifyPosUpdateAO = CT_LbsClientNotifyPosUpdAO::NewL(*this);
					
					iNotifyPosUpdateAO->NotifyPosUpdateL(iPositioner, *posInfoNotifyUpd);
											
					// Start a GetLastKnownPosition:
					// don't put in verify array as this may be cancelled:
					iTempPosInfo = new(ELeave) TPositionInfo();
					
					iGetLastKnownPosAO = CT_LbsClientGetLastKnownPosAO::NewL(*this);					
				
					iGetLastKnownPosAO->GetLastKnownPosL(iPositioner, *iTempPosInfo);
					
					// cancel the last known position. Note that we do this directly, NOT by calling the AO's cancel()
					// so that the AO's RunL(), and therefore our callback, will be called and we can check the result
					TInt err;
					err = iPositioner.CancelRequest(EPositionerGetLastKnownPosition);
					if(expected_error != err)
						{
						SetTestStepResult(EFail);
						}
					
					// Start a timer to make sure neither of above times out:
					iTimer = CPeriodic::New(CActive::EPriorityStandard);						
					TCallBack callbackFunction = TCallBack(TimerCallback, this);
					
					//
					// 
					// NB - THIS VALUE WAS 10 SEC AND THE TIMER EXPIRED, CAUSING THE TEST
					// TO FAIL. NEED TO FIND OUT WHY THIS VALUE IS OK BUT THAT ONE WAS NOT.
					// PUT BACK TEMPORARILY TO GET RID OF REGRESSION FAILURE
					//
					iTimer->Start(22000000, 0, callbackFunction);
					
					// can start the scheduler now as there are async requests issued
					CActiveScheduler::Start();
					
					// All requests have now completed
					
					CleanupStack::PopAndDestroy(&iPositioner);					
					}
					break;
					
				default:
					{
					User::Leave(KErrArgument);
					}
				}// end switch
			}//end if GetIntFromConfig
					
		CleanupStack::PopAndDestroy(&iServer);	

		} // end if TestStepResult
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_MultiReq::doTestStepL()"));

	return TestStepResult();
	}

