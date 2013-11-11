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
// mectest032step.cpp
//

/**
@file
*/

#include "mectest032step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest032Step::~CMecTest032Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_032
//! @SYMTestCaseDesc 	Container Branching Test, Override all extensions and test compaction
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container rootMec
//						2) AppendExtension T1.1 to rootMec
//						3) AppendExtension T2.1 to rootMec
//						4) Open container branchMec against rootMec
//						5) AppendExtension T1.2 to branchMec
//						6) AppendExtension T2.2 to branchMec
//						7) Open container constMec against branchMec
//						8) constMec chain length == 2
//						9) Close branchMec
//						10) constMec chain length == 1
//						11) FindExtension T1 == T1.2
//						12) FindExtension T2 == T2.2
//						13) Close containers
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest032Step::RunTestStepL()
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
    CreateAndAppendExtensionL(branchMec, TTestExtension2::ETypeId, KExtVersion2);
        
    RMetaExtensionContainerC constMec;
    constMec.Open(branchMec);
    
    TUint chainLengthBefore = TMetaExtensionContainerSpy::ChainLength(constMec);
    CleanupStack::PopAndDestroy(&branchMec);

    if (chainLengthBefore == 2 && TMetaExtensionContainerSpy::ChainLength(constMec) == 1)
    	{
    	const TTestExtension1* ext1 = static_cast<const TTestExtension1*>(constMec.FindExtension(TTestExtension1::TypeId()));
    	const TTestExtension2* ext2 = static_cast<const TTestExtension2*>(constMec.FindExtension(TTestExtension2::TypeId()));
    	if (ext1 && ext1->GetTypeId() == TTestExtension1::TypeId() && ext1->iVersion == KExtVersion2
    		&& ext2 && ext2->GetTypeId() == TTestExtension2::TypeId() && ext2->iVersion == KExtVersion2)
    		{
    		result = EPass;
    		}
    	
    	}
    
    constMec.Close();
    CleanupStack::PopAndDestroy(&rootMec);
	
    return result;
    }

