/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for CommDb test 001.01


#if (!defined __STEP_001_01_H__)
#define __STEP_001_01_H__

NONSHARABLE_CLASS(CCommDbTest001_01) : public CTestStepCommDb
{
public:
	CCommDbTest001_01();
	~CCommDbTest001_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	virtual enum TVerdict doTestStepPreambleL();

};

NONSHARABLE_CLASS(CCommDbTest001_03) : public CTestStepCommDb
{
public:
	CCommDbTest001_03();
	~CCommDbTest001_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

};

NONSHARABLE_CLASS(CCommDbTest001_04) : public CTestStepCommDb
{
public:
	CCommDbTest001_04();
	~CCommDbTest001_04();

	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

};

NONSHARABLE_CLASS(CCommDbTest001_05) : public CTestStepCommDb
{
public:
	CCommDbTest001_05();
	~CCommDbTest001_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	virtual enum TVerdict doTestStepPreambleL();

};

NONSHARABLE_CLASS(CCommDbTest001_07) : public CTestStepCommDb
{
public:
	CCommDbTest001_07();
	~CCommDbTest001_07();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

};

NONSHARABLE_CLASS(CCommDbTest001_08) : public CTestStepCommDb
{
public:
	CCommDbTest001_08();
	~CCommDbTest001_08();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

};

NONSHARABLE_CLASS(CCommDbTest001_09) : public CTestStepCommDb
{
public:
	CCommDbTest001_09();
	~CCommDbTest001_09();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

};

NONSHARABLE_CLASS(CCommDbTest001_10) : public CTestStepCommDb
{
public:
	CCommDbTest001_10();
	~CCommDbTest001_10();

	virtual enum TVerdict doTestStepL( void );

};

#endif //(__STEP_001_01_H__)
