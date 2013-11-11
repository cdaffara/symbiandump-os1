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
// mectestpanic16step.cpp
//

/**
@file
*/

#include "mectestpanic16step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic16Step::~CMecTestPanic16Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_116
//! @SYMTestCaseDesc 	Test Panic mecpanic:2 (EInvalidSelfReference) for RMetaExtensionContainer::AppendContainerL()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container cont1
//						2) AppendExtension T1
//						3) AppendContainer cont1 to cont1
//! @SYMTestExpectedResults  Panic mecpanic:2
*/
TVerdict CMecTestPanic16Step::RunTestStepL()
    {
    // Create container, expect success
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId);
    
    container1.AppendContainerL(container1); // PANIC
    
    CleanupStack::PopAndDestroy(&container1);
    
    return EFail;     
    }


