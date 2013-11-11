/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
    @file
    @test 
*/

#ifndef WAPPROT_SUITE_SERVER_H
#define WAPPROT_SUITE_SERVER_H

#include "smsstackbasetestserver.h" 

class CWapProtSuite : public CSmsStackTestServer
    {
public:
    static CWapProtSuite* NewL();
    // Base class pure virtual override
    virtual CTestStep* CreateTestStep(const TDesC& aStepName);
    };

#endif  // WAPPROT_SUITE_SERVER_H
