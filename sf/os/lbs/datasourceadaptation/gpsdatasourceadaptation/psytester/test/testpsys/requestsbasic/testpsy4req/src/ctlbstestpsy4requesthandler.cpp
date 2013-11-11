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
#include "ctlbstestpsy4requesthandler.h"
#include "ctlbs_testPsy4Constants.h"

// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy4RequestHandler* CT_LbsTestPsy4RequestHandler::InstanceL()
    {
    CT_LbsTestPsy4RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsy4RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsy4RequestHandler;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(); // self
		Dll::SetTls(reinterpret_cast<TAny*>(self));
        }

	self->iReferenceCounter++;
	return self;
    }

// ---------------------------------------------------------
// ::Release
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy4RequestHandler::Release()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy4RequestHandler::CT_LbsTestPsy4RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsTestPsy4RequestHandler::~CT_LbsTestPsy4RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy4RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	TUint request = ++iPosRequests;

	TUint32 classType = aInfo.PositionClassType();
	if(classType & EPositionCourseInfoClass || classType & EPositionSatelliteInfoClass)
		{
		request = 1000;	
		}	
    TTime aTime(0);
	TPosition pos;
	switch(request)
		{
		default:
			pos.SetAccuracy(KHorizontalAcc, KVerticalAcc);
			pos.SetCurrentTime();
			pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
			break;
		case 1:
		case 4:
		case 7:
		case 10:
		case 13:
		case 16:
		case 19:
		case 22:
		case 25: // Accuracy missed
			pos.SetCurrentTime();
			pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
			break;
		case 2:
		case 5:
		case 8:
		case 11:
		case 14:
		case 17:
		case 20:
		case 23: // Time missed
            pos.SetTime(aTime);
			pos.SetAccuracy(KHorizontalAcc, KVerticalAcc);	
			pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
			break;
		case 3:
		case 6:
		case 9:
		case 12:
		case 15:
		case 18:
		case 21:
		case 24: // No lat, long, alt
			pos.SetAccuracy(KHorizontalAcc, KVerticalAcc);	
			pos.SetCurrentTime();
		}
	aInfo.SetPosition(pos);	
	}

// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::SetTPositionCourseInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy4RequestHandler::SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo)
	{	
	TUint request = ++iCourseRequests;	
	TUint32 classType = aCourseInfo.PositionClassType();
	if(classType & EPositionSatelliteInfoClass)
		{
		request = 1000;	
		}
	TCourse course;
	switch(request)
		{
		default:
			course.SetSpeed(KVerticalSpeed);
			course.SetHeading(KHeading);
			course.SetSpeedAccuracy(KVerticalSpeedError);
			course.SetHeadingAccuracy(KHeadingError);
			break;
		case 1:
		case 5:
		case 9:
		case 13:
		case 17:
		case 21:
		case 25:// Speed missed
			course.SetHeading(KHeading);
			course.SetSpeedAccuracy(KVerticalSpeedError);
			course.SetHeadingAccuracy(KHeadingError);
			break;
		case 2:
		case 6:
		case 10:
		case 14:
		case 18:
		case 22:// Heading missed
			course.SetSpeed(KVerticalSpeed);
			course.SetSpeedAccuracy(KVerticalSpeedError);
			course.SetHeadingAccuracy(KHeadingError);
			break;
		case 3:
		case 7:
		case 11:
		case 15:
		case 19:
		case 23:// Accuracy missed
			course.SetSpeed(KVerticalSpeed);
			course.SetHeading(KHeading);
			course.SetHeadingAccuracy(KHeadingError);
			break;
		case 4:
		case 8:
		case 12:
		case 16:
		case 20:
		case 24:// Heading Accuracy missed	
			course.SetSpeed(KVerticalSpeed);
			course.SetHeading(KHeading);
			course.SetSpeedAccuracy(KVerticalSpeedError);
			break;
		}
	aCourseInfo.SetCourse(course);
	}

// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::SetTPositionSatelliteInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy4RequestHandler::SetTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo, const TBool& /*aCache*/)
	{
    ++iSatRequests;
   
	ClearTPositionSatelliteInfo(aSatelliteInfo);
    
    TTime satTime;
	if(iSatRequests != 1 && iSatRequests != 11)
		{
		satTime.UniversalTime();
		aSatelliteInfo.SetSatelliteTime(satTime);
		}

	if(iSatRequests != 2 && iSatRequests != 12)
		aSatelliteInfo.SetHorizontalDoP(KSatelliteHorizontalDoP);
	if(iSatRequests != 3 && iSatRequests != 13)
		aSatelliteInfo.SetVerticalDoP(KSatelliteVerticalDoP);		
	if(iSatRequests != 4 && iSatRequests != 14)
		aSatelliteInfo.SetTimeDoP(KSatelliteTimeDoP);
		
	for (TInt  i = 0; i < KSatelliteNumInView; i++)
		{
		TSatelliteData satellite;
		if(iSatRequests != 5 && iSatRequests != 15)
			satellite.SetSatelliteId(KSatelliteId+i);
		if(iSatRequests != 6 && iSatRequests != 16)
			satellite.SetAzimuth(KAzimuth);
		if(iSatRequests != 7 && iSatRequests != 17)
			satellite.SetElevation(KElevation);
		if(iSatRequests != 8 && iSatRequests != 18)
			satellite.SetIsUsed(KIsUsed);
		if(iSatRequests != 9 && iSatRequests != 19)
			satellite.SetSignalStrength(KSignalStrength);
		if(iSatRequests != 10 && iSatRequests != 20)
			aSatelliteInfo.AppendSatelliteData(satellite);
		}
	}
