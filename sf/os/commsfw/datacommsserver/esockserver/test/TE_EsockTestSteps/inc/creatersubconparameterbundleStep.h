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
 @file creatersubconparameterbundleStep.h
*/
#if (!defined CREATERSUBCONPARAMETERBUNDLE_STEP_H)
#define CREATERSUBCONPARAMETERBUNDLE_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing creatersubconparameterbundleStep

@internalComponent
*/
class CcreatersubconparameterbundleStep : public CTe_EsockStepBase
	{
public:
	CcreatersubconparameterbundleStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	void ConstructParametersFromIniL();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KcreatersubconparameterbundleStep,"creatersubconparameterbundleStep");

#endif //CREATERSUBCONPARAMETERBUNDLE_STEP_H

