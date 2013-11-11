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
// This is the header file for CommDb test 044.XX group of scenarios 

#if (!defined __STEP_044_XX_H__)
#define __STEP_044_XX_H__


////////////////////
//	Test step 044.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest044_01) : public CTestStepCommDb
{
public:
	CCommDbTest044_01();
	~CCommDbTest044_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 044.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest044_02) : public CTestStepCommDb
{
public:
	CCommDbTest044_02();
	~CCommDbTest044_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 044.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest044_03) : public CTestStepCommDb
{
public:
	CCommDbTest044_03();
	~CCommDbTest044_03();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_044_XX_H__
