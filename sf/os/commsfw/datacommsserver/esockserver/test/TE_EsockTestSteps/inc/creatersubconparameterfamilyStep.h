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
 @file creatersubconparameterfamilyStep.h
*/
#if (!defined CREATERSUBCONPARAMETERFAMILY_STEP_H)
#define CREATERSUBCONPARAMETERFAMILY_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing creatersubconparameterfamilyStep

@internalComponent
*/
class CcreatersubconparameterfamilyStep : public CTe_EsockStepBase
	{
public:
	CcreatersubconparameterfamilyStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

protected:
	TBool ReadGenSetDescription();
	void ConstructFamilyFromIniL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TPtrC iSubConnectionParamsName;
	};

_LIT(KcreatersubconparameterfamilyStep,"creatersubconparameterfamilyStep");

#endif //CREATERSUBCONPARAMETERFAMILY_STEP_H

