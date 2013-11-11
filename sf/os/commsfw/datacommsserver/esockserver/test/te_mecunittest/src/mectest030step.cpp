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
// mectest030step.cpp
//

/**
@file
*/

#include "mectest030step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest030Step::~CMecTest030Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_030
//! @SYMTestCaseDesc 	Container Branching Test, Open against a base container and FindExtension() without overriding
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container rootMec
//						2) AppendExtension T1.1 to rootMec
//						3) AppendExtension T2.1 to rootMec
//						4) Open container branchMec against rootMec
//						5) Find Extension T2 == T2.1
//						6) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest030Step::RunTestStepL()
    {
    TVerdict result = EFail;

    RMetaExtensionContainer rootMec;
    rootMec.Open();
    CleanupClosePushL(rootMec);
    
    CreateAndAppendExtensionL(rootMec, TTestExtension1::ETypeId, KExtVersion1);
    CreateAndAppendExtensionL(rootMec, TTestExtension2::ETypeId, KExtVersion1);
    
    RMetaExtensionContainer branchMec;
    branchMec.Open(rootMec);
    CleanupClosePushL(branchMec);
    
	const TTestExtension2* ext = static_cast<const TTestExtension2*>(branchMec.FindExtension(TTestExtension2::TypeId()));
	if (ext && ext->TypeId() == TTestExtension2::TypeId() && ext->iVersion == KExtVersion1)
		{
		result = EPass;
		}
    
    CleanupStack::PopAndDestroy(&branchMec);
    CleanupStack::PopAndDestroy(&rootMec);
	
    return result;
    }

