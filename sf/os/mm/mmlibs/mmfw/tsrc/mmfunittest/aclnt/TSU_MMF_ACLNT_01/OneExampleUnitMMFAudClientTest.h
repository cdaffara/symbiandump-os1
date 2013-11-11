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
// OneExampleUnitMMFAudClientTest.cpp
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//


#if (!defined __ONEEXAMPLE_UNITMMFAUDCLIENTTEST_H__)
#define __ONEEXAMPLE_UNITMMFAUDCLIENTTEST_H__

 
/**
 *
 * Example Test
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class COneExampleUnitMMFAudClientTest : public CTestStepUnitMMFAudClient
	{
public:
	static COneExampleUnitMMFAudClientTest* NewL();
	COneExampleUnitMMFAudClientTest();
	~COneExampleUnitMMFAudClientTest();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Example Test 2
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CExampleTest2 : public CTestStepUnitMMFAudClient
	{
public:
	static CExampleTest2* NewL();
	CExampleTest2();
	~CExampleTest2();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Example Test 3
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CExampleTest3 : public CTestStepUnitMMFAudClient
	{
public:
	static CExampleTest3* NewL();
	CExampleTest3();
	~CExampleTest3();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Example Test 4
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CExampleTest4 : public CTestStepUnitMMFAudClient
	{
public:
	static CExampleTest4* NewL();
	CExampleTest4();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Example Test 5
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CExampleTest5 : public CTestStepUnitMMFAudClient
	{
public:
	static CExampleTest5* NewL();
	CExampleTest5();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Panic Test
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CPanicTest : public CTestStepUnitMMFAudClient
	{
public:
	static CPanicTest* NewL();
	CPanicTest();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Preamble Panic Test
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CPrePanicTest : public CTestStepUnitMMFAudClient
	{
public:
	static CPrePanicTest* NewL();
	CPrePanicTest();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Postamble Panic Test
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CPostPanicTest : public CTestStepUnitMMFAudClient
	{
public:
	static CPostPanicTest* NewL();
	CPostPanicTest();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPostambleL();

	};

/**
 *
 * FLogTest
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CFlogTest : public CTestStepUnitMMFAudClient
	{
public:
	static CFlogTest* NewL();
	CFlogTest();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * CTimerTest
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTimerTest : public CTestStepUnitMMFAudClient
	{
public:
	static CTimerTest* NewL();
	CTimerTest();
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Error Code Test
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CErrorCodeTest : public CTestStepUnitMMFAudClient
	{
public:
	static CErrorCodeTest* NewL();
	CErrorCodeTest();
	virtual TVerdict DoTestStepL();

	};


#endif //(__ONEEXAMPLE_UNITMMFAUDCLIENTTEST_H__)
