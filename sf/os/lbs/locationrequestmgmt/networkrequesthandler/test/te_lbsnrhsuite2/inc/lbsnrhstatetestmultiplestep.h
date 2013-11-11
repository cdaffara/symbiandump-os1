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
 @file lbsnrhstatetestmultiplestep.h
*/
 
#ifndef LBSNRHSTATETESTMULTIPLESTEP_H_
#define LBSNRHSTATETESTMULTIPLESTEP_H_

#include "lbsnrhstatetestbasestep.h"


/* Class to simplify testing the state changes / message sequences of the NRH
 * for multiple sessions.

The functions in this class are meant to be easy to use to build up simple 
sequences for the major NRH operations, e.g. Multi-MT-LR.

*/
class CLbsNrhStateTestMultipleStep : public CLbsNrhStateTestBaseStep
	{
public:
	CLbsNrhStateTestMultipleStep();
	~CLbsNrhStateTestMultipleStep();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

public:
	void Multi_MtLr_MoLrL();
	void Multi_MtLr_MoLr_CellL();
	void Multi_MtLr_X3pL();
	void Multi_MoLr_MoLr_CellL();
	void Multi_MoLr_X3pL();
	void Multi_MoLr_Cell_X3pL();
	void Multi_MtLr_MoLr_MoLr_Cell_X3pL();
	void Multi_MtLr_MtLr();
	void Multi_MtLr_MtLr_B();
	void Multi_MtLr_MtLr_CheckMode();
	void Multi_MtLr_NotSupported_CheckMode();
	void Multi_MtLr_Use_Admin_CheckMode();
	void MTLrAndThenAnEmergencyMtLrL();
	void Multi_MtLr_MtLr_CheckModeErrorToAllSessions();
	void Multi_MtLr_MtLr_CheckMode_Measurements();

	};

_LIT(KLbsNrhStateTestMultipleStep, "lbsnrhstatetestmultiplestep");

#endif /*LBSNRHSTATETESTSINGLESTEP_H_*/
