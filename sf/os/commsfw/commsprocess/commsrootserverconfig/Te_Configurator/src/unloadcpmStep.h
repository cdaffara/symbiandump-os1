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
#if (!defined __UNLOADCPM_STEP_H__)
#define __UNLOADCPM_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_ConfiguratorSuiteStepBase.h"
#include <comms-infras/c32startcli.h>
 #include <e32property.h>

//NET-CONFIGURATOR-I-00013-HP
//UnLoading CPM which has been loaded  prevesouly 
class CUnloadCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
	 CUnloadCpm();
	~CUnloadCpm();
	virtual TVerdict doTestStepL();
	};
_LIT(KUnloadCpm,"UnloadCpm");

//NET-CONFIGURATOR-I-0014-HP
//UnLoading CPM which has not been already loaded 
class CUnloadCpmPreNotLoaded : public CTe_ConfiguratorSuiteStepBase
	{
public:
	 CUnloadCpmPreNotLoaded();
	~CUnloadCpmPreNotLoaded();
	virtual TVerdict doTestStepL();
	};
_LIT(KUnloadCpmPreNotLoaded,"UnloadCpmPreNotLoaded");

//NET-CONFIGURATOR-I-0015-HP
//Loading and unload different combination of cpm
class CLoadUnloadDiffCombination: public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadUnloadDiffCombination();
	~CLoadUnloadDiffCombination();
	virtual TVerdict doTestStepL();
	};	
_LIT(KLoadUnloadDiffCombination,"LoadUnloadDiffCombination");

// NET-CONFIGURATOR-I-0016-HP
//Cancelling the Loading  CPM before and after loading
class CCancelLoadCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CCancelLoadCpm();
	~CCancelLoadCpm();
	virtual TVerdict doTestStepL();
	};	
_LIT(KCancelLoadCpm,"CancelLoadCpm");

//NET-CONFIGURATOR-I-0017-HP
//Cancelling the UnLoading  CPM to be implemented
class CCancelUnLoadCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CCancelUnLoadCpm();
	~CCancelUnLoadCpm();
	virtual TVerdict doTestStepL();
	};	
_LIT(KCancelUnLoadCpm,"CancelUnLoadCpm");

#endif

