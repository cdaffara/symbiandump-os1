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
//CLayer test server using ssm

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __CLAYER_SERVERWITHSSM_H__
#define __CLAYER_SERVERWITHSSM_H__

#include <test/testblockcontroller.h>
#include <test/testserver2.h>

class CCLayerTestServerWithSsm : public CTestServer2
    {
public:
    static CCLayerTestServerWithSsm * NewL();
    CTestBlockController*   CreateTestBlock();
    
    CCLayerTestServerWithSsm();
    ~CCLayerTestServerWithSsm();
    };

class CCLayerTestBlock : public CTestBlockController
    {
public:
    CCLayerTestBlock() : CTestBlockController() {}
    ~CCLayerTestBlock() {}

    CDataWrapper* CreateDataL(const TDesC& aData);
    };
    
#endif	//__CLAYER_SERVERWITHSSM_H__
