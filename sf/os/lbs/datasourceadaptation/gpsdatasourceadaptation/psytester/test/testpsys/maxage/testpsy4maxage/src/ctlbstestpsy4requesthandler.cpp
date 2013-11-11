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
#include "ctlbs_testpsy4constants.h"

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
// CT_LbsTestPsy4RequestHandler::Release
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
// CT_LbsTestPsy4RequestHandler::CheckMaxAge
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

TBool CT_LbsTestPsy4RequestHandler::CheckMaxAge(TPositionInfo* aPosInfo) 
	{
    TTime tmp(0);

	if (iMaxAge == tmp)
		{
		return EFalse;
		}

    if ( iPosTime > iMaxAge ) 
        {
        SetPositionInfoL(aPosInfo, ETrue);
	    return ETrue;
        }

    return EFalse;
	}


// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//                          
void CT_LbsTestPsy4RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& aUseCache)
	{
	TPosition pos;
	
	pos.SetAccuracy(KHorizontalAcc, KVerticalAcc);
	
    TTime aTime;
    
    if(aUseCache)
        {
        TUint32 classType = aInfo.PositionClassType();
        
        pos.SetTime(iPosTime);
        
        if (classType & EPositionSatelliteInfoClass || classType & EPositionGenericInfoClass)
            {
            pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
            }
        else if (classType & EPositionCourseInfoClass) 
            {
            iCourseInfoMaxAgeRequest = ETrue;
            pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
            }
        else
            {
            TRealX nan;
            nan.SetNaN();
            pos.SetCoordinate(KLatitude, KLongitude, nan);
            }
            
        aInfo.SetPosition(pos);
        
        }
    else
        {
        aTime.UniversalTime();
        pos.SetTime(aTime);
        iPosTime = aTime;
        pos.SetCoordinate(KLatitude, KLongitude, KAltitude);
	    aInfo.SetPosition(pos);	
        }
	 
	}


// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CT_LbsTestPsy4RequestHandler::SetTPositionCourseInfo(TPositionCourseInfo& aCourseInfo)
	{
    
    TCourse course;
	course.SetSpeed(KVerticalSpeed);
	course.SetHeading(KHeading);
	course.SetSpeedAccuracy(KVerticalSpeedError);
    
    if (iCourseInfoMaxAgeRequest)
        {
         TRealX nan;
        nan.SetNaN();
        course.SetHeadingAccuracy(nan);
        iCourseInfoMaxAgeRequest = EFalse;
        }
	else 
        {
        course.SetHeadingAccuracy(KHeadingError);
        }

	aCourseInfo.SetCourse(course);
	}
    
// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::SetTPositionSatelliteInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CT_LbsTestPsy4RequestHandler::SetTPositionSatelliteInfo(TPositionSatelliteInfo& aSatelliteInfo, const TBool& aUseCache)
	{
      ClearTPositionSatelliteInfo(aSatelliteInfo);
	
    TTime aTime;
    if (aUseCache)
        {
        aTime = iSatTime;
        }
    else
        {
	    aTime.UniversalTime();
	    iSatTime = aTime;
        }

    aSatelliteInfo.SetSatelliteTime(aTime);
	
    if (aUseCache)
        {
        TRealX nan;
        nan.SetNaN();
        aSatelliteInfo.SetHorizontalDoP(nan);
        }
    else
        {
        aSatelliteInfo.SetHorizontalDoP(KSatelliteHorizontalDoP);
        }

	aSatelliteInfo.SetVerticalDoP(KSatelliteVerticalDoP);
	aSatelliteInfo.SetTimeDoP(KSatelliteTimeDoP);

    // Below values are not checked by MaxAgeTest TRESLI-642JKJ
    // Should a warning be issued if not? 

	for (TInt  i = 0; i < KSatelliteNumInView; i++)
		{
		TSatelliteData satellite;
		
		satellite.SetSatelliteId(KSatelliteId+i);
		satellite.SetAzimuth(KAzimuth);
		satellite.SetElevation(KElevation);
		satellite.SetIsUsed(KIsUsed);
		satellite.SetSignalStrength(KSignalStrength);
	
		aSatelliteInfo.AppendSatelliteData(satellite);
		}

	}
