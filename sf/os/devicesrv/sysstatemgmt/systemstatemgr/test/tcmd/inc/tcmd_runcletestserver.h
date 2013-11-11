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
//CmdCleTestServer created to test the commands reconstructed by the CLE

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __CMDTEST_RUNCLETESTSEVER_H_
#define __CMDTEST_RUNCLETESTSEVER_H_

#include <test/testexecuteserverbase.h>

class CCmdCleTestServer : public CTestServer
    {
public:
    static CCmdCleTestServer * NewLC();

    //from CTestServer
    CTestStep* CreateTestStep(const TDesC& aStepName);
    };

#endif //__CMDTEST_RUNCLETESTSEVER_H_
