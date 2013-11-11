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
// mectestpanic11step.cpp
//

/**
@file
*/

#include "mectestpanic11step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic11Step::~CMecTestPanic11Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_111
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainer::FindExtensionL()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) FindExtensionL T1 in container
//! @SYMTestExpectedResults  Panic mecpanic:1
//! @SYMTestPriority 	Critical
*/
TVerdict CMecTestPanic11Step::RunTestStepL()
    {
    RMetaExtensionContainer mec;
    TRAP_IGNORE(const Meta::SMetaData& ext = mec.FindExtensionL(TTestExtension1::TypeId())); // PANIC
    return EFail;
    }


