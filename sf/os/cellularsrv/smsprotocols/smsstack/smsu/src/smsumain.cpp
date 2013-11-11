// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements panic and e32dll functions for SMSU
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsumainTraces.h"
#endif

#include "smsumain.h"

#include "smsstacklog.h"

/**
 *  panic in SMSU
 */
GLDEF_C void SmsuPanic(TSmsuPanic aPanic)
    {
    // Ignore in code coverage  - panic method
    BULLSEYE_OFF
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, SMSUPANIC_1, "SMSUPANIC %d", aPanic);
    _LIT(KSmsuPanic, "SMSU");
    User::Panic(KSmsuPanic,aPanic);
    BULLSEYE_RESTORE
    }
