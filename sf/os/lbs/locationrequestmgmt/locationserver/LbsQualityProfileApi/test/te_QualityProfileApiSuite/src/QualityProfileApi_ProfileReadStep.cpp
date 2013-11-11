// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file QualityProfileApi_ProfileReadStep.cpp
 @internalTechnology
*/

#include "QualityProfileApi_ProfileReadStep.h"
#include "Te_QualityProfileApiSuiteSuiteDefs.h"
#include "LbsInternalInterface.h"
#include "lbsmoduleinfoparser.h"
#include "lbsqualityprofile.h"
#include <lbs/lbsqualityprofileapi.h>

// Locations and paths used when parsing .ini files
_LIT(KLbsDir, "lbs");
_LIT(KQualityProfileIniName, "lbsprofile.ini");

const TInt KSecToMicroSecTransfer = 1000*1000;

CQualityProfileApi_ProfileReadStep::~CQualityProfileApi_ProfileReadStep()
/**
 * Destructor
 */
	{
	}

CQualityProfileApi_ProfileReadStep::CQualityProfileApi_ProfileReadStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KQualityProfileApi_ProfileReadStep);
	}

TVerdict CQualityProfileApi_ProfileReadStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
/* Read in quality profile information from the given .ini file.

The file will likely have multiple quality profiles in it, so
each one must be read in and stored in the given array

@param aFile File to parse for quality profile information
@param aQualityArray Array in which to store the parsed quality profile info.
*/
void CQualityProfileApi_ProfileReadStep::GetQualityProfileInfoL(RFs& aFs, 
		    							RArray<TQualityProfile>& aQualityArray,
		    							const TDesC& aFileName)
	{
  
    // Check the file is present.
	TUint att;
	TInt err = aFs.Att(aFileName, att);
	if(err != KErrNone)
		{
		User::Leave(err);
		}

	// Open the .ini file reader
    CLbsIniFileReader* fileReader = CLbsIniFileReader::NewL(aFs, aFileName);
	CleanupStack::PushL(fileReader);

	_LIT(KKeyProfileID, "ProfileID");
	_LIT(KKeyMaxTime, "MaxTime");
	_LIT(KKeyHorizontalAccuracy, "HorizontalAccuracy");
	_LIT(KKeyVerticalAccuracy, "VerticalAccuracy");
	_LIT(KKeyMeasurementInitialTime, "MeasurementInitialTime");
	_LIT(KKeyMeasurementIntermediateTime, "MeasurementIntermediateTime");
	_LIT(KFinalNetPositionLag, "FinalNetPositionLag");
	
	// Read in all the quality profile info, section by section.
	while (fileReader->FindNextSection())
		{
		TInt result;
		TBool foundVar = fileReader->FindVar(KKeyProfileID, result);
		if (foundVar)
			{
			TQualityProfile qualityProfile;
			
			// Set the profile Id
			qualityProfile.SetQualityProfileId(result);
			
			// Set the request quality
			foundVar = fileReader->FindVar(KKeyMaxTime, result);
			if (foundVar)
				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetMaxFixTime(microseconds);
				}

			TReal32 resultReal32;
			foundVar = fileReader->FindVar(KKeyHorizontalAccuracy, resultReal32);
			if (foundVar)
				{
				qualityProfile.SetMinHorizontalAccuracy(resultReal32);
				}

			foundVar = fileReader->FindVar(KKeyVerticalAccuracy, resultReal32);
			if (foundVar)
				{
				qualityProfile.SetMinVerticalAccuracy(resultReal32);
				}

			foundVar = fileReader->FindVar(KKeyMeasurementInitialTime, result);
			if (foundVar)
				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetMeasurementInitialFixTime(microseconds);
				}

			foundVar = fileReader->FindVar(KKeyMeasurementIntermediateTime, result);
			if (foundVar)
				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetMeasurementIntermediateFixTime(microseconds);
				}
			
 			foundVar = fileReader->FindVar(KFinalNetPositionLag, result);
 			if (foundVar)
 				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetFinalNetPositionLag(microseconds);
 				}
			
			// Add the quality profile to the list
			User::LeaveIfError(aQualityArray.Append(qualityProfile));
			}
		}

	CleanupStack::PopAndDestroy(fileReader);	
	}

void CQualityProfileApi_ProfileReadStep::InitializeQualityProfileInfoL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TBuf<32> privatePath;
	User::LeaveIfError(fs.PrivatePath(privatePath));

	TParse parse;
	parse.Set(privatePath, NULL, NULL);
	parse.AddDir(KLbsDir);
	
	RArray<TQualityProfile> qualityArray;
	CleanupClosePushL(qualityArray);
	
	qualityArray.Reserve(5);

	// Only want to use the first file that is found.
	// The way TFindFile::FindByDir works, it will search
	// C: and D: before Z:, which is what we want.
	TFindFile findFile(fs);
	TInt err = findFile.FindByDir(KQualityProfileIniName, parse.Path());
	if (err == KErrNone)
		{
		GetQualityProfileInfoL(fs, qualityArray, findFile.File());
		}
	
	// Publish the quality profile info
	LbsQualityProfile::InitializeL(qualityArray);
	
	CleanupStack::PopAndDestroy(&qualityArray);
	CleanupStack::PopAndDestroy(&fs);
	}

TVerdict CQualityProfileApi_ProfileReadStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	InitializeQualityProfileInfoL();
	
	if (TestStepResult()==EPass)
		{
		//
		// This step tests that the lbsqualityprofileapi can 
		// successfully access the quality profile data that is stored
		// in lbsprofile.ini
		//
		
		CLbsQualityProfileApi* profileApi = CLbsQualityProfileApi::NewL();
		CleanupStack::PushL(profileApi);

		TReal32 realValue;
		TTimeIntervalMicroSeconds timeValue;
		
		// Check the values of quality profile Id = 0
		TInt err = profileApi->Get(0, KLbsQualityProfileHorizontalAccuracy, realValue);
		TESTL(err == KErrNone);
		TESTL(realValue == TReal32(50));

		err = profileApi->Get(0, KLbsQualityProfileVerticalAccuracy, realValue);
		TESTL(err == KErrNone);
		TESTL(realValue == TReal32(1000));

		err = profileApi->Get(0, KLbsQualityProfileMaxFixTime, timeValue);
		TESTL(err == KErrNone);
		TESTL(timeValue == TTimeIntervalMicroSeconds(130000000));

		err = profileApi->Get(0, KLbsQualityProfileMeasurementInitialFixTime, timeValue);
		TESTL(err == KErrNone);
		TESTL(timeValue == TTimeIntervalMicroSeconds(25000000));

		err = profileApi->Get(0, KLbsQualityProfileMeasurementIntermediateFixTime, timeValue);
		TESTL(err == KErrNone);
		TESTL(timeValue == TTimeIntervalMicroSeconds(10000000));

		// Check the values of quality profile Id = 1
		err = profileApi->Get(1, KLbsQualityProfileHorizontalAccuracy, realValue);
		TESTL(err == KErrNone);
		TESTL(realValue == TReal32(20));

		err = profileApi->Get(1, KLbsQualityProfileVerticalAccuracy, realValue);
		TESTL(err == KErrNone);
		TESTL(realValue == TReal32(500));

		err = profileApi->Get(1, KLbsQualityProfileMaxFixTime, timeValue);
		TESTL(err == KErrNone);
		TESTL(timeValue == TTimeIntervalMicroSeconds(120000000));

		err = profileApi->Get(1, KLbsQualityProfileMeasurementInitialFixTime, timeValue);
		TESTL(err == KErrNone);
		TESTL(timeValue == TTimeIntervalMicroSeconds(15000000));

		err = profileApi->Get(1, KLbsQualityProfileMeasurementIntermediateFixTime, timeValue);
		TESTL(err == KErrNone);
		TESTL(timeValue == TTimeIntervalMicroSeconds(5000000));
				
		CleanupStack::PopAndDestroy(profileApi);

		SetTestStepResult(EPass);
		}
		
		LbsQualityProfile::ShutDownL();
		
		return TestStepResult();
	}



TVerdict CQualityProfileApi_ProfileReadStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
