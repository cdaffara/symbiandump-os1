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
// This is the header file for CommDb test 027.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_027_XX_H__)
#define __STEP_027_XX_H__


////////////////////
//	Test step 027.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest027_01) : public CTestStepCommDb
{
public:
	CCommDbTest027_01();
	~CCommDbTest027_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};


////////////////////
//	Test step 027.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest027_02) : public CTestStepCommDb
{
public:
	CCommDbTest027_02();
	~CCommDbTest027_02();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_027_XX_H__
