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

#ifndef _TE_ETELRELINQUISH_H_
#define _TE_ETELRELINQUISH_H_

#include <etel.h>

class CTestRelinquish : public CEtelTestStep
	{
public:
	CTestRelinquish() ;
	~CTestRelinquish(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestRelinquishOwner : public CEtelTestStep
	{
public:
	CTestRelinquishOwner() ;
	~CTestRelinquishOwner(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestRelinquishCase1 : public CEtelTestStep
	{
public:
	CTestRelinquishCase1() ;
	~CTestRelinquishCase1(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
