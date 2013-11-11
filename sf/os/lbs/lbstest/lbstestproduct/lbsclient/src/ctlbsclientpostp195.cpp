// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ctlbsclientpostp195.h"
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbscommon.h>
#include <lbsrequestor.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp195::CT_LbsClientPosTp195(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP195 - Periodic Updates, Slow PSY");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp195::~CT_LbsClientPosTp195()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp195::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp195::StartL()
    {
	SetupPsyL(iUidTestPsy3);

	TInt numberOfRuns = 5;
	TTimeIntervalMicroSeconds interval = TTimeIntervalMicroSeconds(2000000);
    const TInt slowPsyUpdate = 4000000;
    
    #ifdef __WINS__
    const TInt KErrorInterval = 2500000;
    #else
    const TInt KErrorInterval = 500000;
    #endif

    
    RPositionServer	posServer;
    CleanupClosePushL(posServer);
    RPositioner positioner;
    CleanupClosePushL(positioner);
    TPositionInfo positionInfo = TPositionInfo();

	_LIT(KConnectErr, "Error when connecing to EPos server,  %d");
	TInt err = posServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);

	_LIT(KOpenErr, "Error when opening positioner,  %d");
	err = positioner.Open(posServer, iUidTestPsy3);
	AssertTrueL(err == KErrNone, KOpenErr, err);

	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(interval);
	
	_LIT(KUpdateErr, "Error when setting update interval,  %d");
	err = positioner.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KUpdateErr, err);

    _LIT(KService ,"Service");
	positioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KService);

    //do a couple of request and check the times between requests.

    TTime now, startTime;
    TTimeIntervalMicroSeconds requestTime;
    _LIT(KErrorRequest, "Error in request");
    TRequestStatus status;
    TInt64 reqTime;
    
    for (TInt i = 0; i < numberOfRuns; i++)
        {
        startTime.UniversalTime();
        positioner.NotifyPositionUpdate(positionInfo, status);
        User::WaitForRequest(status);
        err = status.Int();
        AssertTrueL(err == KErrNone, KErrorRequest, err);
        now.UniversalTime();
        requestTime = now.MicroSecondsFrom(startTime);
        _LIT(KError, "Request time, %d microsecs.");
        TBuf<100> buf;
        reqTime = requestTime.Int64();
        buf.Format(KError, reqTime);
        INFO_PRINTF1(buf);
        if (i != 0)
            {
            //check that the time is ok, by accepting an error interval
            TInt var1 = 2000000 + slowPsyUpdate + KErrorInterval;
            TInt var2 = 2000000 + (slowPsyUpdate - KErrorInterval);
            if (reqTime > var1 || reqTime < var2)
                {
                _LIT(KErrorInterval, "The update is not within valid range");
                LogErrorAndLeaveL(KErrorInterval);
                }
            }
        }
    
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);

	}

// End of File
