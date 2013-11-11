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

#if (!defined COMPAREEXTIPLINK5PARAMETERSET_STEP_H)
#define COMPAREEXTIPLINK5PARAMETERSET_STEP_H

#include "compareExtIPLink99ParameterSetStep.h"

/**
Class implementing compareextiplink5parametersetStep

@internalComponent
*/
class Ccompareextiplink5parametersetStep : public Ccompareextiplink99parametersetStep
	{
public:
	Ccompareextiplink5parametersetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	TBool CompareR5Params(CSubConQosR5ParamSet& ps1,CSubConQosR5ParamSet& ps2);
	};

_LIT(Kcompareextiplink5parametersetStep,"compareextiplinkr5parametersetStep");

#endif //COMPAREEXTIPLINK5PARAMETERSET_STEP_H

