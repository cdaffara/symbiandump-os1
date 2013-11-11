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
#include <lbssatellite.h>
#include "epos_cpospsylocationrequesttest.h"
#include "epos_cpospsyhorizontalcapabilitytest.h"
#include "epos_cpospsyverticalcapabilitytest.h"
#include "epos_cpospsyspeedcapabilitytest.h"
#include "epos_cpospsydirectioncapabilitytest.h"
#include "epos_cpospsycompasscapabilitytest.h"
#include "epos_cpospsyaddresscapabilitytest.h"
#include "epos_cpospsymediacapabilitytest.h"
#include "epos_cpospsybuildingcapabilitytest.h"
#include "epos_cpospsynmeacapabilitytest.h"
#include "epos_cpospsysatellitecapabilitytest.h"
#include "epos_cpospsyrequester.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYLocationRequestTest::CPosPSYLocationRequestTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYLocationRequestTest::ConstructL()
    {
    BaseConstructL();
	iRequester = CPosPSYRequester::NewL(this);
    }

// Two-phased constructor.
CPosPSYLocationRequestTest* CPosPSYLocationRequestTest::NewL()
    {
    CPosPSYLocationRequestTest* self = new (ELeave) CPosPSYLocationRequestTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYLocationRequestTest::~CPosPSYLocationRequestTest()
    {
	iPositionArray.ResetAndDestroy();
    iPositionArray.Close();
	
    iTimeToFixArray.Reset();
    iTimeToFixArray.Close();

	delete iPosInfo;
	delete iRequester;
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Location Request Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {   
	aStatus = KRequestPending;
	iRequestStatus = &aStatus;

	// Reset test
	iTestParameters = aTestParameters;
    iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
	iClassesTested = 0;
	iRequestNumber = 0;
	iPositionArray.ResetAndDestroy();
    iTimeToFixArray.Reset();

    TRAP(iTestStatus, ExecuteTestL());
    if (iTestStatus != KErrNone)
		{
		CompleteLocationRequestTest(iTestStatus);
		}
	}

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::CancelTest() 
    {
	iTestStatus = KErrCancel;
	iRequester->Cancel();	

	CompleteLocationRequestTest(KErrCancel);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::HandlePositionCompleteL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::HandlePositionCompleteL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds aRequestTime)
	{
	if (iTestStatus == KErrCancel)
		{
		CompleteLocationRequestTest(iTestStatus);
		return;
		}
	
	if (iRequestNumber >= 1)
		{
		AppendIfValidL(aCompleteCode, aRequestTime);
		// Ownership of iPosInfo has been taken by position array therefore it can be nulled
		iPosInfo = NULL;
		}

	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionInfoClass)
		{
		if (iCurrentTestClass == 0)
			{	
			AddTestResultL(KTestPositionInfo, EDisplayMessage);
			}
		CheckTestStatusL(EPositionInfoClass, aRequestTime);
		}

	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionGenericInfoClass)
		{
		if (iCurrentTestClass == 0)
			{
			AddTestResultL(KTestGenericInfo, EDisplayMessage);
			}
		CheckTestStatusL(EPositionGenericInfoClass, aRequestTime);
		}
    
	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionCourseInfoClass)
		{
		if (iCurrentTestClass == 0)
			{
			AddTestResultL(KTestCourseInfo, EDisplayMessage);
			}
		CheckTestStatusL(EPositionCourseInfoClass, aRequestTime);
		}

	if (iCurrentTestClass == 0 || iCurrentTestClass == EPositionSatelliteInfoClass)
		{
		if (iCurrentTestClass == 0)
			{
			AddTestResultL(KTestSatelliteInfo, EDisplayMessage);
			}
		CheckTestStatusL(EPositionSatelliteInfoClass, aRequestTime);
		}
	}

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckAddressCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckAddressCapabilityL()
    {
    CPosPSYAddressCapabilityTest* addressCapabilityTester = 
        CPosPSYAddressCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    addressCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(addressCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckBuildingCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckBuildingCapabilityL()
    {
    CPosPSYBuildingCapabilityTest* buildingCapabilityTester = 
        CPosPSYBuildingCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    buildingCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(buildingCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckCompassCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckCompassCapabilityL()
    {
    CPosPSYCompassCapabilityTest* compassCapabilityTester = 
        CPosPSYCompassCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    compassCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(compassCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::ChackDirectionCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckDirectionCapabilityL()
    {
    CPosPSYDirectionCapabilityTest* directionCapabilityTester = 
        CPosPSYDirectionCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    directionCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(directionCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckHorizontalAccuracyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckHorizontalAccuracyL()
    {
    CPosPSYHorizontalCapabilityTest* horizontalCapabilityTester = 
        CPosPSYHorizontalCapabilityTest::NewLC(*iTestResults, 
												iPositionArray, 
												iPSYInfo, 
												iTestParameters);
    horizontalCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(horizontalCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckMediaCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckMediaCapabilityL()
    {
    CPosPSYMediaCapabilityTest* mediaCapabilityTester = 
        CPosPSYMediaCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    mediaCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(mediaCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckNMEACapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckNMEACapabilityL()
    {
    CPosPSYNMEACapabilityTest* nmeaCapabilityTester = 
        CPosPSYNMEACapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    nmeaCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(nmeaCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckSatelliteCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckSatelliteCapabilityL()
    {
    CPosPSYSatelliteCapabilityTest* satelliteCapabilityTester = 
        CPosPSYSatelliteCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    satelliteCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(satelliteCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckSpeedCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckSpeedCapabilityL()
    {
    CPosPSYSpeedCapabilityTest* speedCapabilityTester = 
        CPosPSYSpeedCapabilityTest::NewLC(*iTestResults, iPositionArray, iPSYInfo);
    speedCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(speedCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckTimeL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYLocationRequestTest::CheckTimeL()
    {
    TInt nrOfWrongTime = 0;
	TBuf<KLocationRequestBufSize> buf;
    
    for (TInt i = 0; i < iPositionArray.Count(); i++)
        {
        TPosition pos;
		iPositionArray[i]->GetPosition(pos);
       
		if (pos.Time().Int64() == 0)
            {
            nrOfWrongTime++;
            }
        }
    
	if (nrOfWrongTime > 0)
        {   
        _LIT(KError, "%d of %d requests returned time = 0 in position object.");
        buf.Format(KError, nrOfWrongTime, iPositionArray.Count());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "All requests returned correct time in position object.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckTimeToFirstFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYLocationRequestTest::CheckTimeToFirstFixL(
    TTimeIntervalMicroSeconds& aTime)
    {
    if (aTime > iPSYInfo.TimeToFirstFix())
        {
        _LIT(KWarning, "Time to first fix greater than specified.");
        AddTestResultL(KWarning, EWarningMessage);
        }
    else
        {
        _LIT(KInfo, "Time to first fix less or equal than specified.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckTimeToFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYLocationRequestTest::CheckTimeToFixL()
    {
    TInt toLongTimeNr = 0;
    TBuf<KLocationRequestBufSize> buf;
	// Skip checking first value, TimeToFirstFix
    for (TInt i = 1; i < iTimeToFixArray.Count(); i++) 
        {
        if (iTimeToFixArray[i] > iPSYInfo.TimeToFix())
            {
            toLongTimeNr++;
            }
        }

	_LIT(KInfo, "%d of %d  of TimeToFix values are greater than specified value.");
    buf.Format(KInfo, toLongTimeNr, iTimeToFixArray.Count()-1);
    if (toLongTimeNr > (iTimeToFixArray.Count() / iTestParameters.TimeToFixThreshold()))
        {
        AddTestResultL(buf, EWarningMessage);
        }
    else
        {
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckUidL
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CPosPSYLocationRequestTest::CheckUidL()
    {
    TInt nrOfWrongUidNr = 0;
    TBuf<KLocationRequestBufSize> buf;
    
    for (TInt i = 0; i < iPositionArray.Count(); i++)
        {
        if (iPositionArray[i]->ModuleId().iUid != iPSYInfo.PSYBaseInfo().Uid().iUid)
            {
            nrOfWrongUidNr++;
            }
        }
    if (nrOfWrongUidNr > 0)
        {   
        _LIT(KError, "%d of %d Requests returned wrong UID number in position object.");
        buf.Format(KError, nrOfWrongUidNr, iPositionArray.Count());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "All Requests returned correct UID number in position object.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckVerticalAccuracyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//  
void CPosPSYLocationRequestTest::CheckVerticalAccuracyL()
    {
    CPosPSYVerticalCapabilityTest* verticalCapabilityTester = 
        CPosPSYVerticalCapabilityTest::NewLC(*iTestResults, 
											 iPositionArray, 
											 iPSYInfo, 
											 iTestParameters);
    verticalCapabilityTester->TestCapabilityL();
    CleanupStack::PopAndDestroy(verticalCapabilityTester);
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckNrOfCapabilities
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
TInt CPosPSYLocationRequestTest::CheckNrOfCapabilities()
    {
    TInt nrOfCapabilitiesToCheck = 0;
    
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySpeed)
        {
        nrOfCapabilitiesToCheck += 4;
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityDirection)
        {   
        nrOfCapabilitiesToCheck += 2;
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityCompass)
        {
        nrOfCapabilitiesToCheck += 6;
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityAddress)
        {
        nrOfCapabilitiesToCheck += 12;
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityBuilding)
        {
        nrOfCapabilitiesToCheck += 5;
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityNmea)
        {
		//Maximum number of nmea sentences that can be fetched is limited to 10
        nrOfCapabilitiesToCheck += 11; 
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityMedia)
        {
        //Maximum number of media sentences that can be fetched is limited to 10
		nrOfCapabilitiesToCheck += 11;
        }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite)
        {
        nrOfCapabilitiesToCheck += 9;
        }
    if (nrOfCapabilitiesToCheck == 0)
		{
		nrOfCapabilitiesToCheck = KPositionGenericInfoDefaultMaxFields;
		}
    
    return nrOfCapabilitiesToCheck;
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::ExecuteTestL()
    {
	iNrOfCapabilities = CheckNrOfCapabilities();   
    
    if (iPSYInfo.ClassesSupported() != 0)
        {
		iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());

		iPosInfo = new (ELeave) TPositionInfo;
		iRequester->MakeRequest(*iPosInfo);
		iRequestNumber = 1;
	    }
	else
        {
        iTestStatus = KErrNotSupported;
		_LIT(KError, "Unable to check location requests because no classes supported.");
        AddTestResultL(KError, EErrorMessage);
        }
	}

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::PerformTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYLocationRequestTest::PerformTestL()
    {   
	if (iPositionArray.Count() > 0)
		{
		TPositionInfo* position = iPositionArray[0];
    
		if (position->PositionClassType() & EPositionGenericInfoClass ||
			position->PositionClassType() & EPositionInfoClass ||
			position->PositionClassType() & EPositionCourseInfoClass ||
			position->PositionClassType() & EPositionSatelliteInfoClass) 
			{ 
			CheckUidL();
			CheckTimeToFixL();
			CheckTimeL();
			CheckHorizontalAccuracyL();
			CheckVerticalAccuracyL();
			}
    
		if (iNrOfCapabilities > 0)
			{
			if (position->PositionClassType() & EPositionGenericInfoClass ||
				position->PositionClassType() & EPositionCourseInfoClass ||
				position->PositionClassType() & EPositionSatelliteInfoClass) 
				{
				CheckSpeedCapabilityL();
				CheckDirectionCapabilityL();
				}

			if (position->PositionClassType() & EPositionGenericInfoClass ||
				position->PositionClassType() & EPositionSatelliteInfoClass) 
				{
				CheckSatelliteCapabilityL();
				}

			if (position->PositionClassType() & EPositionGenericInfoClass)
				{
				CheckCompassCapabilityL();
				CheckNMEACapabilityL();
				CheckAddressCapabilityL();
				CheckMediaCapabilityL();
				CheckBuildingCapabilityL();
				}
			}
		else
			{
			iTestStatus = KErrNotSupported;
			_LIT(KError, "Unable to check location requests for all classes because no capabilities supported.");
			_LIT(KDelim, "====================================================================================");
			AddTestResultL(KDelim, EDisplayMessage);
			AddTestResultL(KError, EErrorMessage);
			}
		}
    
	iRequestNumber = 0;
	iCurrentTestClass = 0;
	iPositionArray.ResetAndDestroy();
    iTimeToFixArray.Reset();
	// To get a valid first time to fix value
	iRequester->ClosePositioner();
	iRequester->OpenPositionerL(iPSYInfo.PSYBaseInfo().Uid());

	if (iPSYInfo.ClassesSupported() == iClassesTested)
		{
		CompleteLocationRequestTest(KErrNone);
		}
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::SetRequestedFieldIds
//
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CPosPSYLocationRequestTest::SetRequestedFieldIds(
    HPositionGenericInfo* info)
    {
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySpeed)
            {
            info->SetRequestedField(EPositionFieldHorizontalSpeed);
            info->SetRequestedField(EPositionFieldHorizontalSpeedError);
            info->SetRequestedField(EPositionFieldVerticalSpeed);
            info->SetRequestedField(EPositionFieldVerticalSpeedError);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityDirection)
            {   
            info->SetRequestedField(EPositionFieldHeading);
            info->SetRequestedField(EPositionFieldHeadingError);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityCompass)
            {
            info->SetRequestedField(EPositionFieldTrueCourse);
            info->SetRequestedField(EPositionFieldTrueCourseError);
            info->SetRequestedField(EPositionFieldMagneticCourse);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityAddress)
            {
            info->SetRequestedField(EPositionFieldCountry);
            info->SetRequestedField(EPositionFieldCountryCode);
            info->SetRequestedField(EPositionFieldState);
            info->SetRequestedField(EPositionFieldCity);
            info->SetRequestedField(EPositionFieldDistrict);
            info->SetRequestedField(EPositionFieldStreet);
            info->SetRequestedField(EPositionFieldStreetExtension);
            info->SetRequestedField(EPositionFieldLocationName);
            info->SetRequestedField(EPositionFieldPostalCode);
            info->SetRequestedField(EPositionFieldLocality);
            info->SetRequestedField(EPositionFieldCrossing1);
            info->SetRequestedField(EPositionFieldCrossing2);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityBuilding)
            {
            info->SetRequestedField(EPositionFieldBuildingName);
            info->SetRequestedField(EPositionFieldBuildingFloor);
            info->SetRequestedField(EPositionFieldBuildingRoom);
            info->SetRequestedField(EPositionFieldBuildingZone);
            info->SetRequestedField(EPositionFieldBuildingTelephone);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityNmea)
            {
            info->SetRequestedField(EPositionFieldNMEASentences);
            info->SetRequestedField(EPositionFieldNMEASentencesStart);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityMedia)
            {
			info->SetRequestedField(EPositionFieldMediaLinks);    
			info->SetRequestedField(EPositionFieldMediaLinksStart);
            }
    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite)
            {
            info->SetRequestedField(EPositionFieldSatelliteNumInView);
            info->SetRequestedField(EPositionFieldSatelliteNumUsed);
            info->SetRequestedField(EPositionFieldSatelliteTime);
            info->SetRequestedField(EPositionFieldSatelliteHorizontalDoP);
            info->SetRequestedField(EPositionFieldSatelliteVerticalDoP);
            info->SetRequestedField(EPositionFieldSatelliteTimeDoP);
            info->SetRequestedField(EPositionFieldSatellitePositionDoP);
            info->SetRequestedField(EPositionFieldSatelliteSeaLevelAltitude);
            info->SetRequestedField(EPositionFieldSatelliteGeoidalSeparation);
            }
    }

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CompleteLocationRequestTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::CompleteLocationRequestTest(
	TInt aCompletionCode)
	{
	delete iPosInfo;
	iPosInfo = NULL;

	CompleteTest(aCompletionCode);
	}

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::AppendIfValidL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::AppendIfValidL(
	TInt aCompleteCode,
	TTimeIntervalMicroSeconds aRequestTime)
	{
	// Check for unexpected completion codes
	if (aCompleteCode != KErrNone)
		{
		UnexpectedCompletionCode(aCompleteCode, iRequestNumber);
		delete iPosInfo;
		iPosInfo = NULL;
		}
	else
		{
		User::LeaveIfError(iTimeToFixArray.Append(aRequestTime));
		User::LeaveIfError(iPositionArray.Append(iPosInfo));
		}
	}

// ---------------------------------------------------------
// CPosPSYLocationRequestTest::CheckTestStatusL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYLocationRequestTest::CheckTestStatusL(
	TInt aTestStatus,
	TTimeIntervalMicroSeconds aRequestTime)
	{
	delete iPosInfo;
	iPosInfo = NULL;

	// Check TimeToFirstFix
	if (iRequestNumber == 1)
		{
		CheckTimeToFirstFixL(aRequestTime);
		}

	if (iPSYInfo.ClassesSupported() & aTestStatus && 
			!(iClassesTested & aTestStatus))
		{
		iCurrentTestClass = aTestStatus;

		if (iRequestNumber < iTestParameters.NumberOfLocationRequests())
			{
			if (iCurrentTestClass == EPositionInfoClass)
				{
				iPosInfo = new (ELeave) TPositionInfo;
				}
			else if (iCurrentTestClass == EPositionCourseInfoClass)
				{
				iPosInfo = new (ELeave) TPositionCourseInfo;
				}
			else if (iCurrentTestClass == EPositionSatelliteInfoClass)
				{
				iPosInfo = new (ELeave) TPositionSatelliteInfo;
				}
			else
				{
				HPositionGenericInfo* genInfo = 
					HPositionGenericInfo::NewL(KPositionGenericInfoDefaultBufferSize, 
											   iNrOfCapabilities);
				SetRequestedFieldIds(genInfo);
				iPosInfo = genInfo;
				}
			iRequester->MakeRequest(*iPosInfo);
			iRequestNumber++;
			}
		else
			{
			iClassesTested |= aTestStatus;
			PerformTestL();
			}
		}
	else
		{
		_LIT(KTestInfo, "Location request tests not performed because PSY didn't support class type.");
		AddTestResultL(KTestInfo, EInfoMessage);
		}
	}

//  End of File
