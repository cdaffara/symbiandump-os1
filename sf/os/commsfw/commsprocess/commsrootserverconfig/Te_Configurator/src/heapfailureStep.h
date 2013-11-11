/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file loadcpmStep.h
*/
#if (!defined __HEAPFAILURE_STEP_H__)
#define __HEAPFAILURE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_ConfiguratorSuiteStepBase.h"
#include "Te_ConfiguratorSuiteServer.h"
#include <comms-infras/c32startcli.h>
#include "Te_ConfiguratorSuiteDefs.h"
#include <cinidata.h>

//NET-CONFIGURATOR-I-0029-HP
//Heap Allocation failure test for load and unload API
class CHeapAllocationFailure : public CTe_ConfiguratorSuiteStepBase
	{

public:
    CHeapAllocationFailure();
	~CHeapAllocationFailure();
    void CallOOM();
	virtual TVerdict doTestStepL();
	};

_LIT(KHeapAllocationFailure,"HeapAllocationFailure");

//NET-CONFIGURATOR-I-0030-HP
//Heap Allocation failure test for load unload  and  cancel unload API
class CHeapFailurecancelLoad : public CTe_ConfiguratorSuiteStepBase
	{

public:
    CHeapFailurecancelLoad();
	~CHeapFailurecancelLoad();
    void CancelLoad();
	virtual TVerdict doTestStepL();
	};

_LIT(KHeapFailurecancelLoad,"HeapFailurecancelLoad");

//NET-CONFIGURATOR-I-0031-HP
//Heap Allocation failure test for load unload  and  cancel unload API
class CHeapFailurecancelUnLoad : public CTe_ConfiguratorSuiteStepBase
	{

public:
    CHeapFailurecancelUnLoad();
	~CHeapFailurecancelUnLoad();
    void CancelUnLoad();
	virtual TVerdict doTestStepL();
	};

_LIT(KHeapFailurecancelUnLoad,"HeapFailurecancelUnLoad");

//NET-CONFIGURATOR-I-0032-HP
//Heap Allocation failure test for load unload  and  cancel unload API
class CHeapFailureconfigApi : public CTe_ConfiguratorSuiteStepBase
	{

public:
    CHeapFailureconfigApi();
	~CHeapFailureconfigApi();
    void ConfiguratorApis();
	virtual TVerdict doTestStepL();
	};

_LIT(KHeapFailureconfigApi,"HeapFailureconfigApi");

//NET-CONFIGURATOR-I-0033-HP
//Heap Allocation failure test for GetIni API
class CHeapFailurecancelGetIni : public CTe_ConfiguratorSuiteStepBase
	{

public:
    CHeapFailurecancelGetIni();
	~CHeapFailurecancelGetIni();
    void CancelGetIni();
	virtual TVerdict doTestStepL();
	};

_LIT(KHeapFailurecancelGetIni,"HeapFailurecancelGetIni");

//NET-CONFIGURATOR-I-0034-HP
//Heap Allocation failure test for EnumerateModules
class CHeapFailureEnumerateGrp : public CTe_ConfiguratorSuiteStepBase
	{

public:
     CHeapFailureEnumerateGrp();
	 ~CHeapFailureEnumerateGrp();
     void EnumerateGrp();
	 virtual TVerdict doTestStepL();
	};

_LIT(KHeapFailureEnumerateGrp,"HeapFailureEnumerateGrp");

#endif

