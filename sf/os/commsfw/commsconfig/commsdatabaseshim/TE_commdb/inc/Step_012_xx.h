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
// This is the header file for CommDb test 012.XX group of scenarios 


#if (!defined __STEP_012_XX_H__)
#define __STEP_012_XX_H__


////////////////////
//	Test step 012.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_01) : public CTestStepCommDb
{
public:
	CCommDbTest012_01();
	~CCommDbTest012_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();


private:

};

////////////////////
//	Test step 012.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_02) : public CTestStepCommDb
{
public:
	CCommDbTest012_02();
	~CCommDbTest012_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 012.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_03) : public CTestStepCommDb
{
public:
	CCommDbTest012_03();
	~CCommDbTest012_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 012.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_04) : public CTestStepCommDb
{
public:
	CCommDbTest012_04();
	~CCommDbTest012_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 012.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_05) : public CTestStepCommDb
{
public:
	CCommDbTest012_05();
	~CCommDbTest012_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt myOpenViewL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 012.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_06) : public CTestStepCommDb
{
public:
	CCommDbTest012_06();
	~CCommDbTest012_06();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt myOpenViewL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 012.07
////////////////////

NONSHARABLE_CLASS(CCommDbTest012_07) : public CTestStepCommDb
{
public:
	CCommDbTest012_07();
	~CCommDbTest012_07();

	virtual enum TVerdict doTestStepL( void );


private:

};

#endif //(__STEP_012_XX_H__)
