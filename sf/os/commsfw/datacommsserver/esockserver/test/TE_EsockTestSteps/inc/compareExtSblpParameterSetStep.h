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
 @file compareExtSblpParameterSetStep.h
*/
#if (!defined COMPAREEXTSBLPPARAMETERSET_STEP_H)
#define COMPAREEXTSBLPPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing compareextsblpparametersetStep

@internalComponent
*/
class CcompareextsblpparametersetStep : public CTe_EsockStepBase
	{
public:
	CcompareextsblpparametersetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	TBool Compare(CSubConSBLPR5ExtensionParamSet& ps1,CSubConSBLPR5ExtensionParamSet& ps2);

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KcompareextsblpparametersetStep,"compareextsblpparametersetStep");

#endif //COMPAREEXTSBLPPARAMETERSET_STEP_H

