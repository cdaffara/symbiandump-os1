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

#ifndef _TE_ETELCALLOWNER_H_
#define _TE_ETELCALLOWNER_H_

#include <etel.h>

class CTestCallOwner1 : public CEtelTestStep
	{
public:
	CTestCallOwner1() ;
	~CTestCallOwner1(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCallOwner2 : public CEtelTestStep
	{
public:
	CTestCallOwner2() ;
	~CTestCallOwner2(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestCallOwnerCliPanicA : public CEtelTestStep
	{
public:
	CTestCallOwnerCliPanicA() ;
	~CTestCallOwnerCliPanicA(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
