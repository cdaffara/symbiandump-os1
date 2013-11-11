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
 @file moveConnectedSocketToSubconnectionStep.h
*/
#if (!defined moveConnectedSocketToSubconnection_STEP_H)
#define moveConnectedSocketToSubconnection_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <in_sock.h>

/**
Class implementing moveConnectedSocketToSubconnectionStep

@internalComponent
*/
class CmoveConnectedSocketToSubconnectionStep : public CTe_EsockStepBase
	{
public:
	CmoveConnectedSocketToSubconnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
private:
    TInt moveConnectedSocketToSubconnection();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	TInt iCycle;
	};

_LIT(KmoveConnectedSocketToSubconnectionStep,"moveConnectedSocketToSubconnectionStep");

#endif

