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
// This is the header file for CommDb test 006.XX group of scenarios 


#if (!defined __STEP_006_XX_H__)
#define __STEP_006_XX_H__


////////////////////
//	Test step 006.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest006_01) : public CTestStepCommDb
{
public:
	CCommDbTest006_01();
	~CCommDbTest006_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 006.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest006_02) : public CTestStepCommDb
{
public:
	CCommDbTest006_02();
	~CCommDbTest006_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 006.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest006_03) : public CTestStepCommDb
{
public:
	CCommDbTest006_03();
	~CCommDbTest006_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 006.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest006_04) : public CTestStepCommDb
{
public:
	CCommDbTest006_04();
	~CCommDbTest006_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 006.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest006_05) : public CTestStepCommDb
{
public:
	CCommDbTest006_05();
	~CCommDbTest006_05();

	virtual enum TVerdict doTestStepL( void );

private:

};

#endif //(__STEP_006_XX_H__)
