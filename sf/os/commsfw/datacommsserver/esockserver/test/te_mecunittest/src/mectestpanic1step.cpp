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
// mectestpanic1step.cpp
//

/**
@file
*/

#include "mectestpanic1step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic1Step::~CMecTestPanic1Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_101
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainerC::Open()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open constMec with RMetaExtensionContainerC::Open()
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic1Step::RunTestStepL()
    {
    RMetaExtensionContainerC mec;

    mec.Open(); // PANIC
    
    return EFail;
    }

