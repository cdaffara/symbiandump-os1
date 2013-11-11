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
// Implements the GSMU panic function and E32Dll
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "GsmumainTraces.h"
#endif

#include <e32std.h>
#include "Gsmumain.h"
#include "Gsmuelem.h"


/**
 *  panic in the GSMU
 */
GLDEF_C void Panic(TGsmuPanic aPanic)
    {
    // Ignore in code coverage - panic method
    BULLSEYE_OFF    
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, GSMUMAIN_PANIC_1, "GSMUPANIC %d", aPanic);
    
    _LIT(KGsmuPanic, "GSMU");
    User::Panic(KGsmuPanic, aPanic);
    BULLSEYE_RESTORE
    }
