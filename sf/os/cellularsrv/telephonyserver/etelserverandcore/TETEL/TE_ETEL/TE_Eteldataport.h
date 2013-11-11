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

#ifndef _TE_ETELDATAPORT_H_
#define _TE_ETELDATAPORT_H_

#include <etel.h>

class CTestDataPort1 : public CEtelTestStep
	{
public:
	CTestDataPort1() ;
	~CTestDataPort1(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestDataPort2 : public CEtelTestStep
	{
public:
	CTestDataPort2() ;
	~CTestDataPort2(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestDataPort4 : public CEtelTestStep
	{
public:
	CTestDataPort4() ;
	~CTestDataPort4(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
