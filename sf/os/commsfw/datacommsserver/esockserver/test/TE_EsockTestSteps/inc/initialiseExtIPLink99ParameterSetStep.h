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
 @file initialiseExtIPLink99ParameterSetStep.h
*/
#if (!defined INITIALISEEXTIPLINK99PARAMETERSET_STEP_H)
#define INITIALISEEXTIPLINK99PARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include <ip_subconparams.h>
#include "Te_EsockStepBase.h"

/**
Class implementing initialisation of Qos parameter family

@internalComponent
*/
class CinitialiseExtIPLink99ParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseExtIPLink99ParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	
    //current params (.ini)
    TInt iFamilyId;
    TExtensionQosParamsSetIPLink99 iParams;
    TPtrC iSetName;
    TPtrC iBundleName;

protected:
	void ConstructIPLink99SetL();
	TBool ParseIniIPLinkParameters();
	void InitialiseR99Params(CSubConQosIPLinkR99ParamSet* aParams);
	};

_LIT(KinitialiseExtIPLink99ParameterSetStep,"initialiseExtIPLink99ParameterSetStep");

#endif //INITIALISEEXTIPLINK99PARAMETERSET_STEP_H

