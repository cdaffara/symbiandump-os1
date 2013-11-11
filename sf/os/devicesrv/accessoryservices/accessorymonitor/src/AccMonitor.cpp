/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Main functionality of AccMonitor. Constructs the Accessory
*          Monitor implementation class
*
*/


#include <AccMonitor.h>
#include "AccMonitorImpl.h"
#include "acc_debug.h"

// ======== MEMBER FUNCTIONS ========
   
// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
CAccMonitor::CAccMonitor()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitor::CAccMonitor()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitor::CAccMonitor - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
EXPORT_C CAccMonitor* CAccMonitor::NewL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitor::NewL()" );
    
    CAccMonitor* impl = NewLC();
    CleanupStack::Pop( impl );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitor::NewL - return impl" );
    return impl;
    }

// ---------------------------------------------------------------------------
// CAccMonitor::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
EXPORT_C CAccMonitor* CAccMonitor::NewLC()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitor::NewLC()" );
    
    CAccMonitor* impl = new ( ELeave ) CAccMonitorImpl();
    CleanupStack::PushL( impl );
    impl->ConstructL();
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitor::NewLC - return" );
    return impl;
    }

// End of File
