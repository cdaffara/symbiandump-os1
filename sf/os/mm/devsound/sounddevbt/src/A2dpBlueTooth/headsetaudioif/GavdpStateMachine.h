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

#ifndef __GAVDPSTATEMACHINE_H__
#define __GAVDPSTATEMACHINE_H__

#include <e32base.h>
#include <bttypes.h> //TBTDevAddr
#include <es_sock.h> //RSocketServer, RSocket
#include <gavdp.h> //MGavdpUser, RGavdp
#include <bluetoothav.h> //TAvdtpSEPInfo, TSEID
#include <mmf/common/mmfutilities.h> //TFourCC
#include <mmf/server/sounddevice.h> //TMMFStereoSupport
#include "GavdpStates.h"


const TUint KGAVDPCallbackTimeout = 30000000;

class CGavdpTimeout; //fwd ref
class CA2dpAudioCodecConfiguration;

using namespace SymbianSBC;
using namespace SymbianBluetoothAV;	

/**
structure used internally in the GAVDP state machine to keep
track of seps than can be used on the headset to send audio and 
the local symbian device SEPs
*/
struct TUsableSEP
	{
	TSEID iSEID;
	TFourCC iDataType;
	};
	
/**
Callback class implemented by CA2dpBTHeadsetAudioInterface to get events
back from CGAVDPStateMachine
*/	
class MGAVDPStateChangeObserver
	{
public:
/**
Callback when the CGAVDPStateMachine has completed a state change
initiated by a call to ChangeState

@internalComponent
@param aInitialState The state the CGAVDPStateMachine prior to calling ChangeState
@param standard SymbianOS error
*/
	void virtual GAVDPStateChangeComplete(const TGAVDPState& aInitialState, TInt aError) = 0;
	
/**
Callback when the GAVDPStateMachine detects an error condition
event while no state change is in progress

@internalComponent
@param aError standard SymbianOS error
*/
	void virtual GAVDPStateMachineEvent(TInt aError) = 0;
	
/**
Callback when the headset suspends the streaming

@internalComponent
*/
	void virtual GAVDPStateMachineStreamSuspendedByRemoteHeadset() = 0;
	
/**
Callback when the headset requested a reconfiguration of the settings

@internalComponent
@return an error code indicating whether it was possible to perform the reconfiguration
*/
	TInt virtual GAVDPStateMachineReconfigureByRemoteHeadset() = 0;
	
/**
Callback when the headset starts (or restarts) the streaming

@internalComponent
*/
	void virtual GAVDPStateMachineStreamResumedByRemoteHeadset() = 0;
	};


/**
GAVDP state machine used by the CA2dpBTHeadsetAudioInterface.
This class is used by the CA2dpBTHeadsetAudioInterface to
initialize and open the headset, which involved finding SEPs,
configuring , getting a bearer and streaming.  The CGAVDPStateMachine
uses the MGAVDPStateChangeObserver callback to inform the CA2dpBTHeadsetAudioInterface
of when the state change is complete, error conditions, and state changes
initiated by the headset itself
 
@internalComponent
*/
NONSHARABLE_CLASS(CGAVDPStateMachine) : public CActive, public MGavdpUser  
	{
	friend class CGavdpTimeout;
public:
	static CGAVDPStateMachine* NewL(MGAVDPStateChangeObserver& aGAVDPStateChangeObserver, CA2dpAudioCodecConfiguration& aA2dpCodecSettings, RSocketServ& aSocketServer);
	~CGAVDPStateMachine();
	void SetBTAddress(const TBTDevAddr& aRemoteAddress);
	TInt Reconfigure(TBool aSettingsHaveChanged);
	TInt ChangeState(const TGAVDPState& aNewState);
	void CancelChangeState(); //external request to cancel state change
	TGAVDPState State() const;
	RArray<TUsableSEP>& UsableSEPs() const;
	RSocket& BearerSocket() const;
	TBTDevAddr& BTAddress() const;
	TInt CodecCaps(TAvdtpMediaCodecCapabilities*& aCodecCaps) const;
	
	//used by a2dp instead of CodecCaps
	TAvdtpMediaCodecCapabilities* RemoteCodecConfiguration();
private:
	CGAVDPStateMachine(MGAVDPStateChangeObserver& aGAVDPStateChangeObserver, CA2dpAudioCodecConfiguration& aA2dpCodecSettings, RSocketServ& aSocketServer); 
	void ConstructL();
	TInt RegisterLocalSEP();
	void ConnectToGAVDP();
	void DiscoverRemoteSEPs();
	void GetRemoteSEPCapability();
	void GetRemoteSEPCapabilities();
	void ConfigureSEP();
	TInt ConfigureLocalSEP();
	TInt ConfigureRemoteSEP();
	void CreateBearerSocket();
	void StartStream();
	void SuspendStream();
	
	void NextState();
	void StateChangeComplete();
	void CancelChangeState(TInt aReason); //internal request to cancel state change eg due to an error condition
	void Reset(); //utility to reset state to idle and all variables
 	
	//MGavdpUser
	virtual void GAVDP_ConnectConfirm(const TBTDevAddr& aDevice);
	virtual void GAVDP_SEPDiscovered(const TAvdtpSEPInfo& aSEP);
	virtual void GAVDP_SEPDiscoveryComplete();	
	virtual void GAVDP_SEPCapability(TAvdtpServiceCapability* aCapability);
	virtual void GAVDP_SEPCapabilityComplete();		
	virtual void GAVDP_AbortStreamConfirm();
	virtual void GAVDP_StartStreamsConfirm();	
	virtual void GAVDP_SuspendStreamsConfirm();
	virtual void GAVDP_SecurityControlConfirm(const TDesC8& /*aResponseData*/) {};	
	virtual void GAVDP_ConfigurationConfirm();
	virtual void GAVDP_ConfigurationStartIndication(TSEID aLocalSEID, TSEID aRemoteSEID);
	virtual TInt	GAVDP_ConfigurationIndication(TAvdtpServiceCapability* aCapability);
	virtual TInt	GAVDP_ConfigurationEndIndication();
	virtual TInt	GAVDP_StartIndication(TSEID aSEID);
	virtual TInt	GAVDP_SuspendIndication(TSEID aSEID);
	virtual TInt	GAVDP_SecurityControlIndication(TSEID /*aSEID*/, TDes8& /*aSecurityData*/) {return KErrNotSupported;};
	virtual void GAVDP_AbortIndication(TSEID aSEID);
	virtual void GAVDP_ReleaseIndication(TSEID aSEID);
	virtual void GAVDP_BearerReady(RSocket aSocket, const TAvdtpSockAddr& aAddress);
	virtual void GAVDP_Error(TInt aError, const TDesC8& aErrorData);

	//CActive
	void RunL();
	void DoCancel();
private:
	MGAVDPStateChangeObserver& iGAVDPStateChangeObserver;
	TGAVDPState iInitialState;
	TGAVDPState iCurrentState;
	TGAVDPState iNextState;
	TGAVDPState	iTargetState;
	TConfigurationByRemoteHeadsetState iConfigurationByRemoteHeadsetState;
	CA2dpAudioCodecConfiguration& iA2dpCodecSettings;
	TBool iLocalSEPConfigured;
	TBool iChangeOfSelectedHeadsetSEP;
	TBTDevAddr iBTDevAddr;
	TBool iStateChangeInProgress;
	RSocketServ& iSocketServer;
	RSocket iBearerSocket;
	RGavdp		iGavdp;
	TSEID		iSymbianDeviceSEID;
	TSEID		iHeadsetSEID;
	RArray<TSEID> iPotentialSEPs;
	RArray<TUsableSEP> iSymbianDeviceSEPs;
	RArray<TUsableSEP> iUsableSEPs; //SEPs that can actually be used to stream audio
	RPointerArray<TAvdtpServiceCapability> iSEPCapabilities;
	TUint iSEPIterator; //used to iterate through potential SEPs when getting caps
	TUint iSEPCapabilityEntry;
	
	//heap members owned by CGAVDPStateMachine
	CGavdpTimeout* iSignallingTransactionTimeout;
	};
	
/**
The SEP discovery and GetCapabilities do not have built in timeouts,
so a timeout is required

@internalComponent
*/
NONSHARABLE_CLASS(CGavdpTimeout) : public CTimer
	{
public:
	static CGavdpTimeout* NewL(CGAVDPStateMachine& aGAVDPStateMachine);
	void StartTimer(TTimeIntervalMicroSeconds32 aTimeInterval);
private:
	CGavdpTimeout();
	void ConstructL(CGAVDPStateMachine& aGAVDPStateMachine);
	virtual void RunL();
private:
	CGAVDPStateMachine* iGAVDPStateMachine;
	};
	

#endif
	
