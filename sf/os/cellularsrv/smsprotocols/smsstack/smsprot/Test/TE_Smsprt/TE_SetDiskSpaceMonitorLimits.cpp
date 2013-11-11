
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
//

#include "TE_smsmondsk.h"

CSetDiskMonitorLimits::CSetDiskMonitorLimits()
/** 
    Each test step initialises it's own name
*/
    {
    iPartOfMultiStepTestCase = ETrue;
    }

enum TVerdict CSetDiskMonitorLimits::doTestStepL()
/**
	Creates smsu.rsc file which defines the upper and lower limits for the disk space monitor
*/
    {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else    
    TInt highDrop = 3;
    TInt lowDrop  = 8;
    TInt freeDrop = 0;
   
    GetIntFromConfig(_L("DiskMonitorLimits"), _L("highDrop"), highDrop);
    GetIntFromConfig(_L("DiskMonitorLimits"), _L("lowDrop"), lowDrop);
   
    SetHighLowLimitsAndDiskSpaceLevelL(highDrop, lowDrop, freeDrop);
#endif
    
	return TestStepResult();
    }
    
