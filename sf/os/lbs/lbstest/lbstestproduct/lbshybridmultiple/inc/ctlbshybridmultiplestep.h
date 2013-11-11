// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbshybridmultiplestep.h
// This contains the header file for Lbs Hybrid Multiple Test Step Base
// 
//

#ifndef __CT_LBS_HYBRID_MULTIPLE_STEP_H__
#define __CT_LBS_HYBRID_MULTIPLE_STEP_H__

#include <test/testexecutestepbase.h>
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/test/ctlbsagpsmoduleeventlistener.h>
#include <lbs/test/lbsnetextendedprotocolproxy.h>

#include "ctlbshybridmultipledefs.h"
#include "ctlbshybridmultipleserver.h"
#include "clbstestsessionsequence.h"
#include "clbstestmolrsessionsequence.h"
#include "clbstestmtlrsessionsequence.h"
#include "clbstestx3psessionsequence.h"
#include "clbstestngmessagehandler.h"

/* Main test step for running multiple sequences.  It supports running multiple sessions concurrently
 *  over one or two Protocol Modules.  Any number of sessions can be run over the different protocol
 *  modules and the sessions can be either MTLR, MOLR, NI MTLR or X3P.  The test is driven by the test
 *  data ini file, which defines the different sequences.
 *  Unlike the other hybrid tests, this test framework is non-blocking which allows the test to wait
 *  for a message for one session and send a message for another.
 *  
 *  This class is used for setting up for the test (test step pre and post amble) and contains no
 *  implementation of the callbacks.
 */
class CT_LbsHybridMultipleStep : public CTestStep, public MT_AgpsModuleEventHandler, 
									public MLbsTestNgMessageHandler, public MHybridMultipleTestCallBack
{

public:
	virtual ~CT_LbsHybridMultipleStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CT_LbsHybridMultipleStep(CT_LbsHybridMultipleServer& aParent);
	void ConstructL();
	
protected:
	//From MLbsTestNgMessageHandler (CT_LbsHybridMultipleTest implements)
	virtual void OnHandleNGMessage(TUint aPmId, TInt aMessageType) = 0;
	
	//From MHybridMultipleTestCallBack (CT_LbsHybridMultipleTest implements)
	virtual void StopTest(TInt aError) = 0;
	virtual void LogTestStatement(const TDesC& aMessage) = 0;
	virtual void SequenceFinished() = 0;
	virtual void SignalCheckForNewSequences() = 0;
	
	//Read in the Test data Config
	void ReadTestDataConfig(TInt aCurrentNum);
	
	CLbsTestNgMessageHandler* LookupProtocolModuleProxy(TInt aPmId);
	TInt CreateProtocolModuleProxy(TInt aPmId);
	
private: 
	// from MT_AgpsModuleEventHandler
	virtual void OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions);

protected:
	CT_LbsHybridMultipleServer& iParent;
	
	//Contains all the different sequences that the test needs to run
	RPointerArray<CTestSessionSequence> iSequences;
	
	//Contains the different Protocol Proxies for the different protocol Modules
	RPointerArray<CLbsTestNgMessageHandler> iProxy;
	
private:
	//GPS Options Integration Module information
	RPointerArray<TLbsGpsOptions> iExpectedModuleGpsOptions;
	TInt iExpectedModuleGpsOptionsIndex;
	CT_AgpsModuleEventListener* iAgpsModuleEventListener;
};

struct SData			
	{
	TLbsNetSessionId sessionId;
	TVerdict response;
	};

#endif // __CT_LBS_HYBRID_MULTIPLE_STEP_H__
