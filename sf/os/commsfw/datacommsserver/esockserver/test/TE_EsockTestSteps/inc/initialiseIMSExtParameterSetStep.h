// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Initialize IMS parameters
// 
//

/**
 @file
 @internalComponent
*/
#if (!defined INITIALISEEXTIMSPARAMETERSET_STEP_H)
#define INITIALISEEXTIMSPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include <ip_subconparams.h>
#include "Te_EsockStepBase.h"


/**
Class implementing initialisation of IMS parameter family

@internalComponent
*/
class CinitialiseIMSExtParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseIMSExtParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	void ConstructIMSParamSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TPtrC iSetName;
    TPtrC iBundleName;
    TExtensionIMSParamsSet iIMSParams;
	};

_LIT(KinitialiseExtIMSParameterSetStep,"initialiseIMSExtParameterSetStep");

#endif //INITIALISEEXTIMSPARAMETERSET_STEP_H

