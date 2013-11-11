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
// This is the header file for CommDb test 004.XX group of scenarios 


#if (!defined __STEP_004_XX_H__)
#define __STEP_004_XX_H__


////////////////////
//	Test step 004.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest004_01) : public CTestStepCommDb
{
public:
	CCommDbTest004_01();
	~CCommDbTest004_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 004.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest004_02) : public CTestStepCommDb
{
public:
	CCommDbTest004_02();
	~CCommDbTest004_02();

	virtual enum TVerdict doTestStepL( void );
	TBool executeStepL();
	TVerdict doTestStepPreambleL();

private:


};

////////////////////
//	Test step 004.03
////////////////////

//Removed since this test will not compile on Code warrior due to having an array of zero length

NONSHARABLE_CLASS(CCommDbTest004_03) : public CTestStepCommDb
{
public:
	CCommDbTest004_03();
	~CCommDbTest004_03();

	virtual enum TVerdict doTestStepL( void );
	TBool executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 004.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest004_04) : public CTestStepCommDb
{
public:
	CCommDbTest004_04();
	~CCommDbTest004_04();

	virtual enum TVerdict doTestStepL( void );

private:


};

////////////////////
//	Test step 004.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest004_05) : public CTestStepCommDb
{
public:
	CCommDbTest004_05();
	~CCommDbTest004_05();

	virtual enum TVerdict doTestStepL( void );

private:


};

////////////////////
//	Test step 004.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest004_06) : public CTestStepCommDb
{
public:
	CCommDbTest004_06();
	~CCommDbTest004_06();

	virtual enum TVerdict doTestStepL( void );

private:


};

#endif //(__STEP_004_XX_H__)
