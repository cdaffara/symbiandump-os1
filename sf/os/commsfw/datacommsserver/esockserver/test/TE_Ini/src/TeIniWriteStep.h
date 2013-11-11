// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TEINI_WRITE_STEP_H__)
#define __TEINI_WRITE_STEP_H__
#include <test/testexecutestepbase.h>
#include <es_ini.h>
#include "TeIniServer.h"

class CTestIniWriteStep : public CTestStep
	{
public:
	CTestIniWriteStep();
	~CTestIniWriteStep();
	virtual TVerdict	doTestStepL();

private:
	CESockIniData*		iESockIniData;
	};

_LIT(KWriteToIniFile,"WriteToIniFile");

#endif

