// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file plugin2Step.cpp
 @internalTechnology
*/
#include "plugin2Step.h"
#include "te_suplprotocolsuitedefs.h"
#include "lbssystemcontroller.h"
#include "lbssuplpushprops.h"

Cplugin2Step::~Cplugin2Step()
	{
	}

Cplugin2Step::Cplugin2Step()
	{
	SetTestStepName(Kplugin2Step);
	}

/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict Cplugin2Step::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();

	LbsSuplPushProps::InitializeL();

	return TestStepResult();
	}


/** Perform CPluginStep2 test step.
This test verifies that the Test Protocol Module can be loaded via ECOM
using the LBS Network Protocol SPI base class to instantiate the plug-in.

@return TVerdict test result code
*/
TVerdict Cplugin2Step::doTestStepL()
	{
	TLbsNetProtocolModuleParams params(*iGatewayObserver);
	// Load the Test Protocol Module
	INFO_PRINTF1(_L("\t About to load Test Protocol Module plug-in via ECOM"));
	CLbsNetworkProtocolBase* protModule = NULL;
	
	TRAPD (error, protModule = CLbsNetworkProtocolBase::NewL(
										TUid::Uid(KPluginUid),
										params) ) ;
										
	// Confirm that the plug-in has been loaded
	if (!protModule || error)
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


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cplugin2Step::doTestStepPostambleL()
	{
	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	
	return TestStepResult();

	}
