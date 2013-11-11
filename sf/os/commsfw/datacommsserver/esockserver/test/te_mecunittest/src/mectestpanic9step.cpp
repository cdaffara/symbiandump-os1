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
// mectestpanic9step.cpp
//

/**
@file
*/

#include "mectestpanic9step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic9Step::~CMecTestPanic9Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_109
//! @SYMTestCaseDesc 	Test Panic mecpanic:2 (EInvalidSelfReference) for RMetaExtensionContainer::Open(RMetaExtensionContainerC& aMec)
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container against container
//! @SYMTestExpectedResults  Panic mecpanic:2
*/
TVerdict CMecTestPanic9Step::RunTestStepL()
    {
    RMetaExtensionContainer mec;
    mec.Open(mec); // PANIC
    return EFail;
    }


