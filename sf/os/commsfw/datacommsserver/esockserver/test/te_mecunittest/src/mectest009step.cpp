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
// mectest009step.cpp
//

/**
@file
*/

#include "mectest009step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest009Step::~CMecTest009Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_009
//! @SYMTestCaseDesc 	Simple API Test, RMetaExtensionContainer::FindExtension() where extension is last in the list
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) AppendExtension T1 to container
//						3) AppendExtension T2 to container
//						4) FindExtensionL T2 == T2
//						5) Close container
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest009Step::RunTestStepL()
    {
    TVerdict result = EFail;
    
    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    CreateAndAppendExtensionL(mec, TTestExtension1::ETypeId);
    CreateAndAppendExtensionL(mec, TTestExtension2::ETypeId);
    
    const Meta::SMetaData* ext = mec.FindExtension(TTestExtension2::TypeId());
    if (ext != NULL && ext->GetTypeId() == TTestExtension2::TypeId())
    	{
    	result = EPass;
    	}
    
    CleanupStack::PopAndDestroy(&mec);
    
    return result;
    }

