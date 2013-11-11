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

#ifndef _TE_ETELCANCELSERVICE_H_
#define _TE_ETELCANCELSERVICE_H_

#include <etel.h>

class CTestCancelService1 : public CEtelTestStep
	{
public:
	CTestCancelService1() ;
	~CTestCancelService1(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCancelService2 : public CEtelTestStep
	{
public:
	CTestCancelService2() ;
	~CTestCancelService2(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCancelService3 : public CEtelTestStep
	{
public:
	CTestCancelService3() ;
	~CTestCancelService3(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCancelService4 : public CEtelTestStep
	{
public:
	CTestCancelService4() ;
	~CTestCancelService4(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCancelService6 : public CEtelTestStep
	{
public:
	CTestCancelService6() ;
	~CTestCancelService6(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCancelService7 : public CEtelTestStep
	{
public:
	CTestCancelService7() ;
	~CTestCancelService7(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
