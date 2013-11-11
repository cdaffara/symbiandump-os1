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
// Name     : CT_LbsTestPsyPartialUpdate.cpp
//




// INCLUDE FILES
#include <e32base.h> 
#include <lbs/epos_cpositioner.h>
#include "ctlbstestpsypartialupdate.h"
#include "testpsypartialupdate.hrh"
#include "ctlbstestproxypsybase.h"

_LIT(KFileName,  "c:\\logs\\partialupdate.txt");

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsyPartialUpdate::CT_LbsTestPsyPartialUpdate() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsyPartialUpdate::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
	iRequestHandler = new (ELeave) CT_LbsTestProxyPsyBase(PositionerStatus());
	
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	User::LeaveIfError(iFs.Connect());
    }

// Two-phased constructor.
CT_LbsTestPsyPartialUpdate* CT_LbsTestPsyPartialUpdate::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyPartialUpdate* self = new (ELeave) CT_LbsTestPsyPartialUpdate;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
	return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsyPartialUpdate* CT_LbsTestPsyPartialUpdate::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyPartialUpdate* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsyPartialUpdate::~CT_LbsTestPsyPartialUpdate()
    {
	delete iRequestHandler;
    iFs.Close();
    }

// ---------------------------------------------------------
// CT_LbsTestPsyPartialUpdate::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyPartialUpdate::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
	{
	TBuf<256> buf;
	OpenFileForAppend();
	_LIT(KPartialUpdatePsy, " Request issued to PartialUpdatePsy");
    buf.Append(KPartialUpdatePsy);
    _LIT(KAppend, "\r");
    buf.Append(KAppend);
    iFileText.Write(buf);
    iFile.Close();
    	
	TInt err = KErrNone;
	TRequestStatus* status = &aStatus;
    
    TTime tt;
    tt.UniversalTime();
    //Request ID must be unique, use universalime as seed
    // to give a random number
    TInt64 seed = tt.Int64();
    TReal lat = 90 * Math::FRand(seed);
    TReal lon = 90 * Math::FRand(seed);
    TReal32 alt = (TReal32)(90 * Math::FRand(seed));
    TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
    TUint32 request = position->UpdateType();
    TUid implUid = { KPosImplementationUid };
    position->SetModuleId(implUid);

    TTime now;
    now.UniversalTime();
    // Add some timedifference, otherwise the position is considered to be old
    TTimeIntervalMicroSeconds someTime(5); 
    now = now + someTime;
    
    TBool updateAllowedFlag = IsPartialUpdateAllowed();

    if (updateAllowedFlag)
        {
        // Must return with code KPositionPartialUpdate
        err= KPositionPartialUpdate;
        
        TPosition posse;
        lat = 30;
        lon = 40;
        alt = 50;
        posse.SetCoordinate(lat, lon, alt);
        posse.SetTime(now);
        position->SetPosition(posse);
        }
    else
        {
        err= KErrNone;

        TPosition posse;
        posse.SetCoordinate(lat, lon, alt);
        posse.SetTime(now);
        position->SetPosition(posse);
        }

    switch( request)
		{
    	case 4705:
    	       iRequestHandler->SetErrorCode(err);
    	       iRequestHandler->SetTimerDelay(2000000);
    	       iRequestHandler->NotifyPositionUpdate(position, status);
    	       break;
    	 case 4706:
    	       iRequestHandler->SetErrorCode(err);
    	       iRequestHandler->SetTimerDelay(7000000);
    	       iRequestHandler->NotifyPositionUpdate(position, status);
    	       break;
    	 case 4707:
    	       iRequestHandler->SetErrorCode(err);
    	       iRequestHandler->SetTimerDelay(2000000);
    	       iRequestHandler->NotifyPositionUpdate(position, status);
    	       break;
    	 case 4709:
    	       iRequestHandler->SetErrorCode(err);
    	       iRequestHandler->SetTimerDelay(1000000);
    	       iRequestHandler->NotifyPositionUpdate(position, status);
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
    	       User::RequestComplete(status, err);
    	       break;          
		}
	}

// ---------------------------------------------------------
// CT_LbsTestPsyPartialUpdate::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyPartialUpdate::CancelNotifyPositionUpdate()
	{
	TBuf<256> buf;
	OpenFileForAppend();
	_LIT(KPartialUpdatePsy, " Request issued to PartialUpdatePsy is cancelled");
    buf.Append(KPartialUpdatePsy);
    _LIT(KAppend, "\r");
    buf.Append(KAppend);
    iFileText.Write(buf);
    iFile.Close();
    
	if(iRequestHandler->IsActive())
	    {
		iRequestHandler->CancelNotifyPositionUpdate();
		}	 
	}
	
TInt CT_LbsTestPsyPartialUpdate::OpenFileForAppend( )
    {    
    TInt err = iFile.Open(iFs, KFileName, EFileStreamText|EFileWrite);
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err=iFile.Create(iFs, KFileName, EFileStreamText|EFileWrite);
            }

        if (err != KErrNone)
            {
            return err;
            }
        
        }
    iFileText.Set(iFile);
    return KErrNone;
    }	

//  End of File  
