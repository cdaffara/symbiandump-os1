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

#ifndef __GAVDPSTATES_H__
#define __GAVDPSTATES_H__


/**
Class to represent the state of the CGAVDPStateMachine

@internalComponent
*/
class TGAVDPState
	{
public:
enum TGAVDPStateValue
		{
		EGAVDPIdle,
		EConnectedToGavdp,
		ESEPsDiscovered,
		ESEPSelected,
		EConfigured,
		EGAVDPOpen,
		EGAVDPStreaming,
		EGAVDPSuspended,
		EAborting
		};
public:	
inline TGAVDPState();	
inline TGAVDPState(TGAVDPStateValue aGAVDPStateValue);
inline TGAVDPState operator=(const TGAVDPState& aGAVDPState);	
inline TBool operator==(const TGAVDPState& aGAVDPState) const;
inline TBool operator!=(const TGAVDPState& aGAVDPState) const;
inline void operator++();
inline TGAVDPStateValue State() const;
private:
	TGAVDPStateValue iState;
	};


/**
Class used by the CGAVDPStateMachine to track the state of 
a configuration by the headset.

@internalComponent
*/
class TConfigurationByRemoteHeadsetState
	{
public:
enum TConfigurationByRemoteHeadsetStateValue
		{
		ENotBeingConfiguredByRemoteHeadset,
		ERemoteHeadsetConfigurationStart,
		ERemoteHeadsetConfigurationFailed,
		ERemoteHeadsetConfigurationEnd
		};
public:	
	inline TConfigurationByRemoteHeadsetState();
	inline void Reset();
	inline TConfigurationByRemoteHeadsetState operator=(const TConfigurationByRemoteHeadsetStateValue& aConfigByRemoteHeadsetStateValue);	
	inline TBool operator==(const TConfigurationByRemoteHeadsetStateValue aConfigByRemoteHeadsetStateValue) const;
	inline TBool operator!=(const TConfigurationByRemoteHeadsetStateValue aConfigByRemoteHeadsetStateValue) const;
	inline TConfigurationByRemoteHeadsetState::TConfigurationByRemoteHeadsetStateValue State() const;
	inline TSEID SEPRequestedByRemoteHeadset() const;
	inline void SetSEPRequestedByRemoteHeadset(const TSEID& aSEID);
	inline TAvdtpMediaTransportCapabilities* TransportCapsRequestedByRemoteHeadset() const;
	inline void SetTransportCapsRequestedByRemoteHeadset(TAvdtpMediaTransportCapabilities* aTransportCapsRequestedByRemoteHeadset);
	inline TAvdtpMediaCodecCapabilities* CodecConfigRequestedByRemoteHeadset() const;
	inline void SetCodecConfigRequestedByRemoteHeadset(TAvdtpMediaCodecCapabilities* aCodecConfigRequestedByRemoteHeadset);	
private:
	TConfigurationByRemoteHeadsetStateValue iState;
	TSEID iSEPRequestedByRemoteHeadset;
	TAvdtpMediaTransportCapabilities* iTransportCapsRequestedByRemoteHeadset;
	TAvdtpMediaCodecCapabilities* iCodecConfigRequestedByRemoteHeadset;
	};
	
#include "GavdpStates.inl"

#endif
	
