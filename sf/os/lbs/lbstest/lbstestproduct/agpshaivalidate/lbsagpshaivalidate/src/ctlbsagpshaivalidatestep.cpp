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
// @file ctlbshaivalidatesteproot.cpp
// This is the class implementation for the Module Information Tests
// 
//

// EPOC includes.
#include <e32property.h>
#include <ecom/ecom.h> 

// LBS includes. 
#include <lbscommon.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbsmoduleinfo.h>

// LBS test includes.
#include "ctlbsagpshaivalidatestep.h"
#include <lbs/test/ctlbstimerutils.h>
#include "lbstestloggermacros.h"
#include "ctlbs.h"
#include "lbstestutilities.h" 

// config file settings that we read
_LIT(KDeviceStatusChangeTime, "devicestatuschange_time");


/**
 * Destructor
 */
CT_LbsAGPSHAIValidateStep_Main::~CT_LbsAGPSHAIValidateStep_Main()
	{
	// nothing to do - NB this calls the versions in CT_LbsAGPSHAIValidateStep_Main i.e. this class
	CancelAllTimers();
	DeleteAllTimers();
	delete iTinyClass;
	}
	

/**
 * Constructor
 */
CT_LbsAGPSHAIValidateStep_Main::CT_LbsAGPSHAIValidateStep_Main(CT_LbsAGPSHAIServerRoot& aParent) : CT_LbsAGPSHAIValidateStepRoot(aParent)
	{	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::CT_LbsAGPSHAIValidateStep_Main()");
	SetTestStepName(KLbsAGPSHAIValidateStep);
	
	// install scheduler if there isn't one already installed
	if(!CActiveScheduler::Current())
		{
		CActiveScheduler::Install(iParent.iScheduler);	
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::CT_LbsAGPSHAIValidateStep_Main()");		
	}


/**
Public, non-leaving, static constructor
*/
CT_LbsAGPSHAIValidateStep_Main* CT_LbsAGPSHAIValidateStep_Main::New(CT_LbsAGPSHAIServerRoot& aParent)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::New()");
	CT_LbsAGPSHAIValidateStep_Main* self = NULL;
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	TRAPD(err, self = NewL(aParent));
	
	if(KErrNone != err)
		{
		TESTLOG2(ELogP1, "Error %d when creating parent", err);
		}
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::New()");
	return self;
	}


/**
Static Constructor that may leave
*/
CT_LbsAGPSHAIValidateStep_Main* CT_LbsAGPSHAIValidateStep_Main::NewL(CT_LbsAGPSHAIServerRoot& aParent)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::NewL()");
	CT_LbsAGPSHAIValidateStep_Main* self = new CT_LbsAGPSHAIValidateStep_Main(aParent);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::New()");
	return self;

	}
	
/**
2nd phase Constructor that may leave
*/
void CT_LbsAGPSHAIValidateStep_Main::ConstructL()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::ConstructL()");	
	// MUST _MUST_ call the parent class version frist
	CT_LbsAGPSHAIValidateStepRoot::ConstructL();
	CreateAllTimersL();
	iTinyClass = CTinyTestClass::NewL(this);
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::ConstructL()");	
	}
	
	
void CT_LbsAGPSHAIValidateStep_Main::UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime)	
	{
	CT_LbsAGPSHAIValidateStepRoot::UpdateLocation(aStatus, aPosInfoArray, aNumItems, aTargetTime);
	
	if (iStartAnotherRequestFlag)
		{
		iTinyClass->StartAOService();
		iStartAnotherRequestFlag = EFalse;		
		}
	}


void CT_LbsAGPSHAIValidateStep_Main::CreateAllTimersL()
	{
	iDeviceStatusChangeTimer = CT_LbsTimerUtils::NewL(this, KDeviceStatusChangeTimerId);
	}


void CT_LbsAGPSHAIValidateStep_Main::DeleteAllTimers()
	{
	delete iDeviceStatusChangeTimer;
	}
/*
	Cancel all timers. 
*/
void CT_LbsAGPSHAIValidateStep_Main::CancelAllTimers()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::CancelAllTimers()");
	if(iDeviceStatusChangeTimer && iDeviceStatusChangeTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iDeviceStatusChangeTimer is still running. Cancelling timer.");
		iDeviceStatusChangeTimer->CancelTimer();
		}				
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::CancelAllTimers()");		
	}	
	
 	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAGPSHAIValidateStep_Main::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::doTestStepL()");		
		
	
	if (TestStepResult() == EPass)
		{		
		// Carryout unique test actions.
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{	
			TESTLOG2(ELogP2, "				----------- Test Case %d -----------				\n\n", iTestCaseId);		
			
			TESTLOG(ELogP2, "Loading HAI Module\n");
			LoadHAIModuleL();		
			
			switch (iTestCaseId)
				{
				case 1:		// Load and Unload the HAI Module.
					{
					DoLoadUnLoadTestCase();
					}
					break;
				
				case 12:// Request location with low accuracy.
				case 2:	// 'immediate' location request with long max fix time and default accuracy.
				case 3:	// non-immediate location request with long max fix time (default accuracy)
					{
					DoImmediateRequestTestCase();
					}
					break;
					
				case 4:	// Request a second location at different, earlier, target time. 
				case 5:	// Request a second location at different, later, target time before first targettime reached.
					{
					DoSecondLocationRequestTestCase();
					}
					break;
					   
				case 6: // Request with low accuracy requirement implicitly cancelled by a second location at same target time with higher accuracy.
				case 7:	// Request with low accuracy requirement implicitly cancelled by a second location at same target time with lower accuracy.
					{
					DoLowAccuracyLocationRequestTestCase();
					}
					break;
					
				case 9: // Request an 'immediate' location with default maxfix time, then on maxfix time, request further 'immediate' update(s) with shorter maxfixtime (> TTNF).	
					{
					DoTrackingRequestTestCase();
					}
					break;
					
				case 10:	// 'immediate' location request with max fix time = TTFF + processing time (default accuracy).
					{
					DoImmediateRequestWithMaxFixTimeTestCase();
					}
					break;
					
				case 11:	// 'far off' location request with very short max fix time (default accuracy).
					{
					DoFarOffLocationRequestTestCase();
					}
					break;
					
				case 13:	// Make an immediate request with very high accuracy and long maxfix time and then cancel it.
					{					
					DoImmediateRequestWithCancelTestCase();
					}
					break;
					
				case 14:	// Verify that HAI module reports it's status on loading and settles to 'Inactive'
					{
					DoHAISettlesToInActiveTestCase();	
					}
					break;
					
				case 15:	// Verify that HAI module reports 'Active' device state during location update and drops back to 'Inactive' when no outstanding requests.
					{
					DoHAIToggleActiveStatusTestCase();
					}
					break;
					
				case 16:	// Verify that the HAI module drops back to a low power state when the next location is a long way off. 
					{
					DoHAIPowerDropTestCase();
					}
					break;					
						
				case 17:	// Verify that the HAI module reports its data quality status on loading and settles to 'normal' quality
					{
					DoHAIDataQualityStatusTestCase();
					}
					break;
	
				case 18:	// Advise module to go into 'On' power mode.
					{
					DoHAIPowerOnTestCase();
					}
					break;
					
				case 19:	// Advise module to go into 'Standy' power mode while no outstanding location requests.
					{
					DoHAIStandByTestCase();
					}
					break;
					
				case 20:	// Advise module to go into 'Off' power mode while no outstanding location requests.
					{
					DoHAIPowerOffTestCase();
					}
					break;
					
 				case 21:	// Advise module to go into 'Close' power mode while no outstanding location requests, then request a location.
 					{					
					DoHAIAdviseCloseTestCase();
 					}
 					break;								
			
				default:
					{
					User::Panic(_L("LbsAGPSHAIValidateStep_Main"), KErrUnknown);
					}					
				}	
			TESTLOG(ELogP2, "Unloading HAI Module\n");
			UnLoadHAIModule();	
					
			TESTLOG2(ELogP2, "				----------- End Test Case %d -----------				\n\n", iTestCaseId);									
			}
			else
			{
			TESTLOG(ELogP3, "Failed: Test case has no entry in config file.");
			ERR_PRINTF1(_L("<font><b>FAILED: Test case has no entry in config file.</font></b>\n"));
			SetTestStepResult(EFail);
			}
		}
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::doTestStepL()");		
	return TestStepResult();
	}	

//------------------------ new test cases ---------------------------------
// Power handling test cases...	
void CT_LbsAGPSHAIValidateStep_Main::DoHAIPowerOnTestCase()
	{
	// test case code		
	HAIModule()->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeOn);
	ExpectedDeviceStatus() = TPositionModuleStatus::EDeviceReady;
	StartDeviceStatusChangeTimer();
	CActiveScheduler::Start();
	}
	
void CT_LbsAGPSHAIValidateStep_Main::DoHAIStandByTestCase()
	{
	// test case code		
	HAIModule()->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeStandby);
	ExpectedDeviceStatus() = TPositionModuleStatus::EDeviceStandBy;
	AltExpectedDeviceStatus() = TPositionModuleStatus::EDeviceReady;	// not all modules support 'Standby'
	StartDeviceStatusChangeTimer();
	CActiveScheduler::Start();
	}
	
void CT_LbsAGPSHAIValidateStep_Main::DoHAIPowerOffTestCase()
	{
	// test case code		
	HAIModule()->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeOff);
	ExpectedDeviceStatus() = TPositionModuleStatus::EDeviceInactive;
	StartDeviceStatusChangeTimer();
	CActiveScheduler::Start();
	}


/*
	Start a 'device status change' timer. We expect to have received an UpdateDeviceStatus() call before this fires
*/
void CT_LbsAGPSHAIValidateStep_Main::StartDeviceStatusChangeTimer()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::StartImmediateDefaultRequest()");				
	
	TInt configValue;
	if(!GetIntFromConfig(ConfigSection(), KDeviceStatusChangeTime, configValue))
		{
		// timer for 'device status change':	
		User::Panic(_L("unconfigured devicestatuschange_time.\n"), KErrArgument);					
		}
	TESTLOG2(ELogP2, "Starting 'device status change' timer of %d seconds\n", configValue/1000000);
	__ASSERT_DEBUG(!iDeviceStatusChangeTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartDeviceStatusChangeTimer: Timer already running"), KErrGeneral));	
	TTimeIntervalMicroSeconds32 temp = configValue;
	
	// start first device status timer:
	iDeviceStatusChangeTimer->SetTimer(temp);
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::StartImmediateDefaultRequest()");				
		
	}


// called when timer fires
void CT_LbsAGPSHAIValidateStep_Main::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	(void)aTargetTime;
	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::HandleTimerL()");		
	
	switch(aTimerId)
		{	
 		case KDeviceStatusChangeTimerId:
 			{
 			TESTLOG(ELogP2, "'Device Status Change' time reached");
 			// we expect the device to have reported it's device status change by now
 			if(ExpectedDeviceStatus() != LastReportedStatus().DeviceStatus())
 				{
 				TESTLOG2(ELogP3, "WARNING: Device did not report expected device status change %d\n", ExpectedDeviceStatus());
 				WARN_PRINTF2(_L("<font><b>WARNING: Device did not report expected device status change %d.</b></font>\n"), ExpectedDeviceStatus());				
				if((AltExpectedDeviceStatus() != TPositionModuleStatus::EDeviceUnknown) && AltExpectedDeviceStatus() != LastReportedStatus().DeviceStatus()) 
	 				{
	 				TESTLOG2(ELogP3, "WARNING: Device also did not report alternative expected device status change %d\n", AltExpectedDeviceStatus());
	 				WARN_PRINTF2(_L("<font><b>WARNING: Device did not report expected device status change %d.</b></font>\n"), AltExpectedDeviceStatus());				 				
	 				} 				
 				}
 				FinishTest();
 			}
 			break;
 		default:
 			{
 			CT_LbsAGPSHAIValidateStepRoot::HandleTimerL(aTimerId, aTargetTime);
 			}
 			break;
		}	// end switch
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::HandleTimerL()");			
	}

// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CT_LbsAGPSHAIValidateStep_Main::GetAssistanceDataItem(TLbsAssistanceDataItem /*aItem*/, RDataReaderRootBase& /*aDataRoot*/, TTime& /*aTimeStamp*/)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::GetAssistanceDataItem()");
	TESTLOG(ELogP2, "FAILED: Request from HAI Module for assistance data item in Autonomous mode");	
	ERR_PRINTF1(_L("<font><b>FAILED: Request from HAI Module for assistance data item in Autonomous mode</font></b>\n"));	
	SetTestStepResult(EFail);
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::GetAssistanceDataItem()");
	return KErrNotSupported;
	}


// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CT_LbsAGPSHAIValidateStep_Main::GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem /*aItem*/, TTime& /*aTimeStamp*/)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::GetAssistanceDataItemTimeStamp()");
	TESTLOG(ELogP2, "FAILED: Request from HAI Module for assistance data item timestamp in Autonomous mode");
	ERR_PRINTF1(_L("<font><b>FAILED: Request from HAI Module for assistance data item timestamp in Autonomous mode</font></b>\n"));		
	SetTestStepResult(EFail);
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::GetAssistanceDataItemTimeStamp()");			
	return KErrNotSupported;
	}


// from MLbsLocationSourceGpsObserver
// Integration module requests assistance data from network
void CT_LbsAGPSHAIValidateStep_Main::RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::RequestAssistanceData()");
	TESTLOG2(ELogP2, "mask = 0x%x", aDataItemMask);
	if(aDataItemMask != 0)
		{
		ERR_PRINTF2(_L("<font><b>FAILED: HAI Module requested assistance data (mask=0x%x) in autonomous mode.</b></font>\n"), ExpectedDeviceStatus());				
		SetTestStepResult(EFail);
		}
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::RequestAssistanceData()");				
	}


// from MLbsTestStepObserver
void CT_LbsAGPSHAIValidateStep_Main::DoWork()
	{
	StartAnotherRequest();
	}
	
CTinyTestClass* CTinyTestClass::NewL(MLbsTestStepObserver* aOwner)
    {
	CTinyTestClass* self = new (ELeave)CTinyTestClass(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
    }

CTinyTestClass::CTinyTestClass(MLbsTestStepObserver* aOwner)
: CActive(EPriorityStandard),
  iOwner(aOwner)
    { 
    }

void CTinyTestClass::ConstructL()
    {
	CActiveScheduler::Add(this);
    }
    
void CTinyTestClass::RunL()
	{
	iOwner->DoWork();
    }
    
void CTinyTestClass::DoCancel()
    {
	Cancel();
	}
    
void CTinyTestClass::StartAOService()
	{
    TRequestStatus* pStat = &iStatus;	
  	User::RequestComplete(pStat, KErrNone);
  	SetActive();
	}
