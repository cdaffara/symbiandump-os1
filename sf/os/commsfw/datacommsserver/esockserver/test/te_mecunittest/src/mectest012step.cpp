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
// mectest012step.cpp
//

/**
@file
*/

#include "mectest012step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest012Step::~CMecTest012Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_012
//! @SYMTestCaseDesc 	Simple API Test, RMetaExtensionContainerC::FindExtensionL() after opening
//						against a container with extensions
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) AppendExtension T1
//						3) Open constMec container against container
//						4) RMetaExtensionContainer::FindExtensionL T1 == T1
//						5) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest012Step::RunTestStepL()
    {
    TVerdict result = EFail;

    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    CreateAndAppendExtensionL(mec, TTestExtension1::ETypeId);
    
    RMetaExtensionContainerC constMec;
    constMec.Open(mec);
    CleanupStack::PopAndDestroy(&mec);
    CleanupClosePushL(constMec);
    
    const TTestExtension1& ext1 = static_cast<const TTestExtension1&>(constMec.FindExtensionL(TTestExtension1::TypeId()));
    if (ext1.GetTypeId() == TTestExtension1::TypeId())
    	{
    	result = EPass;
    	}
    
    CleanupStack::PopAndDestroy(&constMec);
    return result;
    }

