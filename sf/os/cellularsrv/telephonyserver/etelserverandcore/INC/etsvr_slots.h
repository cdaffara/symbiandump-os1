// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
//



/**
 @file
 @internalTechnology
 @released
*/
	
#if !defined(__ETSVR_SLOTS_H__)
#define __ETSVR_SLOTS_H__

#include <e32def.h>

//
//	splitted from etbuffer.h
//	Number of server-side slots for each notification (greater than 1)
//
/**
@internalTechnology
*/
const TUint KSlotNumbersDefault				= 1;
//
//	RCall
//
/**
@internalComponent
*/
const TUint KSlotNumbersCallCapsChange		= 2;
const TUint KSlotNumbersCallStatusChange	= 4;


//
//	RLine
//
/**
@internalComponent
*/
const TUint KSlotNumbersLineStatusChange	= 4;
const TUint KSlotNumbersLineCallAddedChange = 4;

//
//	splitted from etslotnum.h
//	Number of server-side slots for each notification (greater than 1)
//  These are suggested sizes and can be over-ridden by TSY by setting a different
//  number in the NumberOfSlotsL method
//

//
// RMobilePhone
//
/**
@internalTechnology
*/
const TUint KSlotNotifyModeChange = 2;
/**
@internalComponent
*/
const TUint KSlotNotifyBatteryInfoChange = 2;
const TUint KSlotNotifySignalStrengthChange = 2;
const TUint KSlotNotifyCurrentNetworkChange = 2;
const TUint KSlotNotifyNetworkRegistrationStatusChange = 3;
const TUint KSlotNotifyNITZInfoChange = 2;
const TUint KSlotNotifyCallServiceCapsChange = 2;
const TUint KSlotNotifyCostInfoChange = 3;
const TUint KSlotNotifySecurityEvent = 2;
const TUint KSlotNotifyMessageWaiting = 2;

//
// RMobilePhoneStore
//
/**
@internalComponent
*/
const TUint KSlotNotifyStoreEvent = 3;

//
// RMobileCall
//
/**
@internalComponent
*/
const TUint KSlotNotifyHscsdInfoChange = 2;
const TUint KSlotNotifyMobileCallCapsChange = 3;
const TUint KSlotNotifyCallEvent = 2;
const TUint KSlotNotifyMobileCallStatusChange = 3;
const TUint KSlotNotifyUUSCapsChange = 2;

//
// RMobileLine
//
/**
@internalComponent
*/
const TUint KSlotNotifyMobileLineStatusChange = 3;

//
// RMobileConferenceCall
//
/**
@internalComponent
*/
const TUint KSlotNotifyConferenceCapsChange = 2;
const TUint KSlotNotifyConferenceStatusChange = 2;
const TUint KSlotNotifyConferenceEvent = 2;

//
// RMobileSmsMessaging
//
/**
@internalComponent
*/
const TUint KSlotReceiveSmsMessage = 5;

//
// RMobileBroadcastMessaging
//
/**
@internalComponent
*/
const TUint KSlotReceiveBroadcastMessage = 3;

//
// RMobileUssdMessaging
//
/**
@internalComponent
*/
const TUint KSlotReceiveUssdMessage = 5;



#endif
