// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wapmainTraces.h"
#endif

#include "wapmain.h"
#include <e32std.h>
#include "smsstacklog.h"

void Panic(TWAPSMSPanic aPanic)
     {
     // Ignore in code coverag - panic method
     BULLSEYE_OFF
     OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, WAPMAIN_PANIC_1, "WAPUPANIC %d", aPanic);
     _LIT(KWapProtPanic, "WAPPROT");
     User::Panic(KWapProtPanic, aPanic);
     BULLSEYE_RESTORE
     }
