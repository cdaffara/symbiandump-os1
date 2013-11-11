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
// This is the header file for CommDb test 062.XX group of scenarios 


#if (!defined __STEP_062_XX_H__)
#define __STEP_062_XX_H__


////////////////////
//	Test step 062.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest062_01) : public CTestStepCommDb
{
public:
	CCommDbTest062_01();
	~CCommDbTest062_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 062.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest062_02) : public CTestStepCommDb
{
public:
	CCommDbTest062_02();
	~CCommDbTest062_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 062.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest062_03) : public CTestStepCommDb
{
public:
	CCommDbTest062_03();
	~CCommDbTest062_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:

};

////////////////////
//	Test step 062.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest062_04) : public CTestStepCommDb
{
public:
	CCommDbTest062_04();
	~CCommDbTest062_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommDbOverrideSettings* iOverSet;

};

////////////////////
//	Test step 062.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest062_05) : public CTestStepCommDb
{
public:
	CCommDbTest062_05();
	~CCommDbTest062_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommDbOverrideSettings* iOverSet;

};



#endif //(__STEP_062_XX_H__)
