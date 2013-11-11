/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines a set of operations that can be delivered through 
*				 Remote Control framework between Series 60 components and 
*				 remote targets (e.g. headset).
*
*/


#ifndef REMCONEXTENSIONAPI_H
#define REMCONEXTENSIONAPI_H

#include <e32base.h>

/** 
Maximum character length of the operation parameters send between S60 
components and remote targets. 
*/
const TUint KRemConExtParamMaxLength = 255;

/**
The UID identifying POC outer-layer RemCon interface.
*/
const TInt KRemConExtPocApiUid = 0x1020504D; 

/**
Operation ids belonging to POC interface.
These are public so bearers/converters can access them.
*/
enum TRemConExtPocApiOperationId
	{
	ERemConExtAPOCButtonUp				= 0x01,
	ERemConExtAPOCButtonDown			= 0x02
	};
	
/**
The UID identifying synchronization outer-layer RemCon interface.
*/
const TInt KRemConExtSynchronizationApiUid = 0x10205052;  

/**
Operation ids belonging to volume interface.
These are public so bearers/converters can access them.
*/
enum TRemConExtSynchronizationApiOperationId
	{
	ERemConExtSynchronization				= 0x01
	};

/**
The UID identifying call handling outer-layer RemCon interface.
*/
const TInt KRemConExtCallHandlingApiUid = 0x1020504C; 

/**
Operation ids belonging to call handling interface.
These are public so bearers/converters can access them.
*/
enum TRemConExtCallHandlingApiOperationId
	{
	ERemConExtAnswerCall				= 0x00,
	ERemConExtEndCall					= 0x01,
	// If call status != ongoing call or incoming call,  
	// ERemConExtAnswerEnd means "change radio chanel".
	ERemConExtAnswerEnd					= 0x02,
	ERemConExtVoiceDial					= 0x03,
	ERemConExtLastNumberRedial			= 0x04,
	ERemConExtDialCall					= 0x05,
	ERemConExt3WaysCalling				= 0x06,
	ERemConExtGenerateDTMF				= 0x07,
	ERemConExtSpeedDial					= 0x08
	};

/**
Operation ids for the bearer source
*/
enum TRemConExtCmdSource
    {
    ERemConExtCmdSourceUnKnown = 0,  // Source Unknown
    ERemConExtCmdSourceBluetooth,  // Source BT Accessory
    ERemConExtCmdSourceWired,  // Source Wired Accessory
    ERemConExtCmdSourcePhone  // Source Phone - Media key bearer
    };

#endif 	// REMCONEXTENSIONAPI_H
