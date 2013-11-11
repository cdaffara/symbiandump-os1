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
#if (!defined __LOADCPM_STEP_H__)
#define __LOADCPM_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_ConfiguratorSuiteStepBase.h"
#include <comms-infras/c32startcli.h>

//NET-CONFIGURATOR-I-0001-HP
//Loading a CPM in configurator with OnDemand option 1
class CLoadCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
	CLoadCpm();
	~CLoadCpm();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpm,"LoadCpm");

//NET-CONFIGURATOR-I-0002-HP
//Loading CPM More Than one time in a Configurator

class CLoadCpmMorethanOne : public CTe_ConfiguratorSuiteStepBase
	{
public:
	CLoadCpmMorethanOne();
	~CLoadCpmMorethanOne();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmMorethanOne,"LoadCpmMorethanOne");

//NET-CONFIGURATOR-I-0003-HP
//Loading a CPM in configurator During Boot time

class CLoadCpmBootTime: public CTe_ConfiguratorSuiteStepBase
	{
public:
	CLoadCpmBootTime();
	~CLoadCpmBootTime();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmBootTime,"LoadCpmBootTime");

//NET-CONFIGURATOR-I-0004-HP
//Loading CPM in a Configurator Without CMI files

class CLoadCpmWithoutCMI : public CTe_ConfiguratorSuiteStepBase
	{
public:
    CLoadCpmWithoutCMI();
	~CLoadCpmWithoutCMI();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmWithoutCMI,"LoadCpmWithoutCMI");

//NET-CONFIGURATOR-I-0005-HP
//Loading of CPM which has different heap option

class CLoadCpmWithDiffHeapOption : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadCpmWithDiffHeapOption();
	~CLoadCpmWithDiffHeapOption();
	 virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmWithDiffHeapOption,"LoadCpmWithDiffHeapOption");

//NET-CONFIGURATOR-I-0006-HP
//Loading OnDemand CPM during Boot time

class CLoadOnDemandCpmBootTime : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadOnDemandCpmBootTime();
	~CLoadOnDemandCpmBootTime();
	 virtual TVerdict doTestStepL();
	};
_LIT(KLoadOnDemandCpmBootTime,"LoadOnDemandCpmBootTime");

//NET-CONFIGURATOR-I-0007-HP
//Load same cpm with difference session simultaneously
//To be implemented newly
class CLoadCpmDiffSession : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadCpmDiffSession();
	~CLoadCpmDiffSession();
	 virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmDiffSession,"LoadCpmDiffSession");


//NET-CONFIGURATOR-I-0008-HP
//To be implemented newly
//Load all CPMs in a group where all modules are OnDemand
class CLoadallDemandCpm : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadallDemandCpm();
	~CLoadallDemandCpm();
	 virtual TVerdict doTestStepL();
	};
_LIT(KLoadallDemandCpm,"LoadallDemandCpm");

//NET-CONFIGURATOR-I-0009-HP
//To be implemented newly
//Load CPMs of same group in different session simultaneously.
class CLoadCpmGrpDiffSession : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadCpmGrpDiffSession();
	~CLoadCpmGrpDiffSession();
	 virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmGrpDiffSession,"LoadCpmGrpDiffSession");

//NET-CONFIGURATOR-I-0010-HP
//Loading CPM With corrupted CMI files. Missed some mandatory field
class CLoadCpmCorruptCMI : public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadCpmCorruptCMI();
	~CLoadCpmCorruptCMI();
	 virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmCorruptCMI,"LoadCpmCorruptCMI");

//NET-CONFIGURATOR-I-0011-HP
//Loading CPM in a Configurator With corrupted or broken CMI files ( fields with wrong spelling Group )
class CLoadCpmBrokenCmi: public CTe_ConfiguratorSuiteStepBase
	{
public:
     CLoadCpmBrokenCmi();
	~CLoadCpmBrokenCmi();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmBrokenCmi,"LoadCpmBrokenCmi");

//NET-CONFIGURATOR-I-0012-HP
//Loading CPM in a Configurator With corrupted or broken CMI files ( fields with wrong spelling OnDemand)
class CLoadCpmCorruptOnDemand: public CTe_ConfiguratorSuiteStepBase
	{
public:
    CLoadCpmCorruptOnDemand();
	~CLoadCpmCorruptOnDemand();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmCorruptOnDemand,"LoadCpmCorruptOnDemand");

//NET-CONFIGURATOR-I-0035-HP
//Added on 23.02.2006
//Loading CPM in a Configurator, two cmi file with same worker id  
class CLoadCpmSameWorkerID: public CTe_ConfiguratorSuiteStepBase
	{
public:
    CLoadCpmSameWorkerID();
	~CLoadCpmSameWorkerID();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmSameWorkerID,"LoadCpmSameWorkerID");

class CLoadCpmBindTimeOut : public CTe_ConfiguratorSuiteStepBase
	{
public:
	CLoadCpmBindTimeOut();
	~CLoadCpmBindTimeOut();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadCpmBindTimeOut,"LoadCpmBindTimeOut");

class CLoadOnDemandBootCPMFailToLoad : public CTe_ConfiguratorSuiteStepBase
	{
public:
	CLoadOnDemandBootCPMFailToLoad();
	~CLoadOnDemandBootCPMFailToLoad();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadOnDemandBootCPMFailToLoad,"LoadOnDemandBootCPMFailToLoad");

class COnDemandCPMFailToLoad : public CTe_ConfiguratorSuiteStepBase
	{
public:
	COnDemandCPMFailToLoad();
	~COnDemandCPMFailToLoad();
	virtual TVerdict doTestStepL();
	};
_LIT(KOnDemandCPMFailToLoad,"OnDemandCPMFailToLoad");

class COnDemandCPMFailToLoadBindingOrderChanged : public CTe_ConfiguratorSuiteStepBase
	{
public:
	COnDemandCPMFailToLoadBindingOrderChanged();
	~COnDemandCPMFailToLoadBindingOrderChanged();
	virtual TVerdict doTestStepL();
	};
_LIT(KOnDemandCPMFailToLoadBindingOrderChanged,"OnDemandCPMFailToLoadBindingOrderChanged");

class CLoadOnDemandCPMOneofTwoBindingsFail : public CTe_ConfiguratorSuiteStepBase
	{
public:
	CLoadOnDemandCPMOneofTwoBindingsFail();
	~CLoadOnDemandCPMOneofTwoBindingsFail();
	virtual TVerdict doTestStepL();
	};
_LIT(KLoadOnDemandCPMOneofTwoBindingsFail,"LoadOnDemandCPMOneofTwoBindingsFail");

#endif

