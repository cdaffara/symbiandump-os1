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
// mectest050step.cpp
//

/**
@file
*/

#include "mectest050step.h"
#include "testextensions.h"
#include "metaextensioncontainerspy.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>
#include <comms-infras/ss_commsprov.h>

using namespace ESock;

CMecTest050Step::~CMecTest050Step()
    {
    }


/**
//! @SYMTestCaseID      MEC_UNIT_TEST_050
//! @SYMTestCaseDesc 	Container Merging Test, Open container and append the same container implementation twice
//! @SYMFssID 			COMMS-INFRAS/Esock/MetaExtensionContainer/UnitTest
//! @SYMTestActions 	1) Open container cont1
//						2) AppendExtension T1
//						3) Open constMec1 against cont1
//						4) Open constMec2 against cont1
//						5) Open container cont2
//						6) AppendContainerL constMec1
//						7) AppendContainerL constMec2 == KErrAlreadyExists
//						8) Close
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CMecTest050Step::RunTestStepL()
    {
    TVerdict result = EFail;

    RMetaExtensionContainer cont1;
    cont1.Open();
    CleanupClosePushL(cont1);
    CreateAndAppendExtensionL(cont1, TTestExtension1::ETypeId);
    
    RMetaExtensionContainerC constMec1;
    constMec1.Open(cont1);
    CleanupClosePushL(constMec1);

    RMetaExtensionContainerC constMec2;
    constMec2.Open(cont1);
    CleanupClosePushL(constMec2);

    RMetaExtensionContainer cont2;
    cont2.Open();
    CleanupClosePushL(cont2);
    
    cont2.AppendContainerL(constMec1);
    
    TRAPD(err, cont2.AppendContainerL(constMec2));
    if (err == KErrAlreadyExists)
    	{
    	result = EPass;
    	}
    
    CleanupStack::PopAndDestroy(&cont2);
    CleanupStack::PopAndDestroy(&constMec2);
    CleanupStack::PopAndDestroy(&constMec1);
    CleanupStack::PopAndDestroy(&cont1);
    
    return result;    
    }

