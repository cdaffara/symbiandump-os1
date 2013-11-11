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



#ifndef CTSYPRIVATECRPKEYS_H
#define CTSYPRIVATECRPKEYS_H
    
/**
 * For ME ALS CentRep access
 */
const TUid KCRUidCtsyMEAlsLine = { 0x102029A2 };

/**
 * ME ALS CentRep information
 */
const TUint KCtysMEAlsLineInfo = { 0x00000001 };
const TUint KCtsyMEAlsSupported = { 0x00000002 };

/*
* CFIS Cenrep Information
*/
const TUid KCRUidCtsyPrivateCallForwardingIndicator = { 0x10282DFE };

/**
* SIM support ICC Call Forwardin Indicator
* Clients must use only integer values between 0 - 1 (EFalse-ETrue).
*/
const TUint32 KCtsyCallForwardingSupported                  = 0x00000001;

/**
* -
* Clients must use only integer values between 1 and 10.
* Change affects to...
*/
const TUint32 KCtsyCallForwardingMspId                      = 0x00000002;


/**
* Number plan enum values between 1 - 10.
*/
const TUint32 KCtsyCallForwardingNumberPlan                 = 0x00000003;

/**
* A new "forward-to" number to be registered against the CF condition. This is 
* applicable if iAction=EServiceActionRegister. In other cases, it is set to
* an empty string.
* defined in etelmm.h
*/
const TUint32 KCtsyCallForwardingAction                     = 0x00000004;

/**
* The new "No Reply" time-out in seconds to be registered.
*/
const TUint32 KEFCfisiTimeout                               = 0x00000005;

/**
* Indicates whether the SIM card has changed after bootup or not.
*/
const TUint32 KCtsyIMSI                                     = 0x00000006;

#endif //CTSYPRIVATECRPKEYS_H
