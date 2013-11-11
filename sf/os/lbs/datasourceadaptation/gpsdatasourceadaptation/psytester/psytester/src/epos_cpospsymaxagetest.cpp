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
#include "epos_cpospsymaxagetest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYMaxAgeTest::CPosPSYMaxAgeTest() : 
	iPosInfo(), 
	iPosInfo2()
    {
    }

// EPOC default constructor can leave.
void CPosPSYMaxAgeTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

CPosPSYMaxAgeTest* CPosPSYMaxAgeTest::NewL()
    {
    CPosPSYMaxAgeTest* self = new (ELeave) CPosPSYMaxAgeTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYMaxAgeTest::~CPosPSYMaxAgeTest()
    {
	delete iRequester;
	delete iTimer;
	delete iPosName;
	delete iGenInfo;
	delete iGenInfo2;
    }

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "MaxAge Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;

	iMaxAgeWaitTimeSeconds = aTestParameters.MaxAgeMaxTestTime();
	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
	iClassesTested = 0;
	
	TRAP(iTestStatus, ExecuteTestL());
	if (iTestStatus != KErrNone)
		{
		CompleteMaxAgeTest(iTestStatus);
		}
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::CancelTest()
	{
	iTestStatus = KErrCancel;
	iRequester->Cancel();

	CompleteMaxAgeTest(iTestStatus);
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::ExecuteTestL()
	{
	if (iPSYInfo.ClassesSupported() == iClassesTested)
		{
		CompleteMaxAgeTest(KErrNone);
		}

	// Check MaxAge for both TPositionInfo and HPositionGenericInfo
	if (iPSYInfo.ClassesSupported() & EPositionInfoClass && 
		!(iClassesTested & EPositionInfoClass))
		{
		_LIT(KPosName, "TPositionInfo");
		TestMaxAgeForPositionL(iPosInfo, iPosInfo2, KPosName());
		iClassesTested |= EPositionInfoClass;
		}
	else if (iPSYInfo.ClassesSupported() & EPositionGenericInfoClass && 
		!(iClassesTested & EPositionGenericInfoClass))
        {
		TestMaxAgeGenericL();
		iClassesTested |= EPositionGenericInfoClass;
		}

    else if (iPSYInfo.ClassesSupported() & EPositionCourseInfoClass && 
		!(iClassesTested & EPositionCourseInfoClass))
        {
		_LIT(KPosName, "TPositionCourseInfo");
		TestMaxAgeForPositionL(iPosCourseInfo, iPosCourseInfo2, KPosName());
		iClassesTested |= EPositionCourseInfoClass;
	    }
    
	else if (iPSYInfo.ClassesSupported() & EPositionSatelliteInfoClass && 
		!(iClassesTested & EPositionSatelliteInfoClass))
        {
		_LIT(KPosName, "TPositionSatelliteInfo");
		TestMaxAgeForPositionL(iPosSatInfo, iPosSatInfo2, KPosName());
		iClassesTested |= EPositionSatelliteInfoClass;
        }
    }

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds /*aRequestTime*/)
	{
	if (iTestStatus == KErrCancel)
		{
		CompleteMaxAgeTest(iTestStatus);
		return;
		}
	
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone && 
		aCompleteCode != KErrTimedOut)
		{
		UnexpectedCompletionCode(aCompleteCode);
		}

	if (iRequestNumber == 2 && aCompleteCode == KErrTimedOut)
		{
		delete iTimer;
		iTimer = NULL;
		iTime2.UniversalTime();
		// Should get a cached position
		iRequester->MakeRequest(*iPos2);
		iRequestNumber++;
		}
	else if (iRequestNumber == 1)
		{
		// Should get a cached position
		iRequester->SetMaxAge(iTime1);
		iRequester->MakeRequest(*iPos2);
		iRequestNumber++;
		}
	else if (iRequestNumber == 2) 
		{
		if (IsSamePosition(*iPos, *iPos2) && IsSameTimeInPosition(*iPos, *iPos2) && iRequester->MaxAgeRequested())
			{
			// OK, Cached position
			_LIT(KInfo, "PSY supports MaxAge for %S.");
			TBuf<KMsgSize> buf;
			buf.Format(KInfo, iPosName);
			AddTestResultL(buf, EInfoMessage);

			// Wait 30 sec.
			TTimeIntervalMicroSeconds32 timeInt(iMaxAgeWaitTimeSeconds * 1000000);
			StartSchedulerForAWhileL(timeInt);
			}
		else
			{
			// PSY doesn't support MaxAge
			_LIT(KInfo, "PSY doesn't support MaxAge for %S.");
			TBuf<KMsgSize> buf;
			buf.Format(KInfo, iPosName);
			AddTestResultL(buf, EInfoMessage);

			ExecuteTestL();
			}
		}
	else if (iRequestNumber == 3) 
		{
		if (IsSamePosition(*iPos, *iPos2) && IsSameTimeInPosition(*iPos, *iPos2))
			{
			// Cached position
			_LIT(KInfo, "Retrieved a cached %S in correct way.");
			TBuf<KMsgSize> buf;
			buf.Format(KInfo, iPosName);
			AddTestResultL(buf, EInfoMessage);
			}
		else
			{
			// PSY doesn't support MaxAge
			_LIT(KInfo, "PSY doesn't support MaxAge for %S in a correct way.");
			TBuf<KMsgSize> buf;
			buf.Format(KInfo, iPosName);
			AddTestResultL(buf, EErrorMessage);
			}

		// Time before first request, so cache position shouldn't be achieved.
		iRequester->SetMaxAge(iTime2);

		// Should get a new position
		iRequester->MakeRequest(*iPos2);	

		iRequestNumber++;
		}
	else
		{
		if (IsSamePosition(*iPos, *iPos2))
			{
			if(IsSameTimeInPosition(*iPos, *iPos2))
				{
				// PSY doesn't support MaxAge
				_LIT(KInfo, "PSY doesn't support MaxAge for %S in a correct way.");
				TBuf<KMsgSize> buf;
				buf.Format(KInfo, iPosName);
				AddTestResultL(buf, EErrorMessage);
				}
			else
				{
				// PSY supports Maxage. However, location Information is the same
				_LIT(KInfo, "PSY supports MaxAge for %S. Location has not changed.");
				TBuf<KMsgSize> buf;
				buf.Format(KInfo, iPosName);
				AddTestResultL(buf, EWarningMessage);		
				}
			}
			else
			{
			// OK, shouldn't be same (cached)
			_LIT(KInfo, "Retrieved a new %S in correct way.");
			TBuf<KMsgSize> buf;
			buf.Format(KInfo, iPosName);
			AddTestResultL(buf, EInfoMessage);
			}

		ExecuteTestL();
		}
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::IsSamePosition
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYMaxAgeTest::IsSameTimeInPosition(
	TPositionInfo& aPos, 
    TPositionInfo& aSecondPos)
    {
	// Check if class types are same
	if (aPos.PositionClassType() != aSecondPos.PositionClassType())
		{
		return EFalse;
		}

	// Check if module id are same
	if (aPos.ModuleId() != aSecondPos.ModuleId())
		{
		return EFalse;
		}

	// Check that module id are not 0.
	if (aPos.ModuleId().iUid == 0 || aSecondPos.ModuleId().iUid == 0)
		{
		return EFalse;
		}

	TPosition pos, secondPos;
	aPos.GetPosition(pos);
	aSecondPos.GetPosition(secondPos);

	// Check if time is same
	if (pos.Time() != secondPos.Time())
		{
		return EFalse;
		}
		
		return ETrue;
    }
    
// ---------------------------------------------------------
// CPosPSYMaxAgeTest::IsSamePosition
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYMaxAgeTest::IsSamePosition(
	TPositionInfo& aPos, 
    TPositionInfo& aSecondPos)
    {
	// Check if class types are same
	if (aPos.PositionClassType() != aSecondPos.PositionClassType())
		{
		return EFalse;
		}

	// Check if module id are same
	if (aPos.ModuleId() != aSecondPos.ModuleId())
		{
		return EFalse;
		}

	// Check that module id are not 0.
	if (aPos.ModuleId().iUid == 0 || aSecondPos.ModuleId().iUid == 0)
		{
		return EFalse;
		}

	TPosition pos, secondPos;
	aPos.GetPosition(pos);
	aSecondPos.GetPosition(secondPos);

	// Check if time is same
	// Time is not checked here. Time value is compared in
	// IsSameTimeInPosition()
	// if (pos.Time() != secondPos.Time())
	//	{
	//	return EFalse;
	//	}
	
	// Check if longitude is same
	if (!IsEqual(pos.Longitude(), secondPos.Longitude()))	
		{
		return EFalse;
		}
	// Check if latitude is same
	if (!IsEqual(pos.Latitude(), secondPos.Latitude()))
		{
		return EFalse;
		}
	// Check if altitude is same
	if (!IsEqual(pos.Altitude(), secondPos.Altitude()))
		{
		return EFalse;
		}

	// Check for TPositionCourseInfo
	if (aPos.PositionClassType() & EPositionCourseInfoClass)
		{
		TCourse course1, course2;
		TPositionCourseInfo& courseInfo1 = static_cast<TPositionCourseInfo&> (aPos);
		TPositionCourseInfo& courseInfo2 = static_cast<TPositionCourseInfo&> (aSecondPos);
		courseInfo1.GetCourse(course1);
		courseInfo2.GetCourse(course2);

		if (!IsEqual(course1.Speed(), course2.Speed()))
			{
			return EFalse;
			}
		if (!IsEqual(course1.SpeedAccuracy(), course2.SpeedAccuracy()))
			{
			return EFalse;
			}
		if (!IsEqual(course1.Heading(), course2.Heading()))
			{
			return EFalse;
			}
		if (!IsEqual(course1.HeadingAccuracy(), course2.HeadingAccuracy()))
			{
			return EFalse;
			}
		}

	// Check for TPositionSatelliteInfo
	if (aPos.PositionClassType() & EPositionSatelliteInfoClass)
		{
		TPositionSatelliteInfo& satInfo1 = static_cast<TPositionSatelliteInfo&> (aPos);
		TPositionSatelliteInfo& satInfo2 = static_cast<TPositionSatelliteInfo&> (aSecondPos);
		
		if (satInfo1.NumSatellitesInView() != satInfo2.NumSatellitesInView()) 
			{
			return EFalse;
			}
		if (satInfo1.NumSatellitesUsed() != satInfo2.NumSatellitesUsed()) 
			{
			return EFalse;
			}
		if (satInfo1.SatelliteTime() != satInfo2.SatelliteTime()) 
			{
			return EFalse;
			}
		
		if (!IsEqual(satInfo1.HorizontalDoP(), satInfo2.HorizontalDoP()))
			{
			return EFalse;
			}

		if (!IsEqual(satInfo1.VerticalDoP(), satInfo2.VerticalDoP()))
			{
			return EFalse;
			}
	
		if (!IsEqual(satInfo1.TimeDoP(), satInfo2.TimeDoP()))
			{
			return EFalse;
			}
		}

	// Check for HPositionGenericInfo
	if (aPos.PositionClassType() & EPositionGenericInfoClass)
		{
		HPositionGenericInfo& genInfo1 = static_cast<HPositionGenericInfo&> (aPos);
		HPositionGenericInfo& genInfo2 = static_cast<HPositionGenericInfo&> (aSecondPos);

		TPositionFieldId fieldId;
		fieldId = genInfo2.FirstRequestedFieldId();
		while (fieldId) 
			{
			if (genInfo1.IsFieldAvailable(fieldId) && !genInfo2.IsFieldAvailable(fieldId)) 
				{
				return EFalse;
				}
			if (genInfo2.IsFieldAvailable(fieldId) && !genInfo1.IsFieldAvailable(fieldId)) 
				{
				return EFalse;
				}
			fieldId = genInfo2.NextRequestedFieldId(fieldId);
			}	
		}

	return ETrue;
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::TestMaxAgeForPositionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::TestMaxAgeForPositionL(
	TPositionInfo& aPosInfo, 
	TPositionInfo& aPosInfo2, 
	const TDesC& aPosName)
    {
	iPos = &aPosInfo;
	iPos2 = &aPosInfo2;

	delete iPosName;
	iPosName = NULL;
	iPosName = aPosName.AllocL();

	if (iPos->PositionClassType() & EPositionGenericInfoClass)
		{
		HPositionGenericInfo& genInfo = static_cast<HPositionGenericInfo&> (*iPos);
		HPositionGenericInfo& genInfo2 = static_cast<HPositionGenericInfo&> (*iPos2);

		TPositionModuleInfo::TCapabilities cap = iPSYInfo.Capabilities();
		if (cap & TPositionModuleInfo::ECapabilityHorizontal)
			{
			genInfo.SetRequestedField(EPositionFieldHorizontalSpeed);
			genInfo2.SetRequestedField(EPositionFieldHorizontalSpeed);
			}
		if (cap & TPositionModuleInfo::ECapabilityVertical)
			{
			genInfo.SetRequestedField(EPositionFieldVerticalSpeed);
			genInfo2.SetRequestedField(EPositionFieldVerticalSpeed);
			}
		if (cap & TPositionModuleInfo::ECapabilitySpeed)
			{
			genInfo.SetRequestedField(EPositionFieldHorizontalSpeed);
			genInfo2.SetRequestedField(EPositionFieldHorizontalSpeed);
			}
		if (cap & TPositionModuleInfo::ECapabilityDirection)
			{
			genInfo.SetRequestedField(EPositionFieldHeading);
			genInfo2.SetRequestedField(EPositionFieldHeading);
			}
		if (cap & TPositionModuleInfo::ECapabilitySatellite)
			{
			genInfo.SetRequestedField(EPositionFieldSatelliteNumInView);
			genInfo2.SetRequestedField(EPositionFieldSatelliteNumInView);
			}
		if (cap & TPositionModuleInfo::ECapabilityCompass)
			{
			genInfo.SetRequestedField(EPositionFieldTrueCourse);
			genInfo2.SetRequestedField(EPositionFieldTrueCourse);
			}
		if (cap & TPositionModuleInfo::ECapabilityNmea)
			{
			genInfo.SetRequestedField(EPositionFieldNMEASentences);
			genInfo2.SetRequestedField(EPositionFieldNMEASentences);
			}
		if (cap & TPositionModuleInfo::ECapabilityAddress)
			{
			genInfo.SetRequestedField(EPositionFieldCountry);
			genInfo2.SetRequestedField(EPositionFieldCountry);
			}
		if (cap & TPositionModuleInfo::ECapabilityBuilding)
			{
			genInfo.SetRequestedField(EPositionFieldBuildingName);
			genInfo2.SetRequestedField(EPositionFieldBuildingName);
			}
		if (cap & TPositionModuleInfo::ECapabilityMedia)
			{
			genInfo.SetRequestedField(EPositionFieldMediaLinks);
			genInfo2.SetRequestedField(EPositionFieldMediaLinks);
			}
		}

	iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());
	iTime1.UniversalTime();

	// Retrieving a normal position update
	iRequester->SetMaxAgeRequested(EFalse);
	iRequester->SetMaxAge(TTime(0));
	iRequester->MakeRequest(*iPos);
	iRequestNumber = 1;
    }

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::TestMaxAgeGenericL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::TestMaxAgeGenericL()
	{
	iGenInfo  = HPositionGenericInfo::NewL();
	iGenInfo2 = HPositionGenericInfo::NewL();
	_LIT(KPosName, "HPositionGenericInfo");
	TestMaxAgeForPositionL(*iGenInfo, *iGenInfo2, KPosName());
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::CompleteMaxAgeTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::CompleteMaxAgeTest(TInt aCompletionCode)
	{
	delete iTimer;
	iTimer = NULL;
	delete iPosName;
	iPosName = NULL;
	delete iGenInfo;
	iGenInfo = NULL;
	delete iGenInfo2;
	iGenInfo2 = NULL;

	CompleteTest(aCompletionCode);
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::PeriodicCallback
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYMaxAgeTest::PeriodicCallback(TAny* aAny)
    {
	CPosPSYMaxAgeTest* maxAgeTest = static_cast<CPosPSYMaxAgeTest*> (aAny);
	TInt err = KErrNone;
	
	TRAP(err, maxAgeTest->HandlePositionCompleteL(KErrTimedOut, TTimeIntervalMicroSeconds(0)));	
	
	return KErrNone;
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::StartSchedulerForAWhileL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYMaxAgeTest::StartSchedulerForAWhileL(
	TTimeIntervalMicroSeconds32 aTimeout)
    {
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iTimer->Start(aTimeout, aTimeout, TCallBack(PeriodicCallback, this));
    }

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::IsEqual
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYMaxAgeTest::IsEqual(
	TReal32 aReal1,
	TReal32 aReal2)
    {
    return IsEqual(TRealX(aReal1), TRealX(aReal2));
	}

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::IsEqual
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYMaxAgeTest::IsEqual(
	TReal64 aReal1,
	TReal64 aReal2)
    {
    return IsEqual(TRealX(aReal1), TRealX(aReal2));
    }

// ---------------------------------------------------------
// CPosPSYMaxAgeTest::IsEqual
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYMaxAgeTest::IsEqual(
    TRealX aReal1,
    TRealX aReal2)
    {
    return (aReal1.IsNaN() && aReal2.IsNaN()) || aReal1 == aReal2;
    }

//  End of File
