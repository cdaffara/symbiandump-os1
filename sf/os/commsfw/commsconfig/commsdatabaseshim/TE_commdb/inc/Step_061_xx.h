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
// This is the header file for CommDb test 061.XX group of scenarios 

#if (!defined __STEP_061_XX_H__)
#define __STEP_061_XX_H__


NONSHARABLE_CLASS(CCommDbTest061_xx) : public CTestStepCommDb
{
protected:
	CCommsDbTableView *OpenIspTable(CCommsDatabase* aDb, TInt &NoOfRows, TInt ExpectedCount);

};

////////////////////
//	Test step 061.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest061_01) : public CCommDbTest061_xx
{
public:
	CCommDbTest061_01();
	~CCommDbTest061_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
};

////////////////////
//	Test step 061.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest061_02) : public CCommDbTest061_xx
{
public:
	CCommDbTest061_02();
	~CCommDbTest061_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

};

////////////////////
//	Test step 061.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest061_03) : public CCommDbTest061_xx
{
public:
	CCommDbTest061_03();
	~CCommDbTest061_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

};

////////////////////
//	Test step 061.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest061_04) : public CCommDbTest061_xx
{
public:
	CCommDbTest061_04();
	~CCommDbTest061_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

};


#endif //__STEP_061_XX_H__
