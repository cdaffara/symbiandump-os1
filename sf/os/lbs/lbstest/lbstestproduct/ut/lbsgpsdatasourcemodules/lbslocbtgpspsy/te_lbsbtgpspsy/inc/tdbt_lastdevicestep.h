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
 @file tdbt_lastdevicestep.h
 @internalTechnology
*/
#ifndef __TDBT_LASTDEVICE_STEP_H__
#define __TDBT_LASTDEVICE_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"

#include "LcfCommonProcedures.h"


enum TBTUsageStatus
    {
    EBTTestNotifierUnused = 0, // The test notifier has not been used
    EBTTestNotifierUsed  // The test notifier has been used
    };


// FORWARD DECLARATIONS
class TPositionModuleInfo;
class TPositionInfoBase;
class TPositionInfo;
class TPositionCourseInfo;


class CTDBT_LastDeviceStep : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT_LastDeviceStep();
	~CTDBT_LastDeviceStep();
	virtual TVerdict doTestStepL();
	// Sub test case functions: 

private:	
	/**
	* Runs the test in the module
	*/
	void StartL(TInt aIndex);
	void IsExpectedDeviceTypeL();
	void CheckNotifierUntouched();
	void GetPositionL();
	void SetTestNotifierStatusL(TInt aNotifierState);
	void CheckTestNotifierStatusL(TInt aNotifierState,TBool aFailIfWrong);
    };

_LIT(KTDBT_LastDeviceStep,"TDBTLastDeviceStep");

#endif
