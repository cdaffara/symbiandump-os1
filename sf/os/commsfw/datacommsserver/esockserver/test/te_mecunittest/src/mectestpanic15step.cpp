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
// mectestpanic15step.cpp
//

/**
@file
*/

#include "mectestpanic15step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic15Step::~CMecTestPanic15Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_115
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainer::AppendContainerL()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container cont1
//						2) AppendExtension T1 to cont1
//						3) AppendContainerL unopened cont2 to cont1
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic15Step::RunTestStepL()
    {
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId);
    
    RMetaExtensionContainer container2;
    container1.AppendContainerL(container2); // PANIC
    
    CleanupStack::PopAndDestroy(&container1);
    return EFail;
    }


