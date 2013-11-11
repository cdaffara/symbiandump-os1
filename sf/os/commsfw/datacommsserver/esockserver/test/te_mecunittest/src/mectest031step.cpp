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
// mectest031step.cpp
//

/**
@file
*/

#include "mectest031step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest031Step::~CMecTest031Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_031
//! @SYMTestCaseDesc 	Container Branching Test, Override extension
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container rootMec
//						2) AppendExtension T1.1 to rootMec
//						3) AppendExtension T2.1 to rootMec
//						4) Open container branchMec against rootMec
//						5) AppendExtension T1.2 to branchMec
//						6) FindExtension T1 == T1.2
//						7) FindExtension T2 == T2.1
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest031Step::RunTestStepL()
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
    
    CreateAndAppendExtensionL(branchMec, TTestExtension1::ETypeId, KExtVersion2);
    
	const TTestExtension1* ext1 = static_cast<const TTestExtension1*>(branchMec.FindExtension(TTestExtension1::TypeId()));
	const TTestExtension2* ext2 = static_cast<const TTestExtension2*>(branchMec.FindExtension(TTestExtension2::TypeId()));
	if (ext1 && ext1->TypeId() == TTestExtension1::TypeId() && ext1->iVersion == KExtVersion2
		&& ext2 && ext2->TypeId() == TTestExtension2::TypeId() && ext2->iVersion == KExtVersion1)
		{
		result = EPass;
		}
    
    CleanupStack::PopAndDestroy(&branchMec);
    CleanupStack::PopAndDestroy(&rootMec);
	
    return result;
    }

