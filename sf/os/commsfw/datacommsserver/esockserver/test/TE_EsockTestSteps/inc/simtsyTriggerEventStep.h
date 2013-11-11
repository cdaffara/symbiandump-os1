/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file simtsyPublishSubscribeStep.h
*/
#if (!defined SIMTSYTRIGGEREVENT_STEP_H)
#define SIMTSYTRIGGEREVENT_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include "SimTsySpecificDefs.h"

/**
Class implementing simtsyTriggerEventStep


@internalComponent
*/
class CsimtsyTriggerEventStep : public CTe_EsockStepBase
	{
public:
	CsimtsyTriggerEventStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    
    TSimTsyEventParams iParams;
	};

_LIT(KsimtsyTriggerEventStep,"simtsyTriggerEventStep");

#endif //SIMTSYTRIGGEREVENT_STEP_H

