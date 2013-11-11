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
#include <f32file.h>
#include "ctlbstestproxypsy2positioner.h"
#include "ctlbstestproxypsybase.h"
#include "testproxypsy2.hrh"

_LIT(KFileName,  "c:\\logs\\testproxypsy2.txt");

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestProxyPsy2Positioner::CT_LbsTestProxyPsy2Positioner() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestProxyPsy2Positioner::ConstructL(TAny* aConstructionParameters)
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
CT_LbsTestProxyPsy2Positioner* CT_LbsTestProxyPsy2Positioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy2Positioner* self = new(ELeave) CT_LbsTestProxyPsy2Positioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestProxyPsy2Positioner* CT_LbsTestProxyPsy2Positioner::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestProxyPsy2Positioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestProxyPsy2Positioner::~CT_LbsTestProxyPsy2Positioner()
    {
    delete iRequestHandler;

    // Create a file to be able to verify that unloaded (no events will get throw to the client i disabled)
    RFile file;
    file.Create(iFileserver, KFileName, EFileWrite);
    file.Close();
    iFileserver.Close();
	}

// ---------------------------------------------------------
// CTestPsy2Positioner::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy2Positioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus)
	{
    TBuf<256> buf;
    OpenFileForAppend();
    _LIT(KTestProxyPsy2Positioner, " Request issued to TestProxyPsy2Positioner");
    buf.Append(KTestProxyPsy2Positioner);
    _LIT(KAppend, "\r");
    buf.Append(KAppend);
    iFileText.Write(buf);
    iFile.Close();
    
    TRequestStatus* status = &aStatus;   
    TPositionInfo* posInfo = static_cast<TPositionInfo*> (&aPosInfo);
    TPosition pos;
	pos.SetCoordinate(10, 20, 30);
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
        case 5:
        case 12:
        case 13:
        case 14:
        case 21:
        case 23:
        case 24:
            User::RequestComplete( status, KPositionPartialUpdate);
            break;
        case 2:
        case 3:
        case 4:
        case 11:
        case 15:
        case 22:
            User::RequestComplete( status, KErrLocked);
            break;
        case 6:
        case 9:
        case 10:
        case 25:
            User::RequestComplete( status, KErrDied);
            break;
        case 7:
        case 8:
        case 26:
            User::RequestComplete( status, KErrBadPower);
            break;
        case 4701:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(2000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4702:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(3000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4703:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(2000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4704:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(3000000);
            iRequestHandler->NotifyPositionUpdate(posInfo, status);
            break;
        case 4708:
        case 304:
            iRequestHandler->SetErrorCode(KErrNone);
            iRequestHandler->SetTimerDelay(1000000);			// 1 second
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
            User::RequestComplete( status, KErrDied);
            break;

        }
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsy2Positioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy2Positioner::CancelNotifyPositionUpdate()
	{
	TBuf<256> buf;
    OpenFileForAppend();
    _LIT(KTestProxyPsy2Positioner, " Request issued to TestProxyPsy2Positioner is cancelled");
    buf.Append(KTestProxyPsy2Positioner);
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
// CT_LbsTestProxyPsy2Positioner::TrackingOverridden
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsTestProxyPsy2Positioner::TrackingOverridden() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsy2Positioner::StartTrackingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy2Positioner::StartTrackingL(const TTimeIntervalMicroSeconds& /*aInterval*/)
    {
    iRequestHandler->StartTracking();
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsy2Positioner::StopTracking
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsy2Positioner::StopTracking()
    {
    iRequestHandler->StopTracking();
    }
    
TInt CT_LbsTestProxyPsy2Positioner::OpenFileForAppend( )
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
