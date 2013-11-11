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

CSetDiskSpace::CSetDiskSpace()
    {
    iPartOfMultiStepTestCase = ETrue;
    }

/**
 *  Set free disk space to the required limit
 */
enum TVerdict CSetDiskSpace::doTestStepL()
    {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else
    ReleaseDiskSpaceL();
    
    TInt  freeDrop = 0;
    GetIntFromConfig(ConfigSection(), _L("freeDrop"), freeDrop);         
 
    SetFreeDiskSpaceFromDropLevelL(freeDrop);
#endif
    
	return TestStepResult();
    }
