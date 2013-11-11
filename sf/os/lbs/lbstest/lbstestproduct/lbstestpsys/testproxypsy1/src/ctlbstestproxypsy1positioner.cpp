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
#include "ctlbstestproxypsy1positioner.h"
#include "ctlbstestproxypsybase.h"
#include "testproxypsy1.hrh"

_LIT(KFileName,  "c:\\logs\\testproxypsy1.txt");
_LIT(KFileNameStopTrack, "c:\\logs\\testproxypsy1stoppedtracking.txt"); 

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestProxyPsy1Positioner::CT_LbsTestProxyPsy1Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestProxyPsy1Positioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iRequestHandler = new (ELeave) CT_LbsTestProxyPsyBase(PositionerStatus());
              
    TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	
	const TTimeIntervalMicroSeconds KTimerDelay(3000000);
	iRequestHandler->SetTimerDelay(KTimerDelay);
	
    User::LeaveIfError(iFileserver.Connect());
	}

// Two-phased constructor.
CT_LbsTestProxyPsy1Positioner* CT_LbsTestProxyPsy1Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy1Positioner* self = new(ELeave) CT_LbsTestProxyPsy1Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestProxyPsy1Positioner* CT_LbsTestProxyPsy1Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy1Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestProxyPsy1Positioner::~CT_LbsTestProxyPsy1Positioner()
    {
    delete iRequestHandler;
 
    RFile file;
    file.Create(iFileserver, KFileName, EFileWrite);
    file.Close();    
    iFileserver.Close();
	}

// ---------------------------------------------------------
// CTestPsyPositioner1::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy1Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
    TBuf<256> buf;
    OpenFileForAppend();    
    _LIT(KTestProxyPsy1Positioner, " Request issued to TestProxyPsy1Positioner");
    buf.Append(KTestProxyPsy1Positioner);
    _LIT(KAppend, "\r");
    buf.Append(KAppend);
    iFileText.Write(buf);
    iFile.Close();
    
    TRequestStatus* status = &aStatus;   
    TPositionInfo* posInfo = static_cast<TPositionInfo*> (&aPosInfo);
    TPosition pos;
	pos.SetCoordinate(5, 15, 25);
	posInfo->SetPosition(pos);
	
	TUid implUid = { KPosImplementationUid };
	posInfo->SetModuleId(implUid);	
    TUint32 request = posInfo->UpdateType();
    switch (request)
        {
        // case 1-10 - TC269
        // case 11-15 - TC270
        // case 21-26 - TC271
        case 1:
        case 3:
        case 4:
        case 11:
        case 13:
        case 23:
            User::RequestComplete( status, KErrGeneral);
            break;
        case 2:
        case 12:
        case 14:
        case 15:
        case 26:
            User::RequestComplete( status, KErrNone);
            break;
        case 5:
        case 24:
        case 25:
             User::RequestComplete( status, KErrTimedOut);
             break;
        case 6:
        case 9:
        case 10:
        case 21:
            User::RequestComplete( status, KErrDied);
            break;
        case 7:
        case 8:
        case 22:
            User::RequestComplete( status, KErrBadPower);
            break;
        case 4701:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(9000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4702:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(7000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4703:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(6000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4704:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(4000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4705:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(14000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4707:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(7000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4708:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(9000000);  
            iRequestHandler->NotifyPositionUpdate(posInfo, status);          
            break;
        case 4709:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(9000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);            
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
			 		 	
        default:
            User::RequestComplete( status, KErrBadPower);
            break;
        }
	}

// ---------------------------------------------------------
// CTestPsyPositioner1::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy1Positioner::CancelNotifyPositionUpdate()
	{
	TBuf<256> buf;
    OpenFileForAppend();
    _LIT(KTestProxyPsy1Positioner, " Request issued to TestProxyPsy1Positioner is cancelled");
    buf.Append(KTestProxyPsy1Positioner);
    _LIT(KAppend, "\r");
    buf.Append(KAppend);
    iFileText.Write(buf);
    iFile.Close();
    
    if(iRequestHandler->IsActive())
    	{
    	iRequestHandler->CancelNotifyPositionUpdate();
    	}
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsy1Positioner::TrackingOverridden
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsTestProxyPsy1Positioner::TrackingOverridden() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsy1Positioner::StartTrackingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy1Positioner::StartTrackingL(
	const TTimeIntervalMicroSeconds& /*aInterval*/)
    {
    iRequestHandler->StartTracking();
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsy1Positioner::StopTracking
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy1Positioner::StopTracking()
    {
    iRequestHandler->StopTracking();

    RFile file;
    file.Create(iFileserver, KFileNameStopTrack, EFileWrite);
    file.Close();
    }
    
TInt CT_LbsTestProxyPsy1Positioner::OpenFileForAppend()
    {
    TInt err = iFile.Open(iFileserver, KFileName, EFileStreamText|EFileWrite);
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err = iFile.Create(iFileserver, KFileName, EFileStreamText|EFileWrite);
            }

        if (err != KErrNone)
            {
            return err;
            }
        
        }
    iFileText.Set(iFile);
    return KErrNone;
    }
