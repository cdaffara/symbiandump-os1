// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef _TE_ETELCOREDSTDNC_H_
#define _TE_ETELCOREDSTDNC_H_

#include <etel.h>

class CTestNcTermFaxSess : public CEtelTestStep
	{
public:
	CTestNcTermFaxSess() ;
	~CTestNcTermFaxSess(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestTermFaxSess1 : public CEtelTestStep
	{
public:
	CTestTermFaxSess1() ;
	~CTestTermFaxSess1(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestTermFaxSess2 : public CEtelTestStep
	{
public:
	CTestTermFaxSess2() ;
	~CTestTermFaxSess2(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNcCancelLineCall : public CEtelTestStep
	{
public:
	CTestNcCancelLineCall() ;
	~CTestNcCancelLineCall(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCancelNoReq : public CEtelTestStep
	{
public:
	CTestCancelNoReq() ;
	~CTestCancelNoReq(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNcCancelPhone : public CEtelTestStep
	{
public:
	CTestNcCancelPhone() ;
	~CTestNcCancelPhone(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestLoadTwice : public CEtelTestStep
	{
public:
	CTestLoadTwice() ;
	~CTestLoadTwice(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNcCancelWrFax : public CEtelTestStep
	{
public:
	CTestNcCancelWrFax() ;
	~CTestNcCancelWrFax(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetNcFns : public CEtelTestStep
	{
public:
	CTestGetNcFns() ;
	~CTestGetNcFns(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
