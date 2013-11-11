// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

/**
Message type enumeration.
*/
enum TRemConMessageType
	{
	/** Undefined message type. */
	ERemConMessageTypeUndefined = 0,

	/** Command. */
	ERemConCommand,
	
	/** Response. */
	ERemConResponse,

	/** Errored Response or Reject. */
	/** Treated as a command completion by RemCon */
	ERemConReject,
	
	/** NotifyCommand */
	ERemConNotifyCommand,
	};
	
/**
Message subtype enumeration.
*/
enum TRemConMessageSubType
	{
	/** Default command or response */
	ERemConMessageDefault = 0,
	
	/** Notify command requiring an interim response and (later) changed response */
	ERemConNotifyCommandAwaitingInterim,
	
	/** Notify command requiring only a changed response, after notify has been readdressed */
	ERemConNotifyCommandAwaitingChanged,
	
	/** Notify interim response */
	ERemConNotifyResponseInterim,
	
	/** Notify changed response */
	ERemConNotifyResponseChanged,
	};

#endif // MESSAGETYPE_H
