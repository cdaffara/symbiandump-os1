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


#include <lbs.h>
#include <lbsposition.h>
#include "ctlbssimulationpsypostp400.h"
#include "simulationpsyinternalcrkeys.h"

//Number of tests in this file, increment when a new test is added
const TInt KNrOfTests = 6;
const TInt K30seconds = 30000000;
const TInt K10seconds = 10000000;
const TInt K5seconds = 5000000;
const TInt K0seconds = 0;
// Value for Common Data Set to enable Time Relative Playback
const TInt KTrpModeEnable= 1;

_LIT(KName0, "TP400-0 - TrpMultiClientNmeaStoredFixExist");
_LIT(KName1, "TP400-1 - TrpMultiClientNmeaStoredFixNotExist");
_LIT(KName2, "TP400-2 - TrpMultiClientNmeaNoUpdateAge");
_LIT(KName3, "TP400-3 - TrpMultiClientSpsStoredFixExist");
_LIT(KName4, "TP400-4 - TrpMultiClientSpsStoredFixNotExist");
_LIT(KName5, "TP400-5 - TrpMultiClientSpsNoUpdateAge");

CT_LbsSimulationPsyPosTp400::CT_LbsSimulationPsyPosTp400(CT_LbsServer& aParent,
		TInt aIndex) :
	CT_LbsPortedStepBase(aParent)
	{
	SetTestStepName(GetIndexName(aIndex));
	iIndex = aIndex;
	}

void CT_LbsSimulationPsyPosTp400::StartL()
	{
	// Setup test environment
	iNrOfRequests = 0;
	ConnectL();
	SetupPsyL(iUidSimulationPsy);
	RunTestsL(iIndex);
	}

TVerdict CT_LbsSimulationPsyPosTp400::doTestStepPostambleL()
	{
	SetTrpModeL(iOldTrpMode);
	return CT_LbsPortedStepBase::doTestStepPostambleL();
	}

void CT_LbsSimulationPsyPosTp400::CloseTest()
	{
	Disconnect();
	}

/*
 * Gets the name of an individual test index
 * @param aIndex Indicates which test index name that is asked for.
 */
TPtrC CT_LbsSimulationPsyPosTp400::GetIndexName(TInt aIndex) const
	{
	if (aIndex > KNrOfTests)
		{
		User::Panic(_L("Wrong Test Argument Index"), KErrGeneral);
		}

	const TDesC* names[KNrOfTests] =
		{
		&KName0, &KName1, &KName2, &KName3, &KName4, &KName5
		};
	return *names[aIndex];
	}

/*
 * Contains calls to tests itself. 
 * @param aIndex test number
 */
void CT_LbsSimulationPsyPosTp400::RunTestsL(TInt aIndex)
	{
	_LIT(KStart0, "Testing TrpMultiClientNmeaStoredFixExist() PSY ");
	_LIT(KStart1, "Testing TrpMultiClientNmeaStoredFixNotExist() PSY ");
	_LIT(KStart2, "Testing TrpMultiClientNmeaNoUpdateAge() PSY ");
	_LIT(KStart3, "Testing TrpMultiClientSpsStoredFixExist() PSY ");
	_LIT(KStart4, "Testing TrpMultiClientSpsStoredFixNotExist() PSY ");
	_LIT(KStart5, "Testing TrpMultiClientSpsNoUpdateAge() PSY ");
	_LIT(KEnd0, "TrpMultiClientNmeaStoredFixExist() passed");
	_LIT(KEnd1, "TrpMultiClientNmeaStoredFixNotExist() passed");
	_LIT(KEnd2, "TrpMultiClientNmeaNoUpdateAge() passed");
	_LIT(KEnd3, "TrpMultiClientSpsStoredFixExist() passed");
	_LIT(KEnd4, "TrpMultiClientSpsStoredFixNotExist() passed");
	_LIT(KEnd5, "TrpMultiClientSpsNoUpdateAge() passed");

	switch (aIndex)
		{
		case 0:
			INFO_PRINTF1(KStart0);
			TestTrpMultiClientNmeaStoredFixExistL();
			INFO_PRINTF1(KEnd0);
			break;
		case 1:
			INFO_PRINTF1(KStart1);
			TestTrpMultiClientNmeaStoredFixNotExistL();
			INFO_PRINTF1(KEnd1);
			break;
		case 2:
			INFO_PRINTF1(KStart2);
			TestTrpMultiClientNmeaNoUpdateAgeL();
			INFO_PRINTF1(KEnd2);
			break;
		case 3:
			INFO_PRINTF1(KStart3);
			TestTrpMultiClientSpsStoredFixExistL();
			INFO_PRINTF1(KEnd3);
			break;
		case 4:
			INFO_PRINTF1(KStart4);
			TestTrpMultiClientSpsStoredFixNotExistL();
			INFO_PRINTF1(KEnd4);
			break;
		case 5:
			INFO_PRINTF1(KStart5);
			TestTrpMultiClientSpsNoUpdateAgeL();
			INFO_PRINTF1(KEnd5);
			break;
		default:
			User::Leave(KErrArgument);
		}
	}

void CT_LbsSimulationPsyPosTp400::TestTrpMultiClientNmeaStoredFixExistL()
	{
	_LIT(KTrpMultiClientRequestError, "Error, the fixes are different.");
	
	if (!TwoClientRequestL(EFalse, K0seconds, K30seconds))
		{
		LogErrorAndLeaveL(KTrpMultiClientRequestError);
		}
	
	INFO_PRINTF1(_L("The fixes are identical."));
	}

void CT_LbsSimulationPsyPosTp400::TestTrpMultiClientNmeaStoredFixNotExistL()
	{
	_LIT(KTrpMultiClientRequestError, "Error, The fixes are identical.");
	
	if (TwoClientRequestL(EFalse, K10seconds, K5seconds))
		{
		LogErrorAndLeaveL(KTrpMultiClientRequestError);
		}
	
	INFO_PRINTF1(_L("The fixes are different."));
	}

void CT_LbsSimulationPsyPosTp400::TestTrpMultiClientNmeaNoUpdateAgeL()
	{
	_LIT(KTrpMultiClientRequestError, "Error, The fixes are identical.");
	
	if (TwoClientRequestL(EFalse, K0seconds, K0seconds))
		{
		LogErrorAndLeaveL(KTrpMultiClientRequestError);
		}
	
	INFO_PRINTF1(_L("The fixes are different."));
	}

void CT_LbsSimulationPsyPosTp400::TestTrpMultiClientSpsStoredFixExistL()
	{
	_LIT(KTrpMultiClientRequestError, "Error, the fixes are different.");
	
	if (!TwoClientRequestL(ETrue, K0seconds, K30seconds))
		{
		LogErrorAndLeaveL(KTrpMultiClientRequestError);
		}
	
	INFO_PRINTF1(_L("The fixes are identical."));
	}

void CT_LbsSimulationPsyPosTp400::TestTrpMultiClientSpsStoredFixNotExistL()
	{
	_LIT(KTrpMultiClientRequestError, "Error, The fixes are identical.");
	_LIT(KTrpMultiClientDirectionError, "Error, the position differences do not correspond to direction of movement.");

	TReal latitudeDifference;
	TReal longitudeDifference;

	if (TwoClientRequestL(ETrue, K10seconds, K5seconds, &latitudeDifference,
			&longitudeDifference))
		{
		LogErrorAndLeaveL(KTrpMultiClientRequestError);
		}
	//The latitude and longitude difference should be not as bellow 
	//to correspond movement direction specified in SPS file.
	if (latitudeDifference >= 0 || longitudeDifference <= 0)
		{
		LogErrorAndLeaveL(KTrpMultiClientDirectionError);
		}
	
	INFO_PRINTF3(_L("The fixes are different and Client 2 is further than Client 1.: %d, %d"), longitudeDifference, latitudeDifference);
	}

void CT_LbsSimulationPsyPosTp400::TestTrpMultiClientSpsNoUpdateAgeL()
	{
	_LIT(KTrpMultiClientRequestError, "Error, The fixes are identical.");
	_LIT(KTrpMultiClientDirectionError, "Error, the position differences doesn't correspond direction of movement.");

	TReal latitudeDifference;
	TReal longitudeDifference;

	if (TwoClientRequestL(ETrue, K0seconds, K0seconds, &latitudeDifference,
			&longitudeDifference))
		{
		LogErrorAndLeaveL(KTrpMultiClientRequestError);
		}
	//The latitude and longitude difference should be positive 
	//to correspond movement direction specified in SPS file.
	if (latitudeDifference >= 0 || longitudeDifference <= 0)
		{
		LogErrorAndLeaveL(KTrpMultiClientDirectionError);
		}
	
	INFO_PRINTF3(_L("The fixes are different and Client 2 is further than Client 1.: %d/%d"), longitudeDifference, latitudeDifference);
	}

/*
 * Make two client sessions. First client request position and waits for time of aDelay. 
 * Next set Max Update Age of second client and issue position request of second client. 
 * If aLatitudeDiff and aLongitudeDiff parameters are set method will use them to
 * return calculated difference of client positions.
 * @param aIsSps set to ETrue if function should use SPS simulation mode, EFalse for NMEA simulation mode.
 * @param aSecondClientDelay is delay in milliseconds of second positioner request.
 * @param aMaxUpdateAge value in milliseconds that will be set to Max Update Age option of second positioner.
 * @param aLatitudeDiff this parameter will store difference of latitude between clients position.
 * @param aLongitudeDiff this parameter will store difference of longitude between clients position.
 * @return ETrue if clients positions are equal. Otherwise EFalse.
 */
TBool CT_LbsSimulationPsyPosTp400::TwoClientRequestL(TBool aIsSps,
		TInt aSecondClientDelay, TInt aMaxUpdateAge, TReal* aLatitudeDiff,
		TReal* aLongitudeDiff)
	{
	_LIT(KErrorRequestingPosition, "Position Request Error.");
	_LIT(KGetUpdateOptionsErr, "Error %d when getting update options.");
	_LIT(KSetUpdateOptionsErr, "Error %d when setting Max Update Age option.");

#ifdef __WINS__
	_LIT(KTrpNmeaFile, "z:\\system\\test\\testdata\\trpnmeadatafile.nme");
	_LIT(KTrpSpsFile, "z:\\system\\test\\testdata\\trpspsdatafile.sps");
#else
	_LIT(KTrpNmeaFile, "c:\\system\\test\\testdata\\trpnmeadatafile.nme");
	_LIT(KTrpSpsFile, "c:\\system\\test\\testdata\\trpspsdatafile.sps");
#endif
	
	iOldTrpMode = TrpModeL();
	SetTrpModeL(KTrpModeEnable);
	
	//Creating client subsessions to position server.
	RPositioner positioner1;
	RPositioner positioner2;
	if (aIsSps)
		{
		OpenNewSourceFileLC(KTrpSpsFile, positioner1);
		OpenNewSourceFileLC(KTrpSpsFile, positioner2);
		}
	else
		{
		OpenNewSourceFileLC(KTrpNmeaFile, positioner1);
		OpenNewSourceFileLC(KTrpNmeaFile, positioner2);
		}
	
	//Seting Max Update Age option for second Positioner.
	TPositionUpdateOptions posOption;
	TInt err = positioner2.GetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KGetUpdateOptionsErr, err);

	posOption.SetMaxUpdateAge(aMaxUpdateAge);
	err = positioner2.SetUpdateOptions(posOption);
	AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);

	TPositionInfo fix1;
	TPositionInfo fix2;
	
	//Position request of First client.
	err = PerformSyncRequest(fix1, positioner1);
	AssertTrueL(err == KErrNone, KErrorRequestingPosition, err);
	
	//Delay of second positioner.
	User::After(aSecondClientDelay);
	
	//Position request of Second client.
	err = PerformSyncRequest(fix2, positioner2);
	AssertTrueL(err == KErrNone, KErrorRequestingPosition, err);
	
	//Getting fixed positions.
	TPosition pos1;
	fix1.GetPosition(pos1);
	TPosition pos2;
	fix2.GetPosition(pos2);

	//Closing positioners
	CleanupStack::PopAndDestroy(&positioner2);
	CleanupStack::PopAndDestroy(&positioner1);
	
	//Seting longitude and latitude difference.
	if (aLatitudeDiff != NULL)
		{
		*aLatitudeDiff = pos2.Latitude() - pos1.Latitude();
		}
	if (aLongitudeDiff != NULL)
		{
		*aLongitudeDiff = pos2.Longitude() - pos1.Longitude();
		}
	
	return pos1.Latitude() == pos2.Latitude() && pos1.Longitude() == pos2.Longitude();
	}

/*
 * Set new source file in central repository and open new RPositioner session.
 * Opened session will be left on cleanup stack. 
 * @param aName Name of file to set in central repository.
 * @param aPositioner reference to constructed but unopened RPositioner that will use data from this particular file.
 * 		This parameter will opened and the session will be left on cleanup stack.	
 */
void CT_LbsSimulationPsyPosTp400::OpenNewSourceFileLC(const TDesC& aName,
		RPositioner& aPositioner)
	{
	_LIT(KOpenPositionerErr, "Error when opening positioner,  %d");
	_LIT(KSettingRequestorErr, "Error when setting requester,  %d");
	TInt err = KErrNone;
	SetSimDataFileL(aName);
	err = aPositioner.Open(iPosServer, iUidSimulationPsy);
	AssertTrueL(err == KErrNone, KOpenPositionerErr, err);
	CleanupClosePushL(aPositioner);
	_LIT(KService, "service");
	err = aPositioner.SetRequestor(CRequestor::ERequestorService,
			CRequestor::EFormatApplication, KService);
	AssertTrueL(err == KErrNone, KSettingRequestorErr, err);
	}

/*
 * @return value of Common Data Set in Central Repository.
 */
TInt CT_LbsSimulationPsyPosTp400::TrpModeL() const
	{
	// Get the Common Data Set setting from Central Repository
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	TInt commonDataSet = 0;
	User::LeaveIfError(repository->Get(KCRKeySimPSYCommonDataSet, commonDataSet));
	CleanupStack::PopAndDestroy(repository);
	return commonDataSet;
	}

/*
 * This method sets Common Data Set in Central Repository.
 * @param aNewValue new value for Common Data Set.
 */
void CT_LbsSimulationPsyPosTp400::SetTrpModeL(TInt aNewValue)
	{
	// Set Time Related Playback behaviour in the Central Repository.
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	User::LeaveIfError(repository->Set(KCRKeySimPSYCommonDataSet, aNewValue));
	CleanupStack::PopAndDestroy(repository);
	}

/*
 * Perform a synchronous position request and return error code.
 * @param aModuleInfo position information is returned in this object.
 * @param aPositioner reference to RPositioner that will be used to obtain position request.
 * @return Symbian system error code.
 */
TInt CT_LbsSimulationPsyPosTp400::PerformSyncRequest(TPositionInfoBase& aModuleInfo, const RPositioner& aPositioner) const
	{
	TRequestStatus status = KRequestPending;
	aPositioner.NotifyPositionUpdate(aModuleInfo, status);
	User::WaitForRequest(status);
	return status.Int();
	}

