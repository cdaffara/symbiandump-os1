// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
#ifndef __TSYSSTART2_PROCMON_H__
#define __TSYSSTART2_PROCMON_H__

#include <e32std.h>
#include "tsysstart2_teststep.h"

//APPFWK-SYSSTART-0081



_LIT(KCTestCaseProcMon, "ProcMon");


class CAppFwkSysStart2TestStepProcMon : public CAppFwkSysStart2TestStep
	{
	
public:
	CAppFwkSysStart2TestStepProcMon();
	virtual TVerdict doTestStepL();
	};

#endif // __TSYSSTART2_PROCMON_H__
