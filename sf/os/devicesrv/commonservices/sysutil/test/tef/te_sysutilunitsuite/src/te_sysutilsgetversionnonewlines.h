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

#if (!defined __TE_SYSUTILSGETVERSIONNONEWLINESTEXT_H__)
#define __TE_SYSUTILSGETVERSIONNONEWLINESTEXT_H__
#include "te_sysutilsunittestepbase.h"

_LIT(KSysUtilsGetSWVersionNoNewLinesStep,"SysUtilsGetSWVersionNoNewLines");
_LIT(KSysUtilsGetLangVersionNoNewLinesStep,"SysUtilsGetLangVersionNoNewLines");
_LIT(KSysUtilsGetLangSWVersionNoNewLinesStep,"SysUtilsGetLangSWVersionNoNewLines");
_LIT(KSysUtilsGetPRInformationnNoNewLinesStep,"SysUtilsGetPRInformationNoNewLines");

class CSysUtilsGetSWVersionNoNewLinesStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsGetSWVersionNoNewLinesStep();
	~CSysUtilsGetSWVersionNoNewLinesStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

	};

class CSysUtilsGetLangSWVersionNoNewLinesStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangSWVersionNoNewLinesStep();
~CSysUtilsGetLangSWVersionNoNewLinesStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

};

class CSysUtilsGetPRInformationNoNewLinesStep : public CTeSysUtilsSuiteStepBase
    {
public:
    CSysUtilsGetPRInformationNoNewLinesStep();
    ~CSysUtilsGetPRInformationNoNewLinesStep();
    virtual TVerdict doTestStepL();
    };


#endif
