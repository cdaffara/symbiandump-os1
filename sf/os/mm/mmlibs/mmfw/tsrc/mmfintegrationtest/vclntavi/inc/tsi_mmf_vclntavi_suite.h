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
// This contains CTestMmfVclntAviSuite 
// Header file: Test Suite for Video Client tests
// 
//

#ifndef TSI_MMF_VCLNTAVI_SUITE_H
#define TSI_MMF_VCLNTAVI_SUITE_H

class RTestMmfVclntAviStep;

/**
 *
 * CTestMmfVclntAviSuite
 *
 */
class CTestMmfVclntAviSuite : public CTestSuite
    {
public:
    void InitialiseL();
    virtual ~CTestMmfVclntAviSuite();
    void AddTestStepL(RTestMmfVclntAviStep* ptrTestStep);
    TPtrC GetVersion();

#ifdef __WINS__
protected:
    // TO create the Window Server related objects
    TBool WaitUntilWindowServerReadyL();
    void InitWservL();
    void DeInitWserv();
protected:
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWsSession iWs;
    RWindowGroup iRootWindow;
#endif // __WINS__

    RFs iFileSession;
    // TO use the ECam Test Plug-in
    void CreateDependencyFileL();
    void RemoveDependencyFile();
    };

#endif // __TSI_MMF_VCLNTAVI_SUITE_H__

