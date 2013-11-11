/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file GpsIonosphericModelFinalCheckAndCleanupStep.h
*/
#if (!defined __GPSIONOSPHERICMODELFINALCHECKANDCLEANUP_STEP_H__)
#define __GPSIONOSPHERICMODELFINALCHECKANDCLEANUP_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CGpsIonosphericModelFinalCheckAndCleanupStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CGpsIonosphericModelFinalCheckAndCleanupStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CGpsIonosphericModelFinalCheckAndCleanupStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	};

_LIT(KGpsIonosphericModelFinalCheckAndCleanupStep,"GpsIonosphericModelFinalCheckAndCleanupStep");

#endif
