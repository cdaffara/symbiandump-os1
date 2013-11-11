// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestSuiteSelfTest.h
//

//
#if (!defined __TEST_SUITE_ACTRL_H_)
#define __TEST_SUITE_ACTRL_H_

#include <testframework.h>

/**
 *
 * CTestSuiteActrl is the test suite
 * for the audio controller
 *
 * @lib "TSU_MMF_actrl_01.lib"
 *
 */
class  CTestSuiteActrl : public CTestSuite
	{
private:
		void InitSystemPath();
public:
		void     InitialiseL( void );
		virtual ~CTestSuiteActrl();
		void     AddTestStepL( CTestStepActrl * ptrTestStep );
		TPtrC    GetVersion( void );
		const TDesC& DefaultPath() const;

private:
		TFileName iDefaultPath;
	};


#endif /* __TEST_SUITE_BITMAP_TRANSFORMS_H_  */
