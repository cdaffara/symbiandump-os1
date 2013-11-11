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
 * Description:
 * Constant definitions for Plugged Display Engine's finite state 
 * machines.
 *
 */

#ifndef PDECONSTANTS_H
#define PDECONSTANTS_H

// SYSTEM INCLUDE FILES
#include <e32base.h>

// USER INCLUDE FILES

// FORWARD DECLARATIONS

// DATA TYPES

/** Plugged Display Engine's FSM identifications. */
enum TPDEFSMId
    {
    EPDEFSMIdHDMICableStatus = 0,
    EPDEFSMIdCompositeCableStatus,
    
    // Add new constants before this row.
    // Following constant is not a state.
    // It just indicates maximum index value.
    EPDEFSMIdFirstUnused,
    EPDEFSMIdNbrOfUsedIds = EPDEFSMIdFirstUnused,
    EPDEFSMIdUndefined = 0xff
    };

/**  Constants for used external interfaces/APIs.  */
enum TPDEInterface
    {
    // Interface ids
    EPDEIfUndefined = 0,
    EPDEIfTVOutConfig,
    EPDEIfEDIDHandler,
    EPDEIfAccessoryControl,
    EPDEIfAccessoryConnection,
    EPDEIfCentralRepositoryWatch,
    EPDEIfAsyCommandHandler,
    
    // TV Out Config interface's event ids    
    EIfTVOutConfigEventUndefined = 0x10,
    EIfTVOutConfigEventCableConnected,
    EIfTVOutConfigEventCableDisconnected,
    EIfTVOutConfigEventCableStatusListenFailed,
    EIfTVOutConfigEventEnabled,
    EIfTVOutConfigEventEnableFailed,
    EIfTVOutConfigEventDisabled,
    EIfTVOutConfigEventDisableFailed,
    EIfTVOutConfigEventSet,
    EIfTVOutConfigEventSetFailed,
    EIfTVOutConfigEventSetCopyProtectStatusChanged,
    EIfTVOutConfigEventCopyProtectionListenFailed,
    EIfTVOutConfigEventSettingsChanged,
    EIfTVOutConfigEventSettingsChangesListenFailed,
    EIfTVOutConfigEventSettingsChangesListenStarted,
    
    // EDID Handler interface's event ids    
    EPDEIfEDIDHandlerEventUndefined = 0x20,
    EPDEIfEDIDHandlerEventEdidDataFetched,
    EPDEIfEDIDHandlerEventEdidDataFetchFailed,
    
    // EDID Handler interface's event ids    
    EPDEIfAccessoryControlEventUndefined = 0x30,
    EPDEIfAccessoryControlEventConnected,
    EPDEIfAccessoryControlEventConnectFailed,
    EPDEIfAccessoryControlEventDisconnected,
    EPDEIfAccessoryControlEventDisconnectFailed,

    // Accessory Connection interface's event ids    
    EPDEIfAccessoryConnectionEventUndefined = 0x40,
    EPDEIfAccessoryConnectionEventCableConnected,
    EPDEIfAccessoryConnectionEventCableDisconnected,
    
    // Central Repository interface's event ids    
    EPDEIfCentralRepositoryWatchEventUndefined = 0x50,
    EPDEIfCentralRepositoryWatchEventKeyChanged,
	EPDEIfCentralRepositoryWatchEventOverscanKeyChanged,
    
    // ASY command Handler interface's event ids
    EPDEIfAsyCommandHandlerEventUndefined = 0x60,
    EIfAsyCommandHandlerEventSetCopyProtectionOn,
    EIfAsyCommandHandlerEventSetCopyProtectionOff,
    EIfAsyCommandHandlerEventGetCopyProtectStatus
    
    };

_LIT( KHDMICableStatusFsm, "HDMICableStatusFsm" );
_LIT(KHDMICableStateConnected, "ST_CONNECTED");
_LIT(KHDMICableStateIdle, "ST_IDLE");
_LIT(KHDMICableStateRejected, "ST_REJECTED");

/**  Constants for states of HDMI Cable Status FSM.  */
enum THDMICableState
    {
    EHDMICableStateIdle = 0,
    EHDMICableStateConnected,
    EHDMICableStateRejected,
    // Add new constants before this row.
    // Following constant is not a state.
    // It just indicates maximum index value.
    EHDMICableStateMaxValue,
    EHDMICableStateNbrOfUsedStates = EHDMICableStateMaxValue,
    EHDMICableStateUndefined = 0xff

    };


_LIT( KCompositeCableStatusFsm, "CompositeCableStatusFsm" );
_LIT(KCompositeCableStateConnected, "ST_CONNECTED");
_LIT(KCompositeCableStateIdle, "ST_IDLE");

/**  Constants for states of Composite Cable Status FSM.  */
enum TCompositeCableState
    {
    ECompositeCableStateIdle = 0,
    ECompositeCableStateConnected,
    // Add new constants before this row.
    // Following constant is not a state.
    // It just indicates maximum index value.
    ECompositeCableStateMaxValue,
    ECompositeCableStateNbrOfUsedStates = ECompositeCableStateMaxValue,
    ECompositeCableStateUndefined = 0xff
    };

#endif // PDECONSTANTS_H
