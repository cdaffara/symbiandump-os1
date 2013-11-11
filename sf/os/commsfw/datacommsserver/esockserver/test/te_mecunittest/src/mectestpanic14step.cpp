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
// mectestpanic14step.cpp
//

/**
@file
*/

#include "mectestpanic14step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic14Step::~CMecTestPanic14Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_114
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainer::AppendContainerL()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container cont1
//						2) AppendExtension T1 to cont1
//						3) AppendContainerL cont1 to unopened cont2
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic14Step::RunTestStepL()
    {
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId);
    
    RMetaExtensionContainer container2;
    container2.AppendContainerL(container1); // PANIC
    
    CleanupStack::PopAndDestroy(&container1);
    return EFail;
    }


