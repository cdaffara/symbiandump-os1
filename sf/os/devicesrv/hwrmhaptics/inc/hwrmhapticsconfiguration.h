/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics configuration header file.
*
*/


#ifndef HWRMHAPTICSCONFIGURATION_H
#define HWRMHAPTICSCONFIGURATION_H

#include <e32base.h>
#include <e32uid.h>

const TInt KServerVersionMajor       = 1;
const TInt KServerVersionMinor       = 1;
const TInt KServerVersionBuild       = 1;
const TInt KServerCActivePriority    = CActive::EPriorityUserInput;
const TInt KDefaultAsyncSlots        = 12;
const TInt KHWRMHapticsShutdownDelay = 2000000; // 2s

_LIT( KServerProcessName, "!hwrmhapticsserver" );
_LIT( KServerExeName, "hwrmhapticsserver.exe" );
_LIT( KServerExeDrive, "Z:" );

#endif  // HWRMCONFIGURATION_H
            
// End of File
