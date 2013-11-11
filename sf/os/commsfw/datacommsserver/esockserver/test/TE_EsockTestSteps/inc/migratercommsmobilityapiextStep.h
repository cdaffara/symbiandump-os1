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
 @file migratercommsmobilityapiextStep.h
*/
#if (!defined MIGRATERCOMMSMOBILITYAPIEXT_STEP_H)
#define MIGRATERCOMMSMOBILITYAPIEXT_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing migratercommsmobilityapiext

@internalComponent
*/
class Cmigratercommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Cmigratercommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Kmigratercommsmobilityapiext,"migratercommsmobilityapiextStep");

#endif
