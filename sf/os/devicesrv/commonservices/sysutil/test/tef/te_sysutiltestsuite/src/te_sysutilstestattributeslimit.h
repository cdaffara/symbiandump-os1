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
* file te_sysutilstestattributeslimit.h
* 
*
*/




#ifndef TE_SYSUTILSTESTATTRIBUTESLIMIT_H
#define TE_SYSUTILSTESTATTRIBUTESLIMIT_H
#include "te_sysutilssuitestepbase.h"

_LIT(KSysUtilsTestAttributesLimitStep, "SysUtilsTestAttributesLimitStep");

class CSysUtilsTestAttributesLimitStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsTestAttributesLimitStep();
	~CSysUtilsTestAttributesLimitStep();
	virtual TVerdict doTestStepL();
	};

#endif
