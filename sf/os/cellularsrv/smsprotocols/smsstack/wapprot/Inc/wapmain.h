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
// Contains panic codes for WAPPROT
// 
//

/**
 @file
 @internalAll
*/

#ifndef __WAPMAIN_H__
#define __WAPMAIN_H__

#include <e32base.h>

/**
 *  WAP prot panic codes
 *  @internalComponent
 */
enum TWAPSMSPanic
	{
//  wapthdr.inl & wapthdr.cpp
	KPanicTextHeaderMessageNotSet=0x00,
	KPanicTextHeaderNotTextHeader,
	KPanicTextHeaderTooLong,
    // *Invalid*: does not exist or not a number
	KPanicInvalidParseNumber,

//  wappstor.cpp
    KPanicTooLongSmsFragment=0x10,

//	wapdgrm.h
    KPanicSmsMessageNotComplete=0x20,
    KPanicUsageError,
    KPanicConversionNotAvailable,
    KPanicConversionIncomplete,
    KPanicNServerSession,
    KPanicTooLongData,
    // from WapSmsPanc()
    EWapSmsCantBind,
    EWapSmsCantBindTo,
    EWapSmsSendCallCantBind,
    EWapSmsProcessCallCantBind,
    EWapSmsSetOptionCallCantBind,
    EWapSmsGetOptionCallCantBind,
    EWapSmsCantCreateNetDatabase,
    EWapSmsCantCreateServiceResolver,
    EWapSmsCantCreateHostResolver,
    EWapSmsBadWriteCall,
    EWapSmsBadGetDataCall,
    EWapSmsReadGetTooMuch,
    EWapSmsCloseWithoutDeleteingAllSockets,
    EWapSmsBMABroken,
    EWapSmsReallyReallyBad,

    EWapSmsBadBind,
    EWapSmsAddressTooLong,
    EWapSmsIncompleteSms,
    EWapSmsReceiveOnMessageSender,
    EWapSmsSapMessageSenderBusy,
    EWapSmsSapMessageSenderNothingToSend,
    EWapSmsCannotBindObserver0,
    EWapSmsCannotBindObserver1,
    EWapSmsCannotBindObserver2,
    EWapSmsFailedToSetServiceCenterAddress,
    EWapSmsCurrentBearerNotSms,

	EWapSmsNotFoundInStore,
	EWapSmsOOM,

	KPanicSmsMsgTimeZoneOutOfRange,
    KPanicWapDgrmTimeZoneOutOfRange,
    KPanicUserSuppliedTimeZoneOutOfRange,
    KPanicEncodingError
	};

/**
 *  @internalComponent
 */
void Panic(TWAPSMSPanic aPanic);

#endif // WAPMAIN_H__
