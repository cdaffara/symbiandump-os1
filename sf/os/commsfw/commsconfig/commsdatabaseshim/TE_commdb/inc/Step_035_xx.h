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
// This is the header file for CommDb test 035.XX group of scenarios 


#if (!defined __STEP_035_XX_H__)
#define __STEP_035_XX_H__


////////////////////
//	Test step 035.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest035_01) : public CTestStepCommDb
{
public:
	CCommDbTest035_01();
	~CCommDbTest035_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 035.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest035_02) : public CTestStepCommDb
{
public:
	CCommDbTest035_02();
	~CCommDbTest035_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 035.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest035_03) : public CTestStepCommDb
{
public:
	CCommDbTest035_03();
	~CCommDbTest035_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 035.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest035_04) : public CTestStepCommDb
{
public:
	CCommDbTest035_04();
	~CCommDbTest035_04();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_035_XX_H__
