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
// mectestpanic2step.cpp
//

/**
@file
*/

#include "mectestpanic2step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic2Step::~CMecTestPanic2Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_102
//! @SYMTestCaseDesc 	Test Panic mecpanic:2 (EInvalidSelfReference) for RMetaExtensionContainerC::Open(RMetaExtensionContainerC& aMec)
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open constMec against constMec
//! @SYMTestExpectedResults  Panic mecpanic:2
*/
TVerdict CMecTestPanic2Step::RunTestStepL()
    {
    RMetaExtensionContainerC mec;
    mec.Open(mec); // PANIC
    
    return EFail;    
    }

