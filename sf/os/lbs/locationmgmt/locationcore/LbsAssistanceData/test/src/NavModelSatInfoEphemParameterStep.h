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
 @file NavModelSatInfoEphemParameterStep.h
*/
#if (!defined __NAVMODELSATINFOEPHEMPARAMETER_STEP_H__)
#define __NAVMODELSATINFOEPHEMPARAMETER_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CNavModelSatInfoEphemParameterStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CNavModelSatInfoEphemParameterStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CNavModelSatInfoEphemParameterStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	};

_LIT(KNavModelSatInfoEphemParameterStep,"NavModelSatInfoEphemParameterStep");

#endif
