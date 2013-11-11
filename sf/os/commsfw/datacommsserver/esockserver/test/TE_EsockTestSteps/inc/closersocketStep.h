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
 @file closersocketStep.h
*/
#if (!defined CLOSERSOCKET_STEP_H)
#define CLOSERSOCKET_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing closersubconnectionStep

@internalComponent
*/
class CclosersocketStep : public CTe_EsockStepBase
	{
public:
	CclosersocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iSocketName;
	};

_LIT(KclosersocketStep,"closersocketStep");

#endif

