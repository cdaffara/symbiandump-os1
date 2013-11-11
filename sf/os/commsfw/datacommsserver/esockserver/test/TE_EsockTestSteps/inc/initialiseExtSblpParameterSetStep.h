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
 @file initialiseExtSblpParameterSetStep.h
*/
#if (!defined INITIALISEEXTSBLPPARAMETERSET_STEP_H__)
#define INITIALISEEXTSBLPPARAMETERSET_STEP_H__

#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <ip_subconparams.h>

/**
Class containing extension SBLP parameters

@internalComponent
*/
class TExtensionSetSblpParams
    {
public:
    ~TExtensionSetSblpParams();
    
	CSubConParameterFamily::TParameterSetType iType;
	TAuthToken iAuthorisationToken;
	RArray<TPtrC> iFlowNames;
    };

/**
Class implementing initialiseExtSblpParameterSetStep

@internalComponent
*/
class CinitialiseExtSblpParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseExtSblpParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	void ConstructExtensionSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TExtensionSetSblpParams iParams;
    TPtrC iSetName;
    TPtrC iBundleName;
    };

_LIT(KinitialiseExtSblpParameterSetStep,"initialiseExtSblpParameterSetStep");

#endif //INITIALISEEXTSBLPPARAMETERSET_STEP_H__

