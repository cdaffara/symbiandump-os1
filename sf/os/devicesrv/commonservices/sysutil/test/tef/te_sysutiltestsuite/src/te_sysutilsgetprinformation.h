/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file 
 @test
 @internalTechnology
*/

#ifndef __TE_SYSUTILSGETPRINFORMATION_H__
#define __TE_SYSUTILSGETPRINFORMATION_H__
#include "te_sysutilssuitestepbase.h"
#include <sysutil.h>

class CSysUtilsGetPRInformationStep : public CTeSysUtilsSuiteStepBase
	{
public:
    CSysUtilsGetPRInformationStep();
	~CSysUtilsGetPRInformationStep();
	TVerdict doTestStepL();
	};

_LIT(KSysUtilsGetPRInformationStep,"SysUtilsGetPRInformation");

#endif
