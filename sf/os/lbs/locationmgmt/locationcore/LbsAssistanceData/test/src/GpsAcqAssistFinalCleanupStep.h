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
 @file GpsAcqAssistFinalCleanupStep.h
*/
#if (!defined __GPSACQASSISTCLEANUPSTEP_STEP_H__)
#define __GPSACQASSISTCLEANUPSTEP_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CGpsAcqAssistFinalCleanupStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CGpsAcqAssistFinalCleanupStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CGpsAcqAssistFinalCleanupStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	};

_LIT(KGpsAcqAssistFinalCleanupStep,"GpsAcqAssistFinalCleanupStep");

#endif
