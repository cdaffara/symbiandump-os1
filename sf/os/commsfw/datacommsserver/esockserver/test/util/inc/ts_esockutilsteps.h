// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TS_ESOCKUTILSTEPS_H__)
#define __TS_ESOCKUTILSTEPS_H__

#include "esockloader.h"
#include <networking/teststep.h>

NONSHARABLE_CLASS(CTestStepESockUtil)  : public CTestStep, public MHarnessLogAdaptor
	{
public:
	void Log( TRefByValue<const TDesC16> format, ... );
	void Log( TRefByValue<const TDesC8> format, ... );
	};

NONSHARABLE_CLASS(CTestStepLoadESock)  : public CTestStepESockUtil
	{
public:
	CTestStepLoadESock();
	virtual enum TVerdict doTestStepL();
	};

NONSHARABLE_CLASS(CTestStepUnloadESock)  : public CTestStepESockUtil
	{
public:
	CTestStepUnloadESock();
	virtual enum TVerdict doTestStepL();
	};

NONSHARABLE_CLASS(CTestStepUnloadESockBase)  : public CTestStepESockUtil
	{
public:
	enum TLeak {ENoLeak, EMayLeak, ELeaks};
	CTestStepUnloadESockBase(TLeak aLeak);
	virtual enum TVerdict doTestStepL();
private:
	TLeak iLeak;
	};
	
NONSHARABLE_CLASS(CTestStepUnloadESockLeaks)  : public CTestStepUnloadESockBase
	{
public:
	CTestStepUnloadESockLeaks();
	};
	
NONSHARABLE_CLASS(CTestStepUnloadESockMayLeak)  : public CTestStepUnloadESockBase
	{
public:
	CTestStepUnloadESockMayLeak();
	};

NONSHARABLE_CLASS(CTestStepUnloadESockForced)  : public CTestStepUnloadESockBase
	{
public:
	CTestStepUnloadESockForced();
	};

NONSHARABLE_CLASS(CTestStepStopAllInterfaces)  : public CTestStepESockUtil
	{
public:
	CTestStepStopAllInterfaces();
	virtual enum TVerdict doTestStepL();
	};

NONSHARABLE_CLASS(CTestStepDefaultToConfig1)  : public CTestStepESockUtil
	{
public:
	CTestStepDefaultToConfig1();
	virtual enum TVerdict doTestStepL();
	};

class CTestStepAlwaysFail : public CTestStepESockUtil
	{
public:
	CTestStepAlwaysFail();
	virtual enum TVerdict doTestStepL();
	};

#endif //__TS_ESOCKUTILSTEPS_H__

