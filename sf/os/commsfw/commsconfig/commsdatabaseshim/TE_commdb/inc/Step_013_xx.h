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
// This is the header file for CommDb test 013.XX group of scenarios 


#if (!defined __STEP_013_XX_H__)
#define __STEP_013_XX_H__


////////////////////
//	Test step 013.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest013_01) : public CTestStepCommDb
{
public:
	CCommDbTest013_01();
	~CCommDbTest013_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 013.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest013_02) : public CTestStepCommDb
{
public:
	CCommDbTest013_02();
	~CCommDbTest013_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 013.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest013_03) : public CTestStepCommDb
{
public:
	CCommDbTest013_03();
	~CCommDbTest013_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 013.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest013_04) : public CTestStepCommDb
{
public:
	CCommDbTest013_04();
	~CCommDbTest013_04();

	virtual enum TVerdict doTestStepL( void );
	
private:

};


#endif //(__STEP_013_XX_H__)
