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
 @file acceptcarrierrcommsmobilityapiextStep.h
*/
#if (!defined ACCEPTCARRIERRCOMMSMOBILITYAPIEXT_STEP_H)
#define ACCEPTCARRIERRCOMMSMOBILITYAPIEXT_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing acceptcarrierrcommsmobilityapiextStep

@internalComponent
*/
class Cacceptcarrierrcommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Cacceptcarrierrcommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Kacceptcarrierrcommsmobilityapiext,"acceptcarrierrcommsmobilityapiextStep");

#endif
