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
// This is the header file for CommDb test 005.XX group of scenarios 


#if (!defined __STEP_005_XX_H__)
#define __STEP_005_XX_H__


////////////////////
//	Test step 005.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest005_01) : public CTestStepCommDb
{
public:
	CCommDbTest005_01();
	~CCommDbTest005_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict	doTestStepPreambleL();

private:

};


////////////////////
//	Test step 005.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest005_02) : public CTestStepCommDb
{
public:
	CCommDbTest005_02();
	~CCommDbTest005_02();

	virtual enum TVerdict doTestStepL( void );
	TBool executeStepL();
	TVerdict	doTestStepPreambleL();

private:

};

////////////////////
//	Test step 005.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest005_03) : public CTestStepCommDb
{
public:
	CCommDbTest005_03();
	~CCommDbTest005_03();

	virtual enum TVerdict doTestStepL( void );
	TBool executeStepL();
	TVerdict	doTestStepPreambleL();

private:

};

////////////////////
//	Test step 005.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest005_04) : public CTestStepCommDb
{
public:
	CCommDbTest005_04();
	~CCommDbTest005_04();

	virtual enum TVerdict doTestStepL( void );

private:

};

#endif //(__STEP_005_XX_H__)
