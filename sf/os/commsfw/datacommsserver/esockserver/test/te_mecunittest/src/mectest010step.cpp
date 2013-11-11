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
// mectest010step.cpp
//

/**
@file
*/

#include "mectest010step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest010Step::~CMecTest010Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_010
//! @SYMTestCaseDesc 	Simple API Test, RMetaExtensionContainer::FindExtension() where no extensions are present
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) Find Extension T2 == NULL
//						3) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest010Step::RunTestStepL()
    {
    TVerdict result = EFail;

    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    const Meta::SMetaData* ext = mec.FindExtension(TTestExtension2::TypeId());
    if (ext == NULL)
    	{
    	result = EPass;
    	}
    
    CleanupStack::PopAndDestroy(&mec);
	
    return result;
    }

