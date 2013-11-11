
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

CFreeDiskSpace::CFreeDiskSpace()
    {
    iPartOfMultiStepTestCase = ETrue;
    }

/**
	Free up disk space by deleting the temp files created
*/
TVerdict CFreeDiskSpace::doTestStepL()
   {
#ifndef _DEBUG
    INFO_PRINTF1(_L("This test can only be run when the SMS Stack is in debug mode."));
#else   
	ReleaseDiskSpaceL();
#endif	
	  	
	return TestStepResult();
   }
