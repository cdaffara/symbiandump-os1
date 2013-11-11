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
#include "ctlbssimulationpsypostp242.h"
#include "ctlbssimulationpsypostp242constants.h"

// CONSTANTS
_LIT(KServiceName, "TP242");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp242::CT_LbsSimulationPSYPosTp242(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP242 - NMEA Satellite support");
	SetTestStepName(KTestName);
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp242::~CT_LbsSimulationPSYPosTp242()
	{
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::StartL()
    {    	
	_LIT(KConnectErr, "Error when connecing to Position Server,  %d");
    
#ifdef __WINS__
    _LIT(KNmeaFile, "z:\\system\\test\\testdata\\tp242.nme");
    _LIT(KNmeaFile2, "z:\\system\\test\\testdata\\tp242_East.nme");
    _LIT(KNmeaFileMissingPDoP, "z:\\system\\test\\testdata\\tp242MissingPDoP.nme");
    _LIT(KNmeaFileEmptySpeed, "z:\\system\\test\\testdata\\tp242EmptySpeed.nme");
#else
    _LIT(KNmeaFile, "c:\\system\\test\\testdata\\tp242.nme");
    _LIT(KNmeaFile2, "c:\\system\\test\\testdata\\tp242_East.nme");
    _LIT(KNmeaFileEmptySpeed, "c:\\system\\test\\testdata\\tp242EmptySpeed.nme");
    _LIT(KNmeaFileMissingPDoP, "c:\\system\\test\\testdata\\tp242MissingPDoP.nme");
#endif
    
    TInt err = iPosServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err);
    
    SetFileInUseL(KNmeaFile);
   	
    TPositionSatelliteInfo satInfo;
	// TSatelliteInfo 
    
    RequestL(satInfo);
	CheckTPositionSatelliteInfoL(satInfo);
    CheckTPositionCourseInfoL(satInfo);
    CheckTPositionInfo(satInfo);

    // TCourseInfo
    TPositionCourseInfo courseInfo;
    RequestL(courseInfo);
    CheckTPositionCourseInfoL(courseInfo);
    CheckTPositionInfo(courseInfo);

    HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewLC();
    
    // Satellite generic info
    SetSatelliteRequestedFields(*genericInfo);
    RequestL(*genericInfo);  
    CheckSatelliteGenericInfoL(*genericInfo);
    CheckTPositionInfo(*genericInfo);

    // Speed generic info
    SetSpeedRequestedFields(*genericInfo);
    RequestL(*genericInfo);
    CheckSpeedGenericInfoL(*genericInfo);
    CheckTPositionInfo(*genericInfo);

    // Compass generic info
    SetCompassRequestedFields(*genericInfo);
    RequestL(*genericInfo);
    CheckCompassGenericInfoL(*genericInfo);
    CheckTPositionInfo(*genericInfo);
    
    SetFileInUseL(KNmeaFile2);
    // Test that EPositionFieldMagneticCourse is calculated as:
    //True Course + magnetic variation where magnetic variation is subtracted if westerly and
    // added if easterly
    SetCompassRequestedFields(*genericInfo);
    RequestL(*genericInfo);
    CheckCompassGenericInfoL(*genericInfo, EFalse);
    CheckTPositionInfo(*genericInfo);

    // Test that speed in course info is NaN
    SetFileInUseL(KNmeaFileEmptySpeed); 
    RequestL(courseInfo);
    CheckTPositionCourseInfoEmptySpeedL(courseInfo);

    CheckSupportedNMEAFiles();
    
    //Test that missing PDOP is calculated according sqrt(HDpP^2 +VDoP^2)
    SetFileInUseL(KNmeaFileMissingPDoP);
    SetSatelliteRequestedFields(*genericInfo);
    RequestL(*genericInfo);  
    CheckSatelliteGenericInfoL(*genericInfo, ETrue);

    CleanupStack::PopAndDestroy(genericInfo);
    
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::CloseTest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::CloseTest()
	{
    iPositioner.Close();
	iPosServer.Close();
	}

void CT_LbsSimulationPSYPosTp242::RequestL(TPositionInfoBase& aPositionInfo)
	{
    _LIT(KNotifyErr, 
    		"NotifyPositionUpdate returns status %d when should return status 0");
  
    iPositioner.NotifyPositionUpdate(aPositionInfo, iStatus);
    User::WaitForRequest(iStatus);
    AssertTrueL(iStatus.Int() == KErrNone, KNotifyErr, iStatus.Int());    
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::CheckTPositionInfo()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::CheckTPositionInfo(const TPositionInfo& aTPositionInfo)
    {
    _LIT(KLongitudeErr, "Longitude is incorrect in TPositionInfo");
    _LIT(KLatitudeErr, "Latitude is incorrect in TPositionInfo");
    _LIT(KHorizontalErr, "Latitude is incorrect in TPositionInfo");
    _LIT(KAltitudeErr, "Latitude is incorrect in TPositionInfo");
    _LIT(KAltitudeErrErr, "Latitude is incorrect in TPositionInfo");

    TPosition pos;
	aTPositionInfo.GetPosition(pos);

    AssertTrueL(pos.Longitude() == KLongitude, KLongitudeErr, KErrGeneral);
    AssertTrueL(pos.Latitude() == KLatitude, KLatitudeErr, KErrGeneral);
    AssertTrueL(pos.Altitude() == KAltitude, KAltitudeErr, KErrGeneral);
    AssertTrueL(pos.HorizontalAccuracy() == KHorizontalAcc, KHorizontalErr, KErrGeneral);
    AssertTrueL(pos.VerticalAccuracy() == KVerticalAcc, KAltitudeErrErr, KErrGeneral);

    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::CheckTPositionSatelliteInfo()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::CheckTPositionSatelliteInfoL(const TPositionSatelliteInfo& aSatelliteInfo)
	{
    _LIT(KNoUsedErr, "Number of used satellites is incorrect in GenericInfo");
    _LIT(KHorizontalDoPErr, "HorizontalDoP is incorrect in TPositionSatelliteInfo");
    _LIT(KVerticalDoPErr, "VerticalDoP is incorrect in GenericInfo");
    _LIT(KTimeDoPErr, "TimeDoP is incorrect in TPositionSatelliteInfo");
    _LIT(KSatTimeErr, "Satellite time is incorrect in TPositionSatelliteInfo");
    _LIT(KSatIdErr, "Satellite Id is incorrect in TPositionSatelliteInfo satellite %d"); 
    _LIT(KSatElevationErr, "Satellite Elevation is incorrect in TPositionSatelliteInfo satellite %d"); 
    _LIT(KSatAzimuthErr, "Satellite Azimuth is incorrect in TPositionSatelliteInfo satellite %d"); 
    _LIT(KSatSignalStrengthErr, "Satellite SignalStrength is incorrect in TPositionSatelliteInfo satellite %d"); 
    _LIT(KSatIsUsedErr, "Satellite IsUsed is incorrect in TPositionSatelliteInfo satellite %d"); 

    TTime satTime;
    GetCorrectSatelliteTime(satTime);
    
    AssertTrueL(aSatelliteInfo.SatelliteTime() == satTime, KSatTimeErr, KErrGeneral);
	AssertTrueL(aSatelliteInfo.HorizontalDoP() == KSatelliteHorizontalDoP, KHorizontalDoPErr, KErrGeneral);
	AssertTrueL(aSatelliteInfo.VerticalDoP() == KSatelliteVerticalDoP, KVerticalDoPErr, KErrGeneral);
    AssertTrueL(Math::IsNaN(aSatelliteInfo.TimeDoP()), KTimeDoPErr, KErrGeneral);

    AssertTrueL(aSatelliteInfo.NumSatellitesInView() == KSatelliteNumInView, KNoUsedErr, KErrGeneral);

    for (TInt  i = 1; i <= aSatelliteInfo.NumSatellitesInView(); i++)
	    {
		TSatelliteData satellite;
        aSatelliteInfo.GetSatelliteData(i-1, satellite);
		AssertTrueL(satellite.SatelliteId() == i, KSatIdErr, i);		
        switch(i)
            {
            case 1:
                AssertTrueL(satellite.Elevation() == KElevation1, KSatElevationErr, i);
                AssertTrueL(satellite.Azimuth() == KAzimuth1, KSatAzimuthErr, i);
                AssertTrueL(satellite.SignalStrength() == KSignalStrength1, KSatSignalStrengthErr, i);
                AssertTrueL(satellite.IsUsed() == KIsUsed1, KSatIsUsedErr, i);
                break;
            case 2:
                AssertTrueL(Math::IsNaN(satellite.Elevation()), KSatElevationErr, i);
                AssertTrueL(satellite.Azimuth() == KAzimuth2, KSatAzimuthErr, i);
                AssertTrueL(satellite.SignalStrength() == KSignalStrength2, KSatSignalStrengthErr, i);
                AssertTrueL(satellite.IsUsed() == KIsUsed2, KSatIsUsedErr, i);
                break;
            case 3:
                AssertTrueL(satellite.Elevation() == KElevation3, KSatElevationErr, i);
                AssertTrueL(Math::IsNaN(satellite.Azimuth()), KSatAzimuthErr, i);
                AssertTrueL(satellite.SignalStrength() == KSignalStrength3, KSatSignalStrengthErr, i);
                AssertTrueL(satellite.IsUsed() == KIsUsed3, KSatIsUsedErr, i);
                break;
            case 4:
                AssertTrueL(satellite.Elevation() == KElevation4, KSatElevationErr, i);
                AssertTrueL(satellite.Azimuth() == KAzimuth4, KSatAzimuthErr, i);
                AssertTrueL(satellite.SignalStrength() == KSignalStrength4, KSatSignalStrengthErr, i);
                AssertTrueL(satellite.IsUsed() == KIsUsed4, KSatIsUsedErr, i);
                break;
            case 5:
                AssertTrueL(satellite.Elevation() == KElevation5, KSatElevationErr, i);
                AssertTrueL(satellite.Azimuth() == KAzimuth5, KSatAzimuthErr, i);
                AssertTrueL(satellite.SignalStrength() == KSignalStrength5, KSatSignalStrengthErr, i);
                AssertTrueL(satellite.IsUsed() == KIsUsed5, KSatIsUsedErr, i);
                break;
            case 6:
                AssertTrueL(satellite.Elevation() == KElevation6, KSatElevationErr, i);
                AssertTrueL(satellite.Azimuth() == KAzimuth6, KSatAzimuthErr, i);
                AssertTrueL(satellite.SignalStrength() == KSignalStrength6, KSatSignalStrengthErr, i);
                AssertTrueL(satellite.IsUsed() == KIsUsed6, KSatIsUsedErr, i);
                break;
            }		
		}
	}

// ---------------------------------------------------------
// CTestPsyBase::CheckTPositionCourseInfoL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//		
void CT_LbsSimulationPSYPosTp242::CheckTPositionCourseInfoL(const TPositionCourseInfo& aCourseInfo)
	{	
    _LIT(KHeadingErr,"Heading is incorrect in TPositionCourseInfo");
    _LIT(KSpeedErr, "Speed is incorrect in TPositionCourseInfo");
    _LIT(KSpeedAccuracyErr, "Speed accuracy is incorrect in TPositionCourseInfo");
    _LIT(KHeadingAccuracyErr, "Heading accuracy is incorrect in TPositionCourseInfo");

    TCourse course;
	aCourseInfo.GetCourse(course);
    
    AssertTrueL(course.Speed() == KHorizontalSpeed, KSpeedErr, KErrGeneral);
    AssertTrueL(course.Heading() == KHeading, KHeadingErr, KErrGeneral); 
    AssertTrueL(Math::IsNaN(course.SpeedAccuracy()), KSpeedAccuracyErr, KErrGeneral);
    AssertTrueL(Math::IsNaN(course.HeadingAccuracy()), KHeadingAccuracyErr, KErrGeneral);
    
    }
// ---------------------------------------------------------
// CTestPsyBase::CheckTPositionCourseInfoEmptySpeedL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//		
void CT_LbsSimulationPSYPosTp242::CheckTPositionCourseInfoEmptySpeedL(const TPositionCourseInfo& aCourseInfo)
	{	
    _LIT(KSpeedErr, "Speed is incorrect in TPositionCourseInfo");
    
    TCourse course;
	aCourseInfo.GetCourse(course);
    
    AssertTrueL(Math::IsNaN(course.Speed()), KSpeedErr, KErrGeneral);
    }

// ---------------------------------------------------------
// CTestPsyBase::CheckCourseGenericInfoL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//		
void CT_LbsSimulationPSYPosTp242::CheckSpeedGenericInfoL(const HPositionGenericInfo& aGenericInfo)
	{
    _LIT(KSpeedErr, "Speed is incorrect in GenericInfo");
    _LIT(KHorizontalSpeedErrorErr, "Horizontal speed error could not be set in GenericInfo");
    _LIT(KVerticalSpeedErrorErr, "Vertical speed error could not be available in GenericInfo");
    _LIT(KVerticalSpeedErr, "Vertical speed could not be set in GenericInfo");

    TReal32 speed;
    aGenericInfo.GetValue(EPositionFieldHorizontalSpeed, speed);
	AssertTrueL(KHorizontalSpeed==speed, KSpeedErr, KErrGeneral);
	
    TInt err;
    TReal32 tmp;
    
    err = aGenericInfo.GetValue(EPositionFieldVerticalSpeed, tmp);
    AssertTrueL(err == KErrNotFound, KVerticalSpeedErr, KErrGeneral);
    
    err = aGenericInfo.GetValue(EPositionFieldHorizontalSpeedError, tmp);
    AssertTrueL(err == KErrNotFound, KHorizontalSpeedErrorErr, KErrGeneral);
		
    err = aGenericInfo.GetValue(EPositionFieldVerticalSpeedError, tmp); 
    AssertTrueL(err == KErrNotFound, KVerticalSpeedErrorErr, KErrGeneral);
			
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::SetSatelliteGenericInfoL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::CheckSatelliteGenericInfoL(const HPositionGenericInfo& aGenericInfo, TBool aCalculatedPDoP)
	{
    _LIT(KNoSatErr, "Number of satellites in view is incorrect in GenericInfo");
    _LIT(KNoUsedErr, "Number of used satellites is incorrect in GenericInfo");
    _LIT(KHorizontalDoPErr, "HorizontalDoP is incorrect in GenericInfo");
    _LIT(KVerticalDoPErr, "VerticalDoP is incorrect in GenericInfo");
    _LIT(KTimeDoPErr, "TimeDoP could not be set in GenericInfo");
    _LIT(KPosDoPErr, "PositionDoP is incorrect in GenericInfo");
    _LIT(KAltitudeErr, "SatelliteSeaLevelAltitude is incorrect in GenericInfo");
    _LIT(KGeoidalSeparationErr, "SatelliteGeoidalSeparation is incorrect in GenericInfo");
    _LIT(KSatTimeErr, "Satellite time is incorrect in GenericInfo");

    TInt8 satelliteNumInView;
	aGenericInfo.GetValue(EPositionFieldSatelliteNumInView, satelliteNumInView);
    AssertTrueL(KSatelliteNumInView == satelliteNumInView, KNoSatErr, KErrGeneral);
	
    TInt8 satelliteNumUsed;
	aGenericInfo.GetValue(EPositionFieldSatelliteNumUsed, satelliteNumUsed);
    AssertTrueL(KSatelliteNumUsed == satelliteNumUsed, KNoUsedErr, KErrGeneral);

    TTime satTime;
    GetCorrectSatelliteTime(satTime);

    TTime time;
    aGenericInfo.GetValue(EPositionFieldSatelliteTime, time);
    AssertTrueL(time == satTime, KSatTimeErr, KErrGeneral);

    TReal32 satelliteHorizontalDoP;
    aGenericInfo.GetValue(EPositionFieldSatelliteHorizontalDoP, satelliteHorizontalDoP);
    AssertTrueL(KSatelliteHorizontalDoP == satelliteHorizontalDoP, KHorizontalDoPErr, KErrGeneral);
    
    TReal32 satellitePositionDoP;
    aGenericInfo.GetValue(EPositionFieldSatellitePositionDoP, satellitePositionDoP);

	TReal32 satelliteVerticalDoP;
    aGenericInfo.GetValue(EPositionFieldSatelliteVerticalDoP, satelliteVerticalDoP);

    if (aCalculatedPDoP)
        {
        _LIT(KPosDopCalculated, "The value of PDoP is not calulated if missing %d");
        AssertTrueL(KSatelliteCalPositionDoP == satellitePositionDoP, KPosDopCalculated, KErrGeneral);
        AssertTrueL(KSatelliteCalVerticalDoP == satelliteVerticalDoP, KVerticalDoPErr, KErrGeneral);
        }
    else
        {
        AssertTrueL(KSatellitePositionDoP == satellitePositionDoP, KPosDoPErr, KErrGeneral);
        AssertTrueL(KSatelliteVerticalDoP == satelliteVerticalDoP, KVerticalDoPErr, KErrGeneral);
        }

    TInt err;
    TReal32 satelliteTimeDoP;
	err = aGenericInfo.GetValue(EPositionFieldSatelliteTimeDoP, satelliteTimeDoP);
    AssertTrueL(err == KErrNotFound, KTimeDoPErr, KErrGeneral);
		
    TReal32 satelliteAltitude;
    aGenericInfo.GetValue(EPositionFieldSatelliteSeaLevelAltitude, satelliteAltitude);
	AssertTrueL(KSatelliteAltitude == satelliteAltitude, KAltitudeErr, KErrGeneral); 

    TReal32 satelliteGeoidalSeparation;
	aGenericInfo.GetValue(EPositionFieldSatelliteGeoidalSeparation, satelliteGeoidalSeparation);
	AssertTrueL(KSatelliteGeoidalSeparation == satelliteGeoidalSeparation, KGeoidalSeparationErr, KErrGeneral);  	
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::CheckCompassGenericInfoL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::CheckCompassGenericInfoL(const HPositionGenericInfo& aGenericInfo, TBool aWest)
	{
    _LIT(KTrueCourseErr, "True Course is incorrect in GenericInfo");
    _LIT(KTrueCourseErrorErr, "True course errror could not be set in GenericInfo");
    _LIT(KMagneticCourseErr, "Magnetic Course could not be set GenericInfo");
    _LIT(KMagneticCourseErrEast, "Incorrect EPositionFieldMagneticCourse value when east");
    _LIT(KMagneticCourseErrWest, "Incorrect EPositionFieldMagneticCourse value when weast");

    TReal32 trueCourse;
    aGenericInfo.GetValue(EPositionFieldTrueCourse, trueCourse);
    AssertTrueL(KTrueCourse == trueCourse, KTrueCourseErr, KErrGeneral);

    TInt err;
    TReal tmp;
    err = aGenericInfo.GetValue(EPositionFieldTrueCourseError, tmp);
    AssertTrueL(err == KErrNotFound, KTrueCourseErrorErr, KErrGeneral);
    TReal32 magneticCourse;
    err = aGenericInfo.GetValue(EPositionFieldMagneticCourse, magneticCourse);
    AssertTrueL(err == KErrNone, KMagneticCourseErr, KErrGeneral);
    if (aWest)
        {
        AssertTrueL(magneticCourse == (KTrueCourse+KMagneticVariation), KMagneticCourseErrWest, KErrGeneral);
        }
    else
        {
        AssertTrueL(magneticCourse == (KTrueCourse-KMagneticVariation), KMagneticCourseErrEast, KErrGeneral);
        }

    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::CheckSupportedNMEAFiles
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::CheckSupportedNMEAFiles()
    {
   
#ifdef __WINS__
    _LIT(KMagellanGPS315, "z:\\system\\test\\testdata\\MagellanGPS315.nme");
    _LIT(KFix10MagellanGPS315, "z:\\system\\test\\testdata\\Fix10MagellanGPS315.nme");
    _LIT(KMagellanTracker, "z:\\system\\test\\testdata\\MagellanTracker.nme");
    _LIT(KGarmin12cx, "z:\\system\\test\\testdata\\garmin12cx.nme");
    _LIT(KGarmin3plus, "z:\\system\\test\\testdata\\garmin3plus.nme");
    _LIT(KGarminetrexlegend, "z:\\system\\test\\testdata\\garminetrexlegend.nme");
    _LIT(KMagellanMeridian, "z:\\system\\test\\testdata\\magellanmeridianplatinum.nme");
#else
  _LIT(KMagellanGPS315, "c:\\system\\test\\testdata\\MagellanGPS315.nme");
  _LIT(KFix10MagellanGPS315, "c:\\system\\test\\testdata\\Fix10MagellanGPS315.nme");
  _LIT(KMagellanTracker, "c:\\system\\test\\testdata\\MagellanTracker.nme");
  _LIT(KGarmin12cx, "c:\\system\\test\\testdata\\garmin12cx.nme");
  _LIT(KGarmin3plus, "c:\\system\\test\\testdata\\garmin3plus.nme");
  _LIT(KGarminetrexlegend, "c:\\system\\test\\testdata\\garminetrexlegend.nme");
  _LIT(KMagellanMeridian, "c:\\system\\test\\testdata\\magellanmeridianplatinum.nme");
#endif
    
    _LIT(KVerticalDoPErr, "KVerticalDoP should be NaN in TPositionSatelliteInfo when empty value in NMEA GSA");
     
    for (TInt i=1; i <= 7; i++)
        {
        TPositionCourseInfo courseInfo;
        TPositionSatelliteInfo satInfo;

        switch (i)
            {
            case 1:
                SetFileInUseL(KMagellanGPS315); 
                break;
            case 2:
                SetFileInUseL(KFix10MagellanGPS315);
                break;
            case 3:
                SetFileInUseL(KMagellanTracker);
                AssertTrueL(Math::IsNaN(satInfo.VerticalDoP()), KVerticalDoPErr, KErrGeneral);
                break;
            case 4:
                SetFileInUseL(KGarmin12cx);
                break;
            case 5:
                SetFileInUseL(KGarmin3plus);
                break;
            case 6:
                SetFileInUseL(KGarminetrexlegend);
                break;
            case 7:
                SetFileInUseL(KMagellanMeridian);
                break;
            }
        RequestL(courseInfo);
        RequestL(satInfo);   
        }
    }



// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::GetCorrectSatelliteTime
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::GetCorrectSatelliteTime(TTime& aTime)
    {
    TInt hh = (static_cast<TInt>(KSatelliteTime)/10000);
    TInt mm = (static_cast<TInt>(KSatelliteTime)/100)%100;
    TInt ss = static_cast<TInt>(KSatelliteTime)%100;
    
    TReal frac=0;
    Math::Frac(frac, KSatelliteTime);
    TInt us = static_cast<TInt>(frac * 1000000);

    TInt day = (KSatelliteDate/10000);
    TInt month = (KSatelliteDate/100)%100;
    TInt year = KSatelliteDate%100 + 2000;

    TDateTime dateTime = TDateTime(year, static_cast<TMonth>(month - 1), day, hh, mm, ss, us);
    aTime = TTime(dateTime);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::SetSatelliteRequestedFields()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::SetSatelliteRequestedFields(HPositionGenericInfo& aGenericInfo)
    {
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteNumInView);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteNumUsed);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteTime);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteHorizontalDoP);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteVerticalDoP);
    aGenericInfo.SetRequestedField(EPositionFieldSatellitePositionDoP);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteTimeDoP);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteSeaLevelAltitude);
    aGenericInfo.SetRequestedField(EPositionFieldSatelliteGeoidalSeparation);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::SetSpeedRequestedFields()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::SetSpeedRequestedFields(HPositionGenericInfo& aGenericInfo)
    {
	aGenericInfo.SetRequestedField(EPositionFieldHorizontalSpeed);
    aGenericInfo.SetRequestedField(EPositionFieldHorizontalSpeedError);
	aGenericInfo.SetRequestedField(EPositionFieldVerticalSpeedError);
	aGenericInfo.SetRequestedField(EPositionFieldVerticalSpeed);
    }	

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::SetCompassRequestedFields()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::SetCompassRequestedFields(HPositionGenericInfo& aGenericInfo)
    {
    aGenericInfo.SetRequestedField(EPositionFieldTrueCourse);
    aGenericInfo.SetRequestedField(EPositionFieldTrueCourseError);
    aGenericInfo.SetRequestedField(EPositionFieldMagneticCourse);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::SetFileInUseL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::SetFileInUseL(const TDesC& aFileName)
    {
    _LIT(KOpenErr, "Error when opening positioner,  %d");

    iPositioner.Close();
    SetSimDataFileL(aFileName); 

    TInt err;
    err = iPositioner.Open(iPosServer, iUidSimulationPsy);
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    iPositioner.SetRequestor(CRequestor::ERequestorService,
							CRequestor::EFormatApplication, KServiceName);

    }


// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp242::PrintChecksum
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp242::PrintChecksum(const TDesC8& aSentence)
    {
    TUint8 checksum = 0;
    
    TInt i;
    for (i = 1; i < aSentence.Length() && aSentence[i] != '*'; i++)
        {
        checksum ^= aSentence[i];
        }

    TUint8 lsb = (TUint8)(checksum & 0x0f);
    TUint8 msb = (TUint8)(checksum >> 4); 
   
    _LIT(KStatus, "Checksum field  = %x%x");
    TBuf<32> info;
    info.Format(KStatus, msb, lsb);
	RDebug::Print(info);
	INFO_PRINTF1(info);
    }


//  End of File
