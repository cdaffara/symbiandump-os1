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

/**
 @file
 @internalComponent 
*/


#ifndef _TE_GPRS_H_
#define _TE_GPRS_H_

#include "Te_GprsTestStepBase.h"
#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include "f32file.h"
#include <etel.h>
//#include <autotest.h>
#include <es_sock.h>
#include <in_sock.h>
#include <etelqos.h>
#include <pcktcs.h>
#include "Te_Gprscfgfile.h"


//Test Checking Macros
#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (code != expected ) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_EXPR(boolexpr,msg) {\
	TEST((boolexpr));\
	if (EFalse == (boolexpr)) INFO_PRINTF1(msg);\
	}


// LITs used for ini file.
_LIT(KTestCfgHTTPServerIpAddress,"TestCfgHTTPServerIpAddress");
_LIT(KTestCfgHTTPPageAddress,"TestCfgHTTPPageAddress");
_LIT(KTestCfgPhoneMake, "TestCfgPhoneMake");
_LIT(KTestCfgPhoneMotorola, "Motorola");
_LIT(KTestCfgPhoneEricsson, "Ericsson");

const TInt KOneSecond = 1000000;
const TInt KFiveSeconds = KOneSecond * 5;

class CInitGlobals : public CGprsTestStep
	{
public:
	CInitGlobals() ;
	~CInitGlobals(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsDefaultContextParams : public CGprsTestStep
	{
public:
	CTestGprsDefaultContextParams() ;
	~CTestGprsDefaultContextParams(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsUnSupported : public CGprsTestStep
	{
public:
	CTestGprsUnSupported() ;
	~CTestGprsUnSupported(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsContextAttach : public CGprsTestStep
	{
public:
	CTestGprsContextAttach() ;
	~CTestGprsContextAttach(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsContextDelete : public CGprsTestStep
	{
public:
	CTestGprsContextDelete() ;
	~CTestGprsContextDelete(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsContextLoanComport : public CGprsTestStep
	{
public:
	CTestGprsContextLoanComport() ;
	~CTestGprsContextLoanComport(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsContextUnSupported : public CGprsTestStep
	{
public:
	CTestGprsContextUnSupported() ;
	~CTestGprsContextUnSupported(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsAttach : public CGprsTestStep
	{
public:
	CTestGprsAttach() ;
	~CTestGprsAttach(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsNotification : public CGprsTestStep
	{
public:
	CTestGprsNotification() ;
	~CTestGprsNotification(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsMsClass : public CGprsTestStep
	{
public:
	CTestGprsMsClass() ;
	~CTestGprsMsClass(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsContextConfig : public CGprsTestStep
	{
public:
	CTestGprsContextConfig() ;
	~CTestGprsContextConfig(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsContextConfigNotifications : public CGprsTestStep
	{
public:
	CTestGprsContextConfigNotifications() ;
	~CTestGprsContextConfigNotifications(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenGprsMaxContexts : public CGprsTestStep
	{
public:
	CTestOpenGprsMaxContexts() ;
	~CTestOpenGprsMaxContexts(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsCancel : public CGprsTestStep
	{
public:
	CTestGprsCancel() ;
	~CTestGprsCancel(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsfaxCancel : public CGprsTestStep
	{
public:
	CTestGprsfaxCancel() ;
	~CTestGprsfaxCancel(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenContextByName : public CGprsTestStep
	{
public:
	CTestOpenContextByName() ;
	~CTestOpenContextByName(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestNetworkRegStatus : public CGprsTestStep
	{
public:
	CTestNetworkRegStatus() ;
	~CTestNetworkRegStatus(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGetStaticCaps : public CGprsTestStep
	{
public:
	CTestGetStaticCaps() ;
	~CTestGetStaticCaps(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestOpenExistingContext : public CGprsTestStep
	{
public:
	CTestOpenExistingContext() ;
	~CTestOpenExistingContext(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestGprsDownloadWebPage : public CGprsTestStep
	{
public:
	CTestGprsDownloadWebPage() ;
	~CTestGprsDownloadWebPage(){} ;
	virtual enum TVerdict doTestStepL();
private:
	TInt ListRoutes(RSocket& aSocket);
	void WaitWithTimeout(TRequestStatus& aStatus);
	};

class CTest2GprsConnection : public CGprsTestStep
	{
public:
	CTest2GprsConnection() ;
	~CTest2GprsConnection(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
