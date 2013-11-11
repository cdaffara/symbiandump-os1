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
// mectest011step.cpp
//

/**
@file
*/

#include "mectest011step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest011Step::~CMecTest011Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_011
//! @SYMTestCaseDesc 	Simple API Test, RMetaExtensionContainerC::FindExtension() after opening
//						against a container with extensions
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) AppendExtension T1
//						3) Open constMec container against container
//						4) RMetaExtensionContainer::FindExtension T1 == T1
//						5) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest011Step::RunTestStepL()
    {
    TVerdict result = EFail;

    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    CreateAndAppendExtensionL(mec, TTestExtension1::ETypeId);
    
    RMetaExtensionContainerC constMec;
    constMec.Open(mec);
    CleanupStack::PopAndDestroy(&mec);
    
    const TTestExtension1* ext1 = static_cast<const TTestExtension1*>(constMec.FindExtension(TTestExtension1::TypeId()));
    if (ext1 && ext1->GetTypeId() == TTestExtension1::TypeId())
    	{
    	result = EPass;
    	}
    
    constMec.Close();
    return result;
    }

