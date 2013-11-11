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
 @file QualityProfileApi_ProfileNotReadStep.h
 @internalTechnology
*/
#if (!defined __QUALITYPROFILEAPI_LBSNOTRUNNING_STEP_H__)
#define __QUALITYPROFILEAPI_LBSNOTRUNNING_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_QualityProfileApiSuiteSuiteStepBase.h"

class CQualityProfileApi_ProfileNotReadStep : public CTe_QualityProfileApiSuiteSuiteStepBase
	{
public:
	CQualityProfileApi_ProfileNotReadStep();
	~CQualityProfileApi_ProfileNotReadStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KQualityProfileApi_ProfileNotReadStep,"QualityProfileApi_ProfileNotReadStep");

#endif
