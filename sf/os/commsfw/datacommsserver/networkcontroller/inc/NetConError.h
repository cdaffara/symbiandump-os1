/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file NetConError.h
*/

#ifndef __NETCONERROR_H__
#define __NETCONERROR_H__

// Network Controller external errors and error strings

/**
Network Controller Base error

@internalComponent
*/
const TInt KNetConErrorBase = -3600;

/**
Network Controller Undefined Default Database error

@internalComponent
*/
const TInt KErrNetConDatabaseDefaultUndefined = KNetConErrorBase - 6;

/**
Network Controller Unknown Database Type error

@internalComponent
*/
const TInt KErrNetConDatabaseTypeUnknown = KNetConErrorBase - 7;

/**
Network Controller Database Not Found error

@internalComponent
*/
const TInt KErrNetConDatabaseNotFound = KNetConErrorBase - 8;

/**
Network Controller No GPRS  Network error

@internalComponent
*/
const TInt KErrNetConNoGPRSNetwork = KNetConErrorBase -9;

/**
Network Controller Incorrect MSClass error

@internalComponent
*/
const TInt KErrNetConIncorrectMSClass = KNetConErrorBase -10;

/**
Network Controller Inadequate Signal Strengh error

@internalTechnology
*/
const TInt KErrNetConInadequateSignalStrengh = KNetConErrorBase -11;

/**
Network Controller State Machine Not Available error

@internalTechnology
*/
const TInt KErrNetConStateMachineNotAvailable = KNetConErrorBase -12;

#endif

