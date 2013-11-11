/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_LbsAdminSuiteStepBase.h
*/

#ifndef __TE_LBSADMIN_STEP_BASE__
#define __TE_LBSADMIN_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <lbs/lbsadmin.h>
// Please add your include here if you have 
const TInt KDefaultPMUidInt = 0x10281D70;
const TUid KDefaultPMUid = {KDefaultPMUidInt};
/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsAdminSuiteStepBase : public CTestStep
	{

public:
	virtual ~CTe_LbsAdminSuiteStepBase();
	CTe_LbsAdminSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
	RSemaphore iModifierSem;
	RSemaphore iObserverSem;
	CLbsAdmin* iLbsAdmin;
	};

#endif
