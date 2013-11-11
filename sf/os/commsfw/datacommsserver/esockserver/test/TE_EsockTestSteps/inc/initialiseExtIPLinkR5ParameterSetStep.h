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
// Initialize R5 parameters
// 
//

/**
 @file
 @internalComponent
*/
#if (!defined INITIALISEEXTIPLINKR5PARAMETERSET_STEP_H)
#define INITIALISEEXTIPLINKR5PARAMETERSET_STEP_H

#include "initialiseExtIPLink99ParameterSetStep.h"

/**
Class implementing initialisation of Qos R5 parameter family

@internalComponent
*/
class CinitialiseExtIPLinkR5ParameterSetStep : public CinitialiseExtIPLink99ParameterSetStep
	{
public:
	CinitialiseExtIPLinkR5ParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	void ConstructIPLinkR5SetL();
	void InitialiseR5Params(CSubConQosR5ParamSet* ipLinkR5ParmSet);

private:
    
    TExtensionQosParamsSetIPLinkR5 iLinkR5Params;
	};

_LIT(KinitialiseExtIPLinkR5ParameterSetStep,"initialiseExtIPLinkR5ParameterSetStep");

#endif //INITIALISEEXTIPLINKR5PARAMETERSET_STEP_H

