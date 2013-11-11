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
 @file TDBT3Step.h
 @internalTechnology
*/
#ifndef __TDBT3_STEP_H__
#define __TDBT3_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"
#include "LcfCommonProcedures.h"
#include <EPos_CPositioner.h>

class CTDBT3Step : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT3Step();
	~CTDBT3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	/**
	* Runs the test in the module
	*/
	void StartL(TInt aIndex);
	
    // Sub test case functions:
    void Sub0TCPM2001OutOfMemoryL();
    void RunSubCase0L( CLcfCommonProcedures* aCommon );

    void Sub1TCPM1002FastOpenCloseL();
    void RunSubCase1L( CLcfCommonProcedures* aCommon, const TUid aPsyUid );

    void Sub2TCPM1004SystemTimeChangeL();
    void RunSubCase2L( CLcfCommonProcedures* aCommon, const TUid aPsyUid );

    void Sub3TCPM1003FiveParallelSessionsL();
    void RunSubCase3L( CLcfCommonProcedures* aCommon );
    void Sub3BasicInfoRequestingL( CLcfCommonProcedures* aCommon );
    void Sub3SatelliteInfoRequestingL( CLcfCommonProcedures* aCommon );
    void Sub3SatelliteInfoRequestingPrintBasicInfoL( CLcfCommonProcedures* aCommon );
    void Sub3SatelliteInfoRequestingNoPrintingL( CLcfCommonProcedures* aCommon );
    
    void Sub4TCPM1001TimeoutPerformanceL();
    void RunSubCase4L( CLcfCommonProcedures* aCommon );
    void Sub4TimeoutRequestsL( TInt aTimeOutValue, CLcfCommonProcedures* aCommon );

    void Sub5TCS1001PositionJumpingL();
    void RunSubCase5L( CLcfCommonProcedures* aCommon );

    void Sub6TCS1002CrossingZeroPointsL();
    void RunSubCase6L( CLcfCommonProcedures* aCommon );

    void Sub7TCS2001SatelliteDataL();
    void RunSubCase7L( CLcfCommonProcedures* aCommon );

#if SERVICE_TESTS
    void TestServiceDbOomL();

    CPosServiceDb* TestServiceDbOpenOomLC();
    CPosServiceIdArray* TestServiceArrayOpenOomLC( CPosServiceDb* aServiceDb );
    CPosService* TestServiceLOpenOomLC( CPosServiceDb* aServiceDb, CPosServiceIdArray* aServiceArray );
    CPosService* TestServiceLXOpenOomLC( CPosServiceDb* aServiceDb, CPosServiceIdArray* aServiceArray );
    void TestSetPrivacyLOomL( CPosService* aService );
    void TestCommitServiceLOomL( CPosServiceDb* aServiceDb, CPosService* aService );
    void TestCloseServiceLOomL( CPosServiceDb* aServiceDb, TPosServiceItemId aServiceId );
    void TestDeleteServicesLOomL( CPosServiceDb* aServiceDb, CPosServiceIdArray* aServiceArray );
    void TestFunctionalityAfterNotReadyL( CPosServiceDb* aServiceDb, CPosServiceIdArray* aServiceArray );
    TInt TestFunctionsAfterNotReadyL( CPosServiceDb* aServiceDb, CPosServiceIdArray* aServiceArray );
    void TestFunctionsWithDifferentServiceL( CPosServiceDb* aServiceDb, CPosServiceIdArray* aServiceArray );
#endif

        static void MakeTenPeriodicUpdatesFromBTGPSPsyL( TInt forLogging );
         
        void TcOomInPsyLoadL(const TUid aPsyUid);
        void TcOomWhenRequestingLocationL(const TUid aPsyUid, CLcfCommonProcedures* aCommon);
        void DisplayHeapUsageL(const TInt aOrigHUsage, TInt& aPrevHUsage, TInt& aMaxHUsage, CLcfCommonProcedures* aCommon);
        void TcHeapUsageL(const TUid aPsyUid, CLcfCommonProcedures* aCommon);

        void LogErrorL(TRefByValue<const TDesC> aMsg, ...);
        void LogL(TRefByValue<const TDesC> aMsg, ...);
    
private:
	TBuf<256> iLogBuf;

	};

_LIT(KTDBT3Step,"TDBT3Step");

#endif
