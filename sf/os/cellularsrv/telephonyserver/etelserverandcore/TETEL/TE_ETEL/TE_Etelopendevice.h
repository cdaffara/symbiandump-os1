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

#ifndef _TE_ETELOPENDEVICE_H_
#define _TE_ETELOPENDEVICE_H_

#include <etel.h>

class CTestOpenPhoneFrServ : public CEtelTestStep
	{
public:
	CTestOpenPhoneFrServ() ;
	~CTestOpenPhoneFrServ(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenLineFrServ : public CEtelTestStep
	{
public:
	CTestOpenLineFrServ() ;
	~CTestOpenLineFrServ(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenCallFrServ : public CEtelTestStep
	{
public:
	CTestOpenCallFrServ() ;
	~CTestOpenCallFrServ(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenCallFrPhone : public CEtelTestStep
	{
public:
	CTestOpenCallFrPhone() ;
	~CTestOpenCallFrPhone(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenLineFrPhone : public CEtelTestStep
	{
public:
	CTestOpenLineFrPhone() ;
	~CTestOpenLineFrPhone(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenCallFrLine : public CEtelTestStep
	{
public:
	CTestOpenCallFrLine() ;
	~CTestOpenCallFrLine(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenFaxFrCall : public CEtelTestStep
	{
public:
	CTestOpenFaxFrCall() ;
	~CTestOpenFaxFrCall(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
