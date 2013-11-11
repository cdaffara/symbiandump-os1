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
// This is the header file for CommDb test 010.XX group of scenarios 


#if (!defined __STEP_010_XX_H__)
#define __STEP_010_XX_H__


////////////////////
//	Test step 010.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest010_01) : public CTestStepCommDb
{
public:
	CCommDbTest010_01();
	~CCommDbTest010_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();


private:

};

////////////////////
//	Test step 010.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest010_02) : public CTestStepCommDb
{
public:
	CCommDbTest010_02();
	~CCommDbTest010_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 010.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest010_03) : public CTestStepCommDb
{
public:
	CCommDbTest010_03();
	~CCommDbTest010_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 010.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest010_04) : public CTestStepCommDb
{
public:
	CCommDbTest010_04();
	~CCommDbTest010_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 010.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest010_05) : public CTestStepCommDb
{
public:
	CCommDbTest010_05();
	~CCommDbTest010_05();

	virtual enum TVerdict doTestStepL( void );


private:

};

#endif //(__STEP_010_XX_H__)
