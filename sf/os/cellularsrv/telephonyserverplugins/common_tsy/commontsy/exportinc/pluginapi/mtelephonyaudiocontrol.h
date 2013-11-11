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
//

#ifndef MTELEPHONYAUDIOCONTROL_H
#define MTELEPHONYAUDIOCONTROL_H

// INCLUDES
#include <etelmm.h>

class MTelephonyAudioControlCallback;

// CLASS DECLARATION

/**
 *  Plug in interface implemented by licensees that may be used to 
 *  start/stop audio. Interface is triggered when calls are created 
 *  and destroyed and also indicates state changes of the call.
 *
 * @publishedPartner
 * @released 
 */
 
class MTelephonyAudioControl 
	{

    public: // Enumerations
    
        /** Enumeration for different type calls */	
        enum TTelephonyAudioControlCallType
            {
            ECallTypeVoice
            };

	public: // Pure virtual functions
	
        /**
         * Initialise new call routing.  
         *
         * @param aCallName The name of the call
         * @param aCallType The type of the call
         * @param aEmergency Is the call emergency call or not
         * @param aCallDirection The direction of the call
         */ 
	     virtual void SetupTelephonyAudio( 
             const TName& aCallName, 
             MTelephonyAudioControl::TTelephonyAudioControlCallType aCallType, 
             TBool aEmergency, 
             RMobileCall::TMobileCallDirection aCallDirection ) = 0;
            
        /**
         * Tears down exiting call routing.
         *
         * @param aCallName The name of the call
         * @param aError The reason of tear down
         */                            
	     virtual void TeardownTelephonyAudio( const TName& aCallName, 
		                 	                  TInt aError ) = 0;
		    
        /**
         * Updates exiting call state.
         *
         * @param aCallName The name of the call
         * @param aCallStatus The call status
         */                                 
	     virtual void CallStateChange( 
        	          const TName& aCallName, 
        	          RMobileCall::TMobileCallStatus aCallStatus ) = 0;

	};


#endif  // MTELEPHONYAUDIOCONTROL_H

//  End of file
