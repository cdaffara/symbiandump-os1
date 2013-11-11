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
 @file tdbsisinstallstep.h
 @internalTechnology
*/
#ifndef __CTDBTSISINSTALL_STEP_H__
#define __CTDBTSISINSTALL_STEP_H__
#include <test/TestExecuteStepBase.h>
#include <lbs.h>
#include <lbsCommon.h>
#include <swi/launcher.h>
#include "te_lbssisinstallstepbase.h"
#include "te_lbssisinstalldefs.h"
#include "tdbtsisui.h"

class CTDBTSisInstallStep : public CTe_LbsSisInstallStepBase
	{
public:
	CTDBTSisInstallStep();
	~CTDBTSisInstallStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	void StartL(TInt aIndex);
	void SISInstallTestL(const TDesC& aSisPath);
	};

_LIT(KTDBTSisInstallStep,"SisInstallStep");

#endif //__CTDBTSISINSTALL_STEP_H__
