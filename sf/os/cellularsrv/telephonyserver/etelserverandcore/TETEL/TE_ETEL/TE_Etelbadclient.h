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

#ifndef _TE_ETELBADCLIENT_H_
#define _TE_ETELBADCLIENT_H_

#include <etel.h>
#include <e32base.h>

#include "TE_EtelBase.h"

class CTestBadClient1 : public CEtelTestStep
	{
public:
	CTestBadClient1() ;
	~CTestBadClient1(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient1a : public CEtelTestStep
	{
public:
	CTestBadClient1a() ;
	~CTestBadClient1a(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient1b : public CEtelTestStep
	{
public:
	CTestBadClient1b() ;
	~CTestBadClient1b(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient2 : public CEtelTestStep
	{
public:
	CTestBadClient2() ;
	~CTestBadClient2(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient2a : public CEtelTestStep
	{
public:
	CTestBadClient2a() ;
	~CTestBadClient2a(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient3 : public CEtelTestStep
	{
public:
	CTestBadClient3() ;
	~CTestBadClient3(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient3a : public CEtelTestStep
	{
public:
	CTestBadClient3a() ;
	~CTestBadClient3a(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient4 : public CEtelTestStep
	{
public:
	CTestBadClient4() ;
	~CTestBadClient4(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient5 : public CEtelTestStep
	{
public:
	CTestBadClient5() ;
	~CTestBadClient5(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient6 : public CEtelTestStep
	{
public:
	CTestBadClient6() ;
	~CTestBadClient6(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestBadClient8 : public CEtelTestStep
	{
public:
	CTestBadClient8() ;
	~CTestBadClient8(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
