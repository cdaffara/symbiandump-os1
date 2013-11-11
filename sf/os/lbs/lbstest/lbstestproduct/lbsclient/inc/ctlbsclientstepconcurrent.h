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
// @file ctlbsclientstepconcurrent.h
// This contains the header file for a TEF test step that
// is meant to be executed in several concurrent threads
// for testing multiples clients using simultaneously the 
// LBS Client interface
// 
//

#ifndef CTLBSCLIENTSTEPCONCURRENT_H
#define CTLBSCLIENTSTEPCONCURRENT_H

/**
* LBS includes.
*/
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbslocerrors.h>
#include <test/testexecutestepbase.h>


/**
 * LBS test includes.
 */
#include <lbs/test/tlbsutils.h>
 
/**
 * Literals and constants used
*/  
_LIT(KLbsClientStepConcurrent, "LbsClientStep_Concurrent");

/**
* The CT_LbsClientStep_Concurrent class is meant to be used
* in TEF's concurrent mode of execution. That implies that 
* none of its methods must attemp to use "shared data" owned
* by the server.
*/
class CT_LbsClientStep_Concurrent : public CTestStep
	{
	
public:

	/**
	 * Standard constructors, destructors and test step entry point.
	 */
	~CT_LbsClientStep_Concurrent();
	static CT_LbsClientStep_Concurrent* New();

	virtual TVerdict doTestStepL();

private:

	CT_LbsClientStep_Concurrent();
	void ConstructL();

	void Validate(TPositionInfo& aExpectedPosition, TPositionInfo& aActualPosition, TPositionAreaExtendedInfo& aActualArea, 
			TBool aMccExpected, TBool aMncExpected, TBool aLacExpected, TBool aCidExpected);
			
	TInt VerifyMatchLevel(TPositionAreaExtendedInfo& aExpected, TPositionAreaExtendedInfo& aActual);
	
	
	/**
	 * The functionality provided by the methods below is often
	 * performed from test steps (CT_LbsStep_CreateVerifyPosInfos
	 * and CT_LbsStep_VerifyPosInfos). Those test steps cannot be
	 * run in concurrent mode because they operate on "shared data"
	 * (owned by the server and accessed from each test step sequentially)
	 * Since this test step is meant for concurrent execution (the steps
	 * run concurrently in entirely different threads with their own heap)
	 * data sharing is not possible and therefore using the above test
	 * steps produces errors. That's why those functions are implemented
	 * here. 
	 */
	  
	/* Note that CreateVerifyPositionInfoL does some important stuff inside
	in addition to what its name says: it tells the Test GPS Integration Module
	where in the .ini file the positions that it has to return are. If this
	is not called, the Integration Module will send to the AGPS Manager
	empty arrays of positions and the AGPS Manager may panic. */
	void CreateVerifyPositionInfoL();
	void VerifyPosInfo();

private:

	/** 
	* To store positions from ini file. Those are the same positions the Integration
	* Module has been asked by the test to return when requested to provide a position
	*(normally after the test issued an NPUD). 
	*/
	RPointerArray<TAny>	iVerifyPosInfoArr;
	
	
	/* Positions actually received by this test when using the Location
	* Server's client interface (NPUD, GLKP, GLKPA, etc). In sequential tests,
	* this array is kept in shared data in the server, but we can't
	* do that here if we want to run steps concurrently.
	*/
	RPointerArray<TAny>	iCurrentPosInfoArr;
	
	/**
	 * Again, when testcases run sequentially with "-SharedData" the thread
	 * where they are run by the server is always the same. In that case
	 * often the test script installs the scheduler once using the test step 
	 * CT_LbsStep_InstallScheduler. That's no good for the present test step
	 * because it is going to run in concurrent mode (in its own, not reused,
	 * thread) and therefore it may need its own scheduler if the particular
	 * test needs to use an AO.
	 */
	CActiveScheduler* iScheduler;
	
	
	/**
	 * Current test case value initialised from the ini files value.
	 */
	TInt iTestCaseId;
	

	/**
	 * Server sesssion and subsession used during the tests
	 */
	RPositionServer iServer;
	RPositioner iPositioner;
	
	/** 
	 * Flag used to indicate if the A-GPS integration test module is being used during testing.
	 */
	TBool iTestModuleInUse;
	
	};

#endif //CTLBSCLIENTSTEPCONCURRENT_H
