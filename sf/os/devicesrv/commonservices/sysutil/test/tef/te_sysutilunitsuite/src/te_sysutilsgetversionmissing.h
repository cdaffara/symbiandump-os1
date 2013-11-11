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

#if (!defined __TE_SYSUTILSGETVERSIONMISSING_H__)
#define __TE_SYSUTILSGETVERSIONMMISSING_H__
#include "te_sysutilsunittestepbase.h"

_LIT(KSysUtilsGetSWVersionMissingStep,"SysUtilsGetSWVersionMissing");
_LIT(KSysUtilsGetLangVersionMissingStep,"SysUtilsGetLangSWVersionMissing");
_LIT(KSysUtilsGetLangSWVersionMissingStep,"SysUtilsGetLangVersionMissing");
_LIT(KSysUtilsGetPRInformationMissingStep,"SysUtilsGetPRInformationMissing");

class CSysUtilsGetSWVersionMissingStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsGetSWVersionMissingStep();
	~CSysUtilsGetSWVersionMissingStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	void ProcessError(TInt aErr);

	};


class CSysUtilsGetLangVersionMissingStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangVersionMissingStep();
~CSysUtilsGetLangVersionMissingStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
void ProcessError(TInt aErr);

};

class CSysUtilsGetLangSWVersionMissingStep : public CTeSysUtilsSuiteStepBase
{
public:
CSysUtilsGetLangSWVersionMissingStep();
~CSysUtilsGetLangSWVersionMissingStep();
virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:

};

class CSysUtilsGetPRInformationMissingStep : public CTeSysUtilsSuiteStepBase
    {
public:
    CSysUtilsGetPRInformationMissingStep();
    ~CSysUtilsGetPRInformationMissingStep();
    virtual TVerdict doTestStepL();
    };


#endif
