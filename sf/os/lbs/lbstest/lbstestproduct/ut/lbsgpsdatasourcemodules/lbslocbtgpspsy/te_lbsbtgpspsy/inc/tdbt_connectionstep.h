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
 @file tdbt_connectionstep.h
 @internalTechnology
*/
#ifndef __TDBT_CONNECT_STEP_H__
#define __TDBT_CONNECT_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"

#include "LcfCommonProcedures.h"

// FORWARD DECLARATIONS
class TPositionModuleInfo;
class TPositionInfoBase;
class TPositionInfo;
class TPositionCourseInfo;

class CTDBT_ConnectStep : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT_ConnectStep();
	~CTDBT_ConnectStep();

	virtual TVerdict doTestStepL();

	// Sub test case functions:

private:	
	/**
	* Runs the test in the module
	*/
	void StartL(TInt aIndex);
	void ExpectPositionCancelL();
	void GetPositionL();
    };

_LIT(KTDBT_ConnectStep,"TDBTConnectionStep");

#endif 
