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
// @file CTLbsStepClearLog.h
// This contains the header file to clear the log
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_STEP_CLEARLOG_H__
#define __CT_LBS_STEP_CLEARLOG_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// For logging
#include "LbsLogger.h"
#include <logcli.h>
#include <logview.h>
#include <logwrap.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <logfilterandeventconstants.hrh>
#endif


#include <lbs/test/ctlbsasyncwaiter.h>

_LIT(KLbsStep_ClearLog, "LbsStep_ClearLog"); 

class CT_LbsStep_ClearLog : public CT_LbsStep
	{
public:
	static CT_LbsStep_ClearLog* New(CT_LbsServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsStep_ClearLog(CT_LbsServer& aParent);
		
	};

#endif // __CT_LBS_STEP_CLEARLOG_H__