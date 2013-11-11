// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains suite class declaration for DevVideo integration tests.
// 
//


#ifndef TSI_MMF_DEVVIDEO_SUITE_H
#define TSI_MMF_DEVVIDEO_SUITE_H

// forward declaration
class CTestSuite;

/**
 *
 * CTestSuiteDevVideo
 * - Test suite class
 *
 */
class CTestSuiteDevVideo : public CTestSuite
    {
public:
    virtual ~CTestSuiteDevVideo();

    // from CTestSuite
    // Creates all the test steps and stores them inside CTestSuiteDevVideo
    void InitialiseL();

    // from CTestSuite, gets test suite version
    TPtrC GetVersion() const;

    // from CTestSuite, adds a test step into the suite
    void AddTestStepL(RTestStepDevVideoBase* ptrTestStep);
    };

#endif    // TSI_MMF_DEVVIDEO_SUITE_H
