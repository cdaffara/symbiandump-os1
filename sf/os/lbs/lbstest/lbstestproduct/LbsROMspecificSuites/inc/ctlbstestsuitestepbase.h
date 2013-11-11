/*
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
 @file 
 @internalTechnology
 @test
*/

#ifndef __CT_LBSTESTSUITE_STEP_BASE__
#define __CT_LBSTESTSUITE_STEP_BASE__
#include <test/TestExecuteStepBase.h>
#include "ctlbstestsuitedefs.h"

#include <lbs.h>
#include <lbserrors.h>
#include <lbscommon.h>
#include <lbssatellite.h>
#include <lbspositioninfo.h>
#include <epos_cposmodules.h>
#include <epos_cposmoduleidlist.h>
#include <epos_cposmoduleupdate.h>

class CTe_TestSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_TestSuiteStepBase();
	CTe_TestSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
    CActiveScheduler*   iScheduler;

	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
    RPositionServer iServer;
    RPositioner     iPositioner;
    CPosModules*    iModulesSettings;
    
    TRequestStatus iStatus;
	
	void CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel = KFailedEqualCheck);

	};
#endif
