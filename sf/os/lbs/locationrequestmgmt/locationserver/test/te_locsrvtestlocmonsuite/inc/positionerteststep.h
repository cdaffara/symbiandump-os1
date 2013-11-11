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
#ifndef __LKPOSITIONERTEST_STEP_H__
#define __LKPOSITIONERTEST_STEP_H__

#include <test/TestExecuteStepBase.h>
#include "te_locsrvsuitestepbase.h"
#include <e32property.h> // Used to define the RProperty in GetLastKnownPositionAreaL()  
#include <e32std.h>

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
    
    //Start the modified location server that uses the test location monitor API
    TInt StartEposServer();
    void CheckPosition(TPositionInfo aPosInfo);
    void CheckPositionAreaInfo(TPositionInfo aPosInfo, TPositionAreaInfo aPosAreaInfo);
    
    
    // (LocMonitor) Location Monitor Tests
    // Tests for the extension to the Location Server to handle Last Known Position
    // Area / Clear Database requests which are serviced by the Location Monitor 
    // [the testLocMonitorApi is used in these test cases]
    // Note: [LKP - Last Known Position]
    TVerdict TdLocSrv_Positioner_GetLKPAreaL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_ExtendedL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_CancelL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_And_ClearDBL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_3Clients_CancelL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_And_ClearDB_3ClientsL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_10ClientsL(TInt aIndex);
    
    // Negative Test
    TVerdict TdLocSrv_Positioner_GetLKPArea_NotFoundL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_Extended_IncorrectArgL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_LocMonitor_SubSessionOpenFailL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPArea_TimedOutL(TInt aIndex);
    // PANIC tests - 3 Clients [Session 1 - SubSession1 , SubSession2] [Session 2 - SubSession3]
    TVerdict TdLocSrv_Positioner_GetLKPAreaPending_CloseSubSessionHandleL(TInt aIndex);
    
    
    TVerdict TdLocSrv_Positioner_GetLKPositionL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_CancelL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_And_ClearDBL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_3Clients_CancelL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_And_ClearDB_3ClientsL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_10ClientsL(TInt aIndex);
    
    // Negative Test
    TVerdict TdLocSrv_Positioner_GetLKPosition_NotFoundL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_IncorrectArgL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_LocMonitor_SubSessionOpenFailL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_TimedOutL(TInt aIndex);
    // PANIC tests - 3 Clients [Session 1 - SubSession1 , SubSession2] [Session 2 - SubSession3]
    TVerdict TdLocSrv_Positioner_GetLKPositionPending_CloseSubSessionHandleL(TInt aIndex);
    
    // Different types of requests in parallel
    TVerdict TdLocSrv_Positioner_GetLKPosition_AndLKPArea_3ClientsL(TInt aIndex);
    TVerdict TdLocSrv_Positioner_GetLKPosition_LKPArea_AndClearDB_3ClientsL(TInt aIndex);
    
	};

_LIT(KPositionerTestStep,"PositionerTestStep");

#endif //__LKPOSITIONERTEST_STEP_H__
