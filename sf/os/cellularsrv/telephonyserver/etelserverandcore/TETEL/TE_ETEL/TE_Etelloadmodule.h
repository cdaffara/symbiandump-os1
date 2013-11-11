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

#ifndef _TE_ETELLOADMODULE_H_
#define _TE_ETELLOADMODULE_H_

#include <etel.h>

class CTestLoadModule1 : public CEtelTestStep
	{
public:
	CTestLoadModule1() ;
	~CTestLoadModule1(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestLoadModule2 : public CEtelTestStep
	{
public:
	CTestLoadModule2() ;
	~CTestLoadModule2(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestLoadModule3 : public CEtelTestStep
	{
public:
	CTestLoadModule3() ;
	~CTestLoadModule3(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
