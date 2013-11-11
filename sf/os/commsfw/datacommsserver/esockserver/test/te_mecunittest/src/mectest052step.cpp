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
// mectest052step.cpp
//

/**
@file
*/

#include "mectest052step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest052Step::~CMecTest052Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_052
//! @SYMTestCaseDesc 	Container Merging Test, Open container, append multiple containers, then override extension
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container cont1
//						2) AppendExtension T1.1 to cont1
//						3) AppendExtension T2.1 to cont1
//						4) Open container cont2
//						5) AppendExtension T3.1 to cont2
//						6) AppendExtension T2.2 to cont2
//						7) Open container mergeMec
//						8) AppendContainerL cont1 to mergeMec
//						9) AppendContainerL cont2 to mergeMec
//						10) mergeMec container count == 2
//						11) AppendExtension T3.2 to mergeMec
//						12) mergeMec container count == 3
//						13) Find Extension T1 == T1.1
//						14) Find Extension T2 == T2.2
//						15) Find Extension T3 == T3.2
//						16) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest052Step::RunTestStepL()
    {
    TVerdict result = EFail;
    
    RMetaExtensionContainer container1;
    container1.Open();
    CleanupClosePushL(container1);
    CreateAndAppendExtensionL(container1, TTestExtension1::ETypeId, KExtVersion1);
    CreateAndAppendExtensionL(container1, TTestExtension2::ETypeId, KExtVersion1);

    RMetaExtensionContainer container2;
    container2.Open();
    CleanupClosePushL(container2);
    CreateAndAppendExtensionL(container2, TTestExtension3::ETypeId, KExtVersion1);
    CreateAndAppendExtensionL(container2, TTestExtension2::ETypeId, KExtVersion2);
    
    RMetaExtensionContainer mergeMec;
    mergeMec.Open();
    CleanupClosePushL(mergeMec);
    
    mergeMec.AppendContainerL(container1);
    mergeMec.AppendContainerL(container2);
    
    ESock::CMetaExtensionContainerArray* mergeMecImpl = TMetaExtensionContainerSpy::AppendableContainerArray(mergeMec);
    if (mergeMecImpl && TMetaExtensionContainerSpy::ContainerCount(*mergeMecImpl) == 2)
    	{
    	CreateAndAppendExtensionL(mergeMec, TTestExtension3::ETypeId, KExtVersion2);
    	if (TMetaExtensionContainerSpy::ContainerCount(*mergeMecImpl) == 3)
    		{
			const TTestExtension1* ext1 = static_cast<const TTestExtension1*>(mergeMec.FindExtension(TTestExtension1::TypeId()));
			const TTestExtension2* ext2 = static_cast<const TTestExtension2*>(mergeMec.FindExtension(TTestExtension2::TypeId()));
			const TTestExtension3* ext3 = static_cast<const TTestExtension3*>(mergeMec.FindExtension(TTestExtension3::TypeId()));
			if (ext1 && ext1->GetTypeId() == TTestExtension1::TypeId() && ext1->iVersion == KExtVersion1
				&& ext2 && ext2->GetTypeId() == TTestExtension2::TypeId() && ext2->iVersion == KExtVersion2
				&& ext3 && ext3->GetTypeId() == TTestExtension3::TypeId() && ext3->iVersion == KExtVersion2)
				{
				result = EPass;
				}
    		}
    	}
    	
    CleanupStack::PopAndDestroy(&mergeMec);
    CleanupStack::PopAndDestroy(&container2);
    CleanupStack::PopAndDestroy(&container1);
    return result;
    }

