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

#if (!defined __TE_SYSUTILSGETVERSIONTOOMUCHTEXT_H__)
#define __TE_SYSUTILSGETVERSIONTOOMUCHTEXT_H__
#include "te_sysutilsunittestepbase.h"

class CSysUtilsGetVersionTooMuchTextStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsGetVersionTooMuchTextStep();
	~CSysUtilsGetVersionTooMuchTextStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	void ProcessError(TInt aErr);

	};

_LIT(KSysUtilsGetVersionTooMuchTextStep,"SysUtilsGetVersionTooMuchText");

#endif
