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
 @file cleanallStep.h
*/
#if (!defined CLEANALL_STEP_H)
#define CLEANALL_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing cleanallStep

@internalComponent
*/
class CcleanallStep : public CTe_EsockStepBase
	{
public:
	CcleanallStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepL();

	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	};

_LIT(KcleanallStep,"cleanallStep");

#endif //CLEANALL_STEP_H

