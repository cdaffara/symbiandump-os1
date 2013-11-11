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
#if (!defined __GETINIENUMERATE_STEP_H__)
#define __GETINIENUMERATE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_ConfiguratorSuiteStepBase.h"
#include <comms-infras/c32startcli.h>
#include "Te_ConfiguratorSuiteDefs.h"
#include <cinidata.h>

//NET-CONFIGURATOR-I-0018-HP
//Getting Info about CPM in a Configurator 
class CGetInfoCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CGetInfoCpm();
	~CGetInfoCpm();
	virtual TVerdict doTestStepL();
	};	
_LIT(KGetInfoCpm,"GetInfoCpm");

//NET-CONFIGURATOR-I-0019-HP
//Getting ini data section which should be long
class CGetLongIniInfoCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CGetLongIniInfoCpm();
	~CGetLongIniInfoCpm();
	virtual TVerdict doTestStepL();
	};	
_LIT(KGetLongIniInfoCpm,"GetLongIniInfoCpm");


//NET-CONFIGURATOR-I-0020-HP
//Getting ini data section which should multiple lines of ini data
class CGetMultiLineIniInfoCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CGetMultiLineIniInfoCpm();
	~CGetMultiLineIniInfoCpm();
	virtual TVerdict doTestStepL();
	};	
_LIT(KGetMultiLineIniInfoCpm,"GetMultiLineIniInfoCpm");

// NET-CONFIGURATOR-I-0021-HP
//Getting Info about CPM in a Configurator without CMI files 
class CGetInfoCpmWithoutCMI : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CGetInfoCpmWithoutCMI();
	~CGetInfoCpmWithoutCMI();
	virtual TVerdict doTestStepL();
	};	
_LIT(KGetInfoCpmWithoutCMI,"GetInfoCpmWithoutCMI");

// NET-CONFIGURATOR-I-00022-HP
//Getting Info about CPM with cmi which does not have iniData section in CMI file
class CGetInfoCpmWithoutINI : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CGetInfoCpmWithoutINI();
	~CGetInfoCpmWithoutINI();
	virtual TVerdict doTestStepL();
	};	
_LIT(KGetInfCpmWithoutINI,"GetInfCpmWithoutINI");

// NET-CONFIGURATOR-I-0023-HP
//Getting Info about CPM with cmi iniData overflow
//To be implemented
class CGetInfoCpmIniOverflow : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CGetInfoCpmIniOverflow ();
	~CGetInfoCpmIniOverflow ();
	virtual TVerdict doTestStepL();
	};	
_LIT(KGetInfoCpmIniOverflow,"GetInfoCpmIniOverflow");
//NET-CONFIGURATOR-I-0024-HP
//Request for The list of modules in The configurator
class CListModules : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CListModules();
	~CListModules();
	virtual TVerdict doTestStepL();
	};	
_LIT(KListModules,"ListModules");

//NET-CONFIGURATOR-I-0025-HP
//Enumerate one group and then enumerate another group
class CEnumerateGroup: public CTe_ConfiguratorSuiteStepBase
	{
public:
    CEnumerateGroup();
	~CEnumerateGroup();
	virtual TVerdict doTestStepL();
	};	
_LIT(KEnumerateGroup,"EnumerateGroup");

//NET-CONFIGURATOR-I-0026-HP
//Enumerate cpm with two cmi file which has same CPM name
class CEnumeratCpmwithTwoCmi: public CTe_ConfiguratorSuiteStepBase
	{
public:
    CEnumeratCpmwithTwoCmi();
	~CEnumeratCpmwithTwoCmi();
	virtual TVerdict doTestStepL();
	};	
_LIT(KEnumeratCpmwithTwoCmi,"EnumeratCpmwithTwoCmi");

//NET-CONFIGURATOR-I-0027-HP
//Enumerate cpm in a group which has only one CMI file
class CEnumeratCpmwithOneCmi: public CTe_ConfiguratorSuiteStepBase
	{
public:
    CEnumeratCpmwithOneCmi();
	~CEnumeratCpmwithOneCmi();
	virtual TVerdict doTestStepL();
	};	
_LIT(KEnumeratCpmwithOneCmi,"EnumeratCpmwithOneCmi");


//NET-CONFIGURATOR-I-0028-HP
//Request for The list of modules in The configurator unknown group name
class CListModulesGroupUnknown : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CListModulesGroupUnknown();
	~CListModulesGroupUnknown();
	virtual TVerdict doTestStepL();
	};	
_LIT(KListModulesGroupUnknown,"ListModulesGroupUnknown");


enum EModuleName
	{
  	EModuleNotFoundEarlier = 0,
  	EModuleFoundNow
	};	  


#endif

