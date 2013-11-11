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
// @file ctlbsstepinitializetestchannel.cpp
// This is the class implementation for the step to start the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "ctlbsstepinitializetestchannel.h"
//#include "tlbsutils.h"
//#include "lbstestclient.h"
#include <lbs/test/tlbstestrequestchannel.h>

/**
 * Destructor
 */
CT_LbsStep_InitializeTestChannel::~CT_LbsStep_InitializeTestChannel()
	{
	}


/**
 * Constructor
 */
CT_LbsStep_InitializeTestChannel::CT_LbsStep_InitializeTestChannel(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_InitializeTestChannel);
	}


/**
Static Constructor
*/
CT_LbsStep_InitializeTestChannel* CT_LbsStep_InitializeTestChannel::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_InitializeTestChannel(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_InitializeTestChannel::doTestStepL()
	{
	// Test step used to start the location manager.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_InitializeTestChannel::doTestStepL()"));

	if (TestStepResult() == EPass)
		{	
		//Initialize Test Channel
		RLbsTestChannel::InitializeL(RLbsTestChannel::EChannelHandler2Proxy);
		RLbsTestChannel::InitializeL(RLbsTestChannel::EChannelProxy2Handler);
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_InitializeTestChannel::doTestStepL()"));

	return TestStepResult();
	}
