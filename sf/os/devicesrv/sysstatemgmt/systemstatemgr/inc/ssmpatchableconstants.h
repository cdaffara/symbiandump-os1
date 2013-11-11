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
// Description: Patchable constants defined in SSM
//

#ifndef __SSMPATCHABLECONSTANTS_H__
#define __SSMPATCHABLECONSTANTS_H__
#include <e32def.h>

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
/**
 * Patchable constant to determine if System State Manager(SSM) staged shutdown or existing shutdown server 
 * mechanism is to be used for device shutdown.
 *
 * This value determines if System State Manager/Shutdown server is used for device shutdown.
 * The default value for KSsmGracefulShutdown is 0x0, i.e., Shutdown server is used for device shutdown.
 *
 * If device shutdown is to be done through SSM staged shutdown this value must be set to '0xFFFFFFFF'.
 *
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 * 
 * "patchdata <dll> @ <symbol> <newvalue>"
 *
 * e.g. to enable System State Manager for device shutdown, use the following line:
 * "patchdata ssmcmn.dll @ KSsmGracefulShutdown 0xFFFFFFFF"
 *
 * @SYMPatchable
 * @publishedPartner
 * @released
 *
 */
IMPORT_C extern const TUint32 KSsmGracefulShutdown;

IMPORT_C TUint32 IsSsmGracefulShutdown();
#endif // SYMBIAN_SSM_GRACEFUL_SHUTDOWN

/**
 * 
 * This patchable constant enables the feature of graceful offline notification.
 * The default value for KSsmGracefulOffline is 0x0. 
 * 
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 * 
 * "patchdata <dll> @ <symbol> <newvalue>"
 * By default this feature will not be enabled.
 * e.g. to enable graceful offline notification 
 * "patchdata ssmcmn.dll @ KSsmGracefulOffline 0xFFFFFFFF" 
 * 
 * @SYMPatchable
 * @publishedPartner
 * @released
 * 
 */
IMPORT_C extern const TUint32 KSsmGracefulOffline;

IMPORT_C TUint32 IsSsmGracefulOffline();


/**
 * 
 * This patchable constant enables the feature for checking of Invalid Sim
 * The default value for KSsmInvalidSim is 0x0. 
 * 
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 * 
 * "patchdata <dll> @ <symbol> <newvalue>"
 * By default this feature will not be enabled.
 * e.g. to enable feature to check whether sim is invalid
 * "patchdata ssmcmn.dll @ KSsmInvalidSim 0xFFFFFFFF" 
 * 
 * @SYMPatchable
 * @publishedPartner
 * @released
 * 
 */
IMPORT_C extern const TUint32 KSsmInvalidSim;

#endif	// __SSMPATCHABLECONSTANTS_H__
