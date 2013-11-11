// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ctlbsclientpostp203.h"
#include "ctlbsrequester.h"
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp203::CT_LbsClientPosTp203(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP203 - Periodic Update, default psy");
    SetTestStepName(KTestName); 
    }

// Destructor
CT_LbsClientPosTp203::~CT_LbsClientPosTp203()
    {
    }

// ---------------------------------------------------------
// CPosTp203::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp203::CloseTest()
    {
    delete iPeriodicTimer;
    iPeriodicTimer = NULL;
    }

// ---------------------------------------------------------
// CPosTp203::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp203::StartL()
    {
    const TInt KLongErrorInterval = 3000000;
    
#ifdef __WINS__
    const TInt KNormalErrorInterval = 1500000;
#else
    const TInt KNormalErrorInterval = 500000;
#endif
    
    
    TInt numberOfRuns = 6;
    TInt normalInterval = 4500000;
    // Using the same interval as the slow psy (TestPsy3) to make things easy.
	TTimeIntervalMicroSeconds interval = TTimeIntervalMicroSeconds(normalInterval);
    TTimeIntervalMicroSeconds shortInterval = TTimeIntervalMicroSeconds(1500000);

    // Will happen the first time when default is changed
    //It is required because we change the PSY being in the tracking mode. So, we may have to wait up to 
    //the tracking interval first and then wait a liitle more than the PSY time to fix. 
    TInt longInterval = 8500000; 
    TInt errorInterval = KNormalErrorInterval;

    TInt requestUpdate = normalInterval;

    RPositionServer	posServer;
    CleanupClosePushL(posServer);
	TInt err = posServer.Connect();
	_LIT(KConnectErr, "Error when connecing to EPos server,  %d");
	AssertTrueL(err == KErrNone, KConnectErr, err);  
    
	SetupPsyL(iUidMultiPsy);

    RPositioner positioner;
    CleanupClosePushL(positioner);
    err = positioner.Open(posServer); // Use Default PSY
	_LIT(KOpenErr, "Error when opening positioner,  %d");
	AssertTrueL(err == KErrNone, KOpenErr, err);

	TPositionInfo positionInfo = TPositionInfo();
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(interval);
	
	err = positioner.SetUpdateOptions(posOption);
	_LIT(KUpdateErr, "Error when setting update interval,  %d");
	AssertTrueL(err == KErrNone, KUpdateErr, err);

    _LIT(KService ,"Service");
	positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);

    // Do a couple of request and check the times between requests.
    TTime now, startTime;
    TTimeIntervalMicroSeconds requestTime;
    _LIT(KErrorRequest, "Error in request");
    TRequestStatus status;
    TInt64 reqTime;
   
    for (TInt i = 0; i < numberOfRuns; i++)
        {
        TBuf<100> info;
        _LIT(KInfo, "Making request %d");
    	info.Format(KInfo, i);
        INFO_PRINTF1(info);
        if (i == 2)
            {
            _LIT(KSetTestPSY3, "Setting up TestPsy3");
            INFO_PRINTF1(KSetTestPSY3);
            SetupPsyL(iUidTestPsy3);
            User::After(1000000);
            requestUpdate = longInterval;
            errorInterval = KLongErrorInterval;
            }
        else
            {
            requestUpdate = normalInterval;
            errorInterval = KNormalErrorInterval;
            }
        
        _LIT(KWaitStatus1, "Waiting for status after NotifyPositionUpdate1");
        INFO_PRINTF1(KWaitStatus1);
        
        startTime.UniversalTime();
        positioner.NotifyPositionUpdate(positionInfo, status);
        User::WaitForRequest(status);
        now.UniversalTime();
        
        err = status.Int();
        AssertTrueL(err == KErrNone, KErrorRequest, err);

        requestTime = now.MicroSecondsFrom(startTime);
        _LIT(KError, "Request time, %d microsecs.");
        TBuf<100> buf;
        reqTime = requestTime.Int64();
        buf.Format(KError, reqTime);
        INFO_PRINTF1(buf);
        
        if (i != 0)
            {
            // Check that the time is ok, by accepting an error interval
            if (reqTime > (requestUpdate + errorInterval) || 
            	reqTime < (normalInterval - errorInterval))
                {
                _LIT(KErrorInterval, "The update is not within valid range");
                LogErrorAndLeaveL(KErrorInterval);
                }
            }
        
        // Check that position was returned from correct PSY:
        if (i < 2)
        	{
        	VerifyPositionFromL(positionInfo, iUidMultiPsy);
        	}
        else
        	{
        	VerifyPositionFromL(positionInfo, iUidTestPsy3);
        	}
        }
    
	_LIT(KSetUpdateOptions, "Calling SetUpdateOptions");
	INFO_PRINTF1(KSetUpdateOptions);
	posOption.SetUpdateInterval(shortInterval);
	err = positioner.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KUpdateErr, err);

    // Set up three PSYs the two onces with top priorities supports tracking 
	// and the third one does not
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    _LIT(KSetPriority0, "Enabling Tracking PSY with priority 0");
	INFO_PRINTF1(KSetPriority0);
    moduleUpdate->SetUpdateAvailability(ETrue);
	User::After(1000000);
	
    db->UpdateModuleL(iUidTestTrackingPsy, *moduleUpdate);
    db->SetModulePriorityL(iUidTestTrackingPsy, 0);
	User::After(1000000);
	
	
	
	_LIT(KSetPriority1, "Enabling Timer PSY with priority 1");
	/*INFO_PRINTF1(KSetPriority1);
    db->UpdateModuleL(iUidTestTimerPsy, *moduleUpdate);
    db -> SetModulePriorityL(iUidTestTimerPsy, 1);
	User::After(1000000);*/

	_LIT(KSetPriority2, "Enabling Multi PSY with priority 2");
	/*INFO_PRINTF1(KSetPriority2);
    db->UpdateModuleL(iUidMultiPsy, *moduleUpdate);
    db -> SetModulePriorityL(iUidMultiPsy, 2);
    User::After(1000000); // just to let the events happen in the server.*/
    
    
    // Make one request to ensure that Start tracking is called.
    positioner.NotifyPositionUpdate(positionInfo, status);
    _LIT(KWaitStatus2, "Waiting for status after NotifyPositionUpdate2");
    INFO_PRINTF1(KWaitStatus2);    
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone || err == KPositionQualityLoss, KErrorRequest, err);

    TPosition pos;
    positionInfo.GetPosition(pos);    
    User::After(1000000);

    // TestTrackingPsy returns 65.0 for Tracking requests and 30.0 for ordinary position requests
    _LIT(KErrorAndLeave, "Wrong position returned from tracking PSY");
    if (pos.Latitude() != 65.0 || pos.Longitude() != 65.0 || pos.Altitude() != 65.0)
        {
        LogErrorAndLeaveL(KErrorAndLeave);
        }

	INFO_PRINTF1(KSetPriority1);
    db->UpdateModuleL(iUidTestTimerPsy, *moduleUpdate);
    db -> SetModulePriorityL(iUidTestTimerPsy, 1);
	User::After(2000000); // MH move
	
    // Disable the psy with top priority, should result in switch of psy
    _LIT(KDisablePSY, "Disabling Tracking PSY");
    INFO_PRINTF1(KDisablePSY);
    moduleUpdate->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidTestTrackingPsy, *moduleUpdate);
    // To let the events happen in the server.
    User::After(2000000); 
    
    // Make one request to enable tracking
    positioner.NotifyPositionUpdate(positionInfo, status);
    _LIT(KWaitStatus4, "Waiting for status after NotifyPositionUpdate4");
    INFO_PRINTF1(KWaitStatus4);    
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone || err == KPositionQualityLoss, KErrorRequest, err);

    positionInfo.GetPosition(pos);
    
    // To let the events happen in the server.
    User::After(2000000);
        
    // TestTimerPsy returns 55.0 for Tracking requests and 20.0 for ordinary position requests
    if (pos.Latitude() != 55.0 || pos.Longitude() != 55.0 || pos.Altitude() != 55.0)
        {
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    // Enable the psy with top priority, should result in switch back to that psy
    _LIT(KEnablePSY, "Enabling tracking PSY");
    INFO_PRINTF1(KEnablePSY);
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestTrackingPsy, *moduleUpdate);
    User::After(2000000); // just to let the events happen in the server.

	// To enable tracking    
    positioner.NotifyPositionUpdate(positionInfo, status);
    _LIT(KWaitStatus6, "Waiting for status after NotifyPositionUpdate6");
    INFO_PRINTF1(KWaitStatus6);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone || err == KPositionQualityLoss, KErrorRequest, err);

    // To let the events happen in the server.
	User::After(2000000); 

    positionInfo.GetPosition(pos);
    // TestTrackingPsy returns 65.0 for Tracking requests and 30.0 for ordinary position requests
    if (pos.Latitude() != 65.0 || pos.Longitude() != 65.0 || pos.Altitude() != 65.0)
        {
        LogErrorAndLeaveL(KErrorAndLeave);
        }
    
	INFO_PRINTF1(KSetPriority2);
    db->UpdateModuleL(iUidMultiPsy, *moduleUpdate);
    db -> SetModulePriorityL(iUidMultiPsy, 2);
    // To let the events happen in the server. //MH
    User::After(1000000); 

    // Disable two psys with top priority that supports tracking.
    _LIT(KDisableTimer,"Disabling Timer and Tracking PSY");
    INFO_PRINTF1(KDisableTimer);
    moduleUpdate->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidTestTimerPsy, *moduleUpdate);
    db->UpdateModuleL(iUidTestTrackingPsy, *moduleUpdate);
    User::After(2000000); // just to let the events happen in the server.

    positioner.NotifyPositionUpdate(positionInfo, status);
    _LIT(KWaitStatus8, "Waiting for status after NotifyPositionUpdate8");
    INFO_PRINTF1(KWaitStatus8);
    User::WaitForRequest(status);
    err = status.Int();
    AssertTrueL(err == KErrNone, KErrorRequest, err);

    if (iUidMultiPsy.iUid != positionInfo.ModuleId().iUid)
		{
		_LIT(KIncorrectPsyError,
           "Position from wrong PSY received");
		LogErrorAndLeaveL(KIncorrectPsyError);
		}

    // Disable a PSY during ongoing requesting
    // Enable the the top three PSYs
    _LIT(KEnableTimer,"Enabling Timer PSY");
	INFO_PRINTF1(KEnableTimer);
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidTestTimerPsy, *moduleUpdate);
    db->UpdateModuleL(iUidTestTrackingPsy, *moduleUpdate);
    moduleUpdate->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidMultiPsy, *moduleUpdate);
    // To let the events happen in the server.
    User::After(2000000); 

    const TInt KRequestInterval = 4000000;
    TCallBack callback(DisablePsyL, this);
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    TTimeIntervalMicroSeconds32 disableTime = TTimeIntervalMicroSeconds32(2000000);
    iPeriodicTimer->Start(disableTime, disableTime, callback);
    CT_LbsRequester* request = CT_LbsRequester::NewL(this, KRequestInterval, Logger());   // TTF = 0s
    CleanupStack::PushL(request);

    // To enable tracking
    _LIT(KRequest1, "Calling request->StartRequest1");
    INFO_PRINTF1(KRequest1);
    request->StartRequest(); // only one request should exist

    CActiveScheduler::Start();

    err = request ->CompletionCode();
    _LIT(KErrCode, "Competion code is not KErrNone");
    AssertTrueL(err == KErrNone, KErrCode, err);

    request->GetPositionInfo(positionInfo);
    User::After(2000000);    
   
    // Should get tracking data
    _LIT(KRequest2, "Calling request->StartRequest2");
    INFO_PRINTF1(KRequest2);
    request->StartRequest(); // only one request should exist in reqlog

    CActiveScheduler::Start();

    err = request ->CompletionCode();
    AssertTrueL(err == KErrNone, KErrCode, err);

    request ->GetPositionInfo(positionInfo);
    positionInfo.GetPosition(pos);

    if (iUidTestTimerPsy.iUid != positionInfo.ModuleId().iUid)
		{
		_LIT(KIncorrectPsyError,
           "Position from wrong PSY received");
		LogErrorAndLeaveL(KIncorrectPsyError);
		}
    // TestTimerPsy returns 55.0 for Tracking requests and 20.0 for ordinary position requests
    if (pos.Latitude() != 55.0 || pos.Longitude() != 55.0 || pos.Altitude() != 55.0)
        {
        LogErrorAndLeaveL(KErrorAndLeave);
        }

    _LIT(KSetPSY, "Setting up Tracking PSY");
	INFO_PRINTF1(KSetPSY);
    SetupPsyL(iUidTestTrackingPsy);
    User::After(1500000); // just to let the events happen in the server.

    CT_LbsRequester* request2 = CT_LbsRequester::NewL(this, interval, Logger());   // TTF = 0s
    CleanupStack::PushL(request2);

    iPeriodicTimer->Start(disableTime, disableTime, callback);
    _LIT(KRequest3, "Calling request2->StartRequest");
    INFO_PRINTF1(KRequest3);
    request2->StartRequest(); // only one request should exist in reqlog

    CActiveScheduler::Start();

    err = request2 ->CompletionCode();
    _LIT(KErrNotFoundCode, "Competion code is not KErrNone");
    AssertTrueL(err == KErrNone, KErrNotFoundCode, err);

    CleanupStack::PopAndDestroy(request2);
    CleanupStack::PopAndDestroy(request);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);

    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
	}

TInt CT_LbsClientPosTp203::DisablePsyL(TAny* aSelf)
    {
    // Do not forget to cancel the timer
    CT_LbsClientPosTp203* self = static_cast<CT_LbsClientPosTp203*> (aSelf);
    self -> iPeriodicTimer -> Cancel();
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    moduleUpdate->SetUpdateAvailability(EFalse);
    TUid trackingId;
    trackingId.iUid = KUidTestTrackingPsy;
    db->UpdateModuleL(trackingId, *moduleUpdate);
    CleanupStack::PopAndDestroy(moduleUpdate);
    CleanupStack::PopAndDestroy(db);
    return KErrNone;
    }

void CT_LbsClientPosTp203::RequestCompleteL()
    {
    CActiveScheduler::Stop();
    }

// End of File
