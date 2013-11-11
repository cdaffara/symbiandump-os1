/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMCONFIGURATION_H
#define HWRMCONFIGURATION_H

//  INCLUDES
#include <e32base.h>
#include <e32uid.h>

// CONSTANTS
const TInt KServerVersionMajor          = 1; // !!! update version 
const TInt KServerVersionMinor          = 0;
const TInt KServerVersionBuild          = 0;
const TInt KServerCActivePriority       = CActive::EPriorityUserInput;

// Separate TInt needed for KHWRMSidOnlyPolicy or DLL's won't compile because of static data
const TInt KServerUid2Int                  = 0x101F7A02;  
const TUid KServerUid2                  = { KServerUid2Int }; 

// FM Tx extension - the Audio Policy server receives notifications via P&S
const TInt KAudioPolicyUid2Int          = 0x101F457F;  
const TUid KAudioPolicyUid2             = { KAudioPolicyUid2Int }; 

const TInt KDefaultAsyncSlots           = 4; // Async message slots to be used in client server comms. 

_LIT( KServerProcessName, "!HWRMServer" );
_LIT( KServerExeName, "HWRMServer.exe" );
_LIT( KServerSemaphoreName, "HWRMSemaphore" );
_LIT( KServerFindPattern, "!HWRMServer*" );  // to find both already fully started and not yet fully started processess

_LIT( KServerExeDrive, "Z:" );

// Policies
_LIT_SECURITY_POLICY_PASS(KAlwaysPassPolicy);
_LIT_SECURITY_POLICY_S0(KHWRMSidOnlyPolicy, KServerUid2Int);
_LIT_SECURITY_POLICY_S0(KAudioPolicySidOnlyPolicy, KAudioPolicyUid2Int);
// BRANCH_END
_LIT_SECURITY_POLICY_C1(KWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
_LIT_SECURITY_POLICY_C1(KNoCapability, ECapability_None);

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

#endif  // HWRMCONFIGURATION_H
            
// End of File
