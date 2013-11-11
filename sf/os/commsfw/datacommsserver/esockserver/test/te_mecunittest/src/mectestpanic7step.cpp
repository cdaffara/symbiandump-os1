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
// mectestpanic7step.cpp
//

/**
@file
*/

#include "mectestpanic7step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTestPanic7Step::~CMecTestPanic7Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_107
//! @SYMTestCaseDesc 	Test Panic mecpanic:3 (EAlreadyOpen) for RMetaExtensionContainer::Open()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Create container
//						2) Call RMetaExtensionContainer::Open() on container
//! @SYMTestExpectedResults  Panic mecpanic:3
*/
TVerdict CMecTestPanic7Step::RunTestStepL()
    {
    RMetaExtensionContainer mec;
    mec.Open();
    
    mec.Open(); // PANIC
    
    return EFail;
    }


