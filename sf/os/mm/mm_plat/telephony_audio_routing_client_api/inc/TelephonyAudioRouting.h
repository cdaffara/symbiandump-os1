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
* Description:  Defines the Telephony Audio Routing base class
*
*/


#ifndef TELEPHONYAUDIOROUTING_H
#define TELEPHONYAUDIOROUTING_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MTelephonyAudioRoutingObserver;

// CLASS DECLARATION

/**
*  Used by telephony applications to query outputs and to configure the output
*  where audio will be routed.
*
*  @lib TelephonyAudioRouting.lib
*  @since Series 60 3.1
*/

class CTelephonyAudioRouting : public CBase
    {
    public:  // DATA TYPES

        //Output types
        enum TAudioOutput
            {
            ENotActive,
            ENone,
            EHandset,
            ELoudspeaker,
            EWiredAudioAccessory,
            EBTAudioAccessory,
            ETTY
            };

    public:  // Constructors and destructor

        /**
        * Factory interface returning a new instance of CTelephonyAudioRouting.
        *
        * If the server is not created by the Adaptation, this leaves with KErrNotReady.
        * If the client does not have MultimediaDD capability, this leaves with
        * KErrPermissionDenied.
        *
        * Leaves on failure.
        *
        * @since Series 60 3.1
        */
        IMPORT_C static CTelephonyAudioRouting* NewL(MTelephonyAudioRoutingObserver& aObserver);


        /**
        * Returns array of currently available outputs.
        * The array contains list of following outputs depending on whether the
        * output is available or not at the time of query:
        *  - EHandset
        *  - ELoudspeaker
        *  - EWiredAudioAccessory,
        *  - EWiredAccessory,
        *  - EBTAudioAccessory
        *  - ETTY
        * Audio can be routed to any of the available outputs in the list.
        *
        * @since Series 60 3.1
        * @param none.
        * @return TArray<TAudioOutput>& Array of available outputs.
        */
        virtual const TArray<TAudioOutput>& AvailableOutputs() = 0;


        /**
        * Returns the current output configured by the client application to play
        * telephony audio.
        * Possible values are:
        *  - ENotActive : If output has not been set by any or one of the
        *    telephony apps has called SetOutputL(ENotActive) successfully.
        *  - ENone : If user has destroyed audio link by long key press on the
        *    BT device.
        *  - EHandset or ELoudspeaker or EWiredAudioAccessory or
        *    EBTAudioAccessory or ETTY depending on the last successful
        *    SetOutputL() call. EBTAudioAccessory is also returned when adaptation
        *    routes audio from device to BT accessory automatically when user
        *    initiated long key press event occurs.
        *
        * @since Series 60 3.1
        * @param none.
        * @return TAudioOutput Current output.
        */
        virtual TAudioOutput Output() = 0;


        /**
        * Sends an asynchronous request to Adaptation to set the output
        * where telephony audio will be routed. This request is completed only
        * when the application receives
        * MTelephonyAudioRoutingObserver::SetOutputComplete callback.
        *
        * If MTelephonyAudioRoutingObserver::SetOutputComplete returns KErrNone,
        * aOutput requested by this application is configured by Adaptation.
        * Otherwise there was error configuring aOutput and application should
        * call CTelephonyAudioRouting::Output to get the active output.
        *
        * Leaves on following cases:
        *   - System wide error code when failure to send request.
        *   - KErrArgument when parameter is ENone.
        *   - KErrHardwareNotAvailable if aOutput is not in the list of available
        *       outputs.
        *
        * @since Series 60 3.1
        * @param aOutput : output requested by application. This can only be any
        *           of the outputs from AvailableOutputs() list or ENotActive.
        * @return none.
        */
        virtual void SetOutputL( TAudioOutput aOutput = ENotActive ) = 0;

        /**
        * Returns the second last output that was successfully configured to
        * play telephony audio.
        * Possible values are:
        *  - ENotActive : If output had not been set or one of the telephony
        *                 apps had previously called SetOutputL(ENotActive)
        *                 successfully.
        *  - EHandset or ELoudspeaker or EWiredAudioAccessory or
        *    EBTAudioAccessory or ETTY depending on the second last successful
        *    SetOutputL() call.
        *
        * @since Series 60 3.1
        * @param none.
        * @return TAudioOutput Previous output that was configured for telephony
        *           audio. ENone will never be returned since telephony apps cannot call
        *           SetOutputL(ENone).
        */
        virtual TAudioOutput PreviousOutput() = 0;
        
      	/*
        * Sends the Show Note mode to server along with the next SetOutputL()
        * request.
        *
        * After SetOutputL() request from the Application is succesfully
        * processed by the server and application receives SetOutputComplete()
        * callback with KErrNone, it can query Show Note mode sent to server by
        * calling GetShowNote().
        *
        * Other observers wanting to query Show Note mode can call GetShowNote()
        * after receiving OutputChanged() callback.
        *
        * @since Series 60 3.1
        * @param TBool aMode Show Note mode. ETrue or EFalse.
        * @return KErrNone if succesfull, otherwise system-wide error code.
        */
        virtual TInt SetShowNote( TBool aMode ) = 0;
        
        
        /*
        * Returns the the Show Note mode for the last succesfully completed
        * SetOutputL() request.
        *
        * If the routing is initiated by the Adaptation with BT Long key press
        * this will return ETrue.
        *
        * Once the Show Note mode is sent to server along with the SetOutputL()
        * request, Show Note mode attribute for that client session will be
        * reset to EFalse. GetShowNote() function for this client session will
        * return EFalse until SetOutputL() request is completed and the observer
        * receives OutputComplete() callback. If there is no error completing
        * the request, GetShowNote() returns the Show Note value set by the
        * Application. If there is error completing the request, GetShowNote()
        * will return EFalse.
        *
        * For all other observers, After they receive OutputChanged() callback,
        * GetShowNote() will return the Show Note value set by the Application
        * that called SetOutputL().
        *
        * @since Series 60 3.1
        * @param TBool aMode. On return of this function will contain
        *        Show Note mode. EFalse or ETrue.
        * @return KErrNone if succesfull, otherwise system-wide error code.
        */
        virtual TInt GetShowNote( TBool& aMode ) = 0;


    };
    
// CLASS DECLARATION

/**
*  This class defines the TAR default data structure.
*
*  @lib TelephonyAudioRouting.lib
*  @since 3.0
*/
class TTelephonyAudioRoutingDefaultParams
    {
    public:

        /**
        * Constructor.
        */
        TTelephonyAudioRoutingDefaultParams() : iCurrentAudio(CTelephonyAudioRouting::ENotActive),iPreviousAudio(CTelephonyAudioRouting::ENotActive),iShowNoteMode(EFalse)
        {}

		CTelephonyAudioRouting::TAudioOutput iCurrentAudio;
		CTelephonyAudioRouting::TAudioOutput iPreviousAudio;
		TBool iShowNoteMode;

    };
    

#endif      // TELEPHONYAUDIOROUTING_H

// End of File
