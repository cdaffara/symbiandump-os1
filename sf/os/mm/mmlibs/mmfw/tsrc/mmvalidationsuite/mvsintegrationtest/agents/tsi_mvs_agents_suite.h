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
//


#ifndef TSI_MVS_AGENT_SUITE_H
#define TSI_MVS_AGENT_SUITE_H

#include <testframework.h>

class RTestStepMVSBase;
class RTestMVSVideoAgentSetScrIdAndPlayBase;
class RTestStepMVSAgentsAPRBase;

/**
 *
 * CTestSuiteMVSAgent
 *   defines the test suite for MVS Agent tests
 * 
 */
class CTestSuiteMVSAgent : public CTestSuite
    {
public:
    void InitialiseL();
    virtual ~CTestSuiteMVSAgent();
    void AddTestStepL( RTestStepMVSBase * aTestStep );
    void AddTestStepL( RTestMVSVideoAgentSetScrIdAndPlayBase* aTestStep );
    void AddTestStepL( RTestStepMVSAgentsAPRBase* aTestStep );
    TPtrC GetVersion() const;
    };

#endif // TSI_MMF_MVS_AGENT_SUITE_H

