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
 @file waitforincomingconnectionStep.h
*/
#if (!defined WAITFORINCOMINGCONNECTION_STEP_H)
#define WAITFORINCOMINGCONNECTION_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing waitforincomingconnectionStep

@internalComponent
*/
class CwaitforincomingconnectionStep : public CTe_EsockStepBase
	{
public:
	CwaitforincomingconnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC timerSet;
    TPtrC subConnStatus;
    RTimer iTimer;
    TRSubConnectionParams iParams;  //current params (.ini)
    
	};

_LIT(KwaitforincomingconnectionStep,"waitforincomingconnectionStep");

#endif

