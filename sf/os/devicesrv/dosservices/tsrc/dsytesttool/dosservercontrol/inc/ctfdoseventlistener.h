/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFDosServerEventListener class
*
*/


#ifndef __CTFDOSEVENTLISTENER_H__
#define __CTFDOSEVENTLISTENER_H__

#include <DosSvrServices.h>

class CTFDosServerControl;

NONSHARABLE_CLASS( CTFDosServerEventListener ): public CDosEventListenerBase
    {
    public:
        /**
        * Constructor
        */
        CTFDosServerEventListener( RDosServer& aServer );

        /**
        * Destructor
        */
        virtual ~CTFDosServerEventListener( void );

    private:
        /**
        * Copy constructor is hidden
        */
        CTFDosServerEventListener( const CTFDosServerEventListener& aListener );

        /**
        * Assignment operator is hidden
        */
        CTFDosServerEventListener& operator=( const CTFDosServerEventListener& aListener );

    protected:
    
#ifndef RD_STARTUP_CHANGE  
        /**
        * It notifies that a Dos SelfTest has completed.
        * @param aSelftTestResult The result of the self test.
        */
        void NotifySelfTestStatusL( const TInt aSelfTestResult );

        /**
        * It notifies that the phone side has initiated a shutdown.
        */
        void DoMtcInitiatedShutdownL();

        /**
        * It notifies that the phone is on.
        */
        void PhonePowerOnL();
#endif // RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
        /**
        * It notifies that the SIM card is ready to be fully used.
        * @param aStatus The new status.
        */
		void SimReadyL( EPSSimReadyStatus aStatus );

        /**
        * It notifies if the SIM card has been changed.
        * @param aValue ETrue if it has been or EFalse otherwise.
        */
        void SimChangedL( TBool aValue );

        /**
        * It notifies about a change in the status of the SimLock.
        * @param aStatus The new status.
        */
		void SimLockStatusL(EPSSimLockStatus aStatus);
        /**
        * It notifies about a change in the state of the SIM card.
        * @param aStatus The new state.
        */
        void SimStateL( TDosSimState aState );

        /**
        * It notifies about a change in the status of the calls forwarding settings.
        * @param aStatus The new status.
        */
		void CallsForwardingStatusL(EPSCallsForwardingStatus aStatus);
        /**
        * It notifies about a change in the status of the voice mail settings.
        * @param aStatus The new status.
        */
		void VoiceMailStatusL(EPSVoiceMailStatus aStatus);

        /**
        * It notifies about the existance of a fax message in our voice mail.
        * @param aValue ETrue or EFalse
        */
        void FaxMessageL( TBool aValue );

        /**
        * It notifies about the existance of a E-mail message in our voice mail.
        * @param aValue ETrue or EFalse
        */
        void EmailMessageL( TBool aValue );
#endif //RD_STARTUP_CHANGE

        /**
        * It notifies about a change in the network bars.
        * @param aValue The new value.
        */
    //    void NetworkBarsL( TInt aValue );

#ifndef RD_STARTUP_CHANGE
        /**
        * It notifies about a change in the status of the security code.
        * @param aStatus The new status.
        */
		void SecurityCodeL(EPSSecurityCodeStatus aStatus);
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
        /**
        * It notifies about a change in the status of the current SIM.
        * @param aStatus The new status.
        */
        void CurrentSimOwnedStatusL(EPSCurrentSimOwnedSimStatus aStatus);
#endif //RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
        /**
        * It notifies about a headset button state change
        * @param aMode
        */
		void HeadsetButtonChangedL(EPSButtonState aState);

        /**
        * It notifies about a output volume level changes
        * @param aState
        */
        void OutputVolumeLevelChangedL( TInt aLevel );

        /**
        * It requests to open a BT Audio link between phone and BT Audio Accessory.    
        */
        void AudioLinkOpenReq();

        /**
        * It requests to close the BT Audio link between phone and BTAA.
        * @param aConnectionHandle Handle to the audio connection.
        */    
        void AudioLinkCloseReq( THCIConnHandle aConnectionHandle );

        /**
        * It requests to activate voice recognition.    
        */    
        void ActivateVoiceRecognitionReq();

        /**
        * It requests to abort voice recognition.    
        */    
        void AbortVoiceRecognitionReq();

        /**
        * It requests status of voice recognition.    
        */    
        void VoiceRecognitionStatusReq();

        /**
        * It requests to query speed dial number.
        * @param aIndex A speed dial index.
        */    
        void QuerySpeedDialNumberReq( TInt aIndex );

        /**
        * It requests to query last dialled number.
        */    
        void QueryLastDialledVoiceNumberReq();

        /**
        * It notifies about a change in audio accessory mode.
        * @param aAccMode The new accessorymode.
        */    
        void AccessoryModeChangedInd( TBTAccessoryMode aAccMode );

        /**
        * It notifies about a change in BT audio accessory mode.    
        * @param aDeviceType BT Accessory device's type.
        */
        void BTAccessoryModeChangedInd( TBTDeviceType aDeviceType );

        /**
        * It notifies about a change in audio accessory's speaker volume level.
        * @param aVolume The new volume level.
        */    
        void AccessorySpeakerVolumeSetInd( TUint aVolume );

        /**
        * It notifies about a change in BT connection status.
        * @param aServiceLevelConnParams A struct containing parameters.    
        */
        void ServiceLevelConnEstablishedInd( TBTServiceLevelConnParams aServiceLevelConnParams );    
#endif //__ACCESSORY_FW
        
#ifndef RD_STARTUP_CHANGE
        /**
        * It notifies about a change sms storage status.
        * @param aServiceLevelConnParams A struct containing parameters.    
        */
        void SmsStorageStatusChangedL( TDosSmsStorageStatus aStatus );    
#endif // RD_STARTUP_CHANGE

        /**
        * This a raw event that is intended for expansion of the event functionality by licensees.
        * @param aEvent The event.
        * @param aParameter A pointer to the parameter.
        */
        void GeneralEventL( const TRegisterEvent& aEvent,TAny* aParameter );

#ifndef __ACCESSORY_FW
        /**
        * It notifies about a change in the status of the handsfree mode.
        * @param aStatus The new status.
        */
        void HandsfreeModeChanged(EPSHandsFreeMode aMode);

        /**
        * Notifies of changes in accessory mode
        * @param aMode New accessory mode
        */
        void AccessoryModeChangedL( TDosAccessoryMode aMode );
#endif //__ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
        /**
        * Notifies if SIM is present or not
        * @param aMode SIM present state
        */
        void SimPresentStateChangedL( TDosSimPresentState aState );
#endif //RD_STARTUP_CHANGE

    };


#endif

// End of file
