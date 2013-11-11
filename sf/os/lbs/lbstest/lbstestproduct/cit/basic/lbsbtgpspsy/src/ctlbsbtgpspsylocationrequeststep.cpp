// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test step which makes a standard Location Request
//

#include "ctlbsbtgpspsylocationrequeststep.h"
#include <bt_sock.h>


CT_LbsBtGpsPsyLocationRequestStep::CT_LbsBtGpsPsyLocationRequestStep()
	{
	SetTestStepName(KBTGPSPsyLocationRequestStep);
	}


CT_LbsBtGpsPsyLocationRequestStep::~CT_LbsBtGpsPsyLocationRequestStep()
	{
	}


TVerdict CT_LbsBtGpsPsyLocationRequestStep::doTestStepPreambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPreambleL();
	
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyLocationRequestStep::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		
		//Make the location requests
		for(TInt requestNum=0; requestNum < iTestConfig.iNumOfLocRequests; ++requestNum)
			{
			err = LocationRequest();
			if(err != KErrNone)
				{
				//Fail the test, but keep on trying to get the remaining location requests
				SetTestStepResult(EFail);
				}
			}
		}
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyLocationRequestStep::doTestStepPostambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPostambleL();
	
	return TestStepResult();
	}
