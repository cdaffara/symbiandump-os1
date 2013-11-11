// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cmolrstep2.cpp
*/

#include <lbspositioninfo.h>
#include "cmolrstep2.h"
#include "te_testprotocolmodulesuitedefs.h"

const TUint32 KInvalidDataSet = 0xFF000000;
const TReal32 KLocReqHorizAcc = 0xF0F0F0F0;
const TReal32 KRefPosHorizAcc = 0x0F0F0F0F;
const TReal32 KMobilePosHorizAcc = 0xFF0FFF00;

/** Destructor
*/
CMoLrStep2::~CMoLrStep2()
	{
	}


/** Constructor
*/
CMoLrStep2::CMoLrStep2()
	{
	SetTestStepName(KMoLrStep2);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep2::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMoLrStep2 test step.
This test verifies that the Test Protocol Module correctly handles 
an MO-LR Self Locate sequence where the initial assistance data request
identifies invalid and valid data.

An initial assistance data error should be returned to reflect the invalid 
data set that was requested.

In this test case the set of assistance data returned by the network also 
has missing data and this is to be reported using an error.

@return TVerdict test result code
*/
TVerdict CMoLrStep2::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR with bad assistance data request and missing data from network"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x2222);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataReferenceTime
											 | EAssistanceDataBadSatList
											 |KInvalidDataSet;
	options1.SetDataRequestMask(dataRequestMask1);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation (invalid + valid set)"));	
	iModule->RequestSelfLocation(sessionId1, options1);

	// Check network receives MO-LR request
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::ERegisterLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));

	// Check gateway receives error indication for assistance data
	if ((EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessAssistanceData)) ||
		(KErrArgument != iGatewayObserver->AssistanceDataError()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (KErrArgument)"));
	
	// Generate network Measurement Control with some missing data
	INFO_PRINTF1(_L("\t                ProcessMeasurementControlLocation (partial) <- NET"));
	TPositionInfo refLoc;
	TPosition refPos;
	refPos.SetHorizontalAccuracy(KRefPosHorizAcc);
	refLoc.SetPosition(refPos);
	RLbsAssistanceDataBuilderSet assistData;
	CleanupClosePushL(assistData);
	assistData.OpenL();
	SetDummyAssistanceData(assistData, EAssistanceDataBadSatList);
	TLbsNetPosRequestQuality quality;
	quality.SetMinHorizontalAccuracy(KLocReqHorizAcc);
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);
	
	// Check gateway receives Assistance data with missing data
	if ((EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData)) ||
		(EAssistanceDataBadSatList != iGatewayObserver->AssistanceDataSetMask()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (some missing)"));
	
	// Check network receives request for missing assistance data
	if ((EFail == CheckNetworkCallbackL(
				CNetworkObserver::EMeasurementReportRequestMoreAssistanceData)) ||
		(EAssistanceDataReferenceTime != iNetworkObserver->MoreDataMask()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t       MeasurementReportRequestMoreAssistanceData (missing) -> NET"));

	// Check gateway receives Network Based location
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationUpdate) ||
		KRefPosHorizAcc != iGatewayObserver->ReferencePos().HorizontalAccuracy())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));
	
	// Check gateway receives Location Request
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationRequest) ||
		iGatewayObserver->IsLocEmergency() ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType() ||
		KLocReqHorizAcc != iGatewayObserver->LocQuality().MinHorizontalAccuracy())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Generate network Measurement Control for the missing data
	INFO_PRINTF1(_L("\t           ProcessMeasurementControlLocation (missing data) <- NET"));
	SetDummyAssistanceData(assistData, EAssistanceDataReferenceTime);
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);
	CleanupStack::PopAndDestroy(&assistData);
	
	// Check gateway receives Assistance data with missing data
	if ((EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData)) ||
		((EAssistanceDataBadSatList | EAssistanceDataReferenceTime) != 
				iGatewayObserver->AssistanceDataSetMask()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (all)"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TInt  reason = KErrNone;
	TPositionInfo mobilePosInfo;
	TPosition mobilePos;
	mobilePos.SetHorizontalAccuracy(KMobilePosHorizAcc);
	mobilePosInfo.SetPosition(mobilePos);
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(), 
									reason, mobilePosInfo);

	// Check network receives Location Response
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EMeasurementReportLocation) ||
		KMobilePosHorizAcc != iNetworkObserver->MobilePos().HorizontalAccuracy())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                  MeasurementReportLocation -> NET"));
	
	// Generate network result
	// Note: This does NOT incorporate a network-based location
	INFO_PRINTF1(_L("\t                              ProcessFacilityLcsMoLrResultL <- NET"));
	iNetworkObserver->ProcessFacilityLcsMoLrResultL(KErrNone, NULL);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                             ReleaseLcsMoLr -> NET"));
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	INFO_PRINTF1(_L("- END -"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep2::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
