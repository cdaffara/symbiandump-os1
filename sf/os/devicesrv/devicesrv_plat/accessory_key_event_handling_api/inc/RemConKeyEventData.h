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
* Description:  Definitions for client usaga that are needed when using the 
*                Accessory Key Event Handling API.
*
*/



#ifndef TREMCONKEYEVENTDATA_H
#define TREMCONKEYEVENTDATA_H

//  INCLUDES
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

// CONSTANTS

// MACROS

// DATA TYPES
// Key code
enum TRemConKeyCode
{
    ERemConKeyCodeSendEnd = 1, // Send/End key
    ERemConKeyCodeVolumeUp,    // Volume up adjustment
    ERemConKeyCodeVolumeDown,  // Volume down adjustment
    ERemConKeyCodePlayPause,   // Play/pause playback
    ERemConKeyCodeStop,        // Stop playback
    ERemConKeyCodeBack,        // Previous track, rewind, previous radio 
                               // station etc. (On player short press skips to 
                               // previous track (down&none, up&short), long 
                               // press rewinds current track (down&none, 
                               // n * down&repeat, up&none))
    ERemConKeyCodeForward,     // Next track, fast forward, next radio station
                               // etc. (On player short press skips to next 
                               // track, long press fast forwards current 
                               // track)
    ERemConKeyCodePOC,         // POC (Push to talk)
    ERemConKeyCodeSync,        // Synchronize key (synchronize contacts, 
                               // calendar etc. between terminal and some other 
                               // device)
    ERemConKeyCodeMute,		   // Mute/unmute microphone during ongoing call.
    ERemConKeyCodeRedial,	   // Redial	
    //ERemConKeyCodeSend,		   // Send key
    //ERemConKeyCodeEnd,		   // End key		

    // Customizable key codes
    ERemConKeyCode1 = 10000,
    ERemConKeyCode2,
    ERemConKeyCode3,
    ERemConKeyCode4
};

// Key modifier
enum TRemConKeyModifier
{
    // For future usage (shift-, alt- key modifiers etc.).
    ERemConKeyModifierNone    = 1
};

// Key State
enum TRemConKeyState
{
    ERemConKeyStateUp = 1,     // Key is up, not pressed
    ERemConKeyStateDown        // Key is down, pressed
};

// Key event type
enum TRemConKeyEventType
{
    ERemConKeyEventNone = 1,   // No event type specified
    ERemConKeyEventShortPress, // Short key press event
    ERemConKeyEventLongPress,  // Long key press event
    ERemConKeyEventRepeat      // Repeating key event (key stays down longer
                               // than in long press event)
};

// Key event data structure
// Used in key event handling API of Acc RemConConverter
struct TRemConKeyEventData
    {
        // Key code
        TRemConKeyCode        iKeyCode;
        
        // Key modifier
        TRemConKeyModifier    iKeyModifiers;
        
        // Key state
        TRemConKeyState        iKeyState;
        
        // Key event
        TRemConKeyEventType    iKeyEventType;
    };

#endif      // TREMCONKEYEVENTDATA_H   
            
// End of File
