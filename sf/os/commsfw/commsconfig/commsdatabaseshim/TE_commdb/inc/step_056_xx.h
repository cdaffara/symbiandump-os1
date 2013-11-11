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
 
#if !defined(__STEP_056_XX_H__)
#define __STEP_056_XX_H__



NONSHARABLE_CLASS(CCommDbTest056_01 ) : public CTestStepCommDb
{
public:
	CCommDbTest056_01();
	virtual ~CCommDbTest056_01();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};


NONSHARABLE_CLASS(CCommDbTest056_02) : public CTestStepCommDb
{
public:
	CCommDbTest056_02();
	virtual ~CCommDbTest056_02();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};

#endif
