/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file GpsAcqAssistUtranGpsTimeParamsStep.h
*/
#if (!defined __GPSACQASSISTRRLPGPSTIMEPARAMS_STEP_H__)
#define __GPSACQASSISTRRLPGPSTIMEPARAMS_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CGpsAcqAssistRrlpGpsTimeParamsStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CGpsAcqAssistRrlpGpsTimeParamsStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CGpsAcqAssistRrlpGpsTimeParamsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KGpsAcqAssistRrlpGpsTimeParamsStep,"GpsAcqAssistRrlpGpsTimeParamsStep");

#endif
