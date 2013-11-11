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
// This is the header file for CommDb test 025.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_025_XX_H__)
#define __STEP_025_XX_H__


////////////////////
//	Test step 025.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest025_01) : public CTestStepCommDb
{
public:
	CCommDbTest025_01();
	~CCommDbTest025_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};


////////////////////
//	Test step 025.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest025_02) : public CTestStepCommDb
{
public:
	CCommDbTest025_02();
	~CCommDbTest025_02();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_025_XX_H__
