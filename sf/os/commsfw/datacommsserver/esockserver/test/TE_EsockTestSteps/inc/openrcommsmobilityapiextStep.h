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
 @file openrcommsmobilityapiextStep.h
*/
#if (!defined OPENRCOMMSMOBILITYAPIEXT_STEP_H)
#define OPENRCOMMSMOBILITYAPIEXT_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing openrcommsmobilityapiextStep

@internalComponent
*/
class CopenrcommsmobilityapiextStep : public CTe_EsockStepBase
	{
public:
	CopenrcommsmobilityapiextStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(KopenrcommsmobilityapiextStep,"openrcommsmobilityapiextStep");

#endif //OPENRCOMMSMOBILITYAPIEXT_STEP_H

