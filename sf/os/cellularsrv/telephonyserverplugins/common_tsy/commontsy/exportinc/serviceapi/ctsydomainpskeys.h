/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/
#ifndef CTSYDOMAINPSKEYS_H
#define CTSYDOMAINPSKEYS_H

// INCLUDES
#include <e32std.h>

/**
 * For Call forwarding number
 */  
const TUid KPSUidCtsyCallForwardingInfo = { 0x1020299D };

/**
 * For Emergency call info
 * Provides information whether an emergency call is active or not
 */  
const TUid KPSUidCtsyEmergencyCallInfo = { 0x102029A0 };
    
/** For Emergency call info */
enum KEmergencyCallInfo 
    {
    KCTSYEmergencyCallInfo,
    };
   
/**
* Telephony Call Information API.
*/
const TUid KPSUidCtsyCallInformation = {0x102029AC};
/**
* State of ongoing call(s). The following table shows states if
* in different combinations of phone calls.
* 
* Call 1 state    Call 2 state    TPSCTsyCallState
* =========================================================
* idle            idle            EPSCTsyCallStateNone
* hold            idle            EPSCTsyCallStateHold
* hold            dialling        EPSCTsyCallStateDialling
* hold            ringing         EPSCTsyCallStateRinging
* hold            answering       EPSCTsyCallStateAnswering
* hold            connected       EPSCTsyCallStateConnected
* connected       connected       EPSCTsyCallStateConnected
* connected       ringing         EPSCTsyCallStateRinging
* disconnecting   connected       EPSCTsyCallStateConnected
* disconnecting   hold            EPSCTsyCallStateHold
* disconnecting   idle            EPSCTsyCallStateDisconnecting
* disconnecting   ringing         EPSCTsyCallStateRinging
*
* @type: RProperty::EInt

*  For single calls 
       *       

       *  Mobile originated call states: 

       *     (1) EPSCTsyCallStateNone 
       *     (2) EPSCTsyCallStateDialling 
       *     (3) EPSCTsyCallStateAlerting 
       *     (4) EPSTelephonyCallStateConnected 
       *     (5) EPSCTsyCallStateDisconnecting 
       *     (6) EPSCTsyCallStateNone  
       * 
       *       Note that state transitions (2) --> (5) and (3) --> (5) are also possible.      
       * 
       * Mobile terminated call states: 

       *     (1) EPSCTsyCallStateNone 
       *     (8) EPSCTsyCallStateRinging 
       *     (4) EPSTelephonyCallStateConnected 
       *     (5) EPSCTsyCallStateDisconnecting 
       *     (6) EPSCTsyCallStateNone 
       *     
       *       Note that state transition (8) --> (5) is also possible.      
       * 
       *  If the call is put on Hold the the following state transition are possible: 
       *  (4) --> (9) EPSCTsyCallStateHold --> (4) 
       *  (4) --> (9) --> (5) 

*/ 

/**
 * State of ongoing call.  
 *
 */
const TUint32 KCTsyCallState = 0x00000001; 
enum TPSCTsyCallState
    {
    /** Call has not been initialized */
    EPSCTsyCallStateUninitialized,
    /** None */
    EPSCTsyCallStateNone,
    /** Call is in alerting state */
    EPSCTsyCallStateAlerting,
    /** Call is in ringing state */
    EPSCTsyCallStateRinging,
    /** Call is being dialled */
    EPSCTsyCallStateDialling,
    /** Call is being answered */
    EPSCTsyCallStateAnswering,
    /** Call is being disconnected */
    EPSCTsyCallStateDisconnecting,
    /** Call is connected */
    EPSCTsyCallStateConnected,
    /** Call is held */
    EPSCTsyCallStateHold
    };

/**
* Type of ongoing call.  
*
*/
const TUint32 KCTsyCallType = 0x00000002; 
enum TPSCTsyCallType
    {
    /** Call type has not been initialized */
    EPSCTsyCallTypeUninitialized,
    /** None */
    EPSCTsyCallTypeNone,
    /** CS voice call */
    EPSCTsyCallTypeCSVoice,
    /** Fax call */
    EPSCTsyCallTypeFax,
    /** Data call */
    EPSCTsyCallTypeData,
    /** HSCSD call */
    EPSCTsyCallTypeHSCSD,
    /** H.324 multimedia call */
    EPSCTsyCallTypeH324Multimedia,
    /** VoIP call */
    EPSCTsyCallTypeVoIP
    };    

#endif //CTSYDOMAINPSKEYS_H
