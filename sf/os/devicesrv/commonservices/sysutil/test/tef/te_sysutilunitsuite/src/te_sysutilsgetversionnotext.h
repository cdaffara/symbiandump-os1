/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#if (!defined __TE_SYSUTILSGETVERSIONNOTEXT_H__)
#define __TE_SYSUTILSGETVERSIONNOTEXT_H__
#include "te_sysutilsunittestepbase.h"

_LIT(KSysUtilsGetSWVersionNoTextStep,"SysUtilsGetSWVersionNoText");
_LIT(KSysUtilsGetLangVersionNoTextStep,"SysUtilsGetLangVersionNoText");
_LIT(KSysUtilsGetLangSWVersionNoTextStep,"SysUtilsGetLangSWVersionNoText");
_LIT(KSysUtilsGetPRInformationNoTextStep,"SysUtilsGetPRInformationNoText");

class CSysUtilsGetSWVersionNoTextStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsGetSWVersionNoTextStep();
	~CSysUtilsGetSWVersionNoTextStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

	};

class CSysUtilsGetLangSWVersionNoTextStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangSWVersionNoTextStep();
~CSysUtilsGetLangSWVersionNoTextStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

};

class CSysUtilsGetLangVersionNoTextStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangVersionNoTextStep();
~CSysUtilsGetLangVersionNoTextStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

};

class CSysUtilsGetPRInformationNoTextStep : public CTeSysUtilsSuiteStepBase
    {
public:
    CSysUtilsGetPRInformationNoTextStep();
    ~CSysUtilsGetPRInformationNoTextStep();
    virtual TVerdict doTestStepL();
    };

#endif
