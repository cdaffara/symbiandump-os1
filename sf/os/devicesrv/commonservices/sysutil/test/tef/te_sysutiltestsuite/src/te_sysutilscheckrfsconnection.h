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

#ifndef TE_SYSUTILSCHECKRFSCONNECTION_H_ 
#define TE_SYSUTILSCHECKRFSCONNECTION_H_

#include "te_sysutilssuitestepbase.h"
#include <sysutil.h>

// For DiskSpaceBelowCriticalLevelL
class CSysUtilsCheckRFsConnectionDSStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsCheckRFsConnectionDSStep();
	~CSysUtilsCheckRFsConnectionDSStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	};

_LIT(KSysUtilsCheckRFsConnectionDSStep,"SysUtilsCheckRFsConnectionDS");


// For FFSSpaceBelowCriticalLevelL
class CSysUtilsCheckRFsConnectionFFSStep : public CTeSysUtilsSuiteStepBase
{
public:
	CSysUtilsCheckRFsConnectionFFSStep();
	~CSysUtilsCheckRFsConnectionFFSStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
};

_LIT(KSysUtilsCheckRFsConnectionFFSStep,"SysUtilsCheckRFsConnectionFFS");


// For MMCSpaceBelowCriticalLevelL
class CSysUtilsCheckRFsConnectionMMCStep : public CTeSysUtilsSuiteStepBase
{
public:
	CSysUtilsCheckRFsConnectionMMCStep();
	~CSysUtilsCheckRFsConnectionMMCStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
};

_LIT(KSysUtilsCheckRFsConnectionMMCStep,"SysUtilsCheckRFsConnectionMMC");

#endif /*TE_SYSUTILSCHECKRFSCONNECTION_H_*/
