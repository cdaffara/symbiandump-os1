/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "ctlbssimulationpsypostp300.h"

//TimeOut for normal requests 3 seconds.
const TInt KTimeOut = 3000000;
//TimeOut for parsing very long file 10 seconds.
const TInt KLongTimeOut = 10000000;
// Number of tests in this file, increment when a new test is added
const TInt KNrOfTests = 9;

_LIT(KName0, "TP300-0 - MultipleClientRequestsSps");
_LIT(KName1, "TP300-1 - MultipleClientRequestsNmea");
_LIT(KName2, "TP300-2 - MultipleClientCancellingSps");
_LIT(KName3, "TP300-3 - MultipleClientCancellingNmea");
_LIT(KName4, "TP300-4 - MultipleClientFixPowerUpTimeSps");
_LIT(KName5, "TP300-5 - PartialUpdateSps");
_LIT(KName6, "TP300-6 - AltitudeWraparoundSps");
_LIT(KName7, "TP300-7 - InvalidFieldsInSpsFile");
_LIT(KName8, "TP300-8 - NmeaWithoutFix");
// Nmea raw data files
#ifdef __WINS__
_LIT(KMagellanTracker, "z:\\system\\test\\testdata\\MagellanTracker.nme");
_LIT(KGarmin3plus, "z:\\system\\test\\testdata\\garmin3plus.nme");
#else
_LIT(KMagellanTracker, "c:\\system\\test\\testdata\\MagellanTracker.nme");
_LIT(KGarmin3plus, "c:\\system\\test\\testdata\\garmin3plus.nme");
#endif

_LIT(KService, "service");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsSimulationPsyPosTp300::CT_LbsSimulationPsyPosTp300(CT_LbsServer& aParent, TInt aIndex) :
	CT_LbsPortedStepBase(aParent)
	{
	SetTestStepName(GetIndexName(aIndex));
	iIndex = aIndex;
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPsyPosTp300::~CT_LbsSimulationPsyPosTp300()
	{
	}

// ---------------------------------------------------------
// CT_LbsSimulationPsyPosTp300::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPsyPosTp300::StartL()
	{
	// Setup test environment
	iNrOfRequests = 0;
	ConnectL();
	SetupPsyL(iUidSimulationPsy);
	RunTestsL(iIndex);
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp300::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPsyPosTp300::CloseTest()
	{
	ClosePositioner();
	Disconnect();
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp300::GetIndexName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CT_LbsSimulationPsyPosTp300::GetIndexName(TInt aIndex) const
	{
	if (aIndex > KNrOfTests)
		{
		User::Leave(KErrGeneral);
		}
	
	const TDesC* names[KNrOfTests] =
		{
				&KName0,
				&KName1,
				&KName2,
				&KName3,
				&KName4,
				&KName5,
				&KName6,
				&KName7,
				&KName8
		};
	return *names[aIndex];
	}

void CT_LbsSimulationPsyPosTp300::RunTestsL(TInt aIndex)
	{
	_LIT(KStart, "Testing MultipleClientRequestsSps() PSY ");
	_LIT(KStart1, "Testing MultipleClientRequestsNmea() PSY ");
	_LIT(KStart2, "Testing MultipleClientCancellingSps() PSY ");
	_LIT(KStart3, "Testing MultipleClientCancellingNmea() PSY ");
	_LIT(KStart4, "Testing MultipleClientFixPowerUpTimeSps() PSY ");
	_LIT(KStart5, "Testing SpsPartialUpdateOptionL() PSY ");
	_LIT(KStart6, "Testing LatitudeWrapAroundL() PSY ");
	_LIT(KStart7, "Testing InvalidSpsFileL() PSY ");
	_LIT(KStart8, "Testing NmeaNoFixL() PSY ");
	_LIT(KEnd, "MultipleClientRequestsSps() passed");
	_LIT(KEnd1, "MultipleClientRequestsNmea() passed");
	_LIT(KEnd2, "MultipleClientCancellingSps() passed");
	_LIT(KEnd3, "MultipleClientCancellingNmea() passed");
	_LIT(KEnd4, "MultipleClientFixPowerUpTimeSps() passed");
	_LIT(KEnd5, "SpsPartialUpdateOptionL() passed");
	_LIT(KEnd6, "LatitudeWrapAroundL() passed");
	_LIT(KEnd7, "InvalidSpsFileL() passed");
	_LIT(KEnd8, "NmeaNoFixL() passed");

	switch (aIndex)
		{
		case 0:
			INFO_PRINTF1(KStart);
			TestMultiplyClientAccessL(KNullDesC);
			INFO_PRINTF1(KEnd);
			break;
		case 1:
			INFO_PRINTF1(KStart1);
			TestMultiplyClientAccessL(KMagellanTracker);
			INFO_PRINTF1(KEnd1);
			break;
		case 2:
			INFO_PRINTF1(KStart2);
			TestMultiplyClientAccessL(KNullDesC, ETrue);
			INFO_PRINTF1(KEnd2);
			break;
		case 3:
			INFO_PRINTF1(KStart3);
			TestMultiplyClientAccessL(KGarmin3plus, ETrue);
			INFO_PRINTF1(KEnd3);
			break;
		case 4:
			INFO_PRINTF1(KStart4);
			TestMultiplyClientDataL();
			INFO_PRINTF1(KEnd4);
			break;
		case 5:
			INFO_PRINTF1(KStart5);
			TestSpsPartialUpdateOptionL();
			INFO_PRINTF1(KEnd5);
			break;
		case 6:
			INFO_PRINTF1(KStart6);
			TestLatitudeWrapAroundL();
			INFO_PRINTF1(KEnd6);
			break;
		case 7:
			INFO_PRINTF1(KStart7);
			TestInvalidSpsFileL();
			INFO_PRINTF1(KEnd7);
			break;
		case 8:
			INFO_PRINTF1(KStart8);
			TestNmeaNoFixL();
			INFO_PRINTF1(KEnd8);
			break;
		default:
			User::Leave(KErrArgument);
		}
	}

// ================= ACTUAL TEST FUNCTIONS =======================

// Method either uses simulationMoveNoError.sps or a specified file, which MUST be NMEA 
void CT_LbsSimulationPsyPosTp300::TestMultiplyClientAccessL(const TDesC& aFileName,
		const TBool aCancelLastRequest)
	{
	_LIT(KError1, "Error when requesting location");
	_LIT(KError2, "Error. Client 1 and Client 2 have different fixes.");
	const TInt KTimeToWaitBetweenRequests = 2000000; //time in microseconds
	const TReal KMaxDev = 0.0026;
	// 10 is enough to check if fixes are correct 
	const TInt KNrOfFixes = 10;
#ifdef __WINS__
	_LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationMoveNoError.sps");
#else
	_LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationMoveNoError.sps");
#endif

	// Set data file
	if (aFileName == KNullDesC)
		{
		SetSimDataFileL(KSimulationSource);
		}
	else
		{
		SetSimDataFileL(aFileName);
		}

	INFO_PRINTF1(_L("Opening Positioner"));
	User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

	RPositioner secondPositioner;
	RArray<TPositionInfo> posArray;
	CleanupClosePushL(posArray);
	TInt err = KErrNone;
	// save preceding values
	TTime timeOfFix = 0;
	TPosition myPos = TPosition();
	TPosition myPos2 = TPosition();

	for (TInt i = 0; i <= KNrOfFixes; i++)
		{
		// if using default SPS file, wait for position to change
		if (aFileName != KNullDesC)
			{
			User::After(KTimeToWaitBetweenRequests);
			}
		
		if (aCancelLastRequest && i == KNrOfFixes)
			{
			INFO_PRINTF1(_L("Next request will cancel"));
			PerformRequestL(KService, CRequestor::ERequestorService,
					CRequestor::EFormatApplication);
			//iNrOfRequests is modified by base class we are using it
			//to cancel last made request
			iPositioner.CancelRequest(iNrOfRequests);
			}
		else
			{
			TPositionInfo posInf;
			err = PerformSyncRequest(KService, &posInf);
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error on getting fix = %d"), err);
				LogErrorAndLeaveL(KError1);
				}
			posArray.AppendL(posInf);
			}

		}
	INFO_PRINTF1(_L("Opening second positioner"));
	err = secondPositioner.Open(iPosServer, iUidSimulationPsy);
	User::LeaveIfError(err);
	CleanupClosePushL(secondPositioner);
	err = secondPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KService);
	if (err != KErrNone)
		{
		_LIT(KErrRequestor, "Error when setting requestor.");
		LogErrorAndLeaveL(KErrRequestor, err);
		}
	
	for (TInt i = 0; i <= KNrOfFixes && i < posArray.Count(); i++)
		{
		TPositionInfo posInfo;
		// if using default SPS file, wait for position to change	
		TRequestStatus stat;
		if (aFileName != KNullDesC)
			{
			User::After(KTimeToWaitBetweenRequests);
			}
		secondPositioner.NotifyPositionUpdate(posInfo, stat);
		User::WaitForRequest(stat);
		if (err != KErrNone)
			{
			LogErrorAndLeaveL(KError1);
			}
		posInfo.GetPosition(myPos);
		posArray[i].GetPosition(myPos2);

		//Note that in SPS mode if test machine meet performance difficulties these deviations may be to large.
		TReal lonDeviation = Abs(myPos.Longitude() - myPos2.Longitude());
		TReal latDeviation = Abs(myPos.Latitude() - myPos2.Latitude());
		
		if (aFileName == KNullDesC)//SPS mode, insignificant deviations are unavoidable.
			{
			if (lonDeviation > KMaxDev || latDeviation > KMaxDev)
				{
				INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), lonDeviation, latDeviation);
				LogErrorAndLeaveL(KError2);
				}
			}
		else //NMEA mode, no deviations.
			{
			if (lonDeviation != 0 || latDeviation != 0)
				{
				INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), lonDeviation, latDeviation);
				LogErrorAndLeaveL(KError2);
				}
			}
		}
	
	CleanupStack::PopAndDestroy(); //secondPositioner
	CleanupStack::PopAndDestroy(); //posArray
	}

void CT_LbsSimulationPsyPosTp300::TestMultiplyClientDataL()
	{
	_LIT(KError1, "Error when requesting location");
	_LIT(KError2, "Error. Client 1 and Client 2 has different fix times.");
	// 10 is enough to check if fixes are correct 
	const TInt KNrOfFixes = 10;
	// time difference can be caused by test machine delay
	const TInt64 KMaxTimeDiff = 500000; //25%
	const TInt64 KMaxPowerUpDiff = 1250000; //25%
#ifdef __WINS__
	_LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationMoveNoError.sps");
#else
	_LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationMoveNoError.sps");
#endif

	// Set data file
	SetSimDataFileL(KSimulationSource);

	INFO_PRINTF1(_L("Opening Positioner"));
	User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

	RArray<TTimeIntervalMicroSeconds> timeArray;
	CleanupClosePushL(timeArray);
	TInt err = KErrNone;
	// save preceding values
	TTime requestStart;
	TTime requestFinished;
	TTimeIntervalMicroSeconds requestTime;

	INFO_PRINTF1(_L("Start requesting fixes"));
	for (TInt i = 0; i <= KNrOfFixes; i++)
		{
		// get new position info
		TPositionInfo posInfo;
		requestStart.UniversalTime();
		err = PerformSyncRequest(KService, &posInfo);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error on getting fix = %d"), err);
			LogErrorAndLeaveL(KError1);
			}
		requestFinished.UniversalTime();
		requestTime = requestFinished.MicroSecondsFrom(requestStart);
		timeArray.AppendL(requestTime);
		}

	INFO_PRINTF1(_L("Opening second positioner"));
	RPositioner secondPositioner;
	err = secondPositioner.Open(iPosServer, iUidSimulationPsy);
	User::LeaveIfError(err);
	CleanupClosePushL(secondPositioner);
	err = secondPositioner.SetRequestor(CRequestor::ERequestorService, CRequestor::EFormatApplication, KService);
	if (err != KErrNone)
		{
		_LIT(KErrRequestor, "Error when setting requestor.");
		LogErrorAndLeaveL(KErrRequestor, err);
		}

	for (TInt i = 0; i <= KNrOfFixes; i++)
		{
		TPositionInfo posInfo;
		// get new position info
		TRequestStatus stat;
		requestStart.UniversalTime();
		secondPositioner.NotifyPositionUpdate(posInfo, stat);
		User::WaitForRequest(stat);
		if (err != KErrNone)
			{
			LogErrorAndLeaveL(KError1);
			}
		requestFinished.UniversalTime();
		requestTime = requestFinished.MicroSecondsFrom(requestStart);
		if (i == 0 && (Abs(timeArray[i].Int64() - requestTime.Int64()) > KMaxPowerUpDiff))
			{
			INFO_PRINTF2(_L("Powerup time difference of %d request is to large!"), i);
			INFO_PRINTF2(_L("Time diff = %LD"), Abs(timeArray[i].Int64() - requestTime.Int64()));
			LogErrorAndLeaveL(KError2);
			}
		else if (Abs(timeArray[i].Int64() - requestTime.Int64()) > KMaxTimeDiff)
			{
			INFO_PRINTF2(_L("Time difference of %d request is to large!"), i);
			INFO_PRINTF2(_L("Time diff = %LD"), Abs(timeArray[i].Int64() - requestTime.Int64()));
			LogErrorAndLeaveL(KError2);
			}
		}

	CleanupStack::PopAndDestroy(); //secondPositioner
	CleanupStack::PopAndDestroy(); //timeArray

	}

void CT_LbsSimulationPsyPosTp300::TestSpsPartialUpdateOptionL()
	{
	_LIT(KPartialRequest, "Sending SPS Partial Request.");
	_LIT(KFixReturned, "Complete Fix Returned.");
	_LIT(KError, "Unexpected Error occurred: %d");
	_LIT(KModuleStatusError, "Error when requesting Module Status: %d");
	_LIT(KErrorPartialUpdate, "Error, Returned Fix is not complete.");
	_LIT(KQualityError, "Module Data Quality To Low : %d (See: TDataQualityStatus values)");
#ifdef __WINS__
	_LIT(KSpsDataFile, "z:\\system\\test\\testdata\\simulationdata.sps");
#else
	_LIT(KSpsDataFile, "c:\\system\\test\\testdata\\simulationdata.sps");
#endif

	INFO_PRINTF1(KPartialRequest);
	
	//Setting SPS data file, and performing a request.
	OpenNewSourceFileL(KSpsDataFile);
	SetPartialUpdateL(ETrue);
	TPositionInfo fix;
	TInt err = PerformSyncRequest(KService, &fix);
	AssertTrueL(err == KErrNone, KError, err);

	//The fix should not be partial.
	if (IsPositionInfoPartial(fix))
		{
		LogErrorAndLeaveL(KErrorPartialUpdate);
		}

	//Get Module Status.
	TPositionModuleStatus moduleStatus;
	err = iPosServer.GetModuleStatus(moduleStatus, iUidSimulationPsy);
	AssertTrueL(err == KErrNone, KModuleStatusError, err);

	//Check DataQualityStatus, should be EDataQualityNormal 
	//but for partial updates it is EDataQualityPartial.
	AssertTrueL(moduleStatus.DataQualityStatus()
			== TPositionModuleStatus::EDataQualityNormal, KQualityError,
			moduleStatus.DataQualityStatus());

	INFO_PRINTF1(KFixReturned);
	}

TBool CT_LbsSimulationPsyPosTp300::TestLatitudeWrapAround_HelperL(const TPositionInfo& aPosInfo, const TInt aCaseSelector, const TBool aLastIteration)
	{
	const TReal KDeltaLatitude = 0.01;
	const TReal KMaxLatitude = 90.0;
	_LIT(KError2, "Requested comparison not found");
	
	TPosition myPos;
	aPosInfo.GetPosition(myPos);
	TReal latitude = myPos.Latitude();
	TReal longtitude = myPos.Longitude();
	
	switch (aCaseSelector)
		{
		// crossing north pole
		case 0:
			if (latitude > KMaxLatitude)
				{
				_LIT(KError1, "Invalid latitude. Wraparound did not work properly on N pole (case 0)");
				LogErrorAndLeaveL(KError1);
				}

			return (longtitude < 0 && latitude <= KMaxLatitude);
			
		case 1:
			// crossing south pole
			if (latitude < -KMaxLatitude)
				{
				_LIT(KError1, "Invalid latitude. Wraparound did not work properly on S pole (case 1)");
				LogErrorAndLeaveL(KError1);
				}
			return (longtitude < 0 && latitude >= -KMaxLatitude);
			
		case 2:
			// going to southern hemisphere from northern one
			if (aLastIteration && latitude > 0)
				{
				_LIT(KError1, "Invalid latitude. Wraparound did not work properly going from N to S (case 2)");
				LogErrorAndLeaveL(KError1);
				}

			return (latitude < -KDeltaLatitude && latitude < 0);
			
		case 3:
			// going to northern hemisphere from southern one
			if (aLastIteration && latitude < 0)
				{
				_LIT(KError1, "Invalid latitude. Wraparound did not work properly while going from S to N (case 3)");
				LogErrorAndLeaveL(KError1);
				}

			return (latitude > KDeltaLatitude && latitude > 0);
			
		default:
			LogErrorAndLeaveL(KError2);
			break;
		}
	return EFalse;
	}

void CT_LbsSimulationPsyPosTp300::TestLatitudeWrapAroundL()
	{
	_LIT(KError1, "Error when requesting location");

	// object shall cross the line in no more than 10 iterations, otherwise there is an error 
	const TInt KMaxNrOfRequests = 10;
	const TInt KNrOfSimFiles = 4;
#ifdef __WINS__
	_LIT(KSimulationSource1, "z:\\system\\test\\testdata\\simulationLatWrapAround1.sps");
	_LIT(KSimulationSource2, "z:\\system\\test\\testdata\\simulationLatWrapAround2.sps");
	_LIT(KSimulationSource3, "z:\\system\\test\\testdata\\simulationLatWrapAround3.sps");
	_LIT(KSimulationSource4, "z:\\system\\test\\testdata\\simulationLatWrapAround4.sps");
#else
	_LIT(KSimulationSource1, "c:\\system\\test\\testdata\\simulationLatWrapAround1.sps");
	_LIT(KSimulationSource2, "c:\\system\\test\\testdata\\simulationLatWrapAround2.sps");
	_LIT(KSimulationSource3, "c:\\system\\test\\testdata\\simulationLatWrapAround3.sps");
	_LIT(KSimulationSource4, "c:\\system\\test\\testdata\\simulationLatWrapAround4.sps");
#endif

	const TDesC* SPSSimulationFiles[KNrOfSimFiles] =
		{
		&KSimulationSource1,
		&KSimulationSource2,
		&KSimulationSource3,
		&KSimulationSource4
		};
	for(TInt loopCount = 0; loopCount < KNrOfSimFiles; loopCount++)
		{
		// Set data file
		SetSimDataFileL(*SPSSimulationFiles[loopCount]);
		
		// Open simulationPSY
		User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
		
		TPositionInfo posInfo;
		for (TInt i = 0; i < KMaxNrOfRequests; i++)
			{
			// get new position info
			TBool lastIteration = (KMaxNrOfRequests-i == 1);
			
			TInt err = PerformSyncRequest(KService, &posInfo);
			if (err != KErrNone)
				{
				LogErrorAndLeaveL(KError1);
				}
			
			if (TestLatitudeWrapAround_HelperL(posInfo, loopCount, lastIteration))
				{
				// Latitude was wrapped around correctly. No need to continue with 'for' loop
				break;
				}	
			}
		ClosePositioner();
		}
	}

void CT_LbsSimulationPsyPosTp300::TestInvalidSpsFileL()
	{
#ifdef __WINS__
	_LIT(KSimulationBadInputFile0, "z:\\system\\test\\testdata\\simulationInvalidFields0.sps");
	_LIT(KSimulationBadInputFile1, "z:\\system\\test\\testdata\\simulationInvalidFields1.sps");
	_LIT(KSimulationBadInputFile2, "z:\\system\\test\\testdata\\simulationInvalidFields2.sps");
	_LIT(KSimulationBadInputFile3, "z:\\system\\test\\testdata\\simulationInvalidFields3.sps");
	_LIT(KSimulationBadInputFile4, "z:\\system\\test\\testdata\\simulationInvalidFields4.sps");
	_LIT(KSimulationBadInputFile5, "z:\\system\\test\\testdata\\simulationInvalidFields5.sps");
	_LIT(KSimulationBadInputFile6, "z:\\system\\test\\testdata\\simulationInvalidFields6.sps");
	_LIT(KSimulationBadInputFile7, "z:\\system\\test\\testdata\\simulationInvalidFields7.sps");
	_LIT(KSimulationBadInputFile8, "z:\\system\\test\\testdata\\simulationInvalidFields8.sps");
	_LIT(KSimulationBadInputFile9, "z:\\system\\test\\testdata\\simulationInvalidFields9.sps");
#else
	_LIT(KSimulationBadInputFile0, "c:\\system\\test\\testdata\\simulationInvalidFields0.sps");
	_LIT(KSimulationBadInputFile1, "c:\\system\\test\\testdata\\simulationInvalidFields1.sps");
	_LIT(KSimulationBadInputFile2, "c:\\system\\test\\testdata\\simulationInvalidFields2.sps");
	_LIT(KSimulationBadInputFile3, "c:\\system\\test\\testdata\\simulationInvalidFields3.sps");
	_LIT(KSimulationBadInputFile4, "c:\\system\\test\\testdata\\simulationInvalidFields4.sps");
	_LIT(KSimulationBadInputFile5, "c:\\system\\test\\testdata\\simulationInvalidFields5.sps");
	_LIT(KSimulationBadInputFile6, "c:\\system\\test\\testdata\\simulationInvalidFields6.sps");
	_LIT(KSimulationBadInputFile7, "c:\\system\\test\\testdata\\simulationInvalidFields7.sps");
	_LIT(KSimulationBadInputFile8, "c:\\system\\test\\testdata\\simulationInvalidFields8.sps");
	_LIT(KSimulationBadInputFile9, "c:\\system\\test\\testdata\\simulationInvalidFields9.sps");
#endif

	CheckBadInputFileL(KSimulationBadInputFile0);
	CheckBadInputFileL(KSimulationBadInputFile1);
	CheckBadInputFileL(KSimulationBadInputFile2);
	CheckBadInputFileL(KSimulationBadInputFile3);
	CheckBadInputFileL(KSimulationBadInputFile4);
	CheckBadInputFileL(KSimulationBadInputFile5);
	CheckBadInputFileL(KSimulationBadInputFile6);
	CheckBadInputFileL(KSimulationBadInputFile7);
	CheckBadInputFileL(KSimulationBadInputFile8);
	CheckBadInputFileL(KSimulationBadInputFile9);
	}

void CT_LbsSimulationPsyPosTp300::CheckBadInputFileL(const TDesC& aSimulationSource)
	{ 
	//Set data file
	SetSimDataFileL(aSimulationSource);
	
	TInt err = OpenPositionerByName(iUidSimulationPsy);

	if (err == KErrCorrupt)
		{
		_LIT(iMsg, "SPS file corrupt, this is the expected resulted");
		INFO_PRINTF1(iMsg); 
		} 
	else if (err == KErrNone)
		{
		_LIT(KErr, "The simulation data file was unexpectedly successfully read");
		LogErrorAndLeaveL(KErr);
		}
	else
		{
		_LIT(KErr1, "Unexpected error");
		LogErrorAndLeaveL(KErr1);
		}
	}

void CT_LbsSimulationPsyPosTp300::TestNmeaNoFixL()
	{
	//Literals for NMEA file without fix	
	_LIT(KStart1Msg, "Geting Fix from file with no fixes.");
	_LIT(KNoFixReturned, "No Fix returned, Leave code: %d");
	_LIT(KErrorFixFromFileWithNoFix, "Error: %d, Fix return form file without fixes.");
#ifdef __WINS__
	_LIT(KNoFixNmeaFile, "z:\\system\\test\\testdata\\tp300noFixFile.nme");
#else
	_LIT(KNoFixNmeaFile, "c:\\system\\test\\testdata\\tp300noFixFile.nme");
#endif
	
	//Literals for Long NMEA file
	_LIT(KStart2Msg, "Geting Fix from very long file.");
	_LIT(KGotFix, "Fix returned.");
	_LIT(KErrorNoFixFromLongFile, "Error: %d, Fix not returned form very long file.");
#ifdef __WINS__
	_LIT(KLongNmeaFile, "z:\\system\\test\\testdata\\tp300longFile.nme");
#else
	_LIT(KLongNmeaFile, "c:\\system\\test\\testdata\\tp300longFile.nme");
#endif

	INFO_PRINTF1(KStart1Msg);
	
	//Set NEMA data file without fix and get position fix.
	OpenNewSourceFileL(KNoFixNmeaFile);
	SetUpdateTimeOutL(KTimeOut);
	TPositionInfo fix;
	TInt err = PerformSyncRequest(KService, &fix);
	AssertTrueL(err != KErrNone, KErrorFixFromFileWithNoFix, err);

	INFO_PRINTF2(KNoFixReturned, err);
	
	ClosePositioner();
	
	INFO_PRINTF1(KStart2Msg);
	
	//Set long NEMA data file with fix on the end and get possition fix.
	OpenNewSourceFileL(KLongNmeaFile);
	SetUpdateTimeOutL(KLongTimeOut);
	fix = TPositionInfo();
	err = PerformSyncRequest(KService, &fix);
	AssertTrueL(err == KErrNone, KErrorNoFixFromLongFile, err);

	INFO_PRINTF1(KGotFix);
	}

// ---------------------------------------------------------
// CT_LbsSimulationPsyPosTp300::SetUpdateTimeOutL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPsyPosTp300::SetUpdateTimeOutL(const TInt aInterval)
	{
	_LIT(KGetUpdateOptionsErr, "Error %d when getting update options.");
	_LIT(KSetUpdateOptionsErr, "Error %d when setting update options");
	TPositionUpdateOptions posOption;
	TInt err = iPositioner.GetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KGetUpdateOptionsErr, err);
	posOption.SetUpdateTimeOut(TTimeIntervalMicroSeconds(aInterval));
	err = iPositioner.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);
	}

// ---------------------------------------------------------
// CT_LbsSimulationPsyPosTp300::OpenNewSourceFileL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPsyPosTp300::OpenNewSourceFileL(const TDesC& aName)
	{
	_LIT(KOpenPositionerErr, "Error when opening positioner,  %d");
	_LIT(KSettingRequestorErr, "Error when setting requestor,  %d");
	TInt err = KErrNone;
	SetSimDataFileL(aName);
	err = OpenPositionerByName( iUidSimulationPsy);
	AssertTrueL(err == KErrNone, KOpenPositionerErr, err);
	err = iPositioner.SetRequestor(CRequestor::ERequestorService,
			CRequestor::EFormatApplication, KService);
	AssertTrueL(err == KErrNone, KSettingRequestorErr, err);
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::SetPartialUpdateL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPsyPosTp300::SetPartialUpdateL(TBool aPartial)
	{
	_LIT(KGetUpdateOptionsErr, "Error %d when getting update options.");
	_LIT(KSetUpdateOptionsErr, "Error %d when setting update options.");
	
	TPositionUpdateOptions posOption;
	TInt err = iPositioner.GetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KGetUpdateOptionsErr, err);
	posOption.SetAcceptPartialUpdates(aPartial);
	err = iPositioner.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);
	}

// ---------------------------------------------------------
// CT_LbsSimulationPsyPosTp300::IsPositionInfoPartialL()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsSimulationPsyPosTp300::IsPositionInfoPartial(const TPositionInfo& aPosInfo)
	{
	TBool partialFix = EFalse;
	TPosition pos;
	aPosInfo.GetPosition(pos);

	if(Math::IsNaN(pos.Longitude()))
		{
		partialFix = ETrue;
		}
	else if(Math::IsNaN(pos.Latitude()))
		{
		partialFix = ETrue;
		}
	else if(pos.Time().Int64() == 0)
		{
		partialFix = ETrue;
		}
	else if(Math::IsNaN(pos.Altitude()))
		{
		partialFix = ETrue;
		}
	else if(Math::IsNaN(pos.VerticalAccuracy()))
		{
		partialFix = ETrue;
		}
	else if(Math::IsNaN(pos.HorizontalAccuracy()))
		{
		partialFix = ETrue;
		}

	return partialFix;
	}

