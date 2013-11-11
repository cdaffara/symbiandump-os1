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
//




//  INCLUDES
#include "ctlbsclientpostp2.h"
#include "ctlbsclientpositionconstants.h"
#include <lbssatellite.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp2::CT_LbsClientPosTp2(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP2 - Request location from specific PSY");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp2::~CT_LbsClientPosTp2()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp2::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp2::StartL()
    {
	SetupPsyL(iUidMultiPsy);

    //synchronous request
    ConnectL();

    User::LeaveIfError(OpenPositionerByName(iUidMultiPsy));

    TPositionInfo info = TPositionInfo();

	RequestL(info);
	CheckPositionInfoL(info);
	
	TPositionCourseInfo courseInfo;

	RequestL(courseInfo);
	CheckPositionCourseInfoL(courseInfo);

	TPositionSatelliteInfo satelliteInfo;

	RequestL(satelliteInfo);
	CheckPositionSatelliteInfoL(satelliteInfo);


    HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewLC();

	RequestL(*genericInfo);
	CheckGenericInfoL(*genericInfo);

    CleanupStack::PopAndDestroy(); //genericInfo

	ClosePositioner();

	Disconnect();
	}


// ---------------------------------------------------------
// CT_LbsClientPosTp2::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp2::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg)
    {
  	if (!aCondition)
		{
		LogErrorAndLeaveL(aErrorMsg);
		}
    }

void CT_LbsClientPosTp2::RequestL(TPositionInfoBase& aInfoBase)
	{
	_LIT(KService, "service");
    TInt err = PerformSyncRequest(KService, &aInfoBase);

    if (err != KErrNone)
        {
        _LIT(KErrorRequest, "The request was not completed with KErrorNone, errCode = %d");
		TBuf<100> buf;
		buf.Format(KErrorRequest, err);
        LogErrorAndLeaveL(buf);
        }

	_LIT(KIncorrectPsySyncRequestError,
          "TP2. Position from wrong PSY received (sync request).");
	AssertTrueL(iUidMultiPsy == aInfoBase.ModuleId(), KIncorrectPsySyncRequestError);
	}

void CT_LbsClientPosTp2::CheckPositionInfoL(TPositionInfo& aInfo)
	{
	TPosition pos;
	aInfo.GetPosition(pos);
	CheckPositionL(pos);
	}

void CT_LbsClientPosTp2::CheckPositionCourseInfoL(TPositionCourseInfo& aCourseInfo)
	{
	CheckPositionInfoL(aCourseInfo);
	TCourse course;
	aCourseInfo.GetCourse(course);
	if (course.Heading() != KHeading ||
		course.Speed() != KSpeed ||
		course.SpeedAccuracy() != KSpeedAcc ||
		course.HeadingAccuracy() != KHeadingAcc )
		{
		_LIT(KErrCourse, "Course not correct");
		LogErrorAndLeaveL(KErrCourse);
		}
	}

void CT_LbsClientPosTp2::CheckPositionSatelliteInfoL(TPositionSatelliteInfo& aSatelliteInfo)
	{
	CheckPositionCourseInfoL(aSatelliteInfo);
	if((TUint)aSatelliteInfo.NumSatellitesUsed() != KNumberOfSatellitesUsed ||
	   aSatelliteInfo.SatelliteTime() != TTime(KSatelliteTime) ||
	   (TUint)aSatelliteInfo.NumSatellitesInView() != KNumberOfSatellitesInView ||
	   aSatelliteInfo.HorizontalDoP() != KHorizontalDoPValue ||
	   aSatelliteInfo.VerticalDoP() != KVerticalDoPValue ||
	   aSatelliteInfo.TimeDoP() != KTimeDoPValue)
	   {
		_LIT(KErrBasicSat, "Basic satellite information not correct");
		LogErrorAndLeaveL(KErrBasicSat);
		}
		TInt sats = aSatelliteInfo.NumSatellitesInView();
	for (int i = 0; i < sats; i++)
		{
		TSatelliteData satelliteData;
		TInt err = aSatelliteInfo.GetSatelliteData(i,satelliteData);
		if (err != KErrNone)
			{
			_LIT(KErrGetSat, "Not possible to get satellite data, error code = %d");
			TBuf<100> buf;
			buf.Format(KErrGetSat, err);
			LogErrorAndLeaveL(buf);
			}
		if ( (i%2) == 0 )
			{
			if(	satelliteData.SatelliteId() != (KSatelliteId +i) ||
				satelliteData.Azimuth() != KAzimuth ||
				satelliteData.Elevation() != KElevation ||
				satelliteData.IsUsed() != KIsUsed ||
				satelliteData.SignalStrength() != KSignalStrength)
				{
				_LIT(KErrSatDataEven, "Incorrect satellite data on even satellites");
				LogErrorAndLeaveL(KErrSatDataEven);
				}
			}
		else
			{
			if(	satelliteData.SatelliteId() != (KSatelliteId +i) ||
				satelliteData.Azimuth() != KAzimuthOdd ||
				satelliteData.Elevation() != KElevationOdd ||
				satelliteData.IsUsed() != KIsUsedOdd ||
				satelliteData.SignalStrength() != KSignalStrengthOdd)
				{
				_LIT(KErrSatDataOdd, "Incorrect satellite data on odd satellites");
				LogErrorAndLeaveL(KErrSatDataOdd);
				}
			}

		}
	}

void CT_LbsClientPosTp2::CheckGenericInfoL(HPositionGenericInfo& aGenericInfo)
	{
	CheckPositionInfoL(aGenericInfo);

	}

void CT_LbsClientPosTp2::CheckPositionL(TPosition& aPos)
	{
	if (aPos.Time() != TTime(KPositionTime) ||
		aPos.HorizontalAccuracy() != KHorizontalAcc ||
		aPos.VerticalAccuracy() != KVerticalAcc ||
		aPos.Latitude() != KLatitude ||
		aPos.Longitude() != KLongitude ||
		aPos.Altitude() != KAltitude)
		{
		_LIT(KErrPosition, "Wrong position returned");
		LogErrorAndLeaveL(KErrPosition);
		}
	}
