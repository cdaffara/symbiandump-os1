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


#include "ctlbssimulationpsypostp500.h"
#include "simulationpsyinternalcrkeys.h"

// Number of tests in this file, increment when a new test is added
const TInt KNrOfTests = 8;
// Small variances in the timing can cause the returned position to be slightly different; 
// This constant compensates for that difference. 
const TReal KMaxDev = 0.00005;
// Period of wait between requests
const TInt KTimeToWaitBetweenRequests = 2000000; //time in microseconds


_LIT(KName0, "TP500-0 - CommonDataSequentialAccessNmea");
_LIT(KName1, "TP500-1 - CommonDataSimultaneousAccessNmea");
_LIT(KName2, "TP500-2 - CommonDataSequentialAccessSps");
_LIT(KName3, "TP500-3 - CommonDataSimultaneousAccessSps");
_LIT(KName4, "TP300-4 - ChangeDataSetToIndependent");
_LIT(KName5, "TP300-5 - ChangeDataSetToCommon");
_LIT(KName6, "TP300-6 - MultipleRequestPartialUpdateNmea");
_LIT(KName7, "TP300-7 - PartialUpdateNmea");
// Data files
#ifdef __WINS__
_LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationMoveNoError.sps");
_LIT(KMagellanTracker, "z:\\system\\test\\testdata\\MagellanTracker.nme");
#else
_LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationMoveNoError.sps");
_LIT(KMagellanTracker, "c:\\system\\test\\testdata\\MagellanTracker.nme");
#endif

_LIT(KService, "service");

CT_LbsSimulationPsyPosTp500::CT_LbsSimulationPsyPosTp500(CT_LbsServer& aParent,
		TInt aIndex) :
	CT_LbsPortedStepBase(aParent)
	{
	iIndex = aIndex;
	}

void CT_LbsSimulationPsyPosTp500::StartL()
	{
	// Setup test environment
	SetTestStepName(GetIndexNameL(iIndex));
	iNrOfRequests = 0;
	ConnectL();
	SetupPsyL(iUidSimulationPsy);
	RunTestsL(iIndex);
	}

/*
 * From CT_LbsPortedStepBase
 * Cleans up after a test.
 * Always called, even if the test leaves.
 */
void CT_LbsSimulationPsyPosTp500::CloseTest()
	{
	SetCommonDataSetL(iOldTrpMode);
	ClosePositioner();
	Disconnect();
	}

/*
 * Gets the name of an individual test index
 * @param aIndex Indicates which test index name that is asked for.
 */
TPtrC CT_LbsSimulationPsyPosTp500::GetIndexNameL(TInt aIndex) const
	{
	if (aIndex > KNrOfTests)
		{
		User::Leave(KErrGeneral);
		}

	const TDesC* names[KNrOfTests] =
		{
		&KName0, &KName1, &KName2, &KName3, &KName4, &KName5, &KName6, &KName7
		};
	return *names[aIndex];
	}

/*
 * Contains calls to tests itself. 
 * @param aIndex test number
 */
void CT_LbsSimulationPsyPosTp500::RunTestsL(TInt aIndex)
	{
	_LIT(KStart, "Testing CommonDataSequentialAccessNmea PSY ");
	_LIT(KStart1, "Testing CommonDataSimultaneousAccessNmea PSY ");
	_LIT(KStart2, "Testing CommonDataSequentialAccessSps PSY ");
	_LIT(KStart3, "Testing CommonDataSimultaneousAccessSps PSY ");
	_LIT(KStart4, "Testing ChangeDataSetToIndependent PSY ");
	_LIT(KStart5, "Testing ChangeDataSetToCommon PSY ");
	_LIT(KStart6, "Testing MultipleRequestPartialUpdateNmea PSY ");
	_LIT(KStart7, "Testing PartialUpdateNmea PSY ");
	_LIT(KEnd, "CommonDataSequentialAccessNmea passed");
	_LIT(KEnd1, "CommonDataSimultaneousAccessNmea passed");
	_LIT(KEnd2, "CommonDataSequentialAccessSps passed");
	_LIT(KEnd3, "CommonDataSimultaneousAccessSps passed");
	_LIT(KEnd4, "ChangeDataSetToIndependent passed");
	_LIT(KEnd5, "ChangeDataSetToCommon passed");
	_LIT(KEnd6, "MultipleRequestPartialUpdateNmea passed");
	_LIT(KEnd7, "PartialUpdateNmea passed");

	GetCommonDataSetL(iOldTrpMode);
	switch (aIndex)
		{
		case 0:
			INFO_PRINTF1(KStart);
			TestMultipleClientAccessL(ETrue);
			INFO_PRINTF1(KEnd);
			break;
		case 1:
			INFO_PRINTF1(KStart1);
			TestMultipleClientAccessL(ETrue, ETrue);
			INFO_PRINTF1(KEnd1);
			break;
		case 2:
			INFO_PRINTF1(KStart2);
			TestMultipleClientAccessL(EFalse);
			INFO_PRINTF1(KEnd2);
			break;
		case 3:
			INFO_PRINTF1(KStart3);
			TestMultipleClientAccessL(EFalse, ETrue);
			INFO_PRINTF1(KEnd3);
			break;
		case 4:
			INFO_PRINTF1(KStart4);
			TestMultipleClientChangeDataModeL(EFalse, ETrue);
			INFO_PRINTF1(KEnd4);
			break;
		case 5:
			INFO_PRINTF1(KStart5);
			TestMultipleClientChangeDataModeL(ETrue, EFalse);
			INFO_PRINTF1(KEnd5);
			break;
		case 6:
			INFO_PRINTF1(KStart6);
			TestMultipleClientPartialFixL();
			INFO_PRINTF1(KEnd6);
			break;
		case 7:
			INFO_PRINTF1(KStart7);
			TestNmeaPartialFixL();
			INFO_PRINTF1(KEnd7);
			break;
		default:
			User::Leave(KErrArgument);
		}
	}

void CT_LbsSimulationPsyPosTp500::TestMultipleClientAccessL(
		const TBool aIsNmea, const TBool aSimultaneousRequest,
		const TBool aCommonDataSet,
		const TBool aSwitchDataMode)
	{
	_LIT(KError1, "Error when requesting location");
	_LIT(KError2, "Time out error");
	_LIT(KSetUpdateOptionsErr, "Error %d when setting Max Update Age option.");
	_LIT(KGetUpdateOptionsErr, "Error %d when getting update options.");
	//Max allowed number of loop runs
	const TInt KMaxNumberOfTests = 50;

	// Set data file
	if (!aIsNmea)
		{
		SetSimDataFileL(KSimulationSource);
		}
	else
		{
		SetSimDataFileL(KMagellanTracker);
		}

	SetCommonDataSetL(aCommonDataSet); // set common data mode

	RPositioner secondPositioner;
	TInt err = KErrNone;

	TPositionInfo posInfo1;
	TPositionInfo posInfo2;

	OpenPositionersLC(secondPositioner);
	
	//setting max update age for 0, for iPositioner
	TPositionUpdateOptions posOption;
	TInt err2 = iPositioner.GetUpdateOptions(posOption);
	AssertTrueL(err2 == KErrNone, KGetUpdateOptionsErr, err2);
	posOption.SetMaxUpdateAge(0);
	err2 = iPositioner.SetUpdateOptions(posOption);
	AssertTrueL(err2 == KErrNone, KSetUpdateOptionsErr, err2);
	
	//setting max update age for 0, for second positioner
	err2 = secondPositioner.GetUpdateOptions(posOption);
	AssertTrueL(err2 == KErrNone, KGetUpdateOptionsErr, err2);
	posOption.SetMaxUpdateAge(0);
	err2 = secondPositioner.SetUpdateOptions(posOption);
	AssertTrueL(err2 == KErrNone, KSetUpdateOptionsErr, err2);

	// perform initial request by first positioner
	err = PerformSyncRequest(KService, &posInfo1);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error on getting fix = %d"), err);
		LogErrorAndLeaveL(KError1);
		}

	TRequestStatus stat;
	TPosition myPos1 = TPosition();
	TPosition myPos2 = TPosition();
	// perform two requests, by two different positioners
	if (aSimultaneousRequest)
		{
		// position should be the same only if request times are equal
		TInt loopIteration = 0;
		do	
			{
			PerformSimultaneousRequestsL(secondPositioner,
					posInfo1, posInfo2, KErrNone);
			posInfo2.GetPosition(myPos2);
			posInfo1.GetPosition(myPos1);
			if (loopIteration > KMaxNumberOfTests)
				{
				LogErrorAndLeaveL(KError2);
				}
			loopIteration++;
			}
		while (myPos1.Time() != myPos2.Time());
		}
	else
		{
		PerformStaggeredRequestsL(secondPositioner, aIsNmea, aSwitchDataMode,
					aCommonDataSet, posInfo1, posInfo2);
		posInfo2.GetPosition(myPos2);
		posInfo1.GetPosition(myPos1);
		}

	CheckPositionDataL(aIsNmea, aSimultaneousRequest, myPos1, myPos2);

	CleanupStack::PopAndDestroy(&secondPositioner);
	}

void CT_LbsSimulationPsyPosTp500::TestMultipleClientChangeDataModeL(
		const TBool aIsNmea, const TBool aCommonDataSet,
		const TBool aSwitchDataMode)
	{
	// Set data file
	if (!aIsNmea)
		{
		SetSimDataFileL(KSimulationSource);
		}
	else
		{
		SetSimDataFileL(KMagellanTracker);
		}

	SetCommonDataSetL(aCommonDataSet); // set common data mode

	RPositioner secondPositioner;

	TPositionInfo posInfo1;
	TPositionInfo posInfo2;

	OpenPositionersLC(secondPositioner);

	PerformStaggeredRequestsL(secondPositioner, aIsNmea, aSwitchDataMode,
			aCommonDataSet, posInfo1, posInfo2);

	TPosition myPos1 = TPosition();
	TPosition myPos2 = TPosition();
	
	posInfo2.GetPosition(myPos2);
	posInfo1.GetPosition(myPos1);

	//check output data
	CheckPositionDataL(aIsNmea, !aCommonDataSet, myPos1, myPos2);

	CleanupStack::PopAndDestroy(&secondPositioner);
	}

void CT_LbsSimulationPsyPosTp500::TestMultipleClientPartialFixL()
	{
	_LIT(KErrorPartialUpdate1, "Error, Returned Fix is not complete.");
	_LIT(KErrorPartialUpdate2, "Error, Returned Fix is complete.");
#ifdef __WINS__
	_LIT(KNmeaDataFile, "z:\\system\\test\\testdata\\tp300longfile.nme");
#else
	_LIT(KNmeaDataFile, "c:\\system\\test\\testdata\\tp300longfile.nme");
#endif

	//Setting NMEA data file, and performing a request.
	SetSimDataFileL(KNmeaDataFile);

	RPositioner secondPositioner;

	OpenPositionersLC(secondPositioner);

	SetPartialUpdateL(ETrue, iPositioner);

	TRequestStatus stat;
	TPositionInfo posInfo1;
	TPositionInfo posInfo2;
	//perform request for a full fix
	PerformSimultaneousRequestsL(secondPositioner, posInfo1, posInfo2, KPositionPartialUpdate);

	//The fix should not be partial.
	if (IsPositionInfoPartial(posInfo2))
		{
		LogErrorAndLeaveL(KErrorPartialUpdate1);
		}
	//The fix should be partial.
	if (!IsPositionInfoPartial(posInfo1))
		{
		LogErrorAndLeaveL(KErrorPartialUpdate2);
		}

	CleanupStack::PopAndDestroy(&secondPositioner);
	}

void CT_LbsSimulationPsyPosTp500::TestNmeaPartialFixL()
	{
	_LIT(KPartialRequest, "Sending NMEA Partial Request.");
	_LIT(KFixReturned, "Complete Fix Returned.");
	_LIT(KError, "Unexpected Error occurred: %d");
	_LIT(KModuleStatusError, "Error when requesting Module Status: %d");
	_LIT(KErrorPartialUpdate, "Error, Returned Fix is not complete.");
	_LIT(KQualityError, "Module Data Quality To Low : %d (See: TDataQualityStatus values)");
#ifdef __WINS__
	_LIT(KNmeaDataFile, "z:\\system\\test\\testdata\\fix10magellangps315.nme");
#else
	_LIT(KNmeaDataFile, "c:\\system\\test\\testdata\\fix10magellangps315.nme");
#endif

	INFO_PRINTF1(KPartialRequest);

	//Setting NMEA data file, and performing a request.
	SetSimDataFileL(KNmeaDataFile);
	INFO_PRINTF1(_L("Opening first positioner"));
	User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
	SetPartialUpdateL(ETrue, iPositioner);
	TPositionInfo posInfo;
	TInt err = PerformSyncRequest(KService, &posInfo);
	AssertTrueL(err == KErrNone, KError, err);

	//The fix should not be partial.
	if (IsPositionInfoPartial(posInfo))
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

/*
 * The method sets new Data Mode value and stores it in central repository.
 * @param aCommonDataSet value to be set in central repository.
 */
void CT_LbsSimulationPsyPosTp500::SetCommonDataSetL(const TBool aCommonDataSet) const
	{
	TInt commonDataSet;
	if (aCommonDataSet)
		{
		commonDataSet = 1; // Common Data Set is enabled
		}
	else
		{
		commonDataSet = 0;
		}
	// Set Common Data Set behaviour in the Central Repository
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	User::LeaveIfError(repository-> Set(KCRKeySimPSYCommonDataSet,
			commonDataSet));
	CleanupStack::PopAndDestroy(repository);
	}

/*
 * The method gets Data Mote value from central repository.
 * @param aCommonDataSet value read from central repository.
 */
void CT_LbsSimulationPsyPosTp500::GetCommonDataSetL(TInt& aTrpMode) const
	{
	// Get the Common Data Set setting from Central Repository
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	User::LeaveIfError(repository->Get(KCRKeySimPSYCommonDataSet, aTrpMode));
	CleanupStack::PopAndDestroy(repository);
	}

/*
 * The method opens two positioners. First (iPositioner) one is taken from the base class. 
 * They are expected to be constructed but not opened.
 * @param aPositioner additional positioner which needs to be opened. Requestor is set for it.
 * 			CleanupClosePushL() is used to push aPositioner onto the cleanup stack.
 */
void CT_LbsSimulationPsyPosTp500::OpenPositionersLC(RPositioner& aPositioner)
	{
	TInt err;

	INFO_PRINTF1(_L("Opening first positioner"));
	User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

	INFO_PRINTF1(_L("Opening second positioner"));
	err = aPositioner.Open(iPosServer, iUidSimulationPsy);
	User::LeaveIfError(err);
	CleanupClosePushL(aPositioner);
	err = aPositioner.SetRequestor(CRequestor::ERequestorService,
			CRequestor::EFormatApplication, KService);
	if (err != KErrNone)
		{
		_LIT(KErrRequestor, "Error when setting requestor.");
		LogErrorAndLeaveL(KErrRequestor, err);
		}
	}

/*
 * Allows Positioner to accept partial update. 
 * @param aPartial ETrue for partial updates allowed.
 * @param aPositioner positioner to accept partial update.
 */
void CT_LbsSimulationPsyPosTp500::SetPartialUpdateL(TBool aPartial,
		RPositioner& aPositioner)
	{
	_LIT(KGetUpdateOptionsErr, "Error %d when getting update options.");
	_LIT(KSetUpdateOptionsErr, "Error %d when setting update options.");

	TPositionUpdateOptions posOption;
	TInt err = aPositioner.GetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KGetUpdateOptionsErr, err);
	posOption.SetAcceptPartialUpdates(aPartial);
	err = aPositioner.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);
	}

/*
 * The method checks whether the position is a partial fix.
 * @param aPosInfo position information.
 * @return ETrue for partial position info. Otherwise EFalse.
 */
TBool CT_LbsSimulationPsyPosTp500::IsPositionInfoPartial(
		const TPositionInfo& aPosInfo)
	{
	TBool partialFix = EFalse;
	TPosition pos;
	aPosInfo.GetPosition(pos);

	if (Math::IsNaN(pos.Longitude()))
		{
		partialFix = ETrue;
		}
	else if (Math::IsNaN(pos.Latitude()))
		{
		partialFix = ETrue;
		}
	else if (pos.Time().Int64() == 0)
		{
		partialFix = ETrue;
		}
	else if (Math::IsNaN(pos.Altitude()))
		{
		partialFix = ETrue;
		}
	else if (Math::IsNaN(pos.VerticalAccuracy()))
		{
		partialFix = ETrue;
		}
	else if (Math::IsNaN(pos.HorizontalAccuracy()))
		{
		partialFix = ETrue;
		}

	return partialFix;
	}

/*
 * The method waits required amount of time
 * @param aNmea if it is NMEA test or SPS.
 * @param aTime time to wait.
 */
void CT_LbsSimulationPsyPosTp500::WaitForPositionChange(const TBool aNmea,
		const TInt aTime)
	{
	User::After(aTime);
	if (!aNmea) // sps file
		{
		
		}
	else // NMEA file
		{
		// Witold - czekamy na kod Lukasza
		}
	}

/*
 * The method checks requested position data
 * @param aNmea is it NMEA test or SPS.
 * @param aSimultaneousRequest are two requests made at the same time
 * @param aPos1 first position request information.
 * @param aPos2 second position request information.
 */
void CT_LbsSimulationPsyPosTp500::CheckPositionDataL(const TBool aNmea,
		const TBool aSimultaneousRequest, const TPosition& aPos1, const TPosition& aPos2)
	{
	_LIT(KError1, "Error. Client 1 and Client 2 have different fixes.");
	_LIT(KError2, "Error. Client 1 and Client 2 have the same fixes.");

	//Note that in SPS mode if test machine meet performance difficulties these deviations may be to large.
	TReal lonDeviation = Abs(aPos1.Longitude() - aPos2.Longitude());
	TReal latDeviation = Abs(aPos1.Latitude() - aPos2.Latitude());

	// SPS mode, two requests at the same time
	if (aSimultaneousRequest && !aNmea)
		{
		// Witold - logs, to be removed before final check in
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos1.Longitude(), aPos1.Latitude());
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos2.Longitude(), aPos2.Latitude());
		INFO_PRINTF1(_L("Check 1"));

		if (lonDeviation > KMaxDev || latDeviation > KMaxDev)
			{
			INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), lonDeviation, latDeviation);
			LogErrorAndLeaveL(KError1);
			}
		}
	// NMEA mode, two requests at the same time, no deviations
	else if (aSimultaneousRequest && aNmea)
		{
		// Witold - logs, to be removed before final check in
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos1.Longitude(), aPos1.Latitude());
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos2.Longitude(), aPos2.Latitude());
		INFO_PRINTF1(_L("Check 2"));

		if (lonDeviation != 0 || latDeviation != 0)
			{
			INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos2.Latitude(), aPos1.Latitude());
			LogErrorAndLeaveL(KError1);
			}
		}
	// SPS mode, one request after another
	else if (!aSimultaneousRequest && !aNmea) //SPS mode, insignificant deviations are unavoidable.
		{
		// Witold - logs, to be removed before final check in
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos1.Longitude(), aPos1.Latitude());
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos2.Longitude(), aPos2.Latitude());
		INFO_PRINTF1(_L("Check 3"));
	
		if (aPos2.Longitude() <= aPos1.Longitude() && aPos2.Latitude()
				<= aPos1.Latitude())
			{
			INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos2.Longitude(), aPos1.Longitude());
			LogErrorAndLeaveL(KError2);
			}
		}
	// NMEA mode, one request after another
	else if (!aSimultaneousRequest && aNmea)
		{
		// Witold - logs, to be removed before final check in
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos1.Longitude(), aPos1.Latitude());
		INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), aPos2.Longitude(), aPos2.Latitude());
		INFO_PRINTF1(_L("Check 4"));

		//NMEA mode 
		if (lonDeviation == 0 || latDeviation == 0)
			{
			INFO_PRINTF3(_L("Deviations: Lon=%10.7f, Lat=%10.7f"), lonDeviation, latDeviation);
			LogErrorAndLeaveL(KError2);
			}
		}
	}

/*
 * The method performs two staggered requests for two different positioners
 * @param aPositioner additional positioner sending request
 * @param aIsNmea ETrue if NMEA request
 * @param aSwitchDataMode ETrue if data mode needs to be changed
 * @param aCommonDataSet ETrue if data mode is Common
 * @param aPosInfo1 location fix for the first positioner
 * @param aPosInfo2 location second for the first positioner
 */
void CT_LbsSimulationPsyPosTp500::PerformStaggeredRequestsL(RPositioner& aPositioner,
			const TBool aIsNmea, const TBool aSwitchDataMode,
			const TBool aCommonDataSet, TPositionInfo& aPosInfo1, TPositionInfo& aPosInfo2)
	{
	_LIT(KError1, "Error when requesting location");
	TInt err;
	//INFO_PRINTF1(_L("Request after time"));
	err = PerformSyncRequest(KService, &aPosInfo1);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error on getting fix = %d"), err);
		LogErrorAndLeaveL(KError1);
		}

	// change common data mode if required
	if (aSwitchDataMode)
		{
		SetCommonDataSetL(!aCommonDataSet);
		}

	// wait for position to change
	WaitForPositionChange(aIsNmea, KTimeToWaitBetweenRequests);

	TRequestStatus stat;
	aPositioner.NotifyPositionUpdate(aPosInfo2, stat);
	User::WaitForRequest(stat);
	if (err != KErrNone)
		{
		LogErrorAndLeaveL(KError1);
		}
	}

/*
 * The method performs two simultaneous requests for two different positioners
 * @param aPositioner additional positioner sending request
 * @param aPosInfo1 location fix for the first positioner
 * @param aPosInfo2 location second for the first positioner
 * @param aError expected result of request action
 */
void CT_LbsSimulationPsyPosTp500::PerformSimultaneousRequestsL(RPositioner& aPositioner,
			TPositionInfo& aPosInfo1, TPositionInfo& aPosInfo2, const TInt aError)
	{
	_LIT(KError, "Error when requesting location");
	TRequestStatus stat;
	TInt err;
	INFO_PRINTF1(_L("Request in progress"));
	aPositioner.NotifyPositionUpdate(aPosInfo2, stat);
	err = PerformSyncRequest(KService, &aPosInfo1);
	if (err != aError)
		{
		INFO_PRINTF2(_L("Error on getting fix = %d"), err);
		LogErrorAndLeaveL(KError);
		}
	User::WaitForRequest(stat);
	}
