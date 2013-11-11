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
// This is the header file for CommDb test 011.XX group of scenarios 


#if (!defined __STEP_011_XX_H__)
#define __STEP_011_XX_H__


////////////////////
//	Test step 011.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest011_01) : public CTestStepCommDb
{
public:
	CCommDbTest011_01();
	~CCommDbTest011_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

///////////////////
//	Test step 011.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest011_02) : public CTestStepCommDb
{
public:
	CCommDbTest011_02();
	~CCommDbTest011_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

///////////////////
//	Test step 011.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest011_03) : public CTestStepCommDb
{
public:
	CCommDbTest011_03();
	~CCommDbTest011_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


///////////////////
//	Test step 011.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest011_04) : public CTestStepCommDb
{
public:
	CCommDbTest011_04();
	~CCommDbTest011_04();

	enum TVerdict doTestStepL( void );

private:

};

#endif //(__STEP_011_XX_H__)
