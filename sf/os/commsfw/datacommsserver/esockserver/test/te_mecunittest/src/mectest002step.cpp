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
// mectest002step.cpp
//

/**
@file
*/

#include "mectest002step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest002Step::~CMecTest002Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_002
//! @SYMTestCaseDesc 	Simple API Test, AppendExtension()
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) AppendExtension T1 to container
//						3) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest002Step::RunTestStepL()
    {
    TVerdict result = EFail;
    
    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    TTestExtension1* ext = new(ELeave)TTestExtension1();
    TInt err = mec.AppendExtension(ext);
    if (err == KErrNone)
    	{
    	result = EPass;
    	}
    else
    	{
    	delete ext;
    	}
    
    CleanupStack::PopAndDestroy(&mec);
    return result;
    }

