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
 @file cmolrstep16.cpp
*/

#include <lbspositioninfo.h>
#include "cmolrstep16.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CMoLrStep16::~CMoLrStep16()
	{
	}


/** Constructor
*/
CMoLrStep16::CMoLrStep16()
	{
	SetTestStepName(KMoLrStep16);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep16::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMoLrStep16 test step.
This test verifies that the Test Protocol Module correctly handles 
MO-LR Self Locate when this is not a new client and not GPS autonomous.

@return TVerdict test result code
*/
TVerdict CMoLrStep16::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR Self Locate - not a new client and GPS mode not autonomous"));
	INFO_PRINTF1(_L("\t********************************************************************"));

	INFO_PRINTF1(_L("\t---------------------------"));
	INFO_PRINTF1(_L("\tNo assistance data required"));
	INFO_PRINTF1(_L("\t---------------------------"));
	INFO_PRINTF1(_L("- START (a) -"));

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x1010);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(EFalse);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataNone;
	options1.SetDataRequestMask(dataRequestMask1);
	options1.SetPosMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));	
	iModule->RequestSelfLocation(sessionId1, options1);

	// Check gateway session completes
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete) ||
		KErrNone != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (KErrNone)"));

	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	INFO_PRINTF1(_L("- END (a) -"));
								
	INFO_PRINTF1(_L("\t---------------------------"));
	INFO_PRINTF1(_L("\tAssistance data required"));
	INFO_PRINTF1(_L("\t---------------------------"));
	INFO_PRINTF1(_L("- START (b) -"));

	// Have to reset the observer callback information
	iGatewayObserver->ResetWasObserverCalled();
	iNetworkObserver->ResetWasObserverCalled();

	// Initiate MO-LR
	TLbsNetSessionId sessionId2(TUid::Uid(0x87654321), 0x2020);
	TLbsNetPosRequestOptionsAssistance options2;
	options2.SetNewClientConnected(EFalse);
	TLbsNetPosRequestQuality quality2;
	options2.SetRequestQuality(quality2);
	options2.SetPosMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	TLbsAsistanceDataGroup dataRequestMask2 = EAssistanceDataAlmanac;
	options2.SetDataRequestMask(dataRequestMask2);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));	
	iModule->RequestSelfLocation(sessionId2, options2);
	
	// Check network receives MO-LR request
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::ERegisterLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Cancel MO-LR
	INFO_PRINTF1(_L("\t            ProcessMeasurementControlLocationError (CANCEL) <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationErrorL(KErrCancel);

	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr) ||
		KErrNone != iNetworkObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                             ReleaseLcsMoLr -> NET"));
	
	// Check gateway receives Assistance data cancel
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessAssistanceData) ||
		KErrCancel != iGatewayObserver->AssistanceDataError())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (CANCEL)"));

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
								
	INFO_PRINTF1(_L("- END (b) -"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep16::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
