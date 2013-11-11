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
 @file compareextiplink99parametersetStep.h
*/

#if (!defined COMPAREEXTIPLINK99PARAMETERSET_STEP_H)
#define COMPAREEXTIPLINK99PARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <ip_subconparams.h>

/**
Class implementing compareextiplink99parametersetStep

@internalComponent
*/
class Ccompareextiplink99parametersetStep : public CTe_EsockStepBase
	{
public:
	Ccompareextiplink99parametersetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:

	TBool ParseCompareDetails();
	TBool CompareR4Params(CSubConQosIPLinkR99ParamSet& ps1,CSubConQosIPLinkR99ParamSet& ps2);

    TPtrC iParams1Name;
    TPtrC iParams2Name;
    TInt iFamily1Id;
	TInt iFamily2Id;
	CSubConParameterFamily::TParameterSetType iFamily1Type;
	CSubConParameterFamily::TParameterSetType iFamily2Type;
	};

_LIT(Kcompareextiplink99parametersetStep,"compareextiplink99parametersetStep");

#endif //COMPAREEXTIPLINK99PARAMETERSET_STEP_H

