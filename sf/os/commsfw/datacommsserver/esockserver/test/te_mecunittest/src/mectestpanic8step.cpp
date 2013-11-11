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
// mectestpanic8step.cpp
//

/**
@file
*/

#include "mectestpanic8step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic8Step::~CMecTestPanic8Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_108
//! @SYMTestCaseDesc 	Test Panic mecpanic:3 (EAlreadyOpen) for RMetaExtensionContainer::Open(RMetaExtensionContainerC& aMec)
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container
//						2) AppendExtension T1 to container
//						3) Create container2
//						4) Open container2 against container
//! @SYMTestExpectedResults  Panic mecpanic:3
*/
TVerdict CMecTestPanic8Step::RunTestStepL()
    {
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId);
    
    RMetaExtensionContainer container2;
    container2.Open();
    
    container2.Open(container1); // PANIC
    
    container2.Close();
    CleanupStack::PopAndDestroy(&container1);
    return EFail;
    }


