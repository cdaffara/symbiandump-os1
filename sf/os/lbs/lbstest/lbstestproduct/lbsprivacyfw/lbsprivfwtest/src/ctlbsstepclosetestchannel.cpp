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
// @file ctlbsstepclosetestchannel.cpp
// This is the class implementation for the step to start the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "ctlbsstepclosetestchannel.h"
//#include "tlbsutils.h"
//#include "lbstestclient.h"
#include <lbs/test/tlbstestrequestchannel.h>

/**
 * Destructor
 */
CT_LbsStep_CloseTestChannel::~CT_LbsStep_CloseTestChannel()
	{
	}


/**
 * Constructor
 */
CT_LbsStep_CloseTestChannel::CT_LbsStep_CloseTestChannel(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_CloseTestChannel);
	}


/**
Static Constructor
*/
CT_LbsStep_CloseTestChannel* CT_LbsStep_CloseTestChannel::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_CloseTestChannel(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_CloseTestChannel::doTestStepL()
	{
	// Test step used to start the location manager.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_CloseTestChannel::doTestStepL()"));

	if (TestStepResult() == EPass)
		{	
		//Close Test Channel
		RLbsTestChannel::ShutDownL(RLbsTestChannel::EChannelHandler2Proxy);
		RLbsTestChannel::ShutDownL(RLbsTestChannel::EChannelProxy2Handler);
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_CloseTestChannel::doTestStepL()"));

	return TestStepResult();
	}
