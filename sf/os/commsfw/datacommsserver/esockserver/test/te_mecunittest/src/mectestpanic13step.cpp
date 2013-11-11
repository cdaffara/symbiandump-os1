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
// mectestpanic13step.cpp
//

/**
@file
*/

#include "mectestpanic13step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic13Step::~CMecTestPanic13Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_113
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainer::AppendExtensionL()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) AppendExtensionL T1 to container
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic13Step::RunTestStepL()
    {
    RMetaExtensionContainer mec;
    
    TTestExtension1* ext = new(ELeave)TTestExtension1();
    CleanupStack::PushL(ext);
    TRAP_IGNORE(mec.AppendExtensionL(ext)); // PANIC
    
    CleanupStack::Pop(ext);
    mec.Close();
    
    return EFail;
    }


