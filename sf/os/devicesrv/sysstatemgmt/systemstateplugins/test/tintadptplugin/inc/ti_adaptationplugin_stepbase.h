// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TI_ADAPTATIONPLUGIN_STEPBASE_H__
#define __TI_ADAPTATIONPLUGIN_STEPBASE_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>

//for define, set and unset of P&S keys.
const TUint32 KEmergencyCallRfAdaptationPluginPropertyKey = 0x2000E657;
const TUint32 KStateAdaptationPluginPropertyKey = 0x2000D76A;
const TUint32 KRtcAdaptationPluginPropertyKey = 0x2000D76C;
const TUint32 KSimPluginPropertyKey = 0x2000D76B;
const TUint32 KMiscPluginPropertyKey = 0x2000E658;

const TUid KPropertyCategory={0x2000D75B};

class CTestAdaptStep : public CTestStep
	{
public:
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	void SetActivationCompleted();
	void StopActiveScheduler();
		
protected:
	CActiveScheduler* iActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	TBool iAdptTransitionCompleted;
	};

#endif