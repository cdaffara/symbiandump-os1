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
 @file cnetlocstep8.cpp
*/

#include <lbspositioninfo.h>
#include "cnetlocstep8.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CNetLocStep8::~CNetLocStep8()
	{
	}


/** Constructor
*/
CNetLocStep8::CNetLocStep8()
	{
	SetTestStepName(KNetLocStep8);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CNetLocStep8::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CNetLocStep8 test step.
This test verifies that the Test Protocol Module correctly handles 
an Network Based Location sequence with additional assistance request
added, with an error response.

@return TVerdict test result code
*/
TVerdict CNetLocStep8::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tNetwork Based Location - additional assistance request, with error"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate Network Based Location
	INFO_PRINTF1(_L("\tLBS -> RequestNetworkLocation"));
	TLbsNetSessionId sessionId(TUid::Uid(0x12345678), 0x8888);
	TLbsNetPosRequestOptions options;
	options.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality;
	options.SetRequestQuality(quality);
	iModule->RequestNetworkLocation(sessionId, options);

	// Check network receives Network Based Location request
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::ERegisterLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	TLbsAsistanceDataGroup dataRequestMask = (TUint) EAssistanceDataReferenceTime;
	TPositionInfo refLoc;
	RLbsAssistanceDataBuilderSet assistData;
	CleanupClosePushL(assistData);
	assistData.OpenL();
	SetDummyAssistanceData(assistData, dataRequestMask);
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);
	CleanupStack::PopAndDestroy(&assistData);

	// Check gateway receives Assistance data
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData"));

	// Check gateway receives Network Based location
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationUpdate))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));

	// Check gateway receives Location Request
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceNetworkLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Request more assistance
	INFO_PRINTF1(_L("\tLBS -> RequestAssistanceData (more)"));	
	dataRequestMask = EAssistanceDataBadSatList;
	iModule->RequestAssistanceData(dataRequestMask);

	// Check network receives request for missing assistance data
	if ((EFail == CheckNetworkCallbackL(
				CNetworkObserver::EMeasurementReportRequestMoreAssistanceData)) ||
		(EAssistanceDataBadSatList != iNetworkObserver->MoreDataMask()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t          MeasurementReportRequestMoreAssistanceData (more) -> NET"));

	// Generate network Measurement Control error
	INFO_PRINTF1(_L("\t                     ProcessMeasurementControlLocationError <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationErrorL(KErrLocked);
	
	// Check gateway receives Assistance data error
	if ((EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData)) ||
		(KErrLocked != iGatewayObserver->AssistanceDataError()) ||
		(EAssistanceDataBadSatList != 
				iGatewayObserver->AssistanceDataSetMask()))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (error)"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest (Cancel)"));
	TInt  reason = KErrCancel;
	TPositionInfo posInfo;
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(),
									reason, posInfo);

	// Check network receives cancel response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementControlFailure) ||
		KErrCancel != iNetworkObserver->MeasurementFailure())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                         MeasurementControlFailure (CANCEL) -> NET"));
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr) ||
		KErrNone != iNetworkObserver->SessionCloseReason())
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
TVerdict CNetLocStep8::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
