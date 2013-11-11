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
// mectest054step.cpp
//

/**
@file
*/

#include "mectest054step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest054Step::~CMecTest054Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_054
//! @SYMTestCaseDesc 	Container Merging Test, Open container, append extensions, then append a container
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container cont1
//						2) AppendExtension T1.1 to cont1
//						3) AppendExtension T2.1 to cont1
//						4) Open container mergeMec
//						5) AppendExtension T2.2 to mergeMec
//						5) AppendExtension T3.1 to mergeMec
//						8) AppendContainerL cont1  to mergeMec
//						9) mergeMec container count == 2
//						11) Find Extension T1 == T1.1
//						12) Find Extension T2 == T2.1
//						13) Find Extension T3 == T3.1
//						14) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest054Step::RunTestStepL()
    {
    TVerdict result = EFail;
    
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId, KExtVersion1);
    CreateAndAppendExtensionL(container1, TTestExtension2::ETypeId, KExtVersion1);

    
    RMetaExtensionContainer mergeMec;
    mergeMec.Open();
    CleanupClosePushL(mergeMec);
    
    CreateAndAppendExtensionL(mergeMec, TTestExtension2::ETypeId, KExtVersion2);
    CreateAndAppendExtensionL(mergeMec, TTestExtension3::ETypeId, KExtVersion1);
    
    mergeMec.AppendContainerL(container1);
    
    ESock::CMetaExtensionContainerArray* mergeMecImpl = TMetaExtensionContainerSpy::AppendableContainerArray(mergeMec);
    if (mergeMecImpl && TMetaExtensionContainerSpy::ContainerCount(*mergeMecImpl) == 2)
    	{
		const TTestExtension1* ext1 = static_cast<const TTestExtension1*>(mergeMec.FindExtension(TTestExtension1::TypeId()));
		const TTestExtension2* ext2 = static_cast<const TTestExtension2*>(mergeMec.FindExtension(TTestExtension2::TypeId()));
		const TTestExtension3* ext3 = static_cast<const TTestExtension3*>(mergeMec.FindExtension(TTestExtension3::TypeId()));
		if (ext1 && ext1->GetTypeId() == TTestExtension1::TypeId() && ext1->iVersion == KExtVersion1
			&& ext2 && ext2->GetTypeId() == TTestExtension2::TypeId() && ext2->iVersion == KExtVersion1
			&& ext3 && ext3->GetTypeId() == TTestExtension3::TypeId() && ext3->iVersion == KExtVersion1)
			{
			result = EPass;
			}
    	}
	
    CleanupStack::PopAndDestroy(&mergeMec);
    CleanupStack::PopAndDestroy(&container1);
    return result;
    }

