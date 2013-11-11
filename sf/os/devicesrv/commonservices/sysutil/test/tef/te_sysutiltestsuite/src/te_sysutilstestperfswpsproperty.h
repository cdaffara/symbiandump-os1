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
* file te_sysutilstestperfswpsproperty.h
* 
*
*/




#ifndef TE_SYSUTILSTESTPERFSWPSPROPERTY_H
#define TE_SYSUTILSTESTPERFSWPSPROPERTY_H
#include "te_sysutilssuitestepbase.h"

_LIT(KSysUtilsTestPerfSWPSPropertyStep, "SysUtilsTestPerfSWPSPropertyStep");

class CSysUtilsTestPerfSWPSPropertyStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsTestPerfSWPSPropertyStep();
	~CSysUtilsTestPerfSWPSPropertyStep();
	virtual TVerdict doTestStepL();
	TReal TimeFuncCall();
	};

#endif
