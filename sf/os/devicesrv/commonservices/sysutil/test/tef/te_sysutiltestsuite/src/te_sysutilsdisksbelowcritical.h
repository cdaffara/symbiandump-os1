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

#if (!defined __TE_SYSUTILSDISKSBELOWCRITICAL_H__)
#define __TE_SYSUTILSDISKSBELOWCRITICAL_H__
#include "te_sysutilssuitestepbase.h"
#include <sysutil.h>

class CSysUtilsDisksBelowCriticalStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsDisksBelowCriticalStep();
	~CSysUtilsDisksBelowCriticalStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	void ProcessResult(TBool aResult, TInt aErr);
	};

_LIT(KSysUtilsDisksBelowCriticalStep,"SysUtilsDisksBelowCritical");

#endif
