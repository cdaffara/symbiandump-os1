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
 @file CreateRConnAvailApiExtension.h
*/

#if (!defined SELECTRCONNECTION_H)
#define SELECTRCONNECTION_H
#include <TestExecuteStepBase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing CSelectRConnectionStep

@internalComponent
*/
_LIT(KCSelectRConnectionStep,"SelectRConnectionStep");

class CSelectRConnectionStep : public CTe_EsockStepBase
	{
public:
	CSelectRConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    // Current params from the ini file
    TRConnectionParams iParams;
	};


#endif

