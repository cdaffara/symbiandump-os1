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
 @file initialisersubconparameterfamilyStep.h
*/
#if (!defined INITIALISERSUBCONPARAMETERFAMILY_STEP_H)
#define INITIALISERSUBCONPARAMETERFAMILY_STEP_H
#include <testexecutestepbase.h>
#include <ip_subconparams.h>
#include "Te_EsockStepBase.h"

class TGenericSetParams
    {
public:
    TPtrC iName;
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

class CinitialisersubconparameterfamilyStep : public CTe_EsockStepBase
	{
public:
	CinitialisersubconparameterfamilyStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	TBool ReadGenSetDescription();
	void ConstructFamilyFromIniL();
	void ConstructExtensionSetL(const TDesC& aFieldName, CSubConParameterFamily& aFamily);
	void ConstructGenericSetL(CSubConParameterFamily& aFamily);
	
private:
    TInt iFamilyId;
    TGenericSetParams iGenSetParams;
    TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KinitialisersubconparameterfamilyStep,"initialisersubconparameterfamilyStep");

#endif //INITIALISERSUBCONPARAMETERFAMILY_STEP_H

