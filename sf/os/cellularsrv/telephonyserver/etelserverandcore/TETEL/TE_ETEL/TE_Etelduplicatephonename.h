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

#ifndef _TE_ETELDUPLICATEPHONENAME_H_
#define _TE_ETELDUPLICATEPHONENAME_H_

#include <etel.h>

class CTestDuplicatePhoneName : public CEtelTestStep
	{
public:
	CTestDuplicatePhoneName() ;
	~CTestDuplicatePhoneName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenCallNoName : public CEtelTestStep
	{
public:
	CTestOpenCallNoName() ;
	~CTestOpenCallNoName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestVersion : public CEtelTestStep
	{
public:
	CTestVersion() ;
	~CTestVersion(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestLoadTsySamePhoneName : public CEtelTestStep
	{
public:
	CTestLoadTsySamePhoneName() ;
	~CTestLoadTsySamePhoneName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenPhoneSamePhoneName : public CEtelTestStep
	{
public:
	CTestOpenPhoneSamePhoneName() ;
	~CTestOpenPhoneSamePhoneName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenPhoneModeIdentPhoneName : public CEtelTestStep
	{
public:
	CTestOpenPhoneModeIdentPhoneName() ;
	~CTestOpenPhoneModeIdentPhoneName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestStaticCallPoolTsy : public CEtelTestStep
	{
public:
	CTestStaticCallPoolTsy() ;
	~CTestStaticCallPoolTsy(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
