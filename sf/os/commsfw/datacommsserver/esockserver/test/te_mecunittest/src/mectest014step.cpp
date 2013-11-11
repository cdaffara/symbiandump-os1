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
// mectest014step.cpp
//

/**
@file
*/

#include "mectest014step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest014Step::~CMecTest014Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_014
//! @SYMTestCaseDesc    Test KErrNotFound leave for RMetaExtensionContainerC::FindExtensionL() with un-opened container
//! @SYMFssID           COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions     1) FindExtensionL T1 in constMec
//! @SYMTestExpectedResults  Leaves with KErrNotFound
*/
TVerdict CMecTest014Step::RunTestStepL()
    {
    RMetaExtensionContainerC mec;
    TRAPD(err, const Meta::SMetaData& ext = mec.FindExtensionL(TTestExtension1::TypeId()));
    return err == KErrNotFound ? EPass : EFail;
    }

