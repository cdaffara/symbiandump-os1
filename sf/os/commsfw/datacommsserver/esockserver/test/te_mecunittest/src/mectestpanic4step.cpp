// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mectestpanic4step.cpp
//

/**
@file
*/

#include "mectestpanic4step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic4Step::~CMecTestPanic4Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_104
//! @SYMTestCaseDesc 	Test Panic mecpanic:3 (EAlreadyOpen) for RMetaExtensionContainerC::Open(RMetaExtensionContainerC& aMec)
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container1
//						2) AppendExtension T1 to container1
//						3) Open constMec against container1
//						4) Create container2
//						5) AppendExtension T2 to container2
//						6) Open constMec against container2
//						7) Close
//! @SYMTestExpectedResults  Panic mecpanic:3
*/
TVerdict CMecTestPanic4Step::RunTestStepL()
    {
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId);
    
    RMetaExtensionContainerC constMec;
    constMec.Open(container1);
    CleanupClosePushL(constMec);
    
    RMetaExtensionContainer container2;
    container2.Open();
    CleanupClosePushL(container2);
    CreateAndAppendExtensionL(container2, TTestExtension2::ETypeId);
    
    constMec.Open(container2); // PANIC

    CleanupStack::PopAndDestroy(&container2);
    CleanupStack::PopAndDestroy(&container1);
    CleanupStack::PopAndDestroy(&constMec);
    
    return EFail;
    }


