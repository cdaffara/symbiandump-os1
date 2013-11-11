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


#include "ctlbssimulationpsypostp600.h"
#include "simulationpsyinternalcrkeys.h"

const TInt KTrackingError = 2000000;						// 2 seconds
const TTimeIntervalMicroSeconds KClient1Interval(5000000);	// 5 seconds
const TTimeIntervalMicroSeconds KClient2Interval(10000000);	// 10 seconds

// Number of tests in this file. Increment when a new test is added.
const TInt KNrOfTests = 4;

_LIT(KName0, "TP600-0 - SPS Tracking (Independent Data Set)");
_LIT(KName1, "TP600-1 - NMEA Tracking (Common Data Set)");
_LIT(KName2, "TP600-2 - SPS Tracking (Independent Data Set)");
_LIT(KName3, "TP600-3 - NMEA Tracking (Common Data Set)");

// Data files
#ifdef __WINS__
_LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationMoveNoError.sps");
_LIT(KMagellanTracker, "z:\\system\\test\\testdata\\MagellanTracker.nme");
#else
_LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationMoveNoError.sps");
_LIT(KMagellanTracker, "c:\\system\\test\\testdata\\MagellanTracker.nme");
#endif

_LIT(KService, "service");

CT_LbsSimulationPsyPosTp600::CT_LbsSimulationPsyPosTp600(CT_LbsServer& aParent, TInt aIndex) :
	CT_LbsPortedStepBase(aParent)
	{
	iIndex = aIndex;
	}

void CT_LbsSimulationPsyPosTp600::StartL()
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
void CT_LbsSimulationPsyPosTp600::CloseTest()
	{
	ClosePositioner();
	Disconnect();
	}

TVerdict CT_LbsSimulationPsyPosTp600::doTestStepPostambleL()
	{
	SetCommonDataSetL(iOldTrpMode);
	return CT_LbsPortedStepBase::doTestStepPostambleL();
	}

/*
 * Gets the name of an individual test index
 * @param aIndex Indicates which test index name that is asked for.
 */
TPtrC CT_LbsSimulationPsyPosTp600::GetIndexNameL(TInt aIndex) const
	{
	ASSERT(aIndex < KNrOfTests);
	const TDesC* names[KNrOfTests] =
		{
		&KName0, &KName1, &KName2, &KName3
		};
	return *names[aIndex];
	}

/*
 * Contains calls to tests itself. 
 * @param aIndex test number
 */
void CT_LbsSimulationPsyPosTp600::RunTestsL(TInt aIndex)
	{
	GetCommonDataSetL(iOldTrpMode);
	switch (aIndex)
		{
		case 0:
			RunTestL(KSimulationSource, EFalse, &CT_LbsSimulationPsyPosTp600::CheckSPSTrackingWithIndependentDataSetL);
			break;
		case 1:
			RunTestL(KMagellanTracker, EFalse, &CT_LbsSimulationPsyPosTp600::CheckNMEATrackingWithIndependentDataSetL);
			break;
		case 2:
			RunTestL(KSimulationSource, ETrue, &CT_LbsSimulationPsyPosTp600::CheckSPSTrackingWithCommonDataSetL);
			break;
		case 3:
			RunTestL(KMagellanTracker, ETrue, &CT_LbsSimulationPsyPosTp600::CheckNMEATrackingWithCommonDataSetL);
			break;
					
		default:
			User::Leave(KErrArgument);
		}
	}


void CT_LbsSimulationPsyPosTp600::CheckSPSTrackingWithIndependentDataSetL(
		const TPositionRequest& aRequest1, 
		const TPositionRequest& aRequest2, 
		const TTime& aTimeCompleted,
		TInt aIndex)
	{
	TReal trackErr = 0.5; // possible tracking error 
	_LIT(KDistanceError, "Client 2 is further from the starting position than client 1.");

	CheckRequestTimeL(aRequest1.iStatus, aTimeCompleted.Int64() - aRequest1.iRequestTime.Int64(), aIndex == 0, KClient1Interval.Int64());
	CheckRequestTimeL(aRequest2.iStatus, aTimeCompleted.Int64() - aRequest2.iRequestTime.Int64(), aIndex == 1, KClient2Interval.Int64());
	
	// If client 1 returned a position, it should be further than client 2's last position
	if (aRequest1.iStatus != KRequestPending)
		{
		TReal32 distance1;
		TReal32 distance2;
		
		GetDistanceL(aRequest1, distance1);
		GetDistanceL(aRequest2, distance2);
		
		if (distance1+trackErr < distance2)
			{
			INFO_PRINTF3(_L("FAILED distance1 = %f; distance2 = %f;"), distance1, distance2);
			LogErrorAndLeaveL(KDistanceError);
			}
		}
	}
	
void CT_LbsSimulationPsyPosTp600::CheckNMEATrackingWithIndependentDataSetL(
		const TPositionRequest& aRequest1, 
		const TPositionRequest& aRequest2, 
		const TTime& aTimeCompleted,
		TInt aIndex)
	{
	CheckRequestTimeL(aRequest1.iStatus, aTimeCompleted.Int64() - aRequest1.iRequestTime.Int64(), aIndex == 0, KClient1Interval.Int64());
	CheckRequestTimeL(aRequest2.iStatus, aTimeCompleted.Int64() - aRequest2.iRequestTime.Int64(), aIndex == 1, KClient2Interval.Int64());
	
	const TCoordinate KFixes[] = 
		{ 
		TCoordinate(61.4470216666667, 23.8645783333333),		
		TCoordinate(61.4470350000000, 23.8645616666667),
		TCoordinate(61.4470383333333, 23.8645566666667),
		TCoordinate(61.4470666666667, 23.8645750000000),
		TCoordinate(61.4470855555556, 23.8645866666667),
		TCoordinate(61.4470916666667, 23.8645650000000),
		TCoordinate(61.4470950000000, 23.8645683333333)
		};
	
	// Client 1 should receive every fix from the file
	if (aRequest1.iStatus != KRequestPending)
		{
		static TInt numFixes;
		CheckNthFixL(aRequest1.iPositionInfo, KFixes, numFixes++);
		}

	// Client 2 should receive every fix from the file
	if (aRequest2.iStatus != KRequestPending)
		{
		static TInt numFixes;
		CheckNthFixL(aRequest2.iPositionInfo, KFixes, numFixes++);			
		}
	}

void CT_LbsSimulationPsyPosTp600::CheckSPSTrackingWithCommonDataSetL(
		const TPositionRequest& aRequest1, 
		const TPositionRequest& aRequest2, 
		const TTime& aTimeCompleted,
		TInt aIndex)
	{
	_LIT(KDistanceError1, "Client 1 should be further from the start than Client 2.");
	_LIT(KDistanceError2, "Client 2 should be further from the start than Client 1.");
		
	CheckRequestTimeL(aRequest1.iStatus, aTimeCompleted.Int64() - aRequest1.iRequestTime.Int64(), aIndex == 0, KClient1Interval.Int64());
	CheckRequestTimeL(aRequest2.iStatus, aTimeCompleted.Int64() - aRequest2.iRequestTime.Int64(), aIndex == 1, KClient2Interval.Int64());

	TReal32 distance1;
	TReal32 distance2;
	
	GetDistanceL(aRequest1, distance1);
	GetDistanceL(aRequest2, distance2);

	// If client 1 returned a position, it should be further than client 2's last position
	if (aRequest1.iStatus != KRequestPending
			&& distance1 < distance2)
		{
		LogErrorAndLeaveL(KDistanceError1);
		}
	
	// If client 2 returned a position, it should be further than client 1's last position
	if (aRequest2.iStatus != KRequestPending
			&& distance2 < distance1)
		{
		LogErrorAndLeaveL(KDistanceError2);
		}
	}
	
void CT_LbsSimulationPsyPosTp600::CheckNMEATrackingWithCommonDataSetL(
		const TPositionRequest& aRequest1, 
		const TPositionRequest& aRequest2, 
		const TTime& aTimeCompleted,
		TInt aIndex)
	{
	_LIT(KErrIncorrectFix, "Retrieved fixes are not the same.");
	
	CheckRequestTimeL(aRequest1.iStatus, aTimeCompleted.Int64() - aRequest1.iRequestTime.Int64(), aIndex == 0, KClient1Interval.Int64());
	CheckRequestTimeL(aRequest2.iStatus, aTimeCompleted.Int64() - aRequest2.iRequestTime.Int64(), aIndex == 1, KClient2Interval.Int64());
	
	if (aIndex > 0)
		{
		TPosition pos;
		TPosition pos2;
		TReal32 distance;
		if ( aRequest1.iStatus != KRequestPending && aRequest2.iStatus != KRequestPending )
			{
			aRequest1.iPositionInfo.GetPosition(pos);
			aRequest2.iPositionInfo.GetPosition(pos2);
			pos.Distance(pos2, distance);
			if (distance != 0)
				{
				LogErrorAndLeaveL(KErrIncorrectFix);
				}
			}
		}
	}

void CT_LbsSimulationPsyPosTp600::RunTestL(const TDesC& aDataFile, TBool aTrp, TCompareFunction aCheckL)
	{
	SetSimDataFileL(aDataFile);
	SetCommonDataSetL(aTrp);

	RPositioner secondPositioner;
	OpenPositionersLC(secondPositioner);

	// Set client 1 to track every 5 seconds
	TPositionUpdateOptions options(KClient1Interval);
	iPositioner.SetUpdateOptions(options);

	// Set client 2 to track every 10 seconds
	TPositionUpdateOptions options2(KClient2Interval);
	secondPositioner.SetUpdateOptions(options2);
	
	TTime initialTime;
	initialTime.UniversalTime();

		{
		TPositionRequest request1(iPositioner);
		TPositionRequest request2(secondPositioner);
		
		for (TInt i = 0; i < 10; i++)
			{
			request1.MakeRequestL();
			request2.MakeRequestL();
			
			User::WaitForRequest(request1.iStatus, request2.iStatus);
			
			TTime timeAfterRequest;
			timeAfterRequest.UniversalTime();
			
			// Check that requested positions meet test criteria
			(*this.*aCheckL)(request1, request2, timeAfterRequest, i);
			}
		}

	CleanupStack::PopAndDestroy(&secondPositioner);
	}

void CT_LbsSimulationPsyPosTp600::CheckNthFixL(const TPositionInfo& aPosInfo, const TCoordinate* aFixes, TInt aIndex)
	{
	_LIT(KErrIncorrectFix, "Retrieved fix more than 1 metre away from fix in data file.");
	
	TPosition pos;
	aPosInfo.GetPosition(pos);
	
	TReal32 distance;
	pos.Distance(aFixes[aIndex], distance);
	if (distance >= 1)
		{
		LogErrorAndLeaveL(KErrIncorrectFix);
		}
	}

void CT_LbsSimulationPsyPosTp600::GetDistanceL(const TRequestStatus& aStatus, const TPositionInfo& aPosInfo, TReal32& aDistance)
	{
	const TCoordinate KStartingPosition(78.3385, 56.3385);	
	
	if (aStatus != KRequestPending)
		{
		TPosition pos;
		aPosInfo.GetPosition(pos);
		pos.Distance(KStartingPosition, aDistance);
		}
	}

void CT_LbsSimulationPsyPosTp600::GetDistanceL(const TPositionRequest& aRequest, TReal32& aDistance)
	{
	GetDistanceL(aRequest.iStatus, aRequest.iPositionInfo, aDistance);
	}


void CT_LbsSimulationPsyPosTp600::CheckRequestTimeL(const TRequestStatus& aStatus, TInt aDelta, TBool aFirst, TInt aExpected)
	{
	_LIT(KError, "Tracking time incorrect");
	if (aStatus != KRequestPending &&
		!aFirst && 
		(aDelta > aExpected + KTrackingError))
		{
		LogErrorAndLeaveL(KError);
		}
	}

/*
 * The method sets new Data Mode value and stores it in central repository.
 * @param aCommonDataSet value to be set in central repository.
 */
void CT_LbsSimulationPsyPosTp600::SetCommonDataSetL(const TBool aCommonDataSet) const
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
void CT_LbsSimulationPsyPosTp600::GetCommonDataSetL(TInt& aTrpMode) const
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
void CT_LbsSimulationPsyPosTp600::OpenPositionersLC(RPositioner& aPositioner)
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

CT_LbsSimulationPsyPosTp600::TPositionRequest::TPositionRequest(RPositioner& aPositioner) :
	iPositioner(aPositioner)
	{
	}

CT_LbsSimulationPsyPosTp600::TPositionRequest::~TPositionRequest()
	{
	if (iStatus == KRequestPending)
		{
		iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
		}
	}

void CT_LbsSimulationPsyPosTp600::TPositionRequest::MakeRequestL()
	{
	if (iStatus != KRequestPending)
		{
		iPositioner.NotifyPositionUpdate(iPositionInfo, iStatus);
		iRequestTime.UniversalTime();
		}	
	}
