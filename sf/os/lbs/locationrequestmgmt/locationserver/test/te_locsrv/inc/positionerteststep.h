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
 @file PositionerTestStep.h
 @internalTechnology
*/
#ifndef __POSITIONERTEST_STEP_H__
#define __POSITIONERTEST_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_locsrvsuitestepbase.h"

class CPositionerTestStep : public CTe_locsrvSuiteStepBase
	{
public:
	CPositionerTestStep();
	~CPositionerTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
    void TestByIndexL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_All_Open_CloseL();
    TVerdict TdLocSrv_Positioner_Open_No_ConnectionL();
    TVerdict TdLocSrv_Positioner_Open_TwiceL();
    TVerdict TdLocSrv_Positioner_Request_before_OpenL();
    TVerdict TdLocSrv_Positioner_Close_No_OpenL();
    TVerdict TdLocSrv_Positioner_Open_No_ModulesL();
    TVerdict TdLocSrv_Positioner_Open_Specific_ModuleL();
    TVerdict TdLocSrv_Positioner_Open_Bad_ModuleL();
    TVerdict TdLocSrv_Positioner_No_RequestorL();
    TVerdict TdLocSrv_Positioner_RequestorL();
    TVerdict TdLocSrv_Positioner_Set_GetUpdateOptionsL();
    TVerdict TdLocSrv_Positioner_Close_With_RequestsL();
    TVerdict TdLocSrv_Positioner_Double_NotifyPositionUpdateL();
    TVerdict TdLocSrv_Positioner_Double_CancelRequestL();
    TVerdict TdLocSrv_Positioner_NotifyPositionUpdate_StandardL();
    TVerdict TdLocSrv_Positioner_NotifyPositionUpdate_GenericL();
    TVerdict TdLocSrv_Positioner_NotifyPositionUpdate_Extended_SupportL();
    TVerdict TdLocSrv_Positioner_NotifyPositionUpdate_Extended_UnsupportedL();
    TVerdict TdLocSrv_Positioner_Tracking_SessionL();
    TVerdict TdLocSrv_Positioner_Tracking_Session_ChangingL();
    TVerdict TdLocSrv_Positioner_Cancel_Tracking_SessionL();
    TVerdict TdLocSrv_Positioner_ModuleDisabledEnabledL();
	};

_LIT(KPositionerTestStep,"PositionerTestStep");

#endif
