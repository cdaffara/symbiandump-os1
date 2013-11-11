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
// Name     : CT_LbsTestPsyPartialUpdate2.cpp
//




// INCLUDE FILES
#include <e32base.h> 
#include <lbs/epos_cpositioner.h>
#include "ctlbstestpsypartialupdate2.h"
#include "testpsypartialupdate2.hrh"
#include "ctlbstestproxypsybase.h"

_LIT(KFileName,  "c:\\logs\\partialupdate2.txt");

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsyPartialUpdate2::CT_LbsTestPsyPartialUpdate2() 
    {
    }

// EPOC default constructor can leave.
void CT_LbsTestPsyPartialUpdate2::ConstructL(TAny* aConstructionParameters)
    {
	BaseConstructL(aConstructionParameters);
	iRequestHandler = new (ELeave) CT_LbsTestProxyPsyBase(PositionerStatus());
	
	TUid implUid = { KPosImplementationUid };
	iRequestHandler->SetModuleId(implUid);
	
	User::LeaveIfError(iFs.Connect());
    }

// Two-phased constructor.
CT_LbsTestPsyPartialUpdate2* CT_LbsTestPsyPartialUpdate2::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyPartialUpdate2* self = new (ELeave) CT_LbsTestPsyPartialUpdate2;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
	return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsyPartialUpdate2* CT_LbsTestPsyPartialUpdate2::NewL(TAny* aConstructionParameters)
    {
    CT_LbsTestPsyPartialUpdate2* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsyPartialUpdate2::~CT_LbsTestPsyPartialUpdate2()
    {
     delete iRequestHandler; 
     iFs.Close();
    }

// ---------------------------------------------------------
// CT_LbsTestPsyPartialUpdate2::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyPartialUpdate2::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
	{
	TBuf<256> buf;
	OpenFileForAppend();
	_LIT(KPartialUpdatePsy2, " Request issued to PartialUpdatePsy2");
    buf.Append(KPartialUpdatePsy2);
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
        err = KPositionPartialUpdate;

        TPosition posse;
        lat = 55;
        lon = 65;
        alt = 75;
        posse.SetCoordinate(lat, lon, alt);
        posse.SetTime(now);
        position->SetPosition(posse);
        }
    else
        {
        err = KErrNone;

        TPosition posse;
        posse.SetCoordinate(lat, lon, alt);
        posse.SetTime(now);
        position->SetPosition(posse);
        }

	switch( request)
		{
    	case 4706:
    	       iRequestHandler->SetErrorCode(err);
    	       iRequestHandler->SetTimerDelay(2000000);
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
// CT_LbsTestPsyPartialUpdate2::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsyPartialUpdate2::CancelNotifyPositionUpdate()
	{
	TBuf<256> buf;
	OpenFileForAppend();
	_LIT(KPartialUpdatePsy2, " Request issued to PartialUpdatePsy2 is cancelled");
    buf.Append(KPartialUpdatePsy2);
    _LIT(KAppend, "\r");
    buf.Append(KAppend);
    iFileText.Write(buf);    
    iFile.Close();
    
	if(iRequestHandler->IsActive())
	    {
	      iRequestHandler->CancelNotifyPositionUpdate(); 
		}	 
	}

TInt CT_LbsTestPsyPartialUpdate2::OpenFileForAppend( )
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
