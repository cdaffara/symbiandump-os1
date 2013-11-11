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
// mectest005step.cpp
//

/**
@file
*/

#include "mectest005step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest005Step::~CMecTest005Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_005
//! @SYMTestCaseDesc 	Simple API Test, RMetaExtensionContainer::FindExtension()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) AppendExtension T1 to container
//						3) FindExtension T1 == T1
//						4) Close container
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest005Step::RunTestStepL()
    {
    TVerdict result = EFail;
    
    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    CreateAndAppendExtensionL(mec, TTestExtension1::ETypeId);
    
    const Meta::SMetaData* ext = mec.FindExtension(TTestExtension1::TypeId());
    if (ext != NULL && ext->GetTypeId() == TTestExtension1::TypeId())
    	{
    	result = EPass;
    	}
    
    CleanupStack::PopAndDestroy(&mec);
    
    return result;
    }

