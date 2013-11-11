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

#if (!defined __TE_ESOCKUTILSTEPS_H__)
#define __TE_ESOCKUTILSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_esockutilserver.h"
#include "esockloader.h"
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

class CTestStepESockUtil : public CTestStep, public MHarnessLogAdaptor
	{
public:
	void Log( TRefByValue<const TDesC16> format, ... );
	void Log( TRefByValue<const TDesC8> format, ... );
	};

class CTestStepLoadESock : public CTestStepESockUtil
	{
public:
	CTestStepLoadESock();
	virtual TVerdict doTestStepL();
	};

class CTestStepUnloadESock : public CTestStepESockUtil
	{
public:
	enum TLeak {ENoLeak, EMayLeak, ELeaks};
	CTestStepUnloadESock(CommsFW::TCFShutdownType aType, TLeak aLeak, TBool aExpectTimeout);
	virtual TVerdict doTestStepL();
private:
	CommsFW::TCFShutdownType iType;
	TLeak iLeak;
	TBool iExpectTimeout;
	};

class CTestStepStopAllInterfaces : public CTestStepESockUtil
	{
public:
	CTestStepStopAllInterfaces();
	virtual TVerdict doTestStepL();
	};

class CTestStepDefaultToConfig1 : public CTestStepESockUtil
	{
public:
	CTestStepDefaultToConfig1();
	virtual TVerdict doTestStepL();
	};


//from genconnn test 6_2
class CTestStepIsNetworkNotActive : public CTestStepESockUtil
	{
public:
	CTestStepIsNetworkNotActive();
	virtual TVerdict doTestStepL();
	};


class CTestStepDelay : public CTestStepESockUtil
	{
public:
	CTestStepDelay();
	virtual TVerdict doTestStepL();
	};

class CTestStepAlwaysFail : public CTestStepESockUtil
	{
public:
	CTestStepAlwaysFail();
	virtual TVerdict doTestStepL();
	};
	
class CTestStepResetAllocFails : public CTestStepESockUtil
	{
public:
	CTestStepResetAllocFails();
	virtual TVerdict doTestStepL();
	};
	
class CTestStepLoadConfig : public CTestStepESockUtil
	{
public:
	CTestStepLoadConfig();
	virtual TVerdict doTestStepL();
	TInt UpdateIniL(const TDesC &aFileSpec, const TDesC &aSectName, const TDesC &aKeyName, const TDesC &aValue);
	};
	
class CTestStepCopyFiles : public CTestStepESockUtil
	{
public:
	CTestStepCopyFiles();
	virtual TVerdict doTestStepL();
	};


_LIT(KTestStepLoadESock,"TE_ESockUtilLoadESock");
_LIT(KTestStepUnloadESock,"TE_ESockUtilUnloadESock");
_LIT(KTestStepUnloadESockForced,"TE_ESockUtilUnloadESockForced");
_LIT(KTestStepUnloadESockForcedTimeout,"TE_ESockUtilUnloadESockForcedTimeout");
_LIT(KTestStepUnloadESock_Leaks,"TE_ESockUtilUnloadESock_Leaks");
_LIT(KTestStepUnloadESock_MayLeak,"TE_ESockUtilUnloadESock_MayLeak");
_LIT(KTestStepStopAllInterfaces,"TE_ESockUtilStopAllInterfaces");
_LIT(KTestStepDefaultToConfig1, "TE_ESockDefaultToConfig1");
_LIT(KTestStepIsNetworkNotActive, "TE_ESockUtilIsNetworkNotActive");
_LIT(KTestStepDelay, "TE_ESockUtilDelay");
_LIT(KTestStepAlwaysFail, "TE_ESockUtilAlwaysFail");
_LIT(KTestStepResetAllocFails, "TE_ESockUtilResetAllocFails");
_LIT(KTestStepLoadConfig, "LoadConfig");
_LIT(KTestStepCopyFiles, "CopyFiles");

#endif // __TE_ESOCKUTILSTEPS_H__


