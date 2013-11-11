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
 @file removersocketfromrsubconnectionoomStep.h
*/
#if (!defined REMOVERSOCKETFROMRSUBCONNECTIONOOM_STEP_H)
#define REMOVERSOCKETFROMRSUBCONNECTIONOOM_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing removersocketfromrsubconnectionoomStep

@internalComponent
*/
class CremoversocketfromrsubconnectionoomStep : public CTe_EsockStepBase
	{
public:
	CremoversocketfromrsubconnectionoomStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TBool DescriptionExists();
	TInt RemoveSocketFromSubConnectionOOM();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KremoversocketfromrsubconnectionoomStep,"removersocketfromrsubconnectionoomStep");

#endif

