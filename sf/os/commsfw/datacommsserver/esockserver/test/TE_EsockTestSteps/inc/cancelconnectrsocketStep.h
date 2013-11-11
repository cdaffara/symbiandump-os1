/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cancelconnectrsocketStep.h
*/
#if (!defined CANCELCONNECTRSOCKET_STEP_H)
#define CANCELCONNECTRSOCKET_STEP_H
#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing cancelconnectrsocketStep

@internalComponent
*/
class CCancelConnectrsocketStep : public CTe_EsockStepBase
	{
public:
	CCancelConnectrsocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketParams iParams; //current params (.ini)
	};

_LIT(KCancelConnectrsocketStep,"cancelconnectrsocketStep");

#endif

