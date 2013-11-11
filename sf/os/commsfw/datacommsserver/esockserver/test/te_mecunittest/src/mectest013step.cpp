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
// mectest013step.cpp
//

/**
@file
*/

#include "mectest013step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest013Step::~CMecTest013Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_013
//! @SYMTestCaseDesc    Test NULL return for RMetaExtensionContainerC::FindExtension() with un-opened container
//! @SYMFssID           COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions     1) FindExtension T1 in constMec
//! @SYMTestExpectedResults  NULL return code
*/
TVerdict CMecTest013Step::RunTestStepL()
    {
    RMetaExtensionContainerC mec;
    const Meta::SMetaData* ext = mec.FindExtension(TTestExtension1::TypeId());
    return ext == NULL ? EPass : EFail;
    }

