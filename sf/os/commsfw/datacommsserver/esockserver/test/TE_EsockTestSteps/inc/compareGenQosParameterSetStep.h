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
 @file compareGenQosParameterSetStep.h
*/

#if (!defined COMPAREGENQOSPARAMETERSET_STEP_H)
#define COMPAREGENQOSPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <cs_subconparams.h>

/**
Class implementing comparegenqosparametersetStep

@internalComponent
*/
class CcomparegenqosparametersetStep : public CTe_EsockStepBase
	{
public:
	CcomparegenqosparametersetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	TBool Compare(CSubConQosGenericParamSet& ps1,CSubConQosGenericParamSet& ps2);

private:
    TPtrC iParams1Name;
    TPtrC iParams2Name;
    TInt iFamily1Id;
	TInt iFamily2Id;
	CSubConParameterFamily::TParameterSetType iFamily1Type;
	CSubConParameterFamily::TParameterSetType iFamily2Type;
	};

_LIT(KcomparegenqosparametersetStep,"comparegenqosparametersetStep");

#endif //COMPAREGENQOSPARAMETERSET_STEP_H

