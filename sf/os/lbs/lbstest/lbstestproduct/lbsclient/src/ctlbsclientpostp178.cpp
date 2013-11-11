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



//  INCLUDES
#include "ctlbsclientpostp178.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp178::CT_LbsClientPosTp178(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "Tp178 - Request Timeout");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp178::~CT_LbsClientPosTp178()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp178::CloseTest
// Always called after the test, even if the test leaves
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp178::CloseTest()
{
    ClosePositioner();
    Disconnect();
}


TInt Timeout178(TAny* /*aNothing*/)
    {
    CActiveScheduler::Stop();
    return KErrNone;
    }

void StartSchedulerForAWhile178L(TTimeIntervalMicroSeconds32 aTimeout)
    {
    CPeriodic* timer = CPeriodic::NewL(CActive::EPriorityStandard);
    CleanupStack::PushL(timer);
    timer->Start(aTimeout, aTimeout, TCallBack(Timeout178));
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy(timer);  
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp178::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp178::StartL()
    {
    _LIT(KServiceAccept, "SAAA");
    
    SetupPsyL(iUidTestPsy3);
    
    RPositioner positioner;
    TPositionInfo info = TPositionInfo();
    
    ConnectL();
    
    User::LeaveIfError(positioner.Open(iPosServer,iUidTestPsy3));
    CleanupClosePushL(positioner);
    
    TInt Err = positioner.SetRequestor(CRequestor::ERequestorService,
        CRequestor::EFormatApplication, KServiceAccept);
    
    TPositionUpdateOptions updateOptionsLong, updateOptionsShort;
    
    TTimeIntervalMicroSeconds longInterval(7000000);
    
    updateOptionsLong.SetUpdateTimeOut(longInterval);
    Err = positioner.SetUpdateOptions(updateOptionsLong);
    
    TTimeIntervalMicroSeconds shortInterval(2000000);
    updateOptionsShort.SetUpdateTimeOut(shortInterval);
    
    TPositionUpdateOptions theUpdateOptions;
    Err = positioner.GetUpdateOptions(theUpdateOptions);
    
    if (theUpdateOptions.UpdateTimeOut() != updateOptionsLong.UpdateTimeOut() ||
        updateOptionsLong.UpdateTimeOut() != longInterval)
    {
        _LIT(KUpdateOptions, "The update option was not set correctly");
        LogErrorAndLeaveL(KUpdateOptions);
    }
    
    _LIT(KDelayMsg, "The successfull requests was completed within %d microsecs.");
    _LIT(KCancelMsg, "The canceled requests was completed within %d microsecs.");
    
    TRequestStatus status;
    
    for (TInt i = 0; i < 10; i++) // makes 10 test inorder to get some statistic
    {         
        
        positioner.SetUpdateOptions(updateOptionsLong);
        TTime requestStartTime;
        
        requestStartTime.UniversalTime();
        positioner.NotifyPositionUpdate(info, status);
        User::WaitForRequest(status);
        TTime requestStopTime;
        requestStopTime.UniversalTime();
        
        TTimeIntervalMicroSeconds durationMicro = requestStopTime.MicroSecondsFrom(requestStartTime);
        
        TInt duration = durationMicro.Int64();
        
        TBuf<100> timeMsg;
        timeMsg.Format(KDelayMsg, duration);
        INFO_PRINTF1(timeMsg);
        
        //check error status
        if (status != KErrNone)
        {
            _LIT(KErrPositionRequest, "error code returned from NotifyPositionUpdate, error code = %d");
            TBuf<100> buf;
            buf.Format(KErrPositionRequest, status.Int());
            LogErrorAndLeaveL(buf);
        }
        
        
        TTimeIntervalMicroSeconds shortInterval(2000000);
        updateOptionsShort.SetUpdateTimeOut(shortInterval);
        
        positioner.SetUpdateOptions(updateOptionsShort);
        
        requestStartTime.UniversalTime();
        positioner.NotifyPositionUpdate(info, status);
        User::WaitForRequest(status);
        requestStopTime.UniversalTime();
        
        durationMicro = requestStopTime.MicroSecondsFrom(requestStartTime);
        
        duration = durationMicro.Int64();
        
#ifdef __WINS__
        TTimeIntervalMicroSeconds winsFail(100000);
        durationMicro = TTimeIntervalMicroSeconds(durationMicro.Int64()+winsFail.Int64());
#endif
        
        timeMsg.Format(KCancelMsg, duration);
        INFO_PRINTF1(timeMsg);
        
        
        if (status != KErrTimedOut)
        {
            _LIT(KErrPositionRequest, "Request did not returned KErrTimedOut, status code = %d.");
            TBuf<100> buf;
            buf.Format(KErrPositionRequest, status.Int());
            LogErrorAndLeaveL(buf);
        }

        //Check that the request was not aborted before the Interval
        //Remove this condition if a lot of data is needed in test log.
        if (durationMicro < shortInterval)
        {
            _LIT(KErrInterval, " The request was aborted before the set timed out ");
            LogErrorAndLeaveL(KErrInterval);
        }
        
        
    } // end for loop
    CleanupStack::PopAndDestroy(1); //positioner
    Disconnect();

    // Do timeout test
    TestTimeoutL();

    // Do cancel test
    TestCancelL();

    // ESLI-5QRA7U just check that it is not possible to set a timeout that is less than
    // the update interval
    TestOptionL();
    }
    
// ---------------------------------------------------------
// CT_LbsClientPosTp178::TestTimeout
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp178::TestTimeoutL()
    {
    _LIT(KSTART, "TestTimeout()");
    _LIT(KEND, "TestTimeout() passed");
    _LIT(KTIME, "Request took %d us");
    
    // Setup verification stuff
    INFO_PRINTF1(KSTART);
    
    //TPrivDlgDiagnostics tmpdiag;
    TTime requestStartTime;
    TTime requestStopTime;
    TTimeIntervalMicroSeconds durationMicro;
    
    TBuf<100> buf;
    
	_LIT(KService, "SAAA");
    TPositionUpdateOptions updateOptions;
    
    TTimeIntervalMicroSeconds longInterval(7000000);
    ConnectL();
    SetupPsyL(iUidTestPsy3);

    OpenPositionerByName(iUidTestPsy3);    
    updateOptions.SetUpdateTimeOut(longInterval);
    
    // Access protected member (iPositioner) here
    iPositioner.SetUpdateOptions(updateOptions);
    
    _LIT(KONE, "Perform first request, should NOT time out");
    INFO_PRINTF1(KONE);
    
    requestStartTime.UniversalTime();
    //Make one request to verify psy
    PerformRequestL(KService, CRequestor::ERequestorService, CRequestor::EFormatApplication);
    
    CheckRequestResultL(KErrNone);
    requestStopTime.UniversalTime();

    durationMicro = requestStopTime.MicroSecondsFrom(requestStartTime);
    TInt duration = durationMicro.Int64();
    
    buf.Format(KTIME, duration);
    INFO_PRINTF1(buf);
    
    ClosePositioner();
    
    OpenPositionerByName(iUidTestPsy3);
    // ECancelButton means that the Automatised class should wait for a cancel call
    // before returning result
    
    updateOptions.SetUpdateTimeOut(longInterval);
    
    // Access protected (iPositioner) member here
    iPositioner.SetUpdateOptions(updateOptions);
    
    _LIT(KTWO, "Perform second request, should not time out");
    INFO_PRINTF1(KTWO);
    
    //Make one request to verify psy
    TPositionInfo info = TPositionInfo();
    requestStartTime.UniversalTime();
    TInt err = PerformSyncRequest(KService, &info);
    requestStopTime.UniversalTime();
    
    if (err != KErrNone)
    {
        _LIT(KError, "Wrong result from iStatus, should be KErrNone, was %d");
        TBuf<100> buf;
        buf.Format(KError, err);
        LogErrorAndLeaveL(buf);
    }
    
    durationMicro = requestStopTime.MicroSecondsFrom(requestStartTime); 
    duration = durationMicro.Int64();
    
    buf.Format(KTIME, duration);
    INFO_PRINTF1(buf);
    
    ClosePositioner();
    Disconnect();

    INFO_PRINTF1(KEND);
}

// ---------------------------------------------------------
// CT_LbsClientPosTp178::TestCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp178::TestCancelL()
    {
    _LIT(KSTART, "TestCancel()");
    _LIT(KEND, "TestCancel() passed");
    
    // Setup verification stuff
    INFO_PRINTF1(KSTART);
    
	_LIT(KService, "SAAA");
    ConnectL();
    SetupPsyL(iUidTestPsy3);    
   
    OpenPositionerByName(iUidTestPsy3);
    
    PerformRequestL(KService, CRequestor::ERequestorService, CRequestor::EFormatApplication);
    StartSchedulerForAWhile178L(2000000);
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate); 
    CheckRequestResultL(KErrCancel);

    ClosePositioner();
    Disconnect();

    INFO_PRINTF1(KEND);
}
    

void CT_LbsClientPosTp178::TestOptionL()
    {
    SetupPsyL(iUidTestPsy3);
    
    RPositioner positioner;
    
    ConnectL();
    
    User::LeaveIfError(positioner.Open(iPosServer,iUidTestPsy3));
    CleanupClosePushL(positioner);
    
    _LIT(KKalle, "Kalle");
    positioner.SetRequestor(CRequestor::ERequestorService,
        CRequestor::EFormatApplication, KKalle);
    
    TPositionUpdateOptions updateOptions;
    
    TTimeIntervalMicroSeconds timeoutInterval(4000000);
    TTimeIntervalMicroSeconds periodicInterval(7000000);
    
    updateOptions.SetUpdateTimeOut(timeoutInterval);
    updateOptions.SetUpdateInterval(periodicInterval);
    TInt err = positioner.SetUpdateOptions(updateOptions);
    if (err != KErrArgument)
        {
        _LIT(KErrArg, "Possible to set timout < update interval, error code = %d");
        TBuf<100> buf;
        buf.Format(KErrArg, err);
        INFO_PRINTF1(buf);
        LogErrorAndLeaveL(buf);
        }

    //should never be runned;
    CleanupStack::PopAndDestroy(1); //positioner
    Disconnect();
    }

    
// End of File
    
