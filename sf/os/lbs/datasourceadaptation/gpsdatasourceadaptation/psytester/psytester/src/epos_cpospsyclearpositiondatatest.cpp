// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "epos_cpospsyclearpositiondatatest.h"
#include "epos_cpospsyrequester.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYClearPositionDataTest::CPosPSYClearPositionDataTest()
    {
	iCourse.SetSpeed(KSpeed);
	iCourse.SetHeading(KHeading);
	iCourse.SetSpeedAccuracy(KSpeedAccuracy);
	iCourse.SetHeadingAccuracy(KHeadingError);    
		
	iPosition.SetTime(KTime);
	iPosition.SetAccuracy(KHorizontalAccuracy, KVerticalAccuracy);
	iPosition.SetCoordinate(KLatitude, KLongitude, KAltitude);
	}

// EPOC default constructor can leave.
void CPosPSYClearPositionDataTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYClearPositionDataTest* CPosPSYClearPositionDataTest::NewL()
    {
    CPosPSYClearPositionDataTest* self = new (ELeave) CPosPSYClearPositionDataTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYClearPositionDataTest::~CPosPSYClearPositionDataTest()
    {
	delete iPosInfo;
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Clear Position Data Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {   
	aStatus = KRequestPending;
	iTestParameters = aTestParameters;
    iRequestStatus = &aStatus;
	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
	iClassesTested = 0;
	iRequestNumber = 0;

    TRAP(iTestStatus, ExecuteTestL());
    if (iTestStatus != KErrNone)
		{
		CompleteClearPositionDataTest(iTestStatus);
		}
	}

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::CancelTest() 
    {
	iTestStatus = KErrCancel;
	iRequester->Cancel();	

	delete iPosInfo;
	iPosInfo = NULL;

	CompleteTest(KErrCancel);
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	if (iTestStatus == KErrCancel)
		{
		CompleteClearPositionDataTest(iTestStatus);
		return;
		}

	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionInfoClass)
		{
		_LIT(KTitleTestInfo, "==== Testing clearing of TPositionInfo ====");
		AddTestResultL(KTitleTestInfo, EDisplayMessage);
		if (iPSYInfo.ClassesSupported() & EPositionInfoClass && 
			!(iClassesTested & EPositionInfoClass))
			{
			iCurrentTestClass = EPositionInfoClass;
			
			if (iRequestNumber == 0) 
				{
				iClassesTested |= EPositionInfoClass;
				PerformTestL(aCompleteCode);
				}
			}
		else
			{
			_LIT(KTestInfo, "Clearing of position data tests not performed with TPositionInfo because PSY didn't support it.");
			AddTestResultL(KTestInfo, EInfoMessage);
			}
		}

	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionGenericInfoClass)
		{
		if (iRequestNumber == 0)
			{
			_LIT(KTestInfoGeneric, "==== Testing clearing of HPositionGenericInfo ====");
			AddTestResultL(KTestInfoGeneric, EDisplayMessage);
			}
		if (iPSYInfo.ClassesSupported() & EPositionGenericInfoClass && 
			!(iClassesTested & EPositionGenericInfoClass))
			{
			iCurrentTestClass = EPositionGenericInfoClass;
			
			if (iRequestNumber == 1) 
				{
				iClassesTested |= EPositionGenericInfoClass;
				PerformTestL(aCompleteCode);	
				}
			else
				{
				HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC(KPositionGenericInfoDefaultBufferSize * 2, 
													  KPositionGenericInfoDefaultMaxFields * 2);

				SetHPositionGenericInfoDataL(*genInfo);
				CleanupStack::Pop(genInfo);
				genInfo->SetPosition(iPosition);
				iPosInfo = static_cast<TPositionInfo*> (genInfo);
				iRequester->MakeRequest(*iPosInfo);
				iRequestNumber++;
				}
			}
		else
			{
			_LIT(KTestInfo, "Clearing of position data tests not performed with HPositionGenericInfo because PSY didn't support it.");
			AddTestResultL(KTestInfo, EInfoMessage);
			}
		}
    
	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionCourseInfoClass)
		{
		if (iRequestNumber == 0)
			{
			_LIT(KTestInfo, "==== Testing clearing of TPositionCourseInfo ====");
			AddTestResultL(KTestInfo, EDisplayMessage);
			}
		if (iPSYInfo.ClassesSupported() & EPositionCourseInfoClass && 
			!(iClassesTested & EPositionCourseInfoClass))
			{
			iCurrentTestClass = EPositionCourseInfoClass;
			if (iRequestNumber == 1) 
				{
				iClassesTested |= EPositionCourseInfoClass;
				PerformTestL(aCompleteCode);
				}
			else
				{
				TPositionCourseInfo* courseInfo = new (ELeave) TPositionCourseInfo;
				courseInfo->SetCourse(iCourse);
				courseInfo->SetPosition(iPosition);
				iPosInfo = static_cast<TPositionInfo*> (courseInfo);
				iRequester->MakeRequest(*iPosInfo);
				iRequestNumber++;
				}
			}
		else
			{
			_LIT(KTestInfo, "Clearing of position data tests not performed with TPositionCourseInfo because PSY didn't support it.");
			AddTestResultL(KTestInfo, EInfoMessage);
			}
		}

	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionSatelliteInfoClass)
		{
		if (iRequestNumber == 0)
			{
			_LIT(KTestInfo, "==== Testing clearing of TPositionSatelliteInfo ====");
			AddTestResultL(KTestInfo, EDisplayMessage);
			}
		if (iPSYInfo.ClassesSupported() & EPositionSatelliteInfoClass &&
			!(iClassesTested & EPositionSatelliteInfoClass))
			{
			iCurrentTestClass = EPositionSatelliteInfoClass;
			if (iRequestNumber == 1)
				{
				iClassesTested |= EPositionSatelliteInfoClass;
				PerformTestL(aCompleteCode);
				}
			else
				{
				TPositionSatelliteInfo* satInfo = new (ELeave) TPositionSatelliteInfo;
				
				satInfo->SetSatelliteTime(KSatelliteTime);
				satInfo->SetHorizontalDoP(KSatelliteHorizontalDoP);
				satInfo->SetVerticalDoP(KSatelliteVerticalDoP);
				satInfo->SetTimeDoP(KSatelliteTimeDoP);

				for (TInt  i = 0; i < KSatelliteNumInView; i++)
					{
					TSatelliteData satellite;
					satellite.SetSatelliteId(KSatelliteId+i);
					satellite.SetAzimuth(KAzimuth);
					satellite.SetElevation(KElevation);
					satellite.SetIsUsed(KIsUsed);
					satellite.SetSignalStrength(KSignalStrength);
	
					satInfo->AppendSatelliteData(satellite);
					}
				
				satInfo->SetCourse(iCourse);
				satInfo->SetPosition(iPosition);
				iPosInfo = static_cast<TPositionInfo*> (satInfo);
				iRequester->MakeRequest(*iPosInfo);
				iRequestNumber++;
				}
			}
		else
			{
			_LIT(KTestInfo, "Clearing of position data tests not performed with TPositionSatelliteInfo because PSY didn't support it.");
			AddTestResultL(KTestInfo, EInfoMessage);
			}
		}
	}

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::CheckGenericInfoClearsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYClearPositionDataTest::CheckGenericInfoClearsL()
    {
	TBuf<KClearPositionDataBufSize> buf;
	TReal32 real32Num(0);
	TInt8 shortNum(0);
	TTime time(0);
	TUint8 unsignedShortNum(0);
	HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*> (iPosInfo);
	
	if (genInfo->GetValue(EPositionFieldCountry, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldCountryCode, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldState, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldCity, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldDistrict, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldStreet, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldStreetExtension, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldLocationName, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldPostalCode, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldLocality, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldCrossing1, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldCrossing2, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldHorizontalSpeed, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldHorizontalSpeedError, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldVerticalSpeedError, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldVerticalSpeed, real32Num) != KErrNotFound &&	
		genInfo->GetValue(EPositionFieldHeading, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldHeadingError, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldTrueCourse, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldTrueCourseError, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldMagneticCourse, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldBuildingName, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldBuildingFloor, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldBuildingRoom, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldBuildingZone, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldBuildingTelephone, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldNMEASentences, unsignedShortNum) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldNMEASentencesStart, buf) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteNumInView, shortNum) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteNumUsed, shortNum) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteTime, time) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteHorizontalDoP, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteVerticalDoP, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteTimeDoP, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatellitePositionDoP, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteSeaLevelAltitude, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldSatelliteGeoidalSeparation, real32Num) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldMediaLinks, unsignedShortNum) != KErrNotFound &&
		genInfo->GetValue(EPositionFieldMediaLinksStart, buf) != KErrNotFound)
		{
		_LIT(KError, "All HPositionGenericInfo fields were not cleared for HPositionGenericInfo.");
        AddTestResultL(KError, EErrorMessage);
		}
	else
		{
		_LIT(KInfo, "All HPositionGenericInfo fields were cleared for HPositionGenericInfo.");
        AddTestResultL(KInfo, EInfoMessage);
		}
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::CheckSatelliteInfoClearsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYClearPositionDataTest::CheckSatelliteInfoClearsL()
    {
    TBool allIsCleared = ETrue;
	TPositionSatelliteInfo* satInfo = static_cast<TPositionSatelliteInfo*> (iPosInfo);

	if (satInfo->SatelliteTime() == KSatelliteTime)
		{
		_LIT(KError, "Satellite Time was not cleared for TPositionSatelliteInfo.");
        AddTestResultL(KError, EErrorMessage);
		allIsCleared = EFalse;
		}
	if (!Math::IsNaN(satInfo->HorizontalDoP()))
		{
		if (satInfo->HorizontalDoP() == KSatelliteHorizontalDoP)
			{	
			_LIT(KError, "Horizontal DoP was not cleared for TPositionSatelliteInfo.");
			AddTestResultL(KError, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(satInfo->VerticalDoP()))
		{
		if (satInfo->VerticalDoP() == KSatelliteVerticalDoP)
			{
			_LIT(KError, "Vertical DoP was not cleared for TPositionSatelliteInfo.");
			AddTestResultL(KError, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(satInfo->TimeDoP()))
		{
		if (satInfo->TimeDoP() == KSatelliteTimeDoP)
			{
			_LIT(KError, "Time DoP was not cleared for TPositionSatelliteInfo.");
			AddTestResultL(KError, EErrorMessage);
			allIsCleared = EFalse;
			}
		}

	TBool satelliteDataNotCleared = EFalse;
	for (TInt  i = 0; i < KSatelliteNumInView; i++)
		{
		TSatelliteData satellite;
		TInt error = satInfo->GetSatelliteData(i, satellite);
		if (error != KErrNotFound)
			{
			if (!Math::IsNaN(satellite.Azimuth()))
				{
				if (satellite.Azimuth() == KAzimuth)
					{
					satelliteDataNotCleared = ETrue;
					}
				}
			if (!Math::IsNaN(satellite.Elevation()))
				{
				if (satellite.Elevation() == KElevation)
					{
					satelliteDataNotCleared = ETrue;
					}
				}

			if (satellite.SatelliteId() == KSatelliteId ||
				satellite.SignalStrength() == KSignalStrength)
				{
				satelliteDataNotCleared = ETrue;
				}
			}
		}
	
	if (satelliteDataNotCleared)
		{
		_LIT(KError, "Satellite Data was not cleared for all satellites in TPositionSatelliteInfo.");
		AddTestResultL(KError, EErrorMessage);
		allIsCleared = EFalse;
		}

	if (allIsCleared)
		{
		_LIT(KInfo, "All fields were cleared for TPositionSatelliteInfo.");
		AddTestResultL(KInfo, EInfoMessage);
		}
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::CheckCourseInfoClearsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYClearPositionDataTest::CheckCourseInfoClearsL(
	const TDesC& aPositionType)
    {
    TCourse course;
	TBool allIsCleared = ETrue;

	TPositionCourseInfo* courseInfo = static_cast<TPositionCourseInfo*> (iPosInfo);
	courseInfo->GetCourse(course);	

	if (!Math::IsNaN(course.Speed()))
		{
		if (course.Speed() == KSpeed)
			{
			_LIT(KError, "Speed not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}	
	if (!Math::IsNaN(course.Heading()))
		{
		if (course.Heading() == KHeading)
			{
			_LIT(KError, "Heading not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(course.SpeedAccuracy()))
		{
		if (course.SpeedAccuracy() == KSpeedAccuracy)
			{
			_LIT(KError, "Speed Accuracy not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(course.HeadingAccuracy()))
		{
		if (course.HeadingAccuracy() == KHeadingError)
			{	
			_LIT(KError, "Heading Accuracy not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	
	if (allIsCleared)
		{
		_LIT(KInfo, "All TPositionCourseInfo fields were cleared for ");
		AddMessageL(KInfo, aPositionType, EInfoMessage);
		}
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::CheckPositionClearsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYClearPositionDataTest::CheckPositionClearsL(
	const TDesC& aPositionType)
    {
	TPosition position;
	iPosInfo->GetPosition(position);
    
	TBool allIsCleared = ETrue;

	if (position.Time() == KTime)
		{
		_LIT(KError, "Time not cleared for ");
		AddMessageL(KError, aPositionType, EErrorMessage);
		allIsCleared = EFalse;
		}
	if (!Math::IsNaN(position.HorizontalAccuracy()))
		{
		if (position.HorizontalAccuracy() == KHorizontalAccuracy)
			{
			_LIT(KError, "Horizontal Accuracy not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(position.VerticalAccuracy()))
		{
		if (position.VerticalAccuracy() == KVerticalAccuracy)
			{
			_LIT(KError, "Vertical Accuracy not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}	
		}
	if (!Math::IsNaN(position.Latitude()))
		{
		if (position.Latitude() == KLatitude)
			{
			_LIT(KError, "Latitude not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(position.Longitude()))
		{
		if (position.Longitude() == KLongitude)
			{
			_LIT(KError, "Longitude not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	if (!Math::IsNaN(position.Altitude()))
		{
		if (position.Altitude() == KAltitude)
			{
			_LIT(KError, "Altitude not cleared for ");
			AddMessageL(KError, aPositionType, EErrorMessage);
			allIsCleared = EFalse;
			}
		}
	
	if (allIsCleared)
		{
		_LIT(KInfo, "All TPositionInfo fields were cleared for ");
		AddMessageL(KInfo, aPositionType, EInfoMessage);
		}
	}

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::ExecuteTestL()
    {
	 if (iPSYInfo.ClassesSupported() != 0)
        {
		iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());

		// Following two rows is to remove lint warning
		delete iPosInfo; 
		iPosInfo = NULL;

		iPosInfo = new (ELeave) TPositionInfo;
		iPosInfo->SetPosition(iPosition);
		iRequester->MakeRequest(*iPosInfo);
		iRequestNumber = 0;
		}	
	 else
		{
        iTestStatus = KErrNotSupported;
		_LIT(KError, "Unable to check clearing of position data because no classes supported");
        AddTestResultL(KError, EErrorMessage);
		}
	}

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::PerformTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYClearPositionDataTest::PerformTestL(
	TInt aCompleteCode)
    {   
	_LIT(KPositionInfoType, "TPositionInfo");
	_LIT(KPositionCourseType, "TPositionCourseInfo");
	_LIT(KPositionSatelliteType, "TPositionSatelliteInfo");
	_LIT(KPositionGenericType, "HPositionGenericInfo");

	if (aCompleteCode == KErrNone)
		{
		if (iCurrentTestClass == EPositionInfoClass) 
			{ 
			CheckPositionClearsL(KPositionInfoType);
			}
    
		else if (iCurrentTestClass == EPositionCourseInfoClass) 
			{
			CheckPositionClearsL(KPositionCourseType);
			CheckCourseInfoClearsL(KPositionCourseType);
			}
    
		else if (iCurrentTestClass == EPositionSatelliteInfoClass) 
			{
			CheckPositionClearsL(KPositionSatelliteType);
			CheckCourseInfoClearsL(KPositionSatelliteType);
			CheckSatelliteInfoClearsL();
			}
    
		else if (iCurrentTestClass == EPositionGenericInfoClass)
			{
			CheckPositionClearsL(KPositionGenericType);
			CheckGenericInfoClearsL();
			}
		}
	else
		{
		UnexpectedCompletionCode(aCompleteCode, iRequestNumber);	
		}
    
	iRequestNumber = 0;
	iCurrentTestClass = 0;
	delete iPosInfo;
	iPosInfo = NULL;
    
	if (iPSYInfo.ClassesSupported() == iClassesTested)
		{
		CompleteClearPositionDataTest(KErrNone);
		}
    }

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::CompleteClearPositionDataTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::CompleteClearPositionDataTest(
	TInt aCompletionCode)
	{
	delete iPosInfo;
	iPosInfo = NULL;
	CompleteTest(aCompletionCode);
	}

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::SetHPositionGenericInfoDataL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::SetHPositionGenericInfoDataL(
	HPositionGenericInfo& aGenericInfo)
	{
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCountry, 
											 KCountry));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCountryCode, 
											 KCountryCode));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldState, 
											 KState));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCity, 
											 KCity));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldDistrict,
											 KDistrict));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldStreet, 
										     KStreet));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldStreetExtension, 
											 KStreetExtension));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldLocationName, 
											 KLocationName));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldPostalCode, 
											 KPostalCode));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldLocality,	
											 KLocality));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCrossing1, 
											 KCrossing1));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldCrossing2, 
											 KCrossing2));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHorizontalSpeed, 
										     KHorizontalSpeed));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHorizontalSpeedError, 
										     KHorizontalSpeedError ));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldVerticalSpeedError, 
											 KVerticalSpeedError));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldVerticalSpeed, 
											 KVerticalSpeed));		
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeading, 
											 KHeading));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldHeadingError, 
											 KHeadingError));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldTrueCourse, 
											 KTrueCourse));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldTrueCourseError, 
											 KTrueCourseError));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMagneticCourse, 
											 KMagneticCourse));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingName, 
										     KBuildingName));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingFloor, 
											 KBuildingFloor));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingRoom, 
											 KBuildingRoom));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingZone, 
										     KBuildingZone));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldBuildingTelephone, 
											 KBuildingTelephone));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentences, 
										     KNMEASentences));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldNMEASentencesStart, 
										     KNMEASentencesStart));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumInView, 
											 KSatelliteNumInView));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteNumUsed, 
											 KSatelliteNumUsed));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteTime, 
											 KSatelliteTime));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteHorizontalDoP, 
										     KSatelliteHorizontalDoP));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteVerticalDoP, 
											 KSatelliteVerticalDoP));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteTimeDoP, 
											 KSatelliteTimeDoP));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatellitePositionDoP, 
											 KSatellitePositionDoP));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteSeaLevelAltitude, 
											 KSatelliteSeaLevelAltitude));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldSatelliteGeoidalSeparation, 
											 KSatelliteGeoidalSeparation));
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinks, 
											 KMediaLinks));	
	User::LeaveIfError(aGenericInfo.SetValue(EPositionFieldMediaLinksStart, 
											 KMediaLinksStart));
	}

// ---------------------------------------------------------
// CPosPSYClearPositionDataTest::AddMessageL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYClearPositionDataTest::AddMessageL(
	const TDesC& aFirstMessage, 
	const TDesC& aSecondMessage,
	TInt aMessageType)
	{
	TBuf<KClearPositionDataBufSize> buf;
	buf.Append(aFirstMessage);
	buf.Append(aSecondMessage);
	buf.Append('.');
	AddTestResultL(buf, aMessageType);
	}

//  End of File
