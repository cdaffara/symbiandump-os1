// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

// A "utility class" that contains a number of static functions that don't 
// have anywhere else obvious to live.

#ifndef __EGLTEST_ENDPOINT_UTIL_H__
#define __EGLTEST_ENDPOINT_UTIL_H__

#include <e32base.h>

// Forward declare 
class CTestExecuteLogger;

NONSHARABLE_CLASS(TEndpointUtil)
    {
public:
    static void SetLoggerForProcessWrapperL(CTestExecuteLogger& aLogger);
    };

#endif
