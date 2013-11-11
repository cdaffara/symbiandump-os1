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
// This is the header file for CommDb test 023.XX group of scenarios 


#if (!defined __STEP_023_XX_H__)
#define __STEP_023_XX_H__


////////////////////
//	Test step 023.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest023_01) : public CTestStepCommDb
{
public:
	CCommDbTest023_01();
	~CCommDbTest023_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 023.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest023_02) : public CTestStepCommDb
{
public:
	CCommDbTest023_02();
	~CCommDbTest023_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};



////////////////////
//	Test step 023.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest023_03) : public CTestStepCommDb
{
public:
	CCommDbTest023_03();
	~CCommDbTest023_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};




////////////////////
//	Test step 023.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest023_04) : public CTestStepCommDb
{
public:
	CCommDbTest023_04();
	~CCommDbTest023_04();

	virtual enum TVerdict doTestStepL( void );

private:

};


////////////////////
//	Test step 023.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest023_05) : public CTestStepCommDb
{
public:
	CCommDbTest023_05();
	~CCommDbTest023_05();

	virtual enum TVerdict doTestStepL( void );

private:

};

////////////////////
//	Test step 023.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest023_06) : public CTestStepCommDb
{
public:
	CCommDbTest023_06();
	~CCommDbTest023_06();

	virtual enum TVerdict doTestStepL( void );

private:

};


#endif //(__STEP_023_XX_H__)
