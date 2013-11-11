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
// mectest001step.cpp
//

/**
@file
*/

#include "mectest001step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest001Step::~CMecTest001Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_001
//! @SYMTestCaseDesc 	Simple API Test, Open() and Close() with no other actions
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) Close container
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest001Step::RunTestStepL()
    {
    // Create container, expect success
    RMetaExtensionContainer mec;
    mec.Open();
    mec.Close();
    
    // If it reaches here without crashing its a pass
    return EPass;
    }

