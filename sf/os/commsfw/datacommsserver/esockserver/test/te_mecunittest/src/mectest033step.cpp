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
// mectest033step.cpp
//

/**
@file
*/

#include "mectest033step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest033Step::~CMecTest033Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_033
//! @SYMTestCaseDesc 	Container Branching Test, Use RMetaExtensionContainerC after opening against
//						a branched container that adds no extensions
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container rootMec
//						2) AppendExtension T1.1 to rootMec
//						3) AppendExtension T2.1 to rootMec
//						4) Open container branchMec against rootMec
//						7) Open container constMec against branchMec
//						8) FindExtension T2 in constMec == T2
//						9) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest033Step::RunTestStepL()
    {
    TVerdict result = EFail;

    RMetaExtensionContainer rootMec;
    rootMec.Open();
    CleanupClosePushL(rootMec);
    
    CreateAndAppendExtensionL(rootMec, TTestExtension1::ETypeId, KExtVersion1);
    CreateAndAppendExtensionL(rootMec, TTestExtension2::ETypeId, KExtVersion1);
    
    RMetaExtensionContainer branchMec;
    branchMec.Open(rootMec);

    RMetaExtensionContainerC constMec;
    constMec.Open(branchMec);
    
	const TTestExtension2* ext2 = static_cast<const TTestExtension2*>(constMec.FindExtension(TTestExtension2::TypeId()));
    if (ext2 && ext2->GetTypeId() == TTestExtension2::TypeId() && ext2->iVersion == KExtVersion1)
		{
		result = EPass;
		}
    
    constMec.Close();
    branchMec.Close();
    CleanupStack::PopAndDestroy(&rootMec);
	
    return result;
    }

