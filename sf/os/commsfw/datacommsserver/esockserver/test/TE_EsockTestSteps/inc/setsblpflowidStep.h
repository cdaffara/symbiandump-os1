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
 @file createsblpflowidStep.h
*/
#if (!defined SETSBLPFLOWID_STEP_H__)
#define SETSBLPFLOWID_STEP_H__

#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing setsblpflowidStep

@internalComponent
*/
class CsetsblpflowidStep : public CTe_EsockStepBase
	{
public:
	CsetsblpflowidStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
private:
    //current params (.ini)
    TPtrC iFlowExtName;
  	TInt iMediaComponent;
	TInt iIPFlowNumber;
	};

_LIT(KsetsblpflowidStep,"setsblpflowidStep");
#endif

