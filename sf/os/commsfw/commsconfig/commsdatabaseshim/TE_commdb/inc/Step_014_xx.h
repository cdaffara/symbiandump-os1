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
// This is the header file for CommDb test 014.XX group of scenarios 


#if (!defined __STEP_014_XX_H__)
#define __STEP_014_XX_H__


////////////////////
//	Test step 014.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest014_01) : public CTestStepCommDb
{
public:
	CCommDbTest014_01();
	~CCommDbTest014_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 014.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest014_02) : public CTestStepCommDb
{
public:
	CCommDbTest014_02();
	~CCommDbTest014_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 014.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest014_03) : public CTestStepCommDb
{
public:
	CCommDbTest014_03();
	~CCommDbTest014_03();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 014.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest014_04) : public CTestStepCommDb
{
public:
	CCommDbTest014_04();
	~CCommDbTest014_04();

	virtual enum TVerdict doTestStepL( void );


private:

};

#endif //(__STEP_014_XX_H__)
