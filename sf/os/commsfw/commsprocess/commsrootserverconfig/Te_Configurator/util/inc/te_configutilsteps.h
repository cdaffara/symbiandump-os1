// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TE_CONFIGUTILSTEPS_H__)
#define __TE_CONFIGUTILSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_configutilserver.h"
#include "configloader.h"
#include <cfshared.h>

class CEasyTestStep : public CTestStep
/**
This class makes things easier, the test steps doesnt have to
explicitely call SetTestStepResult, they just return the result.
*/
	{
public:
	virtual TVerdict doTestStepPreambleL()
		{
		MergeVerdict(easyTestStepPreambleL());
		return TestStepResult();
		}
	virtual TVerdict doTestStepL()
		{
		MergeVerdict(easyTestStepL());
		return TestStepResult();
		}
	virtual TVerdict doTestStepPostambleL()
		{
		MergeVerdict(easyTestStepPostambleL());
		return TestStepResult();
		}

	virtual TVerdict easyTestStepPreambleL()
		{ return EPass; }

	virtual TVerdict easyTestStepL() = 0;
	virtual TVerdict easyTestStepPostambleL()
		{ return EPass; }

	void MergeVerdict(TVerdict aVerdict)
		{
		if(TestStepResult() == EPass)
			{
			SetTestStepResult(aVerdict);
			}
		}
	};

class CTestStepConfigUtil : public CTestStep, public MHarnessLogAdaptor
	{
public:
	void Log( TRefByValue<const TDesC16> format, ... );
	void Log( TRefByValue<const TDesC8> format, ... );
	};

class CTestStepLoadConfig : public CTestStepConfigUtil
	{
public:
	CTestStepLoadConfig();
	virtual TVerdict doTestStepL();
	};

class CTestStepUnLoadConfig : public CTestStepConfigUtil
	{
public:
	CTestStepUnLoadConfig(CommsFW::TCFShutdownType aType);
	virtual TVerdict doTestStepL();
private:
	CommsFW::TCFShutdownType iType;
	};


_LIT(KTestStepLoadConfig,"TE_ConfigUtilLoadConfig");
_LIT(KTestStepUnLoadConfig,"TE_ConfigUtilUnLoadConfig");
_LIT(KTestStepUnLoadConfigForced,"TE_ConfigUtilUnLoadConfigForced");

#endif // __TE_C32UTILSTEPS_H__


