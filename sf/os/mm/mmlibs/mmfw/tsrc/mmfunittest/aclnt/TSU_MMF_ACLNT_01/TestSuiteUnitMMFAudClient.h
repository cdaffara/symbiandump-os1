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
//

#if (!defined __TEST_SUITE_MMF_AUD_CLNT_H_)
#define __TEST_SUITE_MMF_AUD_CLNT_H_

#include <testframework.h>

/**
 *
 * CTestSuiteUnitMMFAudClient, which is the base class for 
 * the UnitMMFAudClient suite.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class  CTestSuiteUnitMMFAudClient : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteUnitMMFAudClient();
	void AddTestStepL( CTestStepUnitMMFAudClient * aTestStep );
	void AddTestStepL( RTestStep * aTestStep );
	TPtrC GetVersion() const;
	void InitSystemPath();
	const TDesC& DefaultPath() const;
	
private: 
	TFileName iDefaultPath;
	};


#endif //__TEST_SUITE_MMF_AUD_CLNT_H_
