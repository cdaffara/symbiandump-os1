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
 
#if !defined(__STEP_059_XX_H__)
#define __STEP_059_XX_H__



NONSHARABLE_CLASS(CCommDbTest059_01 ) : public CTestStepCommDb
{
public:
	CCommDbTest059_01();
	virtual ~CCommDbTest059_01();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};

//PAnic Test
NONSHARABLE_CLASS(CCommDbTest060_01 ) : public CTestStepCommDb
{
public:
	CCommDbTest060_01();
	virtual ~CCommDbTest060_01();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};

NONSHARABLE_CLASS(CCommDbTest060_02 ) : public CTestStepCommDb
{
public:
	CCommDbTest060_02();
	virtual ~CCommDbTest060_02();
	virtual enum TVerdict doTestStepL( void );	
	void ProtectISPTableL(CCommsDatabase*);
	void RemoveProtectionL(CCommsDatabase*);
};
#endif
