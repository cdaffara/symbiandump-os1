/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file compareGenSblpParameterSetStep.h
*/

#if (!defined COMPAREGENSBLPPARAMETERSET_STEP_H)
#define COMPAREGENSBLPPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <cs_subconparams.h>

/**
Class implementing comparegensblpparametersetStep

@internalComponent
*/
class CcomparegensblpparametersetStep : public CTe_EsockStepBase
	{
public:
	CcomparegensblpparametersetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	TBool Compare(CSubConAuthorisationGenericParamSet ps1,CSubConAuthorisationGenericParamSet ps2);

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KcomparegensblpparametersetStep,"comparegensblpparametersetStep");

#endif //COMPAREGENSBLPPARAMETERSET_STEP_H

