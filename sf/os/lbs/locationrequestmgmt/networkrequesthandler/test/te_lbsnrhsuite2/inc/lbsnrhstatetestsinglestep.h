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
 @file lbsnrhstatetestsinglestep.h
*/
 
#ifndef LBSNRHSTATETESTSINGLESTEP_H_
#define LBSNRHSTATETESTSINGLESTEP_H_

#include "lbsnrhstatetestbasestep.h"


/* Class to simplify testing the state changes / message sequences of the NRH.

The functions in this class are meant to be easy to use to build up simple 
sequences for the major NRH operations, e.g. MO-LR, X3P, MT-LR.

They don't do much validation of parameters - that should be covered by other tests.
*/
class CLbsNrhStateTestSingleStep : public CLbsNrhStateTestBaseStep
	{
public:
	CLbsNrhStateTestSingleStep();
	~CLbsNrhStateTestSingleStep();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
public:
	void SingleMtLrL();
	void SingleMtLrL2();
	void SingleMoLrL();
	void NewClientMoLrL();
	void SingleMoLr_CellL();
	void PreemptedX3pL();
	void SingleX3pL();
	
	void SingleSuplMtLrL();
	void SingleSuplMoLrL();
	
	void SingleMtLrMaxFixAgeL();
	void SingleMoLrMaxFixAgeL();

	void SingleMtLrRefAfterLocRequestL();
	
	void SingleEmergencyMtLrL();
	void SingleEmergencyNI();

	void SingleEmergencyNIRefPosBeforeLocReq();
	void SingleEmergencyMtLrLAdviceNotify();
	
	void SingleMtLrTimeoutPrivacyL();
	void SingleMtLrTimeoutLocationL();
	
	void GenericTestL();
private:
	};

_LIT(KLbsNrhStateTestSingleStep, "lbsnrhstatetestsinglestep");

#endif /*LBSNRHSTATETESTSINGLESTEP_H_*/
