/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header for class CTelephonyAudioRoutingManager
*
*/


#ifndef TELEPHONYAUDIOROUTINGMANAGER_H
#define TELEPHONYAUDIOROUTINGMANAGER_H

//  INCLUDES
#include <e32base.h>
#include  <e32std.h>
#include "TelephonyAudioRouting.h"

class MTelephonyAudioRoutingPolicyObserver;
class RTelephonyAudioRoutingManagerSession;

// CLASS DECLARATION

/**
*  Used by Adaptation to notify server regarding changes in audio output.
*
*  @lib TelephonyAudioRoutingManager.lib
*  @since Series 60 3.1
*/
class CTelephonyAudioRoutingManager : public CBase
    {

    public:  // Constructors and destructor

        /**
        * Factory interface returning a new instance of CTelephonyAudioRoutingManager.
        *
        * Instantiate Manager with initial list of available outputs.
        * The array contains list of following outputs depending on whether the
        * output is available or not at the time instance creation:
        *  - EHandset
        *  - ELoudspeaker
        *  - EWiredAudioAccessory,
        *  - EBTAudioAccessory
        *  - ETTY
        *
        * Only one instance of Manager can exist in the system.
        *
        * Leaves on failure.
        * Call to this function to create a second instance will leave with
        * KErrAlreadyExists.
        *
        * @since Series 60 3.1
     	* @param aObserver: ref to client observer class
     	* @param aArray: Array of available outputs
     	* @return CTelephonyAudioRoutingManager* created        
        */
        IMPORT_C static CTelephonyAudioRoutingManager* NewL(
                        MTelephonyAudioRoutingPolicyObserver& aObserver,
                        const TArray<CTelephonyAudioRouting::TAudioOutput>& aArray);

        /**
        * Destructor.
        */
        virtual ~CTelephonyAudioRoutingManager();

        /**
        * Notify the server that active output has changed.
        *
        * This function will be called by Adaptation only if:
        *   - EBTAudioAccessory - In-call audio is routed from the device to BT
        *     audio accessory by the user via long key press. (Audio is
        *     automatically routed by Audio subsystem).
        *   - ENone - In call audio is currently routed to BT audio accessory and
        *     Audio link between device and BT audio accessory is destroyed by
        *     the user via long key press. Audio is not routed anywhere and BT
        *     audio accessory is still a valid output.
        *
        * @since Series 60 3.1
        * @param aOutput : ENone or EBTAudioAccessory.
        * @return none.
        */
        IMPORT_C void OutputChanged(CTelephonyAudioRouting::TAudioOutput aOutput);

        /**
        * Notify the server that MTelephonyAudioRoutingPolicyObserver::OutputChangeRequested
        * request from client has be serviced.
        * If output requested by client is set active, aError will be KErrNone.
        * If for any reason the output requested by the client is not set,
        * Adaptation will call this with active output and reason for not
        * setting the requested output.
        *
        * @since Series 60 3.1
        * @param aOutput : Currently active output
        * @param aError KErrNone if aOutput was requested by client, else system
        *           wide error code.
        * @return none.
        */
        IMPORT_C void OutputChangeRequestCompleted(CTelephonyAudioRouting::TAudioOutput aOutput, TInt aError);

        /**
        * Notify the server that available audio outputs have changed.
        * Adaptation will call this when list of available outputs changes.
        * The array contains list of following outputs depending on whether the
        * output is available or not:
        *  - EHandset
        *  - ELoudspeaker
        *  - EWiredAudioAccessory,
        *  - EBTAudioAccessory
        *  - ETTY
        *
        * @since Series 60 3.1
        * @param aArray : Array of currently active outputs.
        * @return none
        */
        IMPORT_C void AvailableOutputsChanged(const TArray<CTelephonyAudioRouting::TAudioOutput>& aArray);

    protected:

    private:

       /**
        * C++ default constructor.
        */
        CTelephonyAudioRoutingManager(MTelephonyAudioRoutingPolicyObserver& aObserver);
        
        /**
        * 2nd Phase constructor.
        */
        void ConstructL(const TArray<CTelephonyAudioRouting::TAudioOutput>& aArray);

        /**
        * Start the TelephonyAudioRoutingServer
        * @return TInt: KErrNone if server start was successful, other error(s) if not
        */
        TInt StartServer() ;

    private:    // Data

        MTelephonyAudioRoutingPolicyObserver&	iObserver;
        RTelephonyAudioRoutingManagerSession* 	iMgrSession;
        RThread iServer;

    };

#endif      // TELEPHONYAUDIOROUTINGMANAGER_H

// End of File
