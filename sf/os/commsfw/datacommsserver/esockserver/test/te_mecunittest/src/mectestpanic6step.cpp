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
// mectestpanic6step.cpp
//

/**
@file
*/

#include "mectestpanic6step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic6Step::~CMecTestPanic6Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_106
//! @SYMTestCaseDesc 	Test Panic mecpanic:1 (ENoImplementation) for RMetaExtensionContainerC::FindExtensionL()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) FindExtensionL T1 in constMec
//! @SYMTestExpectedResults  Panic mecpanic:1
*/
TVerdict CMecTestPanic6Step::RunTestStepL()
    {
    RMetaExtensionContainerC mec;
    const Meta::SMetaData& ext = mec.FindExtensionL(TTestExtension1::TypeId()); // PANIC
    
    return EFail;
    }


