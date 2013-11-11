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

#ifndef TE_SYSUTILSMMCSPACEBELOWCRITICALRFS_H_
#define TE_SYSUTILSMMCSPACEBELOWCRITICALRFS_H_

#include "te_sysutilssuitestepbase.h"
#include <sysutil.h>

class CSysUtilsMMCSpaceBelowCriticalRfsStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsMMCSpaceBelowCriticalRfsStep();
	~ CSysUtilsMMCSpaceBelowCriticalRfsStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	};

_LIT(KSysUtilsMMCSpaceBelowCriticalRfsStep,"SysUtilsMMCSpaceBelowCriticalRfs");

#endif /*TE_SYSUTILSMMCSPACEBELOWCRITICALRFS_H_*/
