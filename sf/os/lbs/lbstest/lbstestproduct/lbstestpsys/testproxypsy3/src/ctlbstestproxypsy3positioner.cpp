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
#include <lbspositioninfo.h>
#include "ctlbstestproxypsy3positioner.h"
#include "ctlbstestproxypsybase.h"
#include "testproxypsy3.hrh"


// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestProxyPsy3Positioner::CT_LbsTestProxyPsy3Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestProxyPsy3Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = new (ELeave) CT_LbsTestProxyPsyBase(PositionerStatus());
              
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	
	}

// Two-phased constructor.
CT_LbsTestProxyPsy3Positioner* CT_LbsTestProxyPsy3Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy3Positioner* self = new(ELeave) CT_LbsTestProxyPsy3Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestProxyPsy3Positioner* CT_LbsTestProxyPsy3Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy3Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestProxyPsy3Positioner::~CT_LbsTestProxyPsy3Positioner()
    {
    delete iRequestHandler;
	}

// ---------------------------------------------------------
// CTestPsy3Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy3Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
    TPositionInfo* posInfo = static_cast<TPositionInfo*> (&aPosInfo);
    TUid implUid = { KPosImplementationUid };
	posInfo->SetModuleId(implUid);

    TRequestStatus* status = &aStatus;
    TUint32 request = posInfo->UpdateType();
    switch (request)
        {
        // case 1-10 - TC269
        // case 11-15 - TC270
        // case 21-25 - TC271
        case 1:
        case 2:
        case 5:
        case 15:
            iRequestHandler->SetErrorCode(KErrDiskFull);
            break;
        case 3:
        case 4:
        case 7:
        case 10:
        case 11:
        case 13:
        case 14:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
            iRequestHandler->SetErrorCode(KErrPositionBufferOverflow);
            break;
        case 100:
			iRequestHandler->ReportStatus(
				TPositionModuleStatus::EDeviceInitialising, 
				TPositionModuleStatus::EDataQualityUnknown);
			 User::RequestComplete( status, KErrNone );
			 break; 
			 
		case 200:
            iRequestHandler->ReportStatus(
				TPositionModuleStatus::EDeviceActive, 
				TPositionModuleStatus::EDataQualityUnknown);
			User::RequestComplete( status, KErrNone );
			break; 

		case 304:	// return error to cause fallback
 		    iRequestHandler->SetErrorCode(KErrTimedOut);
 		    /*
  	        iRequestHandler->SetErrorCode(KErrNone);
  	        iRequestHandler->SetTimerDelay(9000000);
  	        */
 		    break;
			 
        case 6:
        case 8:
        case 9:
        case 12:
        default:
			iRequestHandler->SetErrorCode(KErrUnknown);
            break;

        }

	iRequestHandler->NotifyPositionUpdate(posInfo, &aStatus);
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsy3Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy3Positioner::CancelNotifyPositionUpdate()
	{
	   if(iRequestHandler->IsActive())
	    {
	    	
	      iRequestHandler->CancelNotifyPositionUpdate();
	      
		}
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsy3Positioner::TrackingOverridden
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsTestProxyPsy3Positioner::TrackingOverridden() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsy3Positioner::StartTrackingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy3Positioner::StartTrackingL(const TTimeIntervalMicroSeconds& /*aInterval*/)
    {
    iRequestHandler->StartTracking();
    }

// ---------------------------------------------------------
// CTestPsyProxyPsy3Positioner::StopTracking
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy3Positioner::StopTracking()
    {
    iRequestHandler->StopTracking();
    }
