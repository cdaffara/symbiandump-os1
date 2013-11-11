/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file compareextiplink5parametersetStep.h
*/

#if (!defined COMPAREIMSEXTPARAMETERSET_STEP_H)
#define COMPAREIMSEXTPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <ip_subconparams.h>

/**
Class implementing CcompareextimsparametersetStep

@internalComponent
*/
class CcompareextimsparametersetStep : public CTe_EsockStepBase
	{
public:
	CcompareextimsparametersetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	TBool Compare(CSubConImsExtParamSet& ps1,CSubConImsExtParamSet& ps2);

private:
    TPtrC iParams1Name;
    TPtrC iParams2Name;
    TInt iFamily1Id;
	TInt iFamily2Id;
	CSubConParameterFamily::TParameterSetType iFamily1Type;
	CSubConParameterFamily::TParameterSetType iFamily2Type;
	};

_LIT(KcompareextimsparametersetStep,"compareextimsparametersetStep");

#endif //COMPAREIMSEXTPARAMETERSET_STEP_H

