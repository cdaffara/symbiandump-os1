// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Location Based Services SUPL Push APIs - common types
// 
//

/**
 @file
 @publishedPartner
 @deprecated
*/
#ifndef SUPL_PUSH_COMMON_H
#define SUPL_PUSH_COMMON_H


//-------------------------------------------------------------------------------
#include <e32base.h>


//-------------------------------------------------------------------------------
/** A type for the unique SUPL Push request identifier. 
@publishedPartner
@deprecated
*/
typedef TUint32 TLbsSuplPushRequestId;

//-------------------------------------------------------------------------------
/** A type for the channel on which a SUPL Push request was received
@see _TLbsSuplPushChannel
@publishedPartner
@deprecated
*/
typedef TUint32 TLbsSuplPushChannel;

/** The response to a privacy request.

Note that the TLbsSuplPushChannel enum may be extended in the future by adding
more enumerated values. To maintain compatibility any unrecognized values 
must be handled as ELbsSuplPushChannelUnknown. 
@see TLbsSuplPushChannel
@publishedPartner
@deprecated
*/
enum _TLbsSuplPushChannel
	{
		/** Not used/unknown */
	ELbsSuplPushChannelUnknown = 0,
		/** Used to open a SUPL SMS Push channel. */
	ELbsSuplPushChannelSMS		= 1,
		/** Used to open a SUPL WAP Push channel. */
	ELbsSuplPushChannelWAP		= 2
	};


#endif //SUPL_PUSH_COMMON_H
