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
// @file ctlbsroamingstep.h
// This contains the header file for Roaming Tests
// 
//

#ifndef __CT_LBS_ROAMING_STEP_H__
#define __CT_LBS_ROAMING_STEP_H__

// LBS includes.
#include <test/testexecutestepbase.h>
#include <lbs.h>
#include <lbscommon.h>

// LBS test includes.
#include "ctlbsnetsimstep.h"
#include "ctlbsassdataserver.h"
#include "ctlbsassdatamoduledata.h"
#include "ctlbsdoposupdate.h"

#define REFPOS_LAT				2
#define REFPOS_LONG				3
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100


// Literals used
_LIT(KLbsRoamingStep, "LbsRoamingStep");

// Callbacks flags.
const TLbsCallbackFlags  KLbsCallback_Got_NotifyUpdateAnswer	= 0x1000;
const TLbsCallbackFlags  KLbsCallback_Got_ModuleGpsModeAnswer	= 0x2000;


class CT_LbsRoamingStep :	    public	CT_LbsNetSimStep,
								public	MT_LbsAssDataModuleDataBusObserver,
								public	MT_LbsDoPosUpdateObserver
	{

public:
	~CT_LbsRoamingStep();

	static CT_LbsRoamingStep* New(CT_LbsAssDataServer& aParent);
	virtual TVerdict doTestStepL();
    

	// MT_LbsAssDataModuleDataBusObserver - Module out data bus updated.
	void HandleDataBusUpdateL(const TModuleDataOut& aModuleDataOut);

    // From MT_LbsDoPosUpdateObserver - Position notify update callback.
	void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus);



private:
	CT_LbsRoamingStep(CT_LbsAssDataServer& aParent);
	void ConstructL();
     
   CT_LbsAssDataServer& iParent;
	
   enum  {	EModuleResponseWaiting = 0, // Waiting for module response
			EModuleResponseGotFirst,    // Module response got first time 			
			EModuleResponseGotSecond,	// Module Response got second time			
            EFirstPosUpdate };
   
     /** Current module response status	
	 */
	 TInt iModuleResponse;
   
    /** Current test case being executed.
	*/
	TInt iTestCaseId;


	/** Active object wrapper to allow MO-LRs.
	*/
	CT_LbsDoPosUpdate* iDoPosUpdate;

	// To allow changing admin settings.
	CLbsAdmin*	iLbsAdmin;
	
	/** Test module data bus monitor, to allow the module to return test status to
		the test step.
	*/
	CT_LbsAssDataModuleDataBusMonitor* iModuleDataBusMonitor;

	/** Reference position used to setup Net sim */
	TPosition iRefPos;
	
	/** passed to NPUD request to hold the position */
	TPositionInfo	iPositionHolder;
	};


#endif //__CT_LBS_ROAMING_STEP_H__
