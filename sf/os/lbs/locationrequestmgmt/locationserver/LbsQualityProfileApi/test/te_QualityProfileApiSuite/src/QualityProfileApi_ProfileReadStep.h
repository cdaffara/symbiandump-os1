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
 @file QualityProfileApi_ProfileReadStep.h
 @internalTechnology
*/
#if (!defined __QUALITYPROFILEAPI_LBSRUNNING_STEP_H__)
#define __QUALITYPROFILEAPI_LBSRUNNING_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_QualityProfileApiSuiteSuiteStepBase.h"
#include "lbsqualityprofile.h"

class CQualityProfileApi_ProfileReadStep : public CTe_QualityProfileApiSuiteSuiteStepBase
	{
public:
	CQualityProfileApi_ProfileReadStep();
	~CQualityProfileApi_ProfileReadStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void InitializeQualityProfileInfoL();
	void GetQualityProfileInfoL(RFs& aFs, RArray<TQualityProfile>& aQualityArray,
			    				const TDesC& aFileName);

	
// Please add/modify your class members here:
private:
	};

_LIT(KQualityProfileApi_ProfileReadStep,"QualityProfileApi_ProfileReadStep");

#endif
