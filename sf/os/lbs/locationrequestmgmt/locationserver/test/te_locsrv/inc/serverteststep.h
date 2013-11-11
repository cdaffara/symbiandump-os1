/**
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
 @file ServerTestStep.h
 @internalTechnology
*/
#ifndef __SERVERTEST_STEP_H__
#define __SERVERTEST_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_locsrvsuitestepbase.h"

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
    TBool CompareModuleInfo(TPositionModuleInfo& m1, TPositionModuleInfo& m2);
    TBool CompareModuleQuality(TPositionModuleInfo& m1, TPositionModuleInfo& m2);
    TBool CompareModuleNames(TPositionModuleInfo& m1, TPositionModuleInfo& m2);
    TBool CompareModuleStatus(TPositionModuleStatus& s1, TPositionModuleStatus& s2);
    TVerdict TdLocSrv_Server_ConnectL();
    TVerdict TdLocSrv_Server_Close_Positioner_OpenL();
    TVerdict TdLocSrv_Server_Anything_Before_ConnectL();
    TVerdict TdLocSrv_Server_Double_ConnectL();
    TVerdict TdLocSrv_Server_WrongCancelRequest2L();
    TVerdict TdLocSrv_Server_Module_InfoL();
    TVerdict TdLocSrv_Server_Module_Info_InvalidL();
    TVerdict TdLocSrv_Server_Module_Status_SyncL();
    TVerdict TdLocSrv_Server_Module_Status_AsyncL();
    TVerdict TdLocSrv_Server_Bad_Module_Status_RequestsL();
    TVerdict TdLocSrv_Server_Close_With_RequestsL();
    TVerdict TdLocSrv_Server_Double_CancelRequestL();
    TVerdict TdLocSrv_Server_Double_NotifyStatusL();
    TVerdict TdLocSrv_Server_VersionL();
	};

_LIT(KServerTestStep,"ServerTestStep");

#endif
