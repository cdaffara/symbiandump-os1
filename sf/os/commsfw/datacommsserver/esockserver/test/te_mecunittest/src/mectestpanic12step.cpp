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
// mectestpanic12step.cpp
//

/**
@file
*/

#include "mectestpanic12step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic12Step::~CMecTestPanic12Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_112
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainer::AppendExtension()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) AppendExtension T1 to container
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic12Step::RunTestStepL()
    {
    RMetaExtensionContainer mec;
    
    TTestExtension1* ext = new(ELeave)TTestExtension1();
    CleanupStack::PushL(ext);
    TInt err = mec.AppendExtension(ext); // PANIC
    
    (void)err;
    CleanupStack::Pop(ext);
    mec.Close();
    
    return EFail;
    }


