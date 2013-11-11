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
// mectestpanic10step.cpp
//

/**
@file
*/

#include "mectestpanic10step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic10Step::~CMecTestPanic10Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_110
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainer::FindExtension()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) FindExtension T1 in container
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic10Step::RunTestStepL()
    {
    RMetaExtensionContainer mec;
    const Meta::SMetaData* ext = mec.FindExtension(TTestExtension1::TypeId()); // PANIC
    return EFail;
    }


