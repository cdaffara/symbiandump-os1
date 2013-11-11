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
 @file cmolrstep17.cpp
*/

#include <lbspositioninfo.h>
#include "cmolrstep17.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CMoLrStep17::~CMoLrStep17()
	{
	}


/** Constructor
*/
CMoLrStep17::CMoLrStep17()
	{
	SetTestStepName(KMoLrStep17);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep17::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMoLrStep17 test step.
This test verifies that the Test Protocol Module correctly handles 
MO-LR Self Locate when GPS mode is autonomous.

@return TVerdict test result code
*/
TVerdict CMoLrStep17::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR Self Locate - GPS mode autonomous"));
	INFO_PRINTF1(_L("\t********************************************************************"));

	INFO_PRINTF1(_L("\t----------------"));
	INFO_PRINTF1(_L("\tNot a new client"));
	INFO_PRINTF1(_L("\t----------------"));
	INFO_PRINTF1(_L("- START (a) -"));

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x1010);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(EFalse);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataNone;
	options1.SetDataRequestMask(dataRequestMask1);
	options1.SetPosMode(TPositionModuleInfo::ETechnologyTerminal);
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
								
	INFO_PRINTF1(_L("\t----------------"));
	INFO_PRINTF1(_L("\tNew client"));
	INFO_PRINTF1(_L("\t----------------"));
	INFO_PRINTF1(_L("- START (b) -"));

	// Have to reset the observer callback information
	iGatewayObserver->ResetWasObserverCalled();
	iNetworkObserver->ResetWasObserverCalled();

	// Initiate MO-LR
	TLbsNetSessionId sessionId3(TUid::Uid(0x87654321), 0x3030);
	TLbsNetPosRequestOptionsAssistance options3;
	options3.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality3;
	options3.SetRequestQuality(quality3);
	TLbsAsistanceDataGroup dataRequestMask3 = EAssistanceDataNone;
	options3.SetDataRequestMask(dataRequestMask3);
	options3.SetPosMode(TPositionModuleInfo::ETechnologyTerminal);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));	
	iModule->RequestSelfLocation(sessionId3, options3);

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

	INFO_PRINTF1(_L("- END (b) -"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep17::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
