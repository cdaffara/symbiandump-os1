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

#include "A2dpCodecUtilities.h"
#include "GavdpStateMachine.h"


/**
GAVDP State Machine Panics
**/
enum TGavdpStateMachinePanic
	{
	EGavdpStateMachineBadState,//0
	EGavdpStateMachineBadConfigByRemoteHeadsetState,//1
	EGavdpStateMachineBadBTAddr,//2
	EGavdpStateMachineUnexpectedCallback,//3
	EGavdpStateMachineDataTypeMismatch//4
	};


static void Panic(TGavdpStateMachinePanic aPanic)
// Panic client
	{
	_LIT(KGavdpStateMachinePanicName, "GAVDP State Mchn");
	User::Panic(KGavdpStateMachinePanicName, aPanic);
	}


/**
Utility function to map a bluetooth code to a standard SymbianOS code
Where a standard Symbian error code exists the error is mapped
to the standard code.  Ideally all HCI, L2CAP, AVDTP, GAVDP errors
should be mapped to standard SymbianOS codes since bluetooth specific
error codes aren't meaningful in the multimedia domain
*/
static TInt ConvertToStandardSymbianOSError(TInt aError)
	{
	TInt error;
	
	switch (aError)
		{
		case KHCIErrorBase-EPageTimedOut://-6004
			error = KErrCouldNotConnect;
			break;
		case KHCIErrorBase-EAuthenticationFailure://-6005
			error = KErrCouldNotConnect;
			break;
		case KHCIErrorBase-EPairingNotAllowed: //-6024
			error = KErrCouldNotConnect;
			break;
		case KHCIErrorBase-ELMPResponseTimeout: //-6034
			error = KErrTimedOut;
			break;
		case KErrHCIConnectFailed: //-6304
			error = KErrCouldNotConnect;
			break;
		case KErrHCILinkDisconnection: //-6305
			error = KErrDisconnected;
			break;
		default:
			error = aError;
			break;
		}
	return error;
	}


/**
Creates CGAVDPStateMachine

@internalComponent
@return CGAVDPStateMachine*
*/
CGAVDPStateMachine* CGAVDPStateMachine::NewL(MGAVDPStateChangeObserver& aGAVDPStateChangeObserver, CA2dpAudioCodecConfiguration& aA2dpCodecSettings, RSocketServ& aSocketServer)
	{
	CGAVDPStateMachine* self = new(ELeave) CGAVDPStateMachine(aGAVDPStateChangeObserver, aA2dpCodecSettings, aSocketServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/**
Constructor
[TODO] change default stereo mode to EMMFJoint when hardware has sufficient bandwidth
Note we make the AO priority one less than normal to give the AOs in GAVDP
preference
*/
CGAVDPStateMachine::CGAVDPStateMachine(MGAVDPStateChangeObserver& aGAVDPStateChangeObserver, CA2dpAudioCodecConfiguration& aA2dpCodecSettings, RSocketServ& aSocketServer) : CActive(EPriorityNormal-1), 
iGAVDPStateChangeObserver(aGAVDPStateChangeObserver) ,iA2dpCodecSettings(aA2dpCodecSettings), iSocketServer(aSocketServer)
	{
	CActiveScheduler::Add(this);
	}


void CGAVDPStateMachine::ConstructL()
	{
	User::LeaveIfError(iGavdp.Open(*this, iSocketServer));
	User::LeaveIfError(RegisterLocalSEP());
	iSignallingTransactionTimeout = CGavdpTimeout::NewL(*this);
	}
	

/**
Internal function to register local SEPs with GAVDP
The function sets the iSymbianDeviceSEID to the TSEID 
of the Symbian device
*/	
TInt CGAVDPStateMachine::RegisterLocalSEP()
	{
	//register SEPs - currently SBC & mp3 SEPs are supported
	TAvdtpSEPInfo avdtpSEPInfo;	
	avdtpSEPInfo.SetInUse(EFalse);
	avdtpSEPInfo.SetMediaType(EAvdtpMediaTypeAudio);
	avdtpSEPInfo.SetIsSink(EFalse);
	TInt err = iGavdp.RegisterSEP(avdtpSEPInfo);	
	TUsableSEP sep;
	sep.iSEID = avdtpSEPInfo.SEID();
	sep.iDataType = KMMFFourCCCodeSBC;
	err = iSymbianDeviceSEPs.Append(sep);
	
	if (!err)
		{
		avdtpSEPInfo.SetInUse(EFalse);
		avdtpSEPInfo.SetMediaType(EAvdtpMediaTypeAudio);
		avdtpSEPInfo.SetIsSink(EFalse);
		err = iGavdp.RegisterSEP(avdtpSEPInfo);
		if (!err)
			{
			sep.iSEID = avdtpSEPInfo.SEID();
			sep.iDataType = KMMFFourCCCodeMP3;
			err = iSymbianDeviceSEPs.Append(sep);
			}
		}			
	if (!err)
		{
		TBool SEPDataTypeFound(EFalse);
		for(TUint i = 0; i< iSymbianDeviceSEPs.Count();i++)
			{
			if (iSymbianDeviceSEPs[i].iDataType == iA2dpCodecSettings.HeadsetCodecDataType()) 
				{
				iSymbianDeviceSEID = iSymbianDeviceSEPs[i].iSEID;
				SEPDataTypeFound = ETrue;
				break;
				}
			}
		if (!SEPDataTypeFound)
			{
			err = KErrNotFound;
			}
		}
	return err;
	}


/**
Standard destructor.
*/
CGAVDPStateMachine::~CGAVDPStateMachine()
	{
	Cancel();
	iBearerSocket.Close();
	iGavdp.Close();
	iPotentialSEPs.Close();
	iUsableSEPs.Close();
	iSymbianDeviceSEPs.Close();
	iSEPCapabilities.ResetAndDestroy();
	iSEPCapabilities.Close();
	delete iSignallingTransactionTimeout;
	}


/**
Function to set the Bluetooth address of the headset.

@param aRemoteAddress Bluetooth address of the headset
*/
void CGAVDPStateMachine::SetBTAddress(const TBTDevAddr& aRemoteAddress)
	{
	if (iBTDevAddr != aRemoteAddress)
		{//must be using a new headet so reset state machine
		Reset();
		iBTDevAddr = aRemoteAddress;
		}
	}
	
	
/**
Function to returns the current state of the GAVDP state machine.

@return the current state of the GAVDP state machine
*/
TGAVDPState CGAVDPStateMachine::State() const
	{
	return iCurrentState ;
	}


/**
Internal function to resets the GAVDP state machine to the EGAVDPIdle state
Note that if the headset is already connected Reseting
will cause a subsequent call to RGavdp.Connect, however
if we are already connected this should complete straight away

Note we don't close the bearer socket here as we need to ensure
this is closed after the RTP streamer is deleted so this is handled by the CA2dpBTHeadsetAudioInterface
*/
void CGAVDPStateMachine::Reset()
	{
	iPotentialSEPs.Reset();
	iUsableSEPs.Reset();
	iSEPCapabilities.ResetAndDestroy();
	iSEPIterator = 0;
	iSEPCapabilityEntry = 0;
	iA2dpCodecSettings.Reset();
	iInitialState = TGAVDPState::EGAVDPIdle;
	iCurrentState = TGAVDPState::EGAVDPIdle;
	iNextState = TGAVDPState::EGAVDPIdle;
	iTargetState = TGAVDPState::EGAVDPIdle;
	iConfigurationByRemoteHeadsetState.Reset();
	iHeadsetSEID.Reset();
	iLocalSEPConfigured = EFalse;
	iChangeOfSelectedHeadsetSEP = EFalse;
	iGavdp.Cancel();
	iSignallingTransactionTimeout->Cancel();
	}


/**
Function to move the GAVDP state machine to the state specified in aNewState
Not all state changes are allowed, however. 
Permitted state changes are anystate to ->EGAVDPIdle,EConnectedToGavdp,ESEPsDiscovered
which causes a reset.  
Anything prior to EGAVDPOpen->EGAVDPOpen
EGAVDPOpen->EGAVDPStreaming
EGAVDPStreaming->EGAVDPOpen

When the state change is completed a GAVDPStateChangeComplete callback is 
made on the MGAVDPStateChangeObserver, which is the CA2dpBTHeadsetAudioInterface

@param aNewState The state we wish to go to
@return standard SymbianOS error code
*/
TInt CGAVDPStateMachine::ChangeState(const TGAVDPState& aNewState)
	{
	if (iStateChangeInProgress)
		{//can't change state if a state change is already in progress
		return KErrNotReady; 
		}
	if (aNewState.State()<= TGAVDPState::ESEPSelected)
		{
		//if the state is ESEPSelected or less then
		//the assumption is we need to reselect the SEP and in
		//effect redo the entire initialization prior to selecting the SEP
		//therefore roll back to EGAVDPIdle and reset.
		Reset();
		}
	else if (aNewState == TGAVDPState::EGAVDPStreaming)
		{
		//must be in the EGAVDPOpen state in order to go to EGAVDPStreaming
		if (iCurrentState != TGAVDPState::EGAVDPOpen)
			{
			return KErrNotReady;
			}
		}
		
	if (aNewState == iCurrentState)
		{
		//then we are already in the requested state so don't move to
		//next state- so just call the RunL and complete
		iStateChangeInProgress = EFalse;//we're not going to change the state
		TRequestStatus* stat = &iStatus;
		if (!IsActive())
			{
			User::RequestComplete(stat, KErrNone);
			SetActive();
			}
		}
	else
		{//need to move to the next state
		iInitialState = iCurrentState; //incase we need to wind back if state change fails
		iTargetState = aNewState;
		NextState();
		}
	return KErrNone;
	}


/**
Function to cause a reconfiguration of the SEP.  The function determines 
whether a reconfiguration is required ie becuase the settings have changed.
And if so the SEP is reconfigured or a new SEP is selected if the reconfiguration
results in a change of SEP ie because the datatype has changed
This function makes a GAVDPStateChangeComplete callback on the CA2dpBTHeadsetAudioInterface
If the SEP changes because of a change in data type then the 	iChangeOfSelectedHeadsetSEP
flag is set which is used by the state machine to cause the media transport
caps to be added to the SEP

@param aSettingsHaveChanged set to ETrue if the settings have changed

@return SymbianOS error code
*/	
TInt CGAVDPStateMachine::Reconfigure(TBool aSettingsHaveChanged)
	{
	//not allowed to reconfigure in the middle of a state change	
	if ((iStateChangeInProgress)||(IsActive()))
		{
		return KErrNotReady;
		}	
		
	//in order to perform a reconfigure the current state must
	//be either EConfigured or EGAVDPOpen
	//reconfiguration by the SymbianOs device in the streaming state is not supported
	//in this implementation even though it is allowed by the GAVDP profile
	if ((iCurrentState != TGAVDPState::EConfigured) &&
		(iCurrentState != TGAVDPState::EGAVDPOpen))
		{
		return KErrNotReady;
		}
		
	__ASSERT_DEBUG((iCurrentState == iTargetState), Panic(EGavdpStateMachineBadState));//this should always be true if no state change is in progress
	
	iChangeOfSelectedHeadsetSEP = EFalse;
	if (aSettingsHaveChanged)
		{//then see if we need to change SEP
		//data type has changed so we need to choose a different SEP
		//and configure it
		//note if no match is found then we go with the exiting
		//selected SEP
		for (TUint i=0;  i<iUsableSEPs.Count() ; i++)
			{
			TUsableSEP sep = iUsableSEPs[i];
			if (sep.iDataType == iA2dpCodecSettings.HeadsetCodecDataType())
				{//then we have a match so use this SEP
				if (sep.iSEID != iHeadsetSEID)
					{//then this is a different SEP to the one we were already using
					iHeadsetSEID = sep.iSEID;
					iLocalSEPConfigured = EFalse;
					iChangeOfSelectedHeadsetSEP = ETrue;//need to config transport capabilites again if change of SEP
					//need to change the symbian device SEP as well
					TBool symbianDeviceSupportsNewSEPDataType(EFalse);
					for(TUint i = 0; i< iSymbianDeviceSEPs.Count();i++)
						{
						if (iSymbianDeviceSEPs[i].iDataType == sep.iDataType) 
							{
							iSymbianDeviceSEID = iSymbianDeviceSEPs[i].iSEID;
							symbianDeviceSupportsNewSEPDataType = ETrue;
							break;
							}
						}
					__ASSERT_DEBUG(symbianDeviceSupportsNewSEPDataType, Panic(EGavdpStateMachineDataTypeMismatch));
					
					//the if condition below is totaly unnecesaary since we have already
					//ASSERTed symbianDeviceSupportsNewSEPDataType, however the ARM 5 compiler
					//does not seem to recognize code in ASSERT statements so the if statement
					//supresses an ARM v5 compiler warning
					if (symbianDeviceSupportsNewSEPDataType)
						{
						iBearerSocket.Close(); //if we change headset SEP then the old bearer is no longer valid 
						}
					}
				}
			}
		}
		
	
		
	//if a reconfigure is required then we need to reset the current state
	//to ESEPSelected and reconfigure 
	iInitialState = iCurrentState; //incase we need to wind back if state change fails
	
	if ((aSettingsHaveChanged)||(iChangeOfSelectedHeadsetSEP))
		{
		iTargetState = iCurrentState;  //need to end up in the same state we started at
		iCurrentState = TGAVDPState::ESEPSelected; //roll back the state machine to ESEPSelected
		iNextState = iCurrentState;
		NextState(); //make sure the NextState is EConfigured
		}
	else
		{//go to RunL to make GAVDPStateChangeComplete callback
		if (!IsActive())
			{
			TRequestStatus* stat = &iStatus;
			User::RequestComplete(stat, KErrNone);
			SetActive();
			}
		}
	return KErrNone;
	}


/**
Advance to Next state
*/	
void CGAVDPStateMachine::NextState()
	{
	iStateChangeInProgress = ETrue;
	TRequestStatus* stat = &iStatus;
	++iNextState;
	if (!IsActive())
		{
		User::RequestComplete(stat, KErrNone);
		SetActive();
		}
	}
	

/**
Internal function to complete the state change
*/	
void CGAVDPStateMachine::StateChangeComplete()
	{
	iCurrentState = iNextState;
	if (iCurrentState == iTargetState)
		{
		iStateChangeInProgress = EFalse;
		iGAVDPStateChangeObserver.GAVDPStateChangeComplete(iInitialState, KErrNone);
		}
	else
		{//we've completed the internal state change but have not reached the target state
		NextState();
		}
	}


/**
Function to cancel a state change initialited by the ChangeState function
This function is called when the change state is explicitly cancelled by the client
*/
void CGAVDPStateMachine::CancelChangeState()
	{
	//if we call this externally then the reason is KErrCancel
	//as the state change has been explicitely cancelled
	CancelChangeState(KErrCancel);
	}

	
/**
Internal function to cancel a state change initiated by the ChangeState function
Usually called when an error occurs that would prevent the state change
from completing
*/	
void CGAVDPStateMachine::CancelChangeState(TInt aReason)
	{
	if (iStateChangeInProgress)
		{//unwind the request as far as possible
		switch (iInitialState.State())
			{
			case TGAVDPState::EGAVDPIdle:
				Reset();
				break;
			case TGAVDPState::EGAVDPOpen:
				iGavdp.Cancel();
				iCurrentState = TGAVDPState::EGAVDPOpen;
				iNextState = TGAVDPState::EGAVDPOpen;
				iTargetState = TGAVDPState::EGAVDPOpen;
				break;
			case TGAVDPState::EGAVDPStreaming:
				iGavdp.Cancel();
				iNextState = TGAVDPState::EGAVDPStreaming;
				iTargetState = TGAVDPState::EGAVDPStreaming;
				break;
			default:
				Panic(EGavdpStateMachineBadState);
				break;		
			}	
		}
	if (IsActive())
		{
		Cancel();
		}
	iStateChangeInProgress = EFalse;
	iGAVDPStateChangeObserver.GAVDPStateChangeComplete(iInitialState, aReason);
	}
	

/**
Internal function to initiate the connection to the headset
*/
void CGAVDPStateMachine::ConnectToGAVDP()
	{
	iGavdp.Connect(iBTDevAddr);
	}
	

/**
MGavdpUser callback to indicate the headset is connected

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_ConnectConfirm(const TBTDevAddr& aDevice)
	{
	if (!iStateChangeInProgress)
		{//connect initiated by remote headset so we'll accept the address
		iBTDevAddr = aDevice;
		iCurrentState == TGAVDPState::EConnectedToGavdp;
		}
	else if (iBTDevAddr == aDevice)
		{//if a state change is in progress then we have already set the bt
		//address so the above should be true
		//if its not we'll treat the connect as a spurious connect and ignore
		if (iCurrentState.State() == TGAVDPState::EGAVDPIdle)
			{//if the above isn't true then must be a spurious connect from the headset
			//now connected to the device in question
			StateChangeComplete();
			}	
		}
	}


/**
MGavdpUser callback to indicate the headset has suspended streaming
Makes a GAVDPStateMachineStreamSuspendedByRemoteHeadset MGAVDPStateChangeObserver
callback on the CA2dpBTHeadsetAudioInterface

@see MGavdpUser
*/
TInt CGAVDPStateMachine::GAVDP_SuspendIndication(TSEID aSEID)
	{
	TInt err = KErrNone;
	//first check the callback is for the symbian device SEP we are using to 
	//send data to the a2dp headset - incase the Symbian device has more than one SEP
	if (aSEID == iSymbianDeviceSEID)
		{
		__ASSERT_DEBUG((iCurrentState.State() == TGAVDPState::EGAVDPStreaming), Panic(EGavdpStateMachineUnexpectedCallback));
	
		//need to stop the RTP streamer from sending more packets to the headset
		iGAVDPStateChangeObserver.GAVDPStateMachineStreamSuspendedByRemoteHeadset();
	
		iCurrentState = TGAVDPState::EGAVDPSuspended;
		}
	else
		{
		err = ConvertToSymbianError::AvdtpError(EAvdtpBadACPSEID);
		}
	return err;
	}


/**
MGavdpUser callback to indicate the headset wished to configure the SymbianDevice

@see MGavdpUser
*/
void CGAVDPStateMachine::GAVDP_ConfigurationStartIndication(TSEID aLocalSEID, TSEID aRemoteSEID)
	{
	//first check the callback is for the symbian device SEP we are using to 
	//send data to the a2dp headset - incase the Symbian device has more than one SEP
	if (aLocalSEID == iSymbianDeviceSEID)
		{
		__ASSERT_DEBUG((iCurrentState.State() != TGAVDPState::EGAVDPStreaming), Panic(EGavdpStateMachineUnexpectedCallback));
		__ASSERT_DEBUG(iConfigurationByRemoteHeadsetState.State() == TConfigurationByRemoteHeadsetState::ENotBeingConfiguredByRemoteHeadset, Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState));
	
		//cancel any current state changes - config is being driven by headset
		Cancel();
		iGavdp.Cancel();
		//no timeouts are required as if the headset goes down ie does not 
		//result in the expected GAVDP_ConfigurationIndication callbacks a GAVDP_Error
		//callback occurs.
		
		iConfigurationByRemoteHeadsetState = TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationStart;
		
		//we just store the remote SEP for now
		iConfigurationByRemoteHeadsetState.SetSEPRequestedByRemoteHeadset(aRemoteSEID);
		}
	//	else we can't return an error if the aLocalSEID is not for the SymbianOS device
	}
	

/**
MGavdpUser callback to request the SymbianOS device to take the configuration passed in aCapability
Only SBC reconfigurations by the remote headset are supported, sample rate changes
are not supported, changes in the number of channels are not suported
if the capability is TSBCCodecCapabilities then the function checks that the values
requested can be accepted.

@see MGavdpUser
*/
TInt CGAVDPStateMachine::GAVDP_ConfigurationIndication(TAvdtpServiceCapability* aCapability)
	{
	//the underlying GAVDP/AVDTP code should ensure that this 
	//callback from GAVDP never occurs once the headset has been configured	
	__ASSERT_DEBUG((iCurrentState.State() != TGAVDPState::EGAVDPStreaming), Panic(EGavdpStateMachineUnexpectedCallback));
	__ASSERT_DEBUG(iConfigurationByRemoteHeadsetState.State() == TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationStart, Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState));
		
	TInt err = KErrNone;

	switch(aCapability->Category())
		{
		case EServiceCategoryMediaTransport:
			{
			TAvdtpMediaTransportCapabilities* mediaTransportCaps = static_cast<TAvdtpMediaTransportCapabilities*>(aCapability);
			iConfigurationByRemoteHeadsetState.SetTransportCapsRequestedByRemoteHeadset(mediaTransportCaps);
			}
			break;
		case EServiceCategoryMediaCodec:
			{
			TAvdtpMediaCodecCapabilities* codecCaps = static_cast<TAvdtpMediaCodecCapabilities*>(aCapability);
			if (codecCaps->MediaType() != EAvdtpMediaTypeAudio)
				{
				err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedCodec);
				}
			//we're only going to allow a reconfiguration for SBC
			if ((codecCaps->MediaCodecType() == EAudioCodecSBC) && (iA2dpCodecSettings.HeadsetCodecDataType() == KMMFFourCCCodeSBC))
				{
				TSBCCodecCapabilities* sbcCodecCapabilities = static_cast<TSBCCodecCapabilities*>(codecCaps);
				TSBCSamplingFrequencyBitmask samplingFreqBitMask = sbcCodecCapabilities->SamplingFrequencies();
				
				//if the headset has already been configured then we don't allow a change 
				//of sampling frequency.  This is because once the DevSound has been configured
				//with a sampling frequency after the DevSound is initialized and hence
				//the a2dpheadsetif is initialized, it is no longer possible to change the sample frequency
				TBool headsetAlreadyConfigured(EFalse);
				if ((iCurrentState.State() == TGAVDPState::EGAVDPOpen) ||
				(iCurrentState.State() == TGAVDPState::EGAVDPSuspended))
					{
					headsetAlreadyConfigured = ETrue;
					}
				//only one bit of the sampling frequency mask should be set
				//note we're using switch statements rather than if then else
				//it makes it easier to pick up the case where we have an invalid
				//value or more than one value has been set in the bit mask
				switch (samplingFreqBitMask)
					{
					case E16kHz:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.SampleRate() != 16000))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedSamplingFrequency);
							}
						break;
					case E32kHz:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.SampleRate() != 32000))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedSamplingFrequency);
							}
						break;
					case E44100Hz:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.SampleRate() != 44100))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedSamplingFrequency);
							}
						break;
					case E48kHz:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.SampleRate() != 48000))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedSamplingFrequency);
							}
						break;
					default:
						err = ConvertToSymbianError::A2dpError(EA2dpInvalidSamplingFrequency);
						break;
					}
				TSBCChannelModeBitmask channelModeBitMask =  sbcCodecCapabilities->ChannelModes();
				
				//if the headset has already been configured then we don't allow a change 
				//of channels.  This is because once the DevSound has been configured
				//with a sampling frequency after the DevSound is initialized and hence
				//the a2dpheadsetif is initialized, it is no longer possible to change the number of channels
				switch (channelModeBitMask)
					{
					case EMono:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.Channels() != EMMFMono))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedChannelMode);
							}
						break;
					case EDualChannel: //we don't support dual channel
						err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedChannelMode);
						break;
					case EStereo:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.Channels() != EMMFStereo))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedChannelMode);
							}
						break;
					case EJointStereo:
						if ((headsetAlreadyConfigured) && (iA2dpCodecSettings.Channels() != EMMFStereo))
							{
							err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedChannelMode);
							}
						break;
					default:
						err = ConvertToSymbianError::A2dpError(EA2dpInvalidChannelMode);
						break;
					}
					
				TSBCBlockLengthBitmask blockLengthBitMask = sbcCodecCapabilities->BlockLengths();
				
				switch (blockLengthBitMask)
					{
					case EBlockLenFour:
						break;
					case EBlockLenEight:
						break;
					case EBlockLenTwelve:
						break;
					case EBlockLenSixteen:
						break;
					default:
						err = ConvertToSymbianError::A2dpError(EA2dpInvalidBlockLength);
						break;
					}
					
				TSBCSubbandsBitmask subbandsBitMask = sbcCodecCapabilities->Subbands();
				
				TUint subbands = 0;
				switch (subbandsBitMask) 
					{
					case EFourSubbands:
						subbands = 4;
						break;
					case EEightSubbands:
						subbands = 8;
						break;
					default:
						err = ConvertToSymbianError::A2dpError(EA2dpInvalidSubbands);
						break;
					}
					
				TSBCAllocationMethodBitmask allocationMethodBitMask = sbcCodecCapabilities->AllocationMethods();
				
				switch (allocationMethodBitMask)
					{
					case ELoudness:
						break;
					case ESNR:
						break;
					default:
						err = ConvertToSymbianError::A2dpError(EA2dpInvalidAllocationMethod);
						break;
					}
					
				TInt bitPoolValue = sbcCodecCapabilities->MinBitpoolValue();
				
				if (bitPoolValue < KMinBitPoolValue)
					{
					err = ConvertToSymbianError::A2dpError(EA2dpInvalidMinimumBitPoolValue);
					}
					
				bitPoolValue = sbcCodecCapabilities->MaxBitpoolValue();
				
				//The bitpool value must be in the range of 2-250 and must not exceed
				// 16*numberOfSubbands*channels
				if ((bitPoolValue > KMaxBitPoolValue)||(bitPoolValue > (16*subbands*iA2dpCodecSettings.Channels())))
					{		
					err = ConvertToSymbianError::A2dpError(EA2dpInvalidMaximumBitPoolValue);
					}
				if (!err)
					{//note ownership of codecCaps is transferred
					iConfigurationByRemoteHeadsetState.SetCodecConfigRequestedByRemoteHeadset(codecCaps);
					}
				}//if (codecCaps->MediaCodecType() == EAudioCodecSBC) && (iA2dpCodecSettings.HeadsetCodecDataType() == KMMFFourCCodeSBC))
			else
				{//only support SBC for now
				err = ConvertToSymbianError::A2dpError(EA2dpNotSupportedCodec);
				}
			}
			break; //case EServiceCategoryMediaCodec:
		default: //	switch(aCapability->Category())
			err = KErrNotSupported;
			break;
		}
	
	if (err)
		{
		iConfigurationByRemoteHeadsetState = TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationFailed;
		if ((iCurrentState.State() < TGAVDPState::EConfigured) && (iCurrentState.State() != TGAVDPState::EGAVDPIdle))
			{//we only abort if the remote headset has not been configured
			//need to abort using SEID of remote headset - this resets config at remote headset
			Cancel(); //probably not necessary - but just in case
			iGavdp.AbortStream(iConfigurationByRemoteHeadsetState.SEPRequestedByRemoteHeadset());
			}
		//else
		//config is a reconfig in suspend streams 
		//wait on a Start_Indication if the headset is ok with the existing config
		//or an Abort_Indication if the headset is not
		}
	
	return err;
	}


/*
MGavdpUser callback to indicate to the SymbianOS device that all the GAVDP_ConfigurationIndications
have been sent.
This callback should only occur if there have been no errored GAVDP_ConfigurationIndications

@see MGavdpUser
*/
TInt CGAVDPStateMachine::GAVDP_ConfigurationEndIndication()
	{	
	//the underlying GAVDP/AVDTP code should ensure that this 
	//callback from GAVDP never occurs once the headset has been configured
	__ASSERT_DEBUG((iCurrentState.State() != TGAVDPState::EGAVDPStreaming), Panic(EGavdpStateMachineUnexpectedCallback));
	__ASSERT_DEBUG(iConfigurationByRemoteHeadsetState.State() == TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationStart, Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState));
	
	TInt err = KErrNone;
	
	TAvdtpMediaCodecCapabilities* configRequestedByRemoteHeadset = iConfigurationByRemoteHeadsetState.CodecConfigRequestedByRemoteHeadset();
	if (configRequestedByRemoteHeadset)
		{
		__ASSERT_DEBUG(iConfigurationByRemoteHeadsetState.SEPRequestedByRemoteHeadset().IsValid(), Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState));
		
		//the headset settings have been accepted so use the headset settings
		//the only usable SEP shall be the once specified by the headset
		//for now this will always be SBC
		TUsableSEP SEP;
		SEP.iSEID = iConfigurationByRemoteHeadsetState.SEPRequestedByRemoteHeadset();
		TFourCC dataTypeSupport;
		switch (configRequestedByRemoteHeadset->MediaCodecType())
			{
			case EAudioCodecSBC:
				dataTypeSupport.Set(KMMFFourCCCodeSBC);
				break;
			case EAudioCodecMPEG12Audio:
				dataTypeSupport.Set(KMMFFourCCCodeMP3);
				break;
			case EAudioCodecMPEG24AAC:
				dataTypeSupport.Set(KMMFFourCCCodeAAC);
				break;
			case EAudioCodecATRAC:
				dataTypeSupport.Set(KMMFFourCCCodeATRAC3);
				break;
			default://should never get here
				Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState);
				break;
			}
		SEP.iDataType.Set(dataTypeSupport);
		iUsableSEPs.Reset();
		err = iUsableSEPs.Append(SEP);
		
		//the only service capabilities are codec caps provided by the headset and transport
		if (!(iA2dpCodecSettings.UpdateRemoteCodecConfiguration(*configRequestedByRemoteHeadset)))
			{
			err = KErrNoMemory;
			}
		if (!err)
			{
			iSEPCapabilities.ResetAndDestroy();
			err = iSEPCapabilities.Append(iConfigurationByRemoteHeadsetState.TransportCapsRequestedByRemoteHeadset());
			if (!err)
				{
				err = iSEPCapabilities.Append(iConfigurationByRemoteHeadsetState.CodecConfigRequestedByRemoteHeadset());
				}
			}	
		}//	if (codecCaps)

	if (!err)
		{
		if ((iStateChangeInProgress) && (iInitialState.State() == TGAVDPState::EGAVDPIdle))
			{//the headset configured us during an a2dpheadsetif Initialize()
			//we're now configured so now wait for GAVDP_Bearer ready
			iCurrentState == TGAVDPState::EConfigured;
			}
		else if (iCurrentState == TGAVDPState::EGAVDPSuspended)
			{
			//we have reconfigured so the SBC codec needs reconfiguring
			err = iGAVDPStateChangeObserver.GAVDPStateMachineReconfigureByRemoteHeadset();
			}
		}
	if (err)
		{
		if ((iCurrentState.State() < TGAVDPState::EConfigured) && (iCurrentState.State() != TGAVDPState::EGAVDPIdle))
			{//we only abort if the remote headset has not been configured
			//need to abort using SEID of remote headset - this resets config at remote headset
			Cancel(); //probably not necessary - but just in case
			iGavdp.AbortStream(iConfigurationByRemoteHeadsetState.SEPRequestedByRemoteHeadset());
			}
		}
		//else
		//config is a reconfig in suspend streams 
		//wait on a Start_Indication if the headset is ok with the existing config
		//or an Abort_Indication if the headset is not
	iConfigurationByRemoteHeadsetState = TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationEnd;
	return err;
	}


/**
MGavdpUser callback to indicate to the SymbianOS device that it has started streaming
There are two circumstances we can get a start indication
1) The headset can request to start the streams itself once we are in the open state
2) The headset can request a restart after suspending the streams eg to perform a reconfiguration

@see MGavdpUser
*/
TInt CGAVDPStateMachine::GAVDP_StartIndication(TSEID aSEID)
	{
	__ASSERT_DEBUG((iCurrentState.State() == TGAVDPState::EGAVDPOpen) ||
	(iCurrentState.State() == TGAVDPState::EGAVDPSuspended), Panic(EGavdpStateMachineUnexpectedCallback));
	__ASSERT_DEBUG((iConfigurationByRemoteHeadsetState.State() != TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationStart),Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState));
	//first check the callback is for the symbian device SEP we are using to 
	//send data to the a2dp headset - incase the Symbian device has more than one SEP
	if (aSEID == iSymbianDeviceSEID)
		{
		if (iCurrentState.State() ==  TGAVDPState::EGAVDPOpen)
			{
			if (iStateChangeInProgress)
				{
				if (iTargetState.State() == TGAVDPState::EGAVDPStreaming)
					{
					//a EGAVDPOpen->Streaming transistion
					//which has been completed for us by the headset
					Cancel();
					iGavdp.Cancel();
					iCurrentState = TGAVDPState::EGAVDPStreaming;
					StateChangeComplete();
					}
				//else only other state change would be open->idle
				//this is unlikely/impossible if we get a GAVDP_StartIndication
				//do nothing
				}
			else
				{
				//note if no state change is in progress then the headset itself
				//has put itself in the streaming state 
				//normally we put the headset in the streaming state
				//when we call OpenDevice() on the a2dpaudioif
				//but if the headset does this then it is no longer possible
				//for the client to change the audio settings 
				iCurrentState = TGAVDPState::EGAVDPStreaming;
				}
			}//(iCurrentState.State() ==  TGAVDPState::EGAVDPOpen)
		else if (iCurrentState.State() ==  TGAVDPState::EGAVDPSuspended)
			{//most probably restarting after a reconfigure
			iCurrentState = TGAVDPState::EGAVDPStreaming;
			iGAVDPStateChangeObserver.GAVDPStateMachineStreamResumedByRemoteHeadset();
			}
		}//if (aSEID == iSymbianDeviceSEID)
	return KErrNone;	
	}
	
	
/**
MGavdpUser callback to indicate to the SymbianOS device that the request to abort the headset
which means it is not configured has completed
Can request an abort stream if headset configuration fails

@see MGavdpUser
*/
void CGAVDPStateMachine::GAVDP_AbortStreamConfirm()
	{
	//if abort stream was caused by aborting the headset in response
	//to a headset configuration in GAVDP_ConfigurationIndication
	//then restart state machine from EConnectedToGavdp
	if (iConfigurationByRemoteHeadsetState.State() == TConfigurationByRemoteHeadsetState::ERemoteHeadsetConfigurationFailed)
		{//then the abort was caused an aborted configuration from the headset
		__ASSERT_DEBUG((iCurrentState.State() < TGAVDPState::EConfigured), Panic(EGavdpStateMachineUnexpectedCallback));
		
		//since we only aborted if the headset was not yet configured
		//there must be a state change in progress and since we cancelled
		//the GAVDPStateMachine should not be active
		__ASSERT_DEBUG((iStateChangeInProgress) && (!IsActive()), Panic(EGavdpStateMachineBadConfigByRemoteHeadsetState));
		
		iConfigurationByRemoteHeadsetState.Reset();	
		//just to make extra sure we'll rediscover the SEPs
		iPotentialSEPs.Reset();
		iUsableSEPs.Reset();
		iSEPCapabilities.ResetAndDestroy();
		iCurrentState = TGAVDPState::EConnectedToGavdp;
		iNextState = TGAVDPState::EConnectedToGavdp;
		iConfigurationByRemoteHeadsetState.Reset();
		iA2dpCodecSettings.Reset();
		iHeadsetSEID.Reset();
		iBearerSocket.Close();
		iLocalSEPConfigured = EFalse;
		iChangeOfSelectedHeadsetSEP = EFalse;
		iSignallingTransactionTimeout->Cancel();	
		NextState();
		}
	iConfigurationByRemoteHeadsetState.Reset();	
	}


/**
MGavdpUser callback to indicate to the SymbianOS device that the headset has aborted
ie it can no longer be regarded as configured
This makes a GAVDPStateMachineEvent MGAVDPStateChangeObserver callback on the CA2dpBTHeadsetAudioInterface
(which inturn will cause a Reset on the GAVDP state machine)

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_AbortIndication(TSEID aSEID)
	{
	//first check the callback is for the symbian device SEP we are using to 
	//send data to the a2dp headset - incase the Symbian device has more than one SEP
	if (aSEID == iSymbianDeviceSEID)
		{
		if (iStateChangeInProgress)
			{
			CancelChangeState(KErrAbort);		
			}
		else 
			{
			iGAVDPStateChangeObserver.GAVDPStateMachineEvent(KErrAbort);
			}
		}
	//else not much we can do it aSEID isn't the Symbian device ID
	iConfigurationByRemoteHeadsetState.Reset();
	}


/**
Internal function to initiate the discovery of the headset SEPs
Note that the SEP discovery does not have a built in timeout
so the GAVDP_SEPDiscovered callback must occur withing the GAVDP callback timeout 30s
or the state change will be cancelled with KErrTimedOut
*/
void CGAVDPStateMachine::DiscoverRemoteSEPs()
	{
	iPotentialSEPs.Reset();
	iGavdp.DiscoverRemoteSEPs();
	iSignallingTransactionTimeout->StartTimer(TTimeIntervalMicroSeconds32(KGAVDPCallbackTimeout));
	}


/**
MGavdpUser callback in response to DiscoverRemoteSEPs

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_SEPDiscovered(const TAvdtpSEPInfo& aSEP)
	{
	//check whether the SEP is suitable for connection
	if ((!aSEP.InUse()) && (aSEP.MediaType() == EAvdtpMediaTypeAudio) && (aSEP.IsSink()))
		{//the SEP is a potentialy suitable for connection
		//so add to list of potential SEPs
		TInt err = iPotentialSEPs.Append(aSEP.SEID());
		if (err)
			{
			CancelChangeState(err);
			}
		}
	//else SEP is not suitable so ignore it
	}


/**
MGavdpUser callback

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_SEPDiscoveryComplete()
	{
	iSignallingTransactionTimeout->Cancel();
	if (iPotentialSEPs.Count())
		{//we have at least one potential SEP
		StateChangeComplete();
		}
	else
		{//no potential SEPs were found so cannot proceed any further
		CancelChangeState(KErrCouldNotConnect);
		}
	}


/**
Internal function to get the capability of an individual SEP
*/
void CGAVDPStateMachine::GetRemoteSEPCapability()
	{
	TAvdtpServiceCategories serviceCategories;
	serviceCategories.SetCapability(EServiceCategoryMediaTransport);
	serviceCategories.SetCapability(EServiceCategoryMediaCodec);
	TSEID SEPtoGetCapabilityOf = iPotentialSEPs[iSEPIterator];
	iGavdp.GetRemoteSEPCapabilities(SEPtoGetCapabilityOf, serviceCategories);
	iSignallingTransactionTimeout->StartTimer(TTimeIntervalMicroSeconds32(KGAVDPCallbackTimeout));
	}


/**
Internal function to iterate through the list of potential SEPs and asking
for the capabilites of each one.
*/	
void CGAVDPStateMachine::GetRemoteSEPCapabilities()
	{
	iSEPCapabilities.ResetAndDestroy(); //clear what's already there
	iUsableSEPs.Reset();
	iSEPIterator = 0;
	iSEPCapabilityEntry = 0;
	//go and get the capability of the first SEP in the list of potential SEPs
	//the GAVDP_SEPCapabilityComplete() callback will cause the next potential
	//SEP capability to be obtained.
	GetRemoteSEPCapability(); 
	}


/**
MGavdpUser callback in response to GetRemoteSEPCapabilities

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_SEPCapability(TAvdtpServiceCapability* aCapability)
	{
	// we own cap, stash it in the iSEPCapabilities array for owning and later use
	TInt err = iSEPCapabilities.Append(aCapability);
	if (err)
		{
		CancelChangeState(err);
		}
	}


/**
MGavdpUser callback in response to GetRemoteSEPCapabilities when all the 
GAVDP_SEPCapability callbacks have been made for an individual SEP

The function iterates through the SEP capabilites looking for transport and media codec support
and if so the SEP is added to the list of usableSEPs.

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_SEPCapabilityComplete()
	{
	TInt err = KErrNone;
	iSignallingTransactionTimeout->Cancel();
	//iterate through the capabilities
	TAvdtpServiceCapability* avdtpServiceCapability;
	TBool mediaTransport(EFalse);
	TBool audioSupport(EFalse);
	TFourCC dataTypeSupport; //note we are assuming here that each SEP only has one codec capability
	TUint i = 0;
	//iterate through the capabilities
	//looking for media transport and codec support
	for	(;iSEPCapabilityEntry < iSEPCapabilities.Count() ; iSEPCapabilityEntry++)
		{
		avdtpServiceCapability = iSEPCapabilities[iSEPCapabilityEntry];
		switch (avdtpServiceCapability->Category())
			{
			case EServiceCategoryMediaTransport:
				mediaTransport = ETrue;
				break;
			case EServiceCategoryMediaCodec:
				{
				TAvdtpMediaCodecCapabilities* codecCaps = static_cast<TAvdtpMediaCodecCapabilities*>(avdtpServiceCapability);
				if (codecCaps->MediaType() == EAvdtpMediaTypeAudio)
					{
					audioSupport = ETrue;
					}
				switch (codecCaps->MediaCodecType())
					{
					case EAudioCodecSBC:
						dataTypeSupport.Set(KMMFFourCCCodeSBC);
						break;
					case EAudioCodecMPEG12Audio:
						dataTypeSupport.Set(KMMFFourCCCodeMP3);
						break;
					case EAudioCodecMPEG24AAC:
						dataTypeSupport.Set(KMMFFourCCCodeAAC);
						break;
					case EAudioCodecATRAC:
						dataTypeSupport.Set(KMMFFourCCCodeATRAC3);
						break;
					default:
						//the datatype is an unsupported datatype
						//so set to NULL to show we can't use it
						dataTypeSupport.Set(KMMFFourCCCodeNULL);
						break;
					}
				}
			}//switch (avdtpServiceCapability->Category())
		}//	for (TUint i=0; i++; i<iSEPCapabilities.Count())
	//check if the capabilities indicate that the SEP is usable
	//for audio transport
	if ((mediaTransport) && (audioSupport) && 
		(dataTypeSupport != KMMFFourCCCodeNULL))
		{//then we can use this SEP to stream audio to the headset
		// however we ie the Symbian device must
		//also be able to support the data type
		//before we can add it to the list of usable SEPs
		TBool symbianDeviceSupportsDataType(EFalse);
		for(TUint i = 0; i< iSymbianDeviceSEPs.Count();i++)
			{
			if (iSymbianDeviceSEPs[i].iDataType == dataTypeSupport) 
				{
				symbianDeviceSupportsDataType = ETrue;
				break;
				}
			}
		if (symbianDeviceSupportsDataType)
			{
			TUsableSEP usableSEP;
			usableSEP.iSEID = iPotentialSEPs[iSEPIterator];
			usableSEP.iDataType.Set(dataTypeSupport);
			err = iUsableSEPs.Append(usableSEP);
			if (err)
				{
				CancelChangeState(err);
				}
			}
		}
	if (!err)
		{
		//check if we have finished going through our list of potential SEPs
		//that we need to find the capabilities of
		iSEPIterator++;
		if (iSEPIterator >= iPotentialSEPs.Count())
			{//then we have finished getting all the capabilities
			//so lets choose a SEP by iterating through the
			//usable SEPs and stopping at the first one that supports the
			//required data type 
			TUsableSEP SEP;
			TBool SEPFound(EFalse);
			for (i=0; i<iUsableSEPs.Count(); i++)
				{
				SEP = iUsableSEPs[i];
				if (SEP.iDataType == iA2dpCodecSettings.HeadsetCodecDataType())
					{//one of the usable SEPs supports the requested data type
					SEPFound = ETrue;
					iHeadsetSEID = SEP.iSEID; //we'll use this SEID for now			
					break;
					}
				}
			iSEPIterator = 0;
			iSEPCapabilityEntry = 0;
			if (SEPFound)
				{
				//we've selected a SEP
				StateChangeComplete(); //so move on to the next state
				}
			else
				{
				//then non  of the SEPs have SBC codec capabilites
				//or no usable SEPs have been found or SEP may be in use
				//since SBC support is mandatory we do not have a suitable SEP
				CancelChangeState(KErrCouldNotConnect);
				}
			}//	if (iPotentialSEPs.Count() >= iSEPIterator)
		else
			{//get the capability of the next SEP
			GetRemoteSEPCapability();
			}
		}
	}


/**
Function used to return a TAvdtpMediaCodecCapabilities structure
that can be used to configure the SEP at the remote end ie on the headset
The capabilities are used to determine the configuration
need to return by pointer rather than by ref as TAvdtpMediaCodecCapabilities is abstract 
*/
TAvdtpMediaCodecCapabilities* CGAVDPStateMachine::RemoteCodecConfiguration()
	{
	TAvdtpMediaCodecCapabilities* codecCaps = NULL;
	TAvdtpMediaCodecCapabilities* codecConfiguration = NULL;
	
	//first get the capabilities of the codec
	TInt error = CodecCaps(codecCaps);
	if (!error)
		{//and use the capabilities to get a valid remote codec configuration
		codecConfiguration = iA2dpCodecSettings.UpdateRemoteCodecConfiguration(*codecCaps);
		}

	return codecConfiguration;
	}


/**
Internal function to configure the local SEP
*/	
TInt CGAVDPStateMachine::ConfigureLocalSEP()
	{
	TInt err = iGavdp.BeginConfiguringLocalSEP(iSymbianDeviceSEID);
	if (err == KErrNotFound)
		{
		//could be a problem with the local SEP no longer being registered
		//so register and try again
		err = RegisterLocalSEP();
		if (!err)
			{
			err = iGavdp.BeginConfiguringLocalSEP(iSymbianDeviceSEID);
			}
		}
	if (!err)
		{//configure local SEP - we only advertise one local SEP with SBC support
		//in future when capability structures are defined for mp3,AAC,ATRAC3
		//then these can also be advertised as local SEPs
		TAvdtpMediaTransportCapabilities media;
		err = iGavdp.AddSEPCapability(media);
		
		if (!err)
			{
			CA2dpLocalCodecCapabilities* localCodecCapabilities = NULL;
			TRAP(err, localCodecCapabilities = CA2dpLocalCodecCapabilities::NewL())
			if (!err)
				{
				TAvdtpMediaCodecCapabilities* localSEPCapability = localCodecCapabilities->LocalCodecCapabilities(iA2dpCodecSettings.HeadsetCodecDataType());
				if (localSEPCapability)
					{
					err = iGavdp.AddSEPCapability(*localSEPCapability);
					}
				else
					{
					err = KErrNoMemory;
					}
				delete localCodecCapabilities;
				localCodecCapabilities = NULL;
				}
			}	
		}
	return err;
	}


/**
Internal function to configure the remote SEP ie the headset

If the SEP has never been configured before then the transport caps are added
If we are doing a reconfigure of the remote SEP then just the codec caps are added
*/
TInt CGAVDPStateMachine::ConfigureRemoteSEP()
	{
	TInt err = iGavdp.BeginConfiguringRemoteSEP(iHeadsetSEID, iSymbianDeviceSEID);
	if (!err)
		{
		if ((iInitialState.State() < TGAVDPState::EConfigured)||(iChangeOfSelectedHeadsetSEP))
			{//then this SEP has never been configured before
			//in which case we need to configure it with transport capabilities
			//note that if the SEP has been configured before ie the above condition
			//is false then we are performing a reconfiguration and are not allowed
			//to reconfigure the media transport capabilities
			TAvdtpMediaTransportCapabilities avdtpMediaTransportCapabilities;
			err = iGavdp.AddSEPCapability(avdtpMediaTransportCapabilities);
			iChangeOfSelectedHeadsetSEP = EFalse;//reset
			}
		if (!err)
			{
			TAvdtpMediaCodecCapabilities* codecConfiguration = RemoteCodecConfiguration();
			//note we are setting a configuration here not a capability
			if (codecConfiguration)
				{
				err = iGavdp.AddSEPCapability(*codecConfiguration);
				}
			else
				{//we were not able to get a valid configuration
				//so abort configuration
				err = KErrAbort;
				}
			}//	if (!err)
		}
	return err;
	}


/**
Internal function to initiate SEP configuration of both the local and remote SEPs
*/			
void CGAVDPStateMachine::ConfigureSEP()
	{
	if (iHeadsetSEID.IsValid())
		{
		TInt err;
		if (!iLocalSEPConfigured)
			{//the local SEP must be configured first before configuring the remote SEP
			err = ConfigureLocalSEP();	
			}
		else
			{//local SEP is already configured so configure remote SEP
			err = ConfigureRemoteSEP();
			}
		if (err)
			{
			CancelChangeState(err);
			}
		else
			{
			iGavdp.CommitSEPConfiguration();
			}
		}
	else
		{//we've requested to configure a SEP before we have
		//a valid SEP to configure
		CancelChangeState(KErrNotReady);
		}
	}
	

/**
MGavdpUser callback to confirm that the SEP configuration has been acepted
This callback should occur for both the local SEP and the remote SEP 
*/
void CGAVDPStateMachine::GAVDP_ConfigurationConfirm()
	{
	TInt err = KErrNone;
	if (!iLocalSEPConfigured)
		{//the local SEP is configured first so this call back must be from
		 //a local SEP configuration
		iLocalSEPConfigured = ETrue;
		//now configure the remote SEP
		err = ConfigureRemoteSEP();
		if (!err)
			{
			iGavdp.CommitSEPConfiguration();
			}
		else
			{
			CancelChangeState(err);
			}
		}
	else
		{
		//local and remote SEPs now configured
		TInt err = iGavdp.Listen();	
		//note that if there is an error above
		// there is not much we can do so ignore it
		StateChangeComplete();
		}
	}
	

/**
Internal function to request a bearer RSocket in order to stream audio to the headset
*/
void CGAVDPStateMachine::CreateBearerSocket()
	{
	//then we need to request for one from the remote SEP
	//this reference code does not support reporting and recovery channels

	if (!iBearerSocket.SubSessionHandle())
		{//we don't already have a bearer socket, create one - note no reporting and recovery implementation
		TInt err = iGavdp.CreateBearerSockets(iHeadsetSEID, EFalse, EFalse);
		if (err)
			{
			CancelChangeState(err);
			}
		}
	else
		{
		//we already have a bearer socket so no need to create a new one
		//just complete the state change
		StateChangeComplete();
		}
	}
	

/**
MGavdpUser callback to supply the RSocket used to stream audio to the headset

This callback can occur either via a direct request from the headset or in response
to a CreateBearerSocket

@see MGavdpUser
*/
void CGAVDPStateMachine::GAVDP_BearerReady(RSocket aSocket, const TAvdtpSockAddr& /*aAddress*/)
	{
	//This call back can occur without a prior call to CreateBearerSocket
	iBearerSocket = aSocket;
	if ((iCurrentState == TGAVDPState::EConfigured)&&(iStateChangeInProgress))
		{//we've completed the state	
		StateChangeComplete();
		}
	// else this came from the headset without a call to RGavdp::CreateBearerSockets
	}


/**
Function to return an array of usable SEPs on the headset.
By 'usable' we mean the SEP supports audio, has media transport caps, is not in use,
has a supported audio codec and the audio codec is supported by the symbianOS device
@return array of usable SEPs
*/	
RArray<TUsableSEP>& CGAVDPStateMachine::UsableSEPs() const
	{
	return const_cast<RArray<TUsableSEP>&>(iUsableSEPs);
	}


/**
Function to return the bearer socket used to stream audio to the headset
@return socket
*/	
RSocket& CGAVDPStateMachine::BearerSocket() const
	{
	return const_cast<RSocket&>(iBearerSocket);
	}


/**
Function to return the headset BT address
@return headset BT address
*/	
TBTDevAddr& CGAVDPStateMachine::BTAddress() const
	{
	return const_cast<TBTDevAddr&>(iBTDevAddr);
	}


/** 
Function to return the SEPCapability for the codec settings
used by CA2dpBTHeadsetAudioInterface to determine the audio settings
ie sample rate / stereo support.
aCodecCaps is set with the TAvdtpMediaCodecCapabilities of the codec in use
if no codec has been specified via the Reconfigure()	function then
the codec defaults to SBC
returns KErrNotReady if no SEP capablities have been obtained from the SEP
return KErrNotSupported if the codec type is not known
aCodecCaps Note that ownership is not transferred to the calling class.

@param aCodecCaps  The pointer points to a codec capabilities structure

@return SymbianOS error code
*/
TInt CGAVDPStateMachine::CodecCaps(TAvdtpMediaCodecCapabilities*& aCodecCaps) const
	{
	TInt err = KErrNotReady;
	TAvdtpServiceCapability* avdtpServiceCapability;
	TFourCC dataType;
	for (TUint i=0;  i<iSEPCapabilities.Count(); i++)
		{
		avdtpServiceCapability = iSEPCapabilities[i];
		if (avdtpServiceCapability->Category() == EServiceCategoryMediaCodec)
			{
			TAvdtpMediaCodecCapabilities* codecCaps = static_cast<TAvdtpMediaCodecCapabilities*>(avdtpServiceCapability);
			switch (codecCaps->MediaCodecType())
				{
				case EAudioCodecSBC:
					dataType.Set(KMMFFourCCCodeSBC);
					break;
				case EAudioCodecMPEG12Audio:
					dataType.Set(KMMFFourCCCodeMP3);
					break;
				case EAudioCodecMPEG24AAC:
					dataType.Set(KMMFFourCCCodeAAC);
					break;
				case EAudioCodecATRAC:
					dataType.Set(KMMFFourCCCodeATRAC3);
					break;
				default:
					err = KErrNotSupported;
					break;
				}
			if (dataType == iA2dpCodecSettings.HeadsetCodecDataType())
				{//then we have the capabilities for the selected datatype
				aCodecCaps = codecCaps;
				err = KErrNone;
				break;
				}	
			}	
		}	//for (TUint i=0;  i<iSEPCapabilities.Count(); i++)
	return err;
	}
	

/**
MGavdpUser callback when headset releases stream

@see MGavdpUser
*/
void CGAVDPStateMachine::GAVDP_ReleaseIndication(TSEID aSEID)
	{
	if (aSEID == iSymbianDeviceSEID)
		{
		if (iStateChangeInProgress)
			{
			CancelChangeState(KErrDisconnected);
			}			
		else
			{
			iGAVDPStateChangeObserver.GAVDPStateMachineEvent(KErrDisconnected);
			}
		TInt err = iGavdp.Listen();//can't do much if this errors
		}
	}


/**
MGavdpUser error callback 

@see MGavdpUser
*/
void CGAVDPStateMachine::GAVDP_Error(TInt aError, const TDesC8& /*aErrorData*/)
	{	
	TInt error = ConvertToStandardSymbianOSError(aError);
	if (iStateChangeInProgress)
		{
		CancelChangeState(error);
		}
	else
		{
		//error must have occured while no state change was in progress
		//this could be due to a problem on the headset eg no longer in range
		//Make callback on a2dpBTHeadsetIf so it can reset back to the idle state
		iGAVDPStateChangeObserver.GAVDPStateMachineEvent(error);
		}
	//start listening for connect event from headset
	TInt err = iGavdp.Listen();//can't do much if this errors	
	}


/**
Internal function to put the headset in the streaming state
*/	
void CGAVDPStateMachine::StartStream()
	{
	//we're only going to start one transport stream
	iGavdp.StartStream(iHeadsetSEID);
	}


/**
MGavdpUser callback in response to StartStream

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_StartStreamsConfirm()
	{
	StateChangeComplete();
	}


/**
Internal function to tell the headset we are suspending the stream
*/	
void CGAVDPStateMachine::SuspendStream()
	{
	//we're only going to suspend one transport stream
	iGavdp.SuspendStream(iHeadsetSEID);
	}


/**
MGavdpUser callback in response to SuspendStream

@see MGavdpUser
*/	
void CGAVDPStateMachine::GAVDP_SuspendStreamsConfirm()
	{
	StateChangeComplete();
	}

	
/**
RunsL for GAVDP state machine
*/
void CGAVDPStateMachine::RunL()
	{
	if (iStateChangeInProgress)
		{
		switch (iCurrentState.State())
			{
			case TGAVDPState::EGAVDPIdle:
				ConnectToGAVDP();
				break;
			case TGAVDPState::EConnectedToGavdp:
				DiscoverRemoteSEPs();
				break;
			case TGAVDPState::ESEPsDiscovered:
				GetRemoteSEPCapabilities();
				break;
			case TGAVDPState::ESEPSelected:
				ConfigureSEP();
				break;
			case TGAVDPState::EConfigured:
				CreateBearerSocket();
				break;
			case TGAVDPState::EGAVDPOpen:
				StartStream();
				break;
			case TGAVDPState::EGAVDPStreaming:
				SuspendStream();
				break;
			default:
				Panic(EGavdpStateMachineBadState);
				break;
			}
		}
	else
		{
		//if RunL should get called when no state
		//change is in progress eg during a reconfiguration
		//then the current state should always be identical to the target state
		__ASSERT_DEBUG((iCurrentState == iTargetState), Panic(EGavdpStateMachineBadState));
		StateChangeComplete();
		}
	}


/**
Cancel
*/
void CGAVDPStateMachine::DoCancel()
	{
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, KErrCancel);
	}




CGavdpTimeout* CGavdpTimeout::NewL(CGAVDPStateMachine& aGAVDPStateMachine)
	{
	CGavdpTimeout* self = new(ELeave)CGavdpTimeout();
	CleanupStack::PushL(self);
	self->ConstructL(aGAVDPStateMachine);
	CleanupStack::Pop();	
	return self;
	}


void CGavdpTimeout::ConstructL(CGAVDPStateMachine& aGAVDPStateMachine)
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	iGAVDPStateMachine = &aGAVDPStateMachine;
	}


CGavdpTimeout::CGavdpTimeout() : CTimer(EPriorityLow)
	{
	}


void CGavdpTimeout::StartTimer(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	Cancel(); //just in case
	After(aTimeInterval);	
	}


void CGavdpTimeout::RunL()
	{
	//the GAVDP callback has timed out - check the GAVDP state machine
	//is in the connected state to cover ourselves in the event of a race
	//condition
	if ((iGAVDPStateMachine->State() == TGAVDPState::EConnectedToGavdp)||
	(iGAVDPStateMachine->State() == TGAVDPState::ESEPsDiscovered))
		{
		iGAVDPStateMachine->CancelChangeState(KErrTimedOut);
		}
	}
	


