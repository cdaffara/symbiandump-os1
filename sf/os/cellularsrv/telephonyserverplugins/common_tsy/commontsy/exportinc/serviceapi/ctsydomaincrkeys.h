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
#ifndef CTSYDOMAINCRPKEYS_H
#define CTSYDOMAINCRPKEYS_H


/******************************************************************************
* Call Forwarding Indicator API
* Keys under this category are used in defining call forwardin information.
******************************************************************************/    
const TUid KCRUidCtsyCallForwardingIndicator = {0x10282826};

/**
* Applicability of request to a mobile service group.
* defined in etelmm.h
*/
const TUint32 KCtsyCallForwardingStatus = 0x00000001;

/**
* The Call Forwarding flavours defined in etelmm.h
* Set when call forwarding status is set
*/
const TUint32 KCtsyCallForwardingPhoneCFCondition = 0x00000002;

/**
* defined in etelmm.h
*/
const TUint32 KCtsyCallForwardingNumber = 0x00000003;


/**
* Current setting for indicator status.
* Can contain a number of flags from RMobilePhone::TCFUIndicatorStatus.
* Key includes TPckg<TUint32> type package 
*/
const TUint32 KCtsyCallForwardingIndicator = 0x00000004;

/**
* Unconditional Call Forwarding status
* Contains a unconditional Call Forwarding status, TUnconditionalCFStatus.
*/
const TUint32 KCtsyUnconditionalCFStatus = 0x00000005;


/******************************************************************************
* Message Waiting Indicator API
* Keys under this category are used in defining ICC message waiting indicators.
******************************************************************************/ 
const TUid KCRUidCtsyMessageWaitingIndicator = {0x10282827};

/**
* The flags to specify whether the indicator should be displayed or not.
* Clients must use integer values to retrieve the data from the central
* repository and use RMobilePhone::TMobilePhoneIndicatorDisplay flags 
* to map the content.  
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingDisplayStatus = 0x00000001;

/**
* The number of voicemail messages waiting for the "main" voice line.
* Clients must use integer values to retrieve the data. 
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingVoiceMailCount = 0x00000002;
 
/**
* The number of voicemail messages waiting for the "auxiliary"/secondary
* voice line.
* Clients must use integer values to retrieve the data. 
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingAuxLineCount = 0x00000003;

/**
* The number of messages waiting for the "data line".
* Clients must use integer values to retrieve the data. 
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingDataCount = 0x00000004;

/**
* The number of messages waiting for the "fax line".
* Clients must use integer values to retrieve the data. 
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingFaxCount = 0x00000005;

/**
* The number of "email" messages waiting. 
* Clients must use integer values to retrieve the data. 
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingEmailCount = 0x00000006;

/**
* The number of "other" messages waiting.
* Clients must use integer values to retrieve the data. 
* Default value is -1, uninitialized.
*/
const TUint32 KCtsyMessageWaitingOtherMessageCount = 0x00000007;

/*  DATA TYPES */ 

/**  Unconditional Call Forwarding status */
enum TUnconditionalCFStatus
    {
    /**  Call forward status unknown */
    KCFIndicatorUnknown = 0x1,
    /**  No calls forwarded */
    KCFNoCallsForwarded = 0x2,
    /**  Voice call forwarded, ALS not supported */
    KCFVoiceForwarded	= 0x4,
    /**  Video call forwarded, ALS not supported */
    KCFVideoForwarded = 0x8,
    /**  Call forwarded to voice mailbox, ALS not supported */
    KCFForwardedToVoiceMailbox = 0x10,
    /**  Voice Call forwarded to voice mailbox, ALS not supported */
    KCFVoiceForwardedToVoiceMailbox = 0x20,
    /**  Video Call forwarded to voice mailbox, ALS not supported */
    KCFVideoForwardedToVoiceMailbox = 0x40,
    /**  Call forwarded on line 1, ALS supported */
    KCFForwardedOnLine1 = 0x80,
    /**  Call forwarded on line 2, ALS supported */
    KCFForwardedOnLine2 = 0x100
    };

#endif //CTSYDOMAINCRPKEYS_H
