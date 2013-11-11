// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/
#ifndef _EIRMANPANICS_H
#define _EIRMANPANICS_H


/**
This class allows the enforcement of the type used in a panic macro.
The compiler should know not to generate any function for each template
instance.
*/
template <typename XAny>
struct TEirPanicCodeTypeChecker
	{
	inline static void Check(XAny) { }
	};
	

#define EIR_SERVER_PANIC(CODE) \
	TEirPanicCodeTypeChecker<TEirServerPanic>::Check(CODE), \
	PANIC(KBTEirServerPanic, CODE)
	
#define EIR_SESSION_PANIC(CODE) \
	TEirPanicCodeTypeChecker<TEirSessionPanic>::Check(CODE), \
	PANIC(KBTEirSessionPanic, CODE)
	
#define EIR_MANAGER_PANIC(CODE) \
	TEirPanicCodeTypeChecker<TEirManagerPanic>::Check(CODE), \
	PANIC(KBTEirManagerPanic, CODE)


enum TEirServerPanic
	{
	EEirServerEirMangerAlreadyExists = 0,
	};
_LIT(KBTEirServerPanic, "Bluetooth Eir Server");


enum TEirSessionPanic
	{
	EEirSessionInvalidEirTag = 0,
	EEirSessionMessageNotAvailable,
	EEirSessionEirFeatureNotSupported,
	};

_LIT(KBTEirSessionPanic, "BT EirManSession");


enum TEirManagerPanic
	{
	EEirManagerNotifieeNotCleared = 0,
//	EEirManagerFeatureSetNotReady = 1,
	EEirManagerEirNotSupported = 2,
	EEirManagerPanicInvalidTag = 3,
	EEirManagerUnexpectedEvent = 4,
	EEirManagerUnexpectedCompleteEvent = 5,
	};
_LIT(KBTEirManagerPanic, "BT Eir Manager");

#endif /*_EIRMANPANICS_H*/
