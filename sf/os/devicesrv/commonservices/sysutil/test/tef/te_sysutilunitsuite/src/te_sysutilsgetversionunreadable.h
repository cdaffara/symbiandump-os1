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

#if (!defined __TE_SYSUTILSGETVERSIONUNREADABLE_H__)
#define __TE_SYSUTILSGETVERSIONUNREADABLE_H__
#include "te_sysutilsunittestepbase.h"

_LIT(KSysUtilsGetSWVersionUnreadableStep,"SysUtilsGetSWVersionUnreadable");
_LIT(KSysUtilsGetLangVersionUnreadableStep,"SysUtilsGetLangVersionUnreadable");
_LIT(KSysUtilsGetLangSWVersionUnreadableStep,"SysUtilsGetLangSWVersionUnreadable");
_LIT(KSysUtilsGetPRInformationUnreadableStep,"SysUtilsGetPRInformationUnreadable");

class CSysUtilsGetSWVersionUnreadableStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsGetSWVersionUnreadableStep();
	~CSysUtilsGetSWVersionUnreadableStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

	};

class CSysUtilsGetLangSWVersionUnreadableStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangSWVersionUnreadableStep();
~CSysUtilsGetLangSWVersionUnreadableStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

};

class CSysUtilsGetLangVersionUnreadableStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangVersionUnreadableStep();
~CSysUtilsGetLangVersionUnreadableStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

};


class CSysUtilsGetPRInformationUnreadableStep : public CTeSysUtilsSuiteStepBase
    {
public:
    CSysUtilsGetPRInformationUnreadableStep();
    ~CSysUtilsGetPRInformationUnreadableStep();
    virtual TVerdict doTestStepL();
    };


#endif
