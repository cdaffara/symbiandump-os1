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
 @file lbsnetgatewaystartstep.cpp
*/
#include "lbsnetgatewaysettohomenetwork.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"
#include <lbs/lbsadmin.h>
#include <simtsy.h>
#include <etelmm.h>
/**
 * Destructor
 */
CLbsNetGatewaySetToHomeNetwork::~CLbsNetGatewaySetToHomeNetwork()

	{
	}
	
/**
 * Constructor
 */
CLbsNetGatewaySetToHomeNetwork::CLbsNetGatewaySetToHomeNetwork()

	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsNetGatewaySetToHomeNetwork);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNetGatewaySetToHomeNetwork::doTestStepPreambleL()

	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CLbsNetGatewaySetToHomeNetwork::doTestStepL()
	{
	INFO_PRINTF1(_L("* Setting to Home Network"));
	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, 
								  KPSSimTsyRegStatChange,
								  RMobilePhone::ERegisteredOnHomeNetwork));
	// Short delay to allow the new value to propagate to Network Gateway.
	User::After(500000);

	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNetGatewaySetToHomeNetwork::doTestStepPostambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

