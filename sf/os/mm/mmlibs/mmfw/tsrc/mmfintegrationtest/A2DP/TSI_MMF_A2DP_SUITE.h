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


#ifndef __TSI_MMF_A2DP_SUITE_H__
#define __TSI_MMF_A2DP_SUITE_H__

#include <testframework.h>
#include <sounddevice.h>

class RTestStepA2dp;

/**
 *
 * CTestSuiteA2dp defines the test suite for A2dp tests
 * 
 *
 * @lib "TSI_MMF_A2DP_SUITE.lib"
 *
 *
 */
class  CTestSuiteA2dp : public CTestSuite 
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteA2dp();
	void AddTestStepL( RTestStepA2dp * aTestStep );
	TPtrC GetVersion() const;
	};

#endif // __TSI_MMF_A2DP_SUITE_H__
