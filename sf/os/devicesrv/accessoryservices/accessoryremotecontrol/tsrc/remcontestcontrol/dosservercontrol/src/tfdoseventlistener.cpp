/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CTFDosServerEventListener class
*
*/


#include <dossvrservices.h>
#include "dsytesttooltrace.h"
#include "tfdosservertypes.h"
#include "ctfdoseventlistener.h"
#include "mtfdosservercontrol.h"

CTFDosServerEventListener::CTFDosServerEventListener( RDosServer& aServer )
: CDosEventListenerBase( aServer )
    {
    }


CTFDosServerEventListener::~CTFDosServerEventListener( void )
    {
    }


static void NotifyEvent( TInt aEvent, TInt aParameter )
    {
    MTFDosServerControl* control = GetDosServerControl();
    if ( control != NULL )
        {
        control->NotifyDosEvent( aEvent, aParameter );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    }
    

#ifndef RD_STARTUP_CHANGE
void CTFDosServerEventListener::NotifySelfTestStatusL( const TInt aSelfTestResult )
    {
    NotifyEvent( ETFDosEvent_NotifySelfTestStatus, aSelfTestResult );
    }

void CTFDosServerEventListener::DoMtcInitiatedShutdownL()
    {
    NotifyEvent( ETFDosEvent_DoMtcInitiatedShutdown, 0 );
    }

void CTFDosServerEventListener::PhonePowerOnL()
    {
    NotifyEvent( ETFDosEvent_PhonePowerOn, 0 );
    }
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
	void CTFDosServerEventListener::SimReadyL( EPSSimReadyStatus aStatus )
		{
		NotifyEvent( ETFDosEvent_SimReady, aStatus );
		}

void CTFDosServerEventListener::SimChangedL( TBool aValue )
    {
    NotifyEvent( ETFDosEvent_SimChanged, aValue );
    }

		
void CTFDosServerEventListener::SimLockStatusL(EPSSimLockStatus aStatus)
	{
	NotifyEvent( ETFDosEvent_SimLockStatus, aStatus );
	}


void CTFDosServerEventListener::SimStateL( TDosSimState aState )
    {
    NotifyEvent( ETFDosEvent_SimState, aState );
    }

	void CTFDosServerEventListener::CallsForwardingStatusL(EPSCallsForwardingStatus aStatus)
		{
		NotifyEvent( ETFDosEvent_CallsForwardingStatus, aStatus );
		}

	void CTFDosServerEventListener::VoiceMailStatusL(EPSVoiceMailStatus aStatus)
		{
		NotifyEvent( ETFDosEvent_VoiceMailStatus, aStatus );
		}

void CTFDosServerEventListener::FaxMessageL( TBool aValue )
    {
    NotifyEvent( ETFDosEvent_FaxMessage, aValue );
    }

void CTFDosServerEventListener::EmailMessageL( TBool aValue )
    {
    NotifyEvent( ETFDosEvent_EmailMessage, aValue );
    }
#endif //RD_STARTUP_CHANGE

/*void CTFDosServerEventListener::NetworkBarsL( TInt aValue )
    {
    NotifyEvent( ETFDosEvent_NetworkBars, aValue );
    }*/

#ifndef RD_STARTUP_CHANGE
	void CTFDosServerEventListener::SecurityCodeL(EPSSecurityCodeStatus aStatus)
		{
		NotifyEvent( ETFDosEvent_SecurityCode, aStatus );
		}
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
	void CTFDosServerEventListener::CurrentSimOwnedStatusL(EPSCurrentSimOwnedSimStatus aStatus)
		{
		NotifyEvent( ETFDosEvent_CurrentSimOwnedStatus, aStatus );
		}
#endif // RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
void CTFDosServerEventListener::AccessoryModeChangedL( TDosAccessoryMode aMode )
    {
    NotifyEvent( ETFDosEvent_AccessoryModeChanged, aMode );
    }

	void CTFDosServerEventListener::HeadsetButtonChangedL(EPSButtonState aState)
		{
		NotifyEvent( ETFDosEvent_HeadsetButtonChanged, aState );
		}

void CTFDosServerEventListener::OutputVolumeLevelChangedL( TInt aLevel )
    {
    NotifyEvent( ETFDosEvent_OutputVolumeLevelChanged, aLevel );
    }

void CTFDosServerEventListener::AudioLinkOpenReq()
    {
    NotifyEvent( ETFDosEvent_AudioLinkOpenReq, 0 );
    }

void CTFDosServerEventListener::AudioLinkCloseReq( THCIConnHandle aConnectionHandle )
    {
    NotifyEvent( ETFDosEvent_AudioLinkCloseReq, aConnectionHandle );
    }

void CTFDosServerEventListener::ActivateVoiceRecognitionReq()
    {
    NotifyEvent( ETFDosEvent_ActivateVoiceRecognitionReq, 0 );
    }

void CTFDosServerEventListener::AbortVoiceRecognitionReq()
    {
    NotifyEvent( ETFDosEvent_AbortVoiceRecognitionReq, 0 );
    }

void CTFDosServerEventListener::VoiceRecognitionStatusReq()
    {
    NotifyEvent( ETFDosEvent_VoiceRecognitionStatusReq, 0 );
    }

void CTFDosServerEventListener::QuerySpeedDialNumberReq( TInt aIndex )
    {
    NotifyEvent( ETFDosEvent_QuerySpeedDialNumberReq, aIndex );
    }

void CTFDosServerEventListener::QueryLastDialledVoiceNumberReq()
    {
    NotifyEvent( ETFDosEvent_QueryLastDialledVoiceNumberReq, 0 );
    }

void CTFDosServerEventListener::AccessoryModeChangedInd( TBTAccessoryMode aAccMode )
    {
    NotifyEvent( ETFDosEvent_AccessoryModeChangedInd, aAccMode );
    }

void CTFDosServerEventListener::BTAccessoryModeChangedInd( TBTDeviceType aDeviceType )
    {
    NotifyEvent( ETFDosEvent_BTAccessoryModeChangedInd, aDeviceType );
    }

void CTFDosServerEventListener::AccessorySpeakerVolumeSetInd( TUint aVolume )
    {
    NotifyEvent( ETFDosEvent_AccessorySpeakerVolumeSetInd, aVolume );
    }

void CTFDosServerEventListener::ServiceLevelConnEstablishedInd( TBTServiceLevelConnParams aServiceLevelConnParams )
    {
    NotifyEvent( ETFDosEvent_ServiceLevelConnEstablishedInd, ( TInt )&aServiceLevelConnParams );
    }
#endif //__ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
void CTFDosServerEventListener::SmsStorageStatusChangedL( TDosSmsStorageStatus aStatus )    
    {
    NotifyEvent( ETFDosEvent_SmsStorageStatusChanged, aStatus );
    }
#endif // RD_STARTUP_CHANGE

void CTFDosServerEventListener::GeneralEventL( const TRegisterEvent& /*aEvent*/,TAny* /*aParameter*/ )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerEventListener::GeneralEventL() - not implemented" ) ) );
    }

#ifndef __ACCESSORY_FW
	void CTFDosServerEventListener::HandsfreeModeChanged(EPSHandsFreeMode aMode)
		{
		NotifyEvent( ETFDosEvent_HandsfreeModeChanged, aMode );
		}
#endif //__ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
void CTFDosServerEventListener::SimPresentStateChangedL( TDosSimPresentState aState )
    {
    NotifyEvent( ETFDosEvent_SimPresentStateChanged, aState );
    }
#endif //RD_STARTUP_CHANGE

