/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file 
 @internalTechnology
 @test
*/
#ifndef __LKSERVERTEST_STEP_H__
#define __LKSERVERTEST_STEP_H__

#include <test/TestExecuteStepBase.h>
#include "te_locsrvsuitestepbase.h"
#include <e32property.h> // Used to define the RProperty for the clear database tests()


class CServerTestStep : public CTe_locsrvSuiteStepBase
	{
public:
	CServerTestStep();
	~CServerTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
    void TestByIndexL(TInt aIndex);
    TInt StartEposServer();
    
    TVerdict TdLocSrv_Server_ClearDBPending_CloseSessionHandleL(TInt aIndex);
    TVerdict TdLocSrv_Server_ClearDB_LocMonitor_SessionOpenFailL(TInt aIndex);
    TVerdict TdLocSrv_Server_ClearDB_2ClientsL(TInt aIndex);
    TVerdict TdLocSrv_Server_ClearDB_AndCancelL(TInt aIndex);
    TVerdict TdLocSrv_Server_ClearDB_AndCancel_2ClientsL(TInt aIndex);
    TVerdict TdLocSrv_Server_ClearDB_TimedOutL(TInt aIndex);
    TVerdict TdLocSrv_Server_ClearDB_10ClientsL(TInt aIndex);
    
    
	};

_LIT(KServerTestStep,"ServerTestStep");

#endif
