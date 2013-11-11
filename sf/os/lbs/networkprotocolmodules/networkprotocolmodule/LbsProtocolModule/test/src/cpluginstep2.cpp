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
 @file cpluginstep2.cpp
*/
#include "cpluginstep2.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CPluginStep2::~CPluginStep2()
	{
	}

/** Constructor
*/
CPluginStep2::CPluginStep2()
	{
	SetTestStepName(KPluginStep2);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CPluginStep2::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	return TestStepResult();
	}


/** Perform CPluginStep2 test step.
This test verifies that the Test Protocol Module can be loaded via ECOM
using the LBS Network Protocol SPI base class to instantiate the plug-in.

@return TVerdict test result code
*/
TVerdict CPluginStep2::doTestStepL()
	{
	TLbsNetProtocolModuleParams params(*iGatewayObserver);
	// Load the Test Protocol Module
	INFO_PRINTF1(_L("\t About to load Test Protocol Module plug-in via ECOM"));
	CLbsNetworkProtocolBase* protModule = CLbsNetworkProtocolBase::NewL(
										TUid::Uid(KPluginUid),
										params);
										
	// Confirm that the plug-in has been loaded
	if (!protModule)
		{
		INFO_PRINTF1(_L("\t Test Protocol Module plug-in NOT loaded"));
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("\t Test Protocol Module plug-in loaded"));
		SetTestStepResult(EPass);
		delete protModule;
		}

	// Close ECom Session
	REComSession::FinalClose();
		
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CPluginStep2::doTestStepPostambleL()
	{
	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	
	return TestStepResult();
	}
