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
// mectest004step.cpp
//

/**
@file
*/

#include "mectest004step.h"
#include "testextensions.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest004Step::~CMecTest004Step()
    {
    }


/**
//! @SYMTestCaseDesc 	Simple API Test, AppendExtension() where extension has already been added
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container
//						2) AppendExtension T1 to container
//						3) AppendExtension T1 == KErrAlreadyExists
//						4) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest004Step::RunTestStepL()
    {
    TVerdict result = EFail;
    
    RMetaExtensionContainer mec;
    mec.Open();
    CleanupClosePushL(mec);
    
    CreateAndAppendExtensionL(mec, TTestExtension1::ETypeId, KExtVersion1);
    
    TTestExtension1* ext = new(ELeave)TTestExtension1(KExtVersion2);
    TInt err = mec.AppendExtension(ext);
    if (err == KErrAlreadyExists)
    	{
    	delete ext;
    	result = EPass;
    	}
    else if (err != KErrNone)
    	{
    	delete ext;
    	}
    
    CleanupStack::PopAndDestroy(&mec);
    return result;
    }

