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





#ifndef HWRMDOMAINPSKEYS_H
#define HWRMDOMAINPSKEYS_H

// INCLUDES
#include <e32std.h>

/**
* Uid for PubSub domain APIs of the HW Resource Manager
*
* @publishedPartner
* @released
*/
const TUid KPSUidHWRM = {0x10205047}; 


// =============================================================================
// HW Resource Blocking API
// =============================================================================

// Use TUid KPSUidHWRM = {0x10205047};

// Key consts in this API should be defined so that they are maskable in bitmask,
// i.e. 1, 2, 4, 8, and so on.

/**
* Vibra blocking by another server.
*
* Set this P&S key to a non-zero value when the vibra needs to be blocked.
* Use with key KPSUidHWRM.
*
* @publishedPartner
* @released
*/
const TUint32 KHWRMAccessoryVibraBlocked = 0x00000001;


// =============================================================================
// Device Switch Status API
// =============================================================================
 
// Use TUid KPSUidHWRM = {0x10205047}; 
 
/**
* Indicates the status of twist.
* Use with KPSUidHWRM.
* 
* @publishedPartner
* @released
*/
const TUint32 KHWRMTwistStatus  = 0x00000100;

/**
* Enumerator for twist status.
*
* @publishedPartner
* @released
*/
enum TPSHWRMTwistStatus
     {
     /**
     Twist state uninitialized.
     */
     EPSHWRMTwistStatusUninitialized = 0,
     /**
     Twist state is closed.
     */
     EPSHWRMTwistClose,
     /**
     Twist state is open.
     */
     EPSHWRMTwistOpen
     };

/** 
* The current grip status.
*
* @publishedPartner
* @released
*/
const TInt KHWRMGripStatus = 0x00000101; 

/**
* Enumerator for grip status.
* @publishedPartner
* @released
*/
enum EPSHWRMGripStatus
    {
    /**
    Grip state uninitialized.
    */
    EPSHWRMGripStatusUninitialized = 0,
    /**
    Grip state is open.
    */
    EPSHWRMGripOpen,
    /**
    Grip state is closed.
    */
    EPSHWRMGripClosed
    };

/**
* Flip status. 
*
* @publishedPartner
* @released
*/
const TUint32 KHWRMFlipStatus         = 0x00000102;

/**
* Enumerator for flip status.
* @publishedPartner
* @released
*/
enum EPSHWRMFlipStatus
    {
    /**
    Flip state uninitialized.
    */
    EPSHWRMFlipStatusUninitialized = 0,
    /**
    Flip state is open.
    */
    EPSHWRMFlipOpen,
    /**
    Flip state is closed.
    */
    EPSHWRMFlipClosed
    };


#endif      // HWRMDOMAINPSKEYS_H

// End of File

