// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dial API Test header file.
// 
//

/**
 @file
*/

#ifndef __TE_DIALTEST_H__
#define __TE_DIALTEST_H__


class CDialTestTest1 : public CDialTestStep
	{
public:
	CDialTestTest1();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest2 : public CDialTestStep
	{
public:
	CDialTestTest2();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest3 : public CDialTestStep
	{
public:
	CDialTestTest3();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest4 : public CDialTestStep
	{
public:
	CDialTestTest4();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest5 : public CDialTestStep
	{
public:
	CDialTestTest5();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest6 : public CDialTestStep
	{
public:
	CDialTestTest6();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTestDtmf2 : public CDialTestStep
	{
public:
	CDialTestTestDtmf2();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest7 : public CDialTestStep
	{
public:
	CDialTestTest7();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestTest8 : public CDialTestStep
	{
public:
	CDialTestTest8();

	virtual enum TVerdict doTestStepL();
	};

class CDialTestIllegalTest : public CDialTestStep
	{
public:
	CDialTestIllegalTest();

	virtual enum TVerdict doTestStepL();
	};


#endif // __TE_DIALTEST_H__

