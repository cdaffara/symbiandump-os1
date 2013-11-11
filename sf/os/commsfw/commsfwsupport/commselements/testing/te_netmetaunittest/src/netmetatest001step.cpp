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
// netmetatest001step.cpp
//

/**
@file
*/

#include "netmetatest001step.h"
#include "testmetadata.h"
#include <elements/metacontainer.h>

using namespace Meta;

CNetMetaTest001Step::~CNetMetaTest001Step()
    {
    }


/**
//! @SYMTestCaseID      NETMETA_UNIT_TEST_001
//! @SYMTestCaseDesc 	
//! @SYMFssID 			COMMS-INFRAS/Elements/NetMeta/UnitTest
//! @SYMTestActions 	
//						
//! @SYMTestExpectedResults  KErrNone.
*/
TVerdict CNetMetaTest001Step::RunTestStepL()
    {
    RMetaDataContainer<TTestMetaData> mdContSource;
    RMetaDataContainer<TTestMetaData> mdContDest;
    
    TTestMetaData data1;
    data1.iValue = 1;
    data1.iText = _L("TTestMetaData 1");

    TTestMetaData data2;
    data2.iValue = 2;
    data2.iText = _L("TTestMetaData 2");

    TTestMetaData data3;
    data3.iValue = 3;
    data3.iText = _L("TTestMetaData 3");
    
    mdContSource.AppendL(&data1);
    mdContSource.AppendL(&data2);
    mdContSource.AppendL(&data3);
    
    mdContDest.Copy(mdContSource);
    
    if (mdContDest.Count() != mdContSource.Count())
        {
        return EFail;
        }
    
    TInt i = mdContSource.Count();
    while (--i >= 0)
        {
        if (mdContDest[i] != mdContSource[i])
            {
            return EFail;
            }
        }
    
    return EPass;
    }
