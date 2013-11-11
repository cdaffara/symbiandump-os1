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

#ifndef _TE_ETELCOREDSTD_H_
#define _TE_ETELCOREDSTD_H_

#include <etel.h>

class CTestOpenPhoneFc : public CEtelTestStep
	{
public:
	CTestOpenPhoneFc() ;
	~CTestOpenPhoneFc(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenAllSubSessInFc : public CEtelTestStep
	{
public:
	CTestOpenAllSubSessInFc() ;
	~CTestOpenAllSubSessInFc(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenAllSubSess : public CEtelTestStep
	{
public:
	CTestOpenAllSubSess() ;
	~CTestOpenAllSubSess(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestServerOpenClose : public CEtelTestStep
	{
public:
	CTestServerOpenClose() ;
	~CTestServerOpenClose(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestUnload : public CEtelTestStep
	{
public:
	CTestUnload() ;
	~CTestUnload(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenMoreSubSess : public CEtelTestStep
	{
public:
	CTestOpenMoreSubSess() ;
	~CTestOpenMoreSubSess(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNotify : public CEtelTestStep
	{
public:
	CTestNotify() ;
	~CTestNotify(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNotify2Client : public CEtelTestStep
	{
public:
	CTestNotify2Client() ;
	~CTestNotify2Client(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
