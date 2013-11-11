/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file AsyncAcceptIncomingConnectionStep.h
*/
#if (!defined ASYNCACCEPTINCOMINGCONNECTION_STEP_H)
#define ASYNCACCEPTINCOMINGCONNECTION_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing AsyncAcceptIncomingConnectionStep.h

@internalComponent
*/
class CAsyncAcceptIncomingConnectionStep : public CTe_EsockStepBase
	{
public:
	CAsyncAcceptIncomingConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC timerSet;
    TPtrC subConnStatus;
    RTimer iTimer;
    TRSubConnectionParams iParams;  //current params (.ini)
    
	};

_LIT(KAsyncAcceptIncomingConnectionStep,"AsyncAcceptIncomingConnectionStep");

#endif

