/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for the AccessoryMonitor capsmodifier
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <StifTestInterface.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    RDebug::Print( _L( "AccessoryMonitor capsmodifier starting" ) );

    // This starts a new session that get capabilites that is used in 
    // accMonitoringTestModule_exe.mmp file.
    TInt r = StartSession();

    RDebug::Print( _L( "AccessoryMonitor capsmodifier ends" ) );
    return r;
    }

// End of File
