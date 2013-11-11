/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file checkbtpsyinromstep.h
 @internalTechnology
*/
#ifndef __CHECKBTPSYINROM_STEP_H__
#define __CHECKBTPSYINROM_STEP_H__
#include <test/TestExecuteStepBase.h>
#include <lbs.h>
#include <lbsCommon.h>
#include "te_lbssisinstallstepbase.h"
#include "te_lbssisinstalldefs.h"

#ifdef SYMBIAN_FEATURE_MANAGER
	#include <featureuids.h>
	#include <featdiscovery.h>
#endif

class CCheckBTPSYinRomStep : public CTe_LbsSisInstallStepBase
	{
public:
	CCheckBTPSYinRomStep();
	~CCheckBTPSYinRomStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void CheckBTPSYinRomL();
	};

_LIT(KCheckBTPSYinRomStep,"CheckBTPSYinRomStep");

#endif //__CHECKBTPSYINROM_STEP_H__
