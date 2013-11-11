/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* file te_sysutiltestpswritepolicy.h
* 
*
*/



#include "te_sysutilcapabilitybase.h"

_LIT(KSysUtilTestPSWritePolicyStep, "SysUtilTestPSWritePolicyStep");

class CSysUtilTestPSWritePolicyStep : public CTeSysUtilSuiteStepBase
	{
public:
	CSysUtilTestPSWritePolicyStep();
	~CSysUtilTestPSWritePolicyStep();
	virtual TVerdict doTestStepL();
private:
	void ProcessError(const TDesC& aProperty, TInt aError, TInt aExpectedError);
	};
