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
 @file closerconnectionStep.h
*/
#if (!defined CLOSERCONNECTION_STEP_H)
#define CLOSERCONNECTION_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing closerconnectionStep

@internalComponent
*/
class CcloserconnectionStep : public CTe_EsockStepBase
	{
public:
	CcloserconnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TRConnectionParams iParams; //current params (.ini)
	};

_LIT(KcloserconnectionStep,"closerconnectionStep");

#endif

