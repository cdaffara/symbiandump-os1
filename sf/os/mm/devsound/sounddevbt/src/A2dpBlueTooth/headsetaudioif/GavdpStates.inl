// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GAVDPSTATES_INL__
#define __GAVDPSTATES_INL__


inline TGAVDPState::TGAVDPState() : iState(EGAVDPIdle) 
	{
	}
	
		
inline TGAVDPState::TGAVDPState(TGAVDPStateValue aGAVDPStateValue)
	{
	iState = aGAVDPStateValue;
	}


inline TGAVDPState TGAVDPState::operator=(const TGAVDPState& aGAVDPState)
	{
	iState = aGAVDPState.iState;
	return *this;
	}

	
inline TBool TGAVDPState::operator==(const TGAVDPState& aGAVDPState) const
	{
	return (iState == aGAVDPState.iState);
	}

	
inline TBool TGAVDPState::operator!=(const TGAVDPState& aGAVDPState) const
	{
	return !(iState == aGAVDPState.iState);
	}


/**
This looks quite long for an inline but it's only used once
*/
inline void TGAVDPState::operator++()
	{
	switch(iState)
		{
		case EGAVDPIdle:
			iState =  EConnectedToGavdp;
			break;
		case EConnectedToGavdp:
			iState =  ESEPsDiscovered;
			break;
		case ESEPsDiscovered:
			iState =  ESEPSelected;
			break;
		case ESEPSelected:
			iState =  EConfigured;
			break;
		case EConfigured:
			iState =  EGAVDPOpen;
			break;
		case EGAVDPOpen:
			iState = EGAVDPStreaming;
			break;
		case EGAVDPStreaming:
			iState = EGAVDPOpen;//go back to open after streaming
			break;
		}
	}

	
inline TGAVDPState::TGAVDPStateValue TGAVDPState::State() const
	{
	return iState;
	}


	
inline TConfigurationByRemoteHeadsetState::TConfigurationByRemoteHeadsetState() : iState(ENotBeingConfiguredByRemoteHeadset), iCodecConfigRequestedByRemoteHeadset(NULL) 
	{
	}

	
inline void TConfigurationByRemoteHeadsetState::Reset()
	{
	iState = ENotBeingConfiguredByRemoteHeadset;
	iSEPRequestedByRemoteHeadset.Reset();
	delete iTransportCapsRequestedByRemoteHeadset; //ownership is with this class
	iTransportCapsRequestedByRemoteHeadset = NULL;
	delete iCodecConfigRequestedByRemoteHeadset; //ownership is with this class
	iCodecConfigRequestedByRemoteHeadset = NULL;
	}


inline TConfigurationByRemoteHeadsetState TConfigurationByRemoteHeadsetState::operator=(const TConfigurationByRemoteHeadsetStateValue& aConfigByRemoteHeadsetStateValue)
	{
	iState = aConfigByRemoteHeadsetStateValue;
	return *this;
	}

	
inline TBool TConfigurationByRemoteHeadsetState::operator==(const TConfigurationByRemoteHeadsetStateValue aConfigByRemoteHeadsetStateValue) const
	{
	return (iState == aConfigByRemoteHeadsetStateValue);
	}

	
inline TBool TConfigurationByRemoteHeadsetState::operator!=(const TConfigurationByRemoteHeadsetStateValue aConfigByRemoteHeadsetStateValue) const
	{
	return (iState != aConfigByRemoteHeadsetStateValue);
	}

		
inline  TConfigurationByRemoteHeadsetState::TConfigurationByRemoteHeadsetStateValue TConfigurationByRemoteHeadsetState::State() const
	{
	return iState;
	}

	
inline TSEID TConfigurationByRemoteHeadsetState::SEPRequestedByRemoteHeadset() const
	{
	return iSEPRequestedByRemoteHeadset;
	}

	
inline void TConfigurationByRemoteHeadsetState::SetSEPRequestedByRemoteHeadset(const TSEID& aSEID)
	{
	iSEPRequestedByRemoteHeadset = aSEID;
	}

	
inline TAvdtpMediaTransportCapabilities* TConfigurationByRemoteHeadsetState::TransportCapsRequestedByRemoteHeadset() const
	{//note ownership stays with TConfigurationByRemoteHeadsetState
	return iTransportCapsRequestedByRemoteHeadset;
	}

	
inline void TConfigurationByRemoteHeadsetState::SetTransportCapsRequestedByRemoteHeadset(TAvdtpMediaTransportCapabilities* aTransportCapsRequestedByRemoteHeadset)
	{//note ownership is transferred to TConfigurationByRemoteHeadsetState
	if (iTransportCapsRequestedByRemoteHeadset)
		{//then the headset is requesting a new configuration, but the old
		//one has not been reset - this shouldn't happen, but could happen
		//if the headset called GAVDP_ConfigurationIndication more than once with 
		//the same config with no intermediate StartStreams or Abort.
		delete iTransportCapsRequestedByRemoteHeadset;
		iTransportCapsRequestedByRemoteHeadset = NULL;
		}
	iTransportCapsRequestedByRemoteHeadset = aTransportCapsRequestedByRemoteHeadset;
	}


inline TAvdtpMediaCodecCapabilities* TConfigurationByRemoteHeadsetState::CodecConfigRequestedByRemoteHeadset() const
	{//note ownership stays with TConfigurationByRemoteHeadsetState
	return iCodecConfigRequestedByRemoteHeadset;
	}

	
inline void TConfigurationByRemoteHeadsetState::SetCodecConfigRequestedByRemoteHeadset(TAvdtpMediaCodecCapabilities* aCodecConfigRequestedByRemoteHeadset)
	{//note ownership is transferred to TConfigurationByRemoteHeadsetState
	if (iCodecConfigRequestedByRemoteHeadset)
		{//then the headset is requesting a new configuration, but the old
		//one has not been reset - this shouldn't happen, but could happen
		//if the headset called GAVDP_ConfigurationIndication more than once with 
		//the same config with no intermediate StartStreams or Abort.
		delete iCodecConfigRequestedByRemoteHeadset;
		iCodecConfigRequestedByRemoteHeadset = NULL;
		}
	iCodecConfigRequestedByRemoteHeadset = aCodecConfigRequestedByRemoteHeadset;
	}

#endif
	
