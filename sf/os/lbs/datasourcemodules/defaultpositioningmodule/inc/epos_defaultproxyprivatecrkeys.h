/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef EPOS_DEFAULTPROXYPRIVATECRKEYS_H
#define EPOS_DEFAULTPROXYPRIVATECRKEYS_H

/**
* @file epos_defaultproxyprivatecrkeys.h
*
* A private Central Repository API for storing configuration parameters 
* like UID of last working GPS PSY. These parameters control functionality 
* of the Default Proxy.
*/


/**
* Default Proxy Configuration API CenRep UID.
*/
const TUid KCRUidDefaultProxyConfiguration = { 0x101f7a7f };

/**
* Contains the UID of last time working PSY that based on GPS technology.
*
* The format of this key is string.
*/
const TUint32 KDefaultProxyLastWorkingGpsPsy = 0x10000001;

/**
* Maximum timeshift value when a GPS PSY is in ready or active state. If a 
* GPS PSY is in ready or active state and it can't give a fix information 
* within timeshift value, default proxy will try next PSY.
*
* The format of this key is integer.
*/
const TUint32 KDefaultProxyTimeshiftInActiveOrReady = 0x10000002;

/**
* Maximum timeshift value when a GPS PSY is in other state than ready or 
* active state. If a GPS PSY is not in active or ready state, and it failed
* to give a fix within this timeshift value, default proxy will try next PSY.
*
* The format of this key is integer.
*/
const TUint32 KDefaultProxyTimeshiftNotInActiveOrReady = 0x10000003;

/**
* Timeout value to change PSY fix state to unknown when a GPS PSY is
* not used any more( no location request on going). 
*/
const TUint32 KDefaultProxyPsyStateUnknownTimeout = 0x10000004;

/**
* Default proxy will check the external GPS periodically. This value defines 
* the periodic length that default proxy shall check the external GPS device.
*/
const TUint32 KDefaultProxyExternalGPSCheckingTimeout = 0x10000005;

/**
* If there is no more location request received in default proxy, default 
* proxy will start the cleanup timer. When cleanup timer expires, default 
* proxy will cancel all location request that has been issued.
*/
const TUint32 KDefaultProxyCleanupTimeout = 0x10000006;


#endif      // EPOS_DEFAULTPROXYPRIVATECRKEYS_H  
            
// End of File
