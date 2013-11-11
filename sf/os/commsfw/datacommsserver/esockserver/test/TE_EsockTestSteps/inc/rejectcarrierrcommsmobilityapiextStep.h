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
 @file rejectrcommsmobilityapiextStep.h
*/
#if (!defined REJECTRCOMMSMOBILITYAPIEXT_STEP_H)
#define REJECTRCOMMSMOBILITYAPIEXT_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing rejectrcommsmobilityapiext

@internalComponent
*/
class Crejectcarrierrcommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Crejectcarrierrcommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Krejectcarrierrcommsmobilityapiext,"rejectcarrierrcommsmobilityapiextStep");

#endif
