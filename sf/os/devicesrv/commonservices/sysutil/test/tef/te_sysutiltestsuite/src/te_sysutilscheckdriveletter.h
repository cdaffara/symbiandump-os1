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
 @internalTechnology
*/

#ifndef TE_SYSUTILSCHECKDRIVELETTER_H_
#define TE_SYSUTILSCHECKDRIVELETTER_H_

#include "te_sysutilssuitestepbase.h"
#include <sysutil.h>


// --------------------------------------------------
//

class CSysUtilsCheckDriveLetterFFSStep : public CTeSysUtilsSuiteStepBase
{
public:
	CSysUtilsCheckDriveLetterFFSStep();
	~CSysUtilsCheckDriveLetterFFSStep();
	virtual TVerdict doTestStepL();

private:
};

_LIT(KSysUtilsCheckDriveLetterFFSStep,"SysUtilsCheckDriveLetterFFS");


// --------------------------------------------------
//


class CSysUtilsCheckDriveLetterMMCStep : public CTeSysUtilsSuiteStepBase
{
public:
	CSysUtilsCheckDriveLetterMMCStep();
	~CSysUtilsCheckDriveLetterMMCStep();
	virtual TVerdict doTestStepL();

private:
};

_LIT(KSysUtilsCheckDriveLetterMMCStep,"SysUtilsCheckDriveLetterMMC");

#endif // TE_SYSUTILSCHECKDRIVELETTER_H_ 


