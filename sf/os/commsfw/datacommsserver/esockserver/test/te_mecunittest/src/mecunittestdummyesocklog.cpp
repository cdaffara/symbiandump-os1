// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mecunittestdummyesocklog.cpp
//

/**
@file
Dummy ESock logging stubs to satisfy linker
*/

#ifdef _DEBUG

#include <comms-infras/ss_log.h>

void ESockLog::Printf(TRefByValue<const TDesC8> aFmt,...)
    {
    (void)aFmt;
    }

void ESockLog::Printf(TRefByValue<const TDesC> aFmt,...)
    {
    (void)aFmt;
    }

void ESockLog::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
    {
    (void)aSubTag;
    (void)aFmt;
    }

void ESockLog::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, ...)
    {
    (void)aSubTag;
    (void)aFmt;
    }

#endif

