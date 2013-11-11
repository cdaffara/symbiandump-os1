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
 @file creatersocketservStep.h
*/
#if (!defined CREATERSOCKETSERV_STEP_H)
#define CREATERSOCKETSERV_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing creatersocketservStep

@internalComponent
*/
class CcreatersocketservStep : public CTe_EsockStepBase
	{
public:
	CcreatersocketservStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iSockServName;
	};

_LIT(KcreatersocketservStep,"creatersocketservStep");

#endif

