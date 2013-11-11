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



//  INCLUDES
#include "ctlbsclientpostp272.h"

#include <e32std.h>

// CONSTANTS                                
const TInt KUpdateInterval  = 10000000;
const TInt KUpdateInterval2 = 15000000;
const TInt KUpdateInterval3 = 2000000;

const TInt KMinTimeLimitRequest1 = 0;
const TInt KMaxTimeLimitRequest1 = 2000000;

const TInt KMinTimeLimitRequest2 = 9000000;
const TInt KMaxTimeLimitRequest2 = 12000000;

const TInt KMinTimeLimitRequest3 = 14000000;
const TInt KMaxTimeLimitRequest3 = 17000000;

const TInt KMinTimeLimitRequest4 = 0;
const TInt KMaxTimeLimitRequest4 = 3000000;//200000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp272::CT_LbsClientPosTp272(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName,"Tp272-Perodic Update, Changing the interval");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp272::~CT_LbsClientPosTp272()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp272::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp272::StartL()
    {
    _LIT(KOpenErr, "Error when opening MultiPSY, %d");
    _LIT(KServiceName, "TP272");
    
    SetupPsyL(iUidMultiPsy);
    
    ConnectL();
    
    TInt err = OpenPositionerByName(iUidMultiPsy);
    AssertTrueSecL(err == KErrNone, KOpenErr, err);
    
    iPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KServiceName);

    SetPeriodicUpdateL(KUpdateInterval);
    
    TInt request=0;
    // NotifyPositionUpdate should be called direct by the server on the first request 
    // even if update interval is used
    RequestL(++request, KMinTimeLimitRequest1, KMaxTimeLimitRequest1);    
   
    // Even if same update interval is set again should the first request complete direct
    SetPeriodicUpdateL(KUpdateInterval);
    RequestL(++request, KMinTimeLimitRequest2, KMaxTimeLimitRequest2);

    // NotifyPositionUpdate should be called after the update interval on the second request 
    // but when the update interval is changed should it be called direct
    RequestL(++request, KMinTimeLimitRequest2, KMaxTimeLimitRequest2, KUpdateInterval2);
    
    // Now should the request time be about 10 sec
    RequestL(++request, KMinTimeLimitRequest3, KMaxTimeLimitRequest3); 

    SetPeriodicUpdateL(KUpdateInterval3);

    // Should return direct since update interval changed again
    RequestL(++request, KMinTimeLimitRequest4, KMaxTimeLimitRequest4);
    

    }

// ---------------------------------------------------------
// CT_LbsClientPosTp272::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp272::CloseTest()
    {
    iPositioner.Close();
    iPosServer.Close();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp272::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp272::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }
  
// ---------------------------------------------------------
// CT_LbsClientPosTp272::RequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp272::RequestL(const TInt& aRequestIndex, 
                         const TInt64& aMinTimeLimit,
                         const TInt64& aMaxTimeLimit,
                         const TInt& aPeriodicUpdateInterval)
	{
    _LIT(KUnexpectedErrCode, "Unexpected error code %d returned by the used PSY");
    _LIT(KRequestMaxTimeErr, "Request took to long");
    _LIT(KRequestMinTimeErr, "Request was to fast");
    _LIT(KRequestTime, "Request took %d microsecs");

    TBuf<100> info;
    _LIT(KInfo, "Making request %d");
    info.Format(KInfo, aRequestIndex);
    INFO_PRINTF1(info);
    
    TTime start, stop;
    start.UniversalTime();

    TPositionInfo posInfo;
    TRequestStatus  status;
    iPositioner.NotifyPositionUpdate(posInfo, status);

    if (aPeriodicUpdateInterval != 0)
        {
        User::After(5000000);
        SetPeriodicUpdateL(KUpdateInterval2);
        }

    User::WaitForRequest(status);
    stop.UniversalTime();  
    
    TInt64 usec = (stop.Int64() - start.Int64());
	info.Format(KRequestTime, usec);
    INFO_PRINTF1(info);

    AssertTrueL(status.Int() == KErrNone, KUnexpectedErrCode, status.Int());
    
    AssertTrueSecL(usec <= aMaxTimeLimit, KRequestMaxTimeErr); 
    AssertTrueSecL(usec >= aMinTimeLimit, KRequestMinTimeErr); 
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp272::SetPeriodicUpdateL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp272::SetPeriodicUpdateL(const TInt& aUpdateInterval)
    {
    _LIT(KUpdateErr, "Error when setting update interval, %d");

    TPositionUpdateOptions updateOptions;
	updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(aUpdateInterval));
    TInt err = iPositioner.SetUpdateOptions(updateOptions);
	AssertTrueL(err == KErrNone, KUpdateErr, err);   

    }

//  End of File
