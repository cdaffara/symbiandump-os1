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
 @file stopsimtsyStep.h
*/
#if (!defined STOPSIMTSY_STEP_H)
#define STOPSIMTSY_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing stopsimtsyStep

@internalComponent
*/
class CstopsimtsyStep : public CTe_EsockStepBase
	{
public:
	CstopsimtsyStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

private:
    //TPtrC iSimTsyName;
	};

_LIT(KstopsimtsyStep,"stopsimtsyStep");

#endif //STOPSIMTSY_STEP_H

