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
 @file initialiseGenSblpParameterSetStep.h
*/
#if (!defined INITIALISEGENSBLPPARAMETERSET_STEP_H)
#define INITIALISEGENSBLPPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include <ip_subconparams.h>
#include "Te_EsockStepBase.h"

/**
Class containing generic SBLP parameters

@internalComponent
*/
class TGenericSetSblpParams
    {
public:
	CSubConParameterFamily::TParameterSetType iType;
   	TBool iHeaderMode;
    };

/**
Class implementing initialiseGenSblpParameterStep

@internalComponent
*/
class CinitialiseGenSblpParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseGenSblpParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	void ConstructGenericSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TGenericSetSblpParams iParams;
    TPtrC iSetName;
    TPtrC iBundleName;
	};

_LIT(KinitialiseGenSblpParameterSetStep,"initialiseGenSblpParameterSetStep");

#endif //INITIALISEGENSBLPPARAMETERSET_STEP_H

