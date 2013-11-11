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
* Description:  Accessory mode definitions and structures
*
*/


#ifndef ACCPOLACCESSORYMODE_H
#define ACCPOLACCESSORYMODE_H

//  INCLUDES
#include <e32std.h>

// DATA TYPES

/**
* Accessory Mode definitions.
*/
enum TAccMode
	{
	EAccModeHandPortable,    //Hand-portable 
	EAccModeWiredHeadset,    //Wired headset
	EAccModeWirelessHeadset, //Wireless headset
	EAccModeWiredCarKit,     //Wired carkit
	EAccModeWirelessCarKit,  //Wireless carkit
	EAccModeTextDevice,      //TTY
	EAccModeLoopset,         //Loopset
	EAccModeMusicStand,      //Musicstand
    EAccModeTVOut,           //TV-Out
    EAccModeHeadphones,      //Headphones
	EAccModeHDMI             //HDMI
	};

/**
* Accessory Mode structure, with active audio information.
*/
struct TAccPolAccessoryMode
	{
	// Accessory mode
	TAccMode iAccessoryMode;
	// ETrue accessory has active audio output, EFalse accessory is silent (no audio).
	// Audio client updates this value with the 
	// RAccessoryAudioControl::AudioRoutingStatusNotify() -method.
	TBool iAudioOutputStatus;
	};
	
#endif //ACCPOLACCESSORYMODE_H   
            
// End of File
