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
* file te_sysutilstestpluginoverride.h
* 
*
*/




#ifndef TE_SYSUTILSTESTPLUGINOVERRIDE_H
#define TE_SYSUTILSTESTPLUGINOVERRIDE_H
#include "te_sysutilssuitestepbase.h"

_LIT(KSysUtilsTestPluginOverrideStep, "SysUtilsTestPluginOverrideStep");

class CSysUtilsTestPluginOverrideStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsTestPluginOverrideStep();
	~CSysUtilsTestPluginOverrideStep();
	virtual TVerdict doTestStepL();
	};

#endif
