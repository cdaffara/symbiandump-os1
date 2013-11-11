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



#include <lbspositioninfo.h>
#include <lbssatellite.h>

#include "ctlbstestPsy6RequestHandler.h"

const TReal32 KHorizontalAcc = 10;
const TReal32 KVerticalAcc = 10;
const TReal64 KLatitude = 65.3;
const TReal64 KLongitude = 11.4;
const TReal32 KAltitude = 2;

CT_LbsTestPsy6RequestHandler* CT_LbsTestPsy6RequestHandler::NewL()
	{
	CT_LbsTestPsy6RequestHandler* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

CT_LbsTestPsy6RequestHandler* CT_LbsTestPsy6RequestHandler::NewLC()
	{
	CT_LbsTestPsy6RequestHandler* self=new (ELeave) CT_LbsTestPsy6RequestHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CT_LbsTestPsy6RequestHandler::ConstructL() 
    {
	CT_LbsTestPsyBase::ConstructL();
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy6RequestHandler::CT_LbsTestPsy6RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsTestPsy6RequestHandler::~CT_LbsTestPsy6RequestHandler()
    {

	}

void CT_LbsTestPsy6RequestHandler::SetPartialSupport(const TBool& aPartialSupport) 
{
    iPartialSupport = aPartialSupport;
}

void CT_LbsTestPsy6RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
    TPosition pos1;
	
	pos1.SetAccuracy(KHorizontalAcc, KVerticalAcc);
	pos1.SetCurrentTime();
	
    if(iPartialSupport)
		{
        ++iPartialRequests;
        if(iPartialRequests % 2)
            {
            // Check that error is returned when a full position is returned as a partial update
            if(iRequests==0)
                {
                TPosition pos;
                pos.SetCurrentTime();
                pos.SetCoordinate(KLatitude, KLongitude);
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            // Create correct partial updates
            else if(iRequests==1)
                {
                TPosition pos;
                // Partial update with TimeStamp set to current time
                pos.SetCurrentTime();
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else if(iRequests==2)
                {
                TPosition pos;
                
                TRealX nan;
                nan.SetNaN();
                // Partial update with Latitiude=NaN and Longitude and Time(0)
                pos.SetCoordinate(nan, KLongitude);
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else if(iRequests==3)
                {
                TPosition pos;
                
                TRealX nan;
                nan.SetNaN();
                // Partial update with Latitiude and Longitude=NaN and Time(0)
                pos.SetCoordinate(KLatitude, nan);
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else if(iRequests==4)
                {
                TPosition pos;
                
                TRealX nan;
                nan.SetNaN();
                // Partial update with Latitiude=NaN and Longitude=Nan and Time(0)
                pos.SetCoordinate(nan, nan);
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else if(iRequests==5)
                {
                TPosition pos;       
                // Partial update with Latitiude=NaN and Longitude=Nan and SetTime(Time(0))
                pos.SetCoordinate(KLatitude, KLongitude);
                pos.SetTime(TTime(0));
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else if(iRequests==6)
                {
                TPosition pos;
                TRealX nan;
                nan.SetNaN();
                // Partial update with Latitiude=NaN and Longitude=Nan and CurrentTime
                pos.SetCoordinate(nan, nan);
                pos.SetCurrentTime();
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else if(iRequests==7)
                {
                TPosition pos;
                TRealX nan;
                nan.SetNaN();
                // Partial update with Latitiude and Longitude=Nan and CurrentTime
                pos.SetCoordinate(KLatitude, nan);
                pos.SetCurrentTime();
                aInfo.SetPosition(pos);	
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            else
                {
                CompleteRequest(KActiveRequest, KPositionPartialUpdate);
                }
            iRequests++;
            }
        else
            {
            CompleteRequest(KActiveRequest, KPositionQualityLoss);
            }
        }
	else
		{
		pos1.SetCoordinate(KLatitude, KLongitude, KAltitude);
        aInfo.SetPosition(pos1);
		}
	}
