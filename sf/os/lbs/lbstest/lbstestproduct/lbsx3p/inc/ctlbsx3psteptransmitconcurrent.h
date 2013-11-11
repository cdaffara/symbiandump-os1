// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// @file ctlbsx3psteptransmitconcurrent.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_X3P_STEP_TRANSMIT_CONCURRENT__
#define __CT_LBS_X3P_STEP_TRANSMIT_CONCURRENT__

/**
 * LBS includes.
 */
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbslocerrors.h>

 /**
  * LBS test includes.
  */
#include "ctlbsx3pserver.h"
#include <lbs/test/tlbsutils.h>
#include "ctlbsx3pdotransmitpos.h"
 
 /**
  * Literals and constants used
  */  
_LIT(KLbsX3PStep_Transmit_Concurrent, "LbsX3PStep_Transmit_Concurrent");
_LIT(KConcurrentDestination, "07712345678");
const TUint KConcurrentTransmitPriority = X3P_PRIORITY_PUSH;


/**
 * The CT_LbsX3PStep_Transmit_Concurrent class was written
 * with the intention of running concurrently using TEF commands.
 * It performs the common tasks of installing a scheduler, creating
 * verifiable position info, performing an X3P task, verifying recieved 
 * positions and finally verifying log info.
 * 
 * \author Peter Turner
 */
class CT_LbsX3PStep_Transmit_Concurrent :public CTestStep,
											  CActive,
											  MT_LbsX3PDoTransmitPosObserver
	{
public:
	/**
	 * Standard constructors, destructors and test step entry point.
	 */
	~CT_LbsX3PStep_Transmit_Concurrent();
	static CT_LbsX3PStep_Transmit_Concurrent* New();
	void ConstructL();
	
	virtual TVerdict doTestStepL();


	void MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo);
	void MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO);

private:
	CT_LbsX3PStep_Transmit_Concurrent();
	
	/**
	 * These methods perform the functions normally found in other test steps.
	 * This functionality had to be copied into methods to run correctly
	 * in a concurrent fashion.
	 */
	void CreateVerifyPositionInfoL();
	void VerifyPosInfo();
	// VerifyLogInfo
	TInt OpenNetSim();
	
	/**
	 * Current test case value initialised from the ini files value.
	 */
	TInt iTestCaseId;
	
	/**
	 * Network Simulator, to allow MT-LRs and to monitor network messages.
	 */
	RLbsNetSimTest iNetSim;
	
	/**
	 * This objects active scheduler
	 */
	CActiveScheduler* iScheduler;

	/**
	 * Server and session used during the tests.
	 */
	RLbsTransmitPositionServer iServer;
	RLbsTransmitPosition iPositionTransmitter;
	CT_LbsX3pRefLocWaiter* iRefLocWaiter;
	
	/**
	 * Position quality used to setup Net sim.
	 */
	TLbsNetPosRequestQuality iNetPosQuality;

	/** 
	 * Reference position used to setup Net sim and later verification.#
	 */
	TPosition iRefPos;
	
	/** 
	 * Shared data to store positions from ini file. CONCURRENT test step
	 * command does not work when the -SharedData parameter is used on the server.
	 * The -SharedData functionality is internalised here.
	 */
	RPointerArray<TAny>	iVerifyPosInfoArr;
	RPointerArray<TAny>	iCurrentPosInfoArr;
	
	/** 
	 * Flag used to indicate if the A-GPS integration test module is being used during testing.
	 */
	TBool iTestModuleInUse;
	
protected:
	/** 
	 * Functions from CActive:
	 */
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
	};

#endif //__CT_LBS_X3P_STEP_TRANSMIT_CONCURRENT__
