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

#ifndef _TE_ETELMEMBERFN_H_
#define _TE_ETELMEMBERFN_H_

#include <etel.h>

class CTestMemberFnSetGet : public CEtelTestStep
	{
public:
	CTestMemberFnSetGet() ;
	~CTestMemberFnSetGet(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestPhoneMemberFn : public CEtelTestStep
	{
public:
	CTestPhoneMemberFn() ;
	~CTestPhoneMemberFn(){} ;
	virtual enum TVerdict doTestStepL();

	};

class CTestLineMemberFn : public CEtelTestStep
	{
public:
	CTestLineMemberFn() ;
	~CTestLineMemberFn(){} ;
	virtual enum TVerdict doTestStepL();

	};

class CTestCallMemberFn : public CEtelTestStep
	{
public:
	CTestCallMemberFn() ;
	~CTestCallMemberFn(){} ;
	virtual enum TVerdict doTestStepL();

	};

class CTestFaxMemberFn : public CEtelTestStep
	{
public:
	CTestFaxMemberFn() ;
	~CTestFaxMemberFn(){} ;
	virtual enum TVerdict doTestStepL();

	};

#endif
