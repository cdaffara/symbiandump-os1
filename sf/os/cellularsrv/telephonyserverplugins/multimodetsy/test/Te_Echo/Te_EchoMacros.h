// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_EchoSingleTestStep.h
// 
//

#ifndef __TE_ECHO_MACROS__
#define __TE_ECHO_MACROS__

#include <test/testexecutestepbase.h>

_LIT(KTestIfErrorFailText, "Failed: return code is %d");

#define TEST_FOR_ERROR_L(a) \
    { \
    if((a)!=(KErrNone)) \
	    { \
	    INFO_PRINTF2(KTestIfErrorFailText,a); \
        SetTestStepResult(EFail); \
        User::Leave(a); \
        } \
    }

#define FAIL_WITH_ERROR_L(a) \
    { \
	INFO_PRINTF2(KTestIfErrorFailText,a); \
    SetTestStepResult(EFail); \
    User::Leave(a); \
    }

#endif
