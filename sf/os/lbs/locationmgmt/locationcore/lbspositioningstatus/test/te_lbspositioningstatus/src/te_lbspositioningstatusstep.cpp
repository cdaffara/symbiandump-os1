// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file te_lbspositioningstatusstepbase.cpp
*/

#include "te_lbspositioningstatusstep.h"
#include "te_lbsstatustestmanager.h"
#include "lbspositioningstatusprops.h" // from internal API


TVerdict CTe_LbsPositioningStatusStep::doTestStepPreambleL()
    {

    INFO_PRINTF1(_L("CTe_LbsPositioningStatusStep::doTestStepPreambleL()"));
    __UHEAP_MARK;
    iSched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iSched);

    LbsPositioningStatusProps::InitializeNiPropertyL();
    LbsPositioningStatusProps::InitializeMoPropertyL();	
    
    iTestManager = CTe_LbsStatusTestManager::NewL(*this);
    SetTestStepResult(EPass);
    return TestStepResult();
    }

TVerdict CTe_LbsPositioningStatusStep::doTestStepPostambleL()
    {
    INFO_PRINTF1(_L("CTe_LbsPositioningStatusStep::doTestStepPostambleL()"));
	iTestManager->Cancel();
    delete iTestManager;
    iTestManager = NULL;
    delete iSched;
    iSched = NULL;
    __UHEAP_MARKEND;
    return TestStepResult();
    }

TVerdict CTe_LbsPositioningStatusStep::doTestStepL()
    {
    INFO_PRINTF1(_L("CTe_LbsPositioningStatusStep::doTestStepL()"));
    iTestManager->StartTest();
    return TestStepResult();
    }

CTe_LbsPositioningStatusStep::CTe_LbsPositioningStatusStep()
    {
    SetTestStepName(KStatusStep);
    }

CTe_LbsPositioningStatusStep::~CTe_LbsPositioningStatusStep()
    {
	if(iTestManager)
		{
		iTestManager->Cancel();
		}
    delete iTestManager;
    delete iSched;
    }


// end of file


