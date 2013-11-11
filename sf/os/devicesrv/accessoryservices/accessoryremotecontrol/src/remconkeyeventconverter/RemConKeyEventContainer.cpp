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
* Description:  A container/accessor class for all CRemConKeyEvent -instances.
*
*/



// INCLUDE FILES
#include "RemConKeyEventContainer.h"
#include "RemConDebug.h"
#include <remconcoreapi.h>
#include <RemConExtensionApi.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemConKeyEventContainer::CRemConKeyEventContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConKeyEventContainer::CRemConKeyEventContainer()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventContainer::CRemConKeyEventContainer()" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRemConKeyEventContainer::ConstructL()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventContainer::ConstructL()" );

    TUid coreAPIUid;
    TUid pocAPIUid;
    TUid syncAPIUid;
    coreAPIUid.iUid = KRemConCoreApiUid;
    TUid extensionAPIUid;
    extensionAPIUid.iUid = KRemConExtCallHandlingApiUid;
    pocAPIUid.iUid = KRemConExtPocApiUid;
    syncAPIUid.iUid = KRemConExtSynchronizationApiUid;
    CRemConKeyEvent* event( NULL );
    TBuf8< KRemConExtParamMaxLength > parameter;
  	
  	//
    // Simple call handling key events:
    //
    
    // send/end key pressed shortly: answer call or end call
    TRemConKeyEventData k1 = { ERemConKeyCodeSendEnd, 
    						   ERemConKeyModifierNone, 
    						   ERemConKeyStateUp, 
    						   ERemConKeyEventShortPress };
    
    event = CRemConKeyEvent::NewL( k1, 
    							   extensionAPIUid, 
    							   ERemConExtAnswerEnd, 
    							   ERemConCoreApiButtonClick );
    							   
 	CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    	
    // send/end key pressed long time: reject ongoing call
    TRemConKeyEventData k2 = { ERemConKeyCodeSendEnd, 
    						   ERemConKeyModifierNone, 
    						   ERemConKeyStateUp, 
    						   ERemConKeyEventLongPress };

    event = CRemConKeyEvent::NewL( k2, 
    							   extensionAPIUid,
        						   ERemConExtEndCall, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
        
    // send/end key has been down long time: voice call
    TRemConKeyEventData k3 = { ERemConKeyCodeSendEnd, 
    						   ERemConKeyModifierNone, 
    						   ERemConKeyStateDown, 
    						   ERemConKeyEventLongPress };
    // set parameter
    TBool param( ETrue );
    parameter.AppendNum( param );
 	
 	event = CRemConKeyEvent::NewL( k3, 
    							   extensionAPIUid, 
    							   ERemConExtVoiceDial, 
    							   parameter );
    							   
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    //
    // POC key events:
    //
        
    // POC key down
    TRemConKeyEventData k4 = { ERemConKeyCodePOC, 
                               ERemConKeyModifierNone, 
                               ERemConKeyStateDown, 
                               ERemConKeyEventRepeat };
                               
    event = CRemConKeyEvent::NewL( k4, 
    							   pocAPIUid, 
        						   ERemConExtAPOCButtonDown, 
        						   ERemConCoreApiButtonPress );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    
    // POC key up (long)
    TRemConKeyEventData k5 = { ERemConKeyCodePOC, 
                               ERemConKeyModifierNone, 
                               ERemConKeyStateUp, 
                               ERemConKeyEventLongPress };
                               
    event = CRemConKeyEvent::NewL( k5, 
    							   pocAPIUid, 
        						   ERemConExtAPOCButtonUp, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    //
    // Volume control key events:
    //
    
    // Volume up: ERemConKeyCodeVolumeUp down; volume up/press
    TRemConKeyEventData k6 = { ERemConKeyCodeVolumeUp, 
                                 ERemConKeyModifierNone, 
                                 ERemConKeyStateDown, 
                                 ERemConKeyEventNone };

    event = CRemConKeyEvent::NewL( k6, 
    							   coreAPIUid, 
        						   ERemConCoreApiVolumeUp, 
        						   ERemConCoreApiButtonPress );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Volume up: ERemConKeyCodeVolumeUp up&short press; volume up/release
    TRemConKeyEventData k7 = { ERemConKeyCodeVolumeUp, 
                               ERemConKeyModifierNone, 
                               ERemConKeyStateUp, 
                               ERemConKeyEventShortPress };

    event = CRemConKeyEvent::NewL( k7, 
    							   coreAPIUid, 
        						   ERemConCoreApiVolumeUp, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Volume up: ERemConKeyCodeVolumeUp up&long press; volume up/release
    TRemConKeyEventData k8 = { ERemConKeyCodeVolumeUp, 
                               ERemConKeyModifierNone, 
                               ERemConKeyStateUp, 
                               ERemConKeyEventLongPress };

    event = CRemConKeyEvent::NewL( k8, 
    							   coreAPIUid, 
        						   ERemConCoreApiVolumeUp, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
        
    // Volume down: ERemConKeyCodeVolumeDown down; volume down/press
    TRemConKeyEventData k9 =  { ERemConKeyCodeVolumeDown, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateDown, 
                                ERemConKeyEventNone };
                                
    event = CRemConKeyEvent::NewL( k9, 
    							   coreAPIUid, 
        						   ERemConCoreApiVolumeDown, 
        						   ERemConCoreApiButtonPress );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Volume down: ERemConKeyCodeVolumeDown down&short press; volume down
    TRemConKeyEventData k10 =  { ERemConKeyCodeVolumeDown, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k10, 
    							   coreAPIUid, 
        						   ERemConCoreApiVolumeDown, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Volume down: ERemConKeyCodeVolumeDown down&long press; volume down
    TRemConKeyEventData k11 = { ERemConKeyCodeVolumeDown, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventLongPress };
                                
    event = CRemConKeyEvent::NewL( k11, 
    							   coreAPIUid, 
        						   ERemConCoreApiVolumeDown, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
        
    //
    // Media key events:
    //
	    
    // Play or Pause has been pressed in accessory: 
    // ERemConCoreApiPausePlayFunction is delivered to S60 application. 
    // It's S60 application's responsibility to interpret whether play 
    // or pause was pressed.
    TRemConKeyEventData k12 = { ERemConKeyCodePlayPause, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k12, 
    							   coreAPIUid, 
        						   ERemConCoreApiPausePlayFunction, 
        						   ERemConCoreApiButtonClick );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
       // Stop: ERemConKeyCodeStop down; stop.
    TRemConKeyEventData k13 = { ERemConKeyCodeStop, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k13, 
    							   coreAPIUid, 
        						   ERemConCoreApiStop, 
        						   ERemConCoreApiButtonClick );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
       // Fast forward 1: ERemConCoreApiFastForward down&repeating; fast 
       // forward.
    TRemConKeyEventData k14 = { ERemConKeyCodeForward, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateDown, 
                                ERemConKeyEventRepeat };
                                
    event = CRemConKeyEvent::NewL( k14, 
    							   coreAPIUid, 
        						   ERemConCoreApiFastForward, 
        						   ERemConCoreApiButtonPress );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
       // Fast forward 2: ERemConCoreApiFastForward up; fast forward.
    TRemConKeyEventData k15 = { ERemConKeyCodeForward, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventLongPress };
                                
    event = CRemConKeyEvent::NewL( k15, 
    							   coreAPIUid, 
        						   ERemConCoreApiFastForward, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
       // Rewind 1: ERemConKeyCodeRewind down&repeating; rewind.
    TRemConKeyEventData k16 = { ERemConKeyCodeBack, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateDown, 
                                ERemConKeyEventRepeat };
                                
    event = CRemConKeyEvent::NewL( k16, 
    							   coreAPIUid, 
        						   ERemConCoreApiRewind, 
        						   ERemConCoreApiButtonPress );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Rewind 2: ERemConKeyCodeRewind up; rewind.
    TRemConKeyEventData k17 = { ERemConKeyCodeBack, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventLongPress };
                                
    event = CRemConKeyEvent::NewL( k17, 
    							   coreAPIUid, 
        						   ERemConCoreApiRewind, 
        						   ERemConCoreApiButtonRelease );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Next track: ERemConCoreApiFastForward up & short press; next track.
    TRemConKeyEventData k18 = { ERemConKeyCodeForward, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k18, 
    							   coreAPIUid, 
        						   ERemConCoreApiForward, 
        						   ERemConCoreApiButtonClick );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    // Previous track: ERemConKeyCodeRewind up & short press; previous 
    // track.
    TRemConKeyEventData k19 = { ERemConKeyCodeBack, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k19, 
    							   coreAPIUid, 
        						   ERemConCoreApiBackward, 
        						   ERemConCoreApiButtonClick );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    //
    // Radio key events:
    //

    // RemConKeyEventConverter does not support specific Radio event op IDs, 
    // the media op IDs are used instead.
    // It is up to the application to interpret the media operation IDs to
    // next channel, previous track etc.
    
    //
    // Sync key events:
    //
    
       // Sync: ERemConKeyCodeSync down.
    TRemConKeyEventData k20 = { ERemConKeyCodeSync, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateDown, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k20, 
    							   syncAPIUid, 
        						   ERemConExtSynchronization, 
        						   ERemConCoreApiButtonClick );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    //
    // Mute key event:
    //
    
       // Mute: ERemConKeyCodeMute up.
    TRemConKeyEventData k21 = { ERemConKeyCodeMute, 
                                ERemConKeyModifierNone, 
                                ERemConKeyStateUp, 
                                ERemConKeyEventShortPress };
                                
    event = CRemConKeyEvent::NewL( k21, 
    							   coreAPIUid, 
        						   ERemConCoreApiMute, 
        						   ERemConCoreApiButtonClick );
    CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );

    // Redial
    TRemConKeyEventData k22 = { ERemConKeyCodeRedial, 
    						   ERemConKeyModifierNone, 
    						   ERemConKeyStateUp, 
    						   ERemConKeyEventShortPress };
    
    event = CRemConKeyEvent::NewL( k22, 
    							   extensionAPIUid, 
    							   ERemConExtLastNumberRedial, 
    							   ERemConCoreApiButtonClick );
    							   
 	CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );
    
    /*
    // Send key
    TRemConKeyEventData k23 = { ERemConKeyCodeSend, 
    						   ERemConKeyModifierNone, 
    						   ERemConKeyStateUp, 
    						   ERemConKeyEventShortPress };
    
    event = CRemConKeyEvent::NewL( k23, 
    							   extensionAPIUid, 
    							   ERemConExtAnswerCall, 
    							   ERemConCoreApiButtonClick );
    							   
 	CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );    

    // End key
    TRemConKeyEventData k24 = { ERemConKeyCodeEnd, 
    						   ERemConKeyModifierNone, 
    						   ERemConKeyStateUp, 
    						   ERemConKeyEventShortPress };
    
    event = CRemConKeyEvent::NewL( k24, 
    							   extensionAPIUid, 
    							   ERemConExtEndCall, 
    							   ERemConCoreApiButtonClick );
    							   
 	CleanupStack::PushL( event );   
    AddEventL(event);
    CleanupStack::Pop( event );    
    */
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRemConKeyEventContainer* CRemConKeyEventContainer::NewL()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventContainer::NewL()" );
	
    CRemConKeyEventContainer* self = new( ELeave ) CRemConKeyEventContainer;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
   
// Destructor
CRemConKeyEventContainer::~CRemConKeyEventContainer()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventContainer::~CRemConKeyEventContainer()" );
    iKeyEventArray.ResetAndDestroy();
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventContainer::~CRemConKeyEventContainer() return void" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventContainer::AddEventL
// Add a key event instance to container.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConKeyEventContainer::AddEventL( 
    const CRemConKeyEvent* aKeyEvent )
    {
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEvent::AddEventL( %d )", aKeyEvent );

    iKeyEventArray.AppendL( aKeyEvent );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventContainer::GetEvents
// Return key event instances.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const RPointerArray<CRemConKeyEvent>& CRemConKeyEventContainer::GetEvents() const
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::GetEvents()" );
    return iKeyEventArray;
    }

//  End of File  
