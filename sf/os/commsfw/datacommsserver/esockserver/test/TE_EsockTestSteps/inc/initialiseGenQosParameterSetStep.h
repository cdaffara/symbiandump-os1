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
 @file initialiseGenQosParameterSetStep.h
*/
#if (!defined INITIALISEGENQOSPARAMETERSET_STEP_H)
#define INITIALISEGENQOSPARAMETERSET_STEP_H
#include <testexecutestepbase.h>
#include <ip_subconparams.h>
#include "Te_EsockStepBase.h"

/**
Class containing generic sub connection parameters

@internalComponent
*/
class TGenericQosParamsSet
    {
public:
    CSubConParameterFamily::TParameterSetType iType;
    TBool iHeaderMode;
    TInt iDownlinkBandwidth;
    TInt iUplinkBandwidth;
    TInt iDownLinkMaximumBurstSize;
    TInt iUpLinkMaximumBurstSize;
    TInt iDownLinkAveragePacketSize;
    TInt iUpLinkAveragePacketSize;
    TInt iDownLinkMaximumPacketSize;
    TInt iUpLinkMaximumPacketSize;
    TInt iDownLinkDelay;
    TInt iUpLinkDelay;
    TInt iDownLinkDelayVariation;
    TInt iUpLinkDelayVariation;
    TInt iDownLinkPriority;
    TInt iUpLinkPriority;
    };

/**
Class implementing initialiseGenQosParameterSetStep

@internalComponent
*/
class CinitialiseGenQosParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseGenQosParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	void ConstructGenericSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TGenericQosParamsSet iParams;
    TPtrC iSetName;
    TPtrC iBundleName;
	};

_LIT(KinitialiseGenQosParameterSetStep,"initialiseGenQosParameterSetStep");

#endif //INITIALISEGENQOSPARAMETERSET_STEP_H

