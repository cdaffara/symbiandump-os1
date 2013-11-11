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

#include <mmf/server/mmfcodec.h>
#include "mmfSBCCodecImplementationUIDs.hrh"
#include <mmffourcc.h>
#include "A2dpCodecUtilities.h"
#include "A2dpBTheadsetAudioIf.h"
#include "MMFBtRoutingSoundDevice.h"	// for TRange

/**
Panics
**/
enum TA2dpBTHeadsetPanic
	{
	EA2dpBTHeadsetUnExpectedState,//0
	EA2dpBTHeadsetNoRTPStreamer,//1
	EA2dpBTHeadsetNoData,//2
	EA2dpBTHeadsetUnexpectedDataType,//3
	EA2dpBTHeadsetBTAddrNotSet, //4
	EA2dpBTHeadsetNoCodec //5
	};


static void Panic(TA2dpBTHeadsetPanic aPanic)
// Panic client
	{
	_LIT(KA2dpBTHeadsetPanicName, "A2DP BT If Panic");
	User::Panic(KA2dpBTHeadsetPanicName, aPanic);
	}


CA2dpBTHeadsetAudioInterface::CA2dpBTHeadsetAudioInterface()
	{
	}

void CA2dpBTHeadsetAudioInterface::ConstructL()
	{
	iA2dpCodecConfiguration = CA2dpAudioCodecConfiguration::NewL();
	User::LeaveIfError(iSocketServer.Connect());
	iGAVDPStateMachine = CGAVDPStateMachine::NewL(*this, *iA2dpCodecConfiguration, iSocketServer);	
	}
	
/**
Creates CA2dpBTHeadsetAudioInterface
Note there should only ever be one CA2dpBTHeadsetAudioInterface*
However this being a singleton is not enforced in this class however
The client of this class ie the CA2dpBTHeadsetAudioServer should enforce this.
 
@return CA2dpBTHeadsetAudioInterface*
*/	
EXPORT_C CA2dpBTHeadsetAudioInterface* CA2dpBTHeadsetAudioInterface::NewL()
	{
	CA2dpBTHeadsetAudioInterface* self = new(ELeave) CA2dpBTHeadsetAudioInterface();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
destructor
*/	
EXPORT_C CA2dpBTHeadsetAudioInterface::~CA2dpBTHeadsetAudioInterface()
	{
	SetSniffMode(ETrue);
	delete iGAVDPStateMachine;
	delete iCodec;
	delete iA2dpCodecConfiguration;
	delete iRTPStreamer;
	iSocketServer.Close();
	}
	

/**
Procedure to perform a GAVDP initialization sequence on an A2DP headset
If the headset is already initialized then this procedure does not perform
any further initialization.
if link has gone down since a previous call to Initialize then
calling this function will perform a full reinitialize.
if link goes down during initialization will generate error
The GAVDPStateChangeComplete callback is called when the headset device
is in the open state.
The headset is in the GAVDP_Open state on completion
It is not possible to perform an Initialize if the headset is already in the
streaming state or is already being initialized.

@param aRemoteAddress Address of the bluetooth headset we are trying to initialize
@param aStatus
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::Initialize(const TBTDevAddr& aRemoteAddress, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(((iGAVDPStateMachine->State() == TGAVDPState::EGAVDPIdle)||
					(iGAVDPStateMachine->State() == TGAVDPState::EGAVDPOpen)), Panic(EA2dpBTHeadsetUnExpectedState));
	__ASSERT_ALWAYS(!iReconfiguring, Panic(EA2dpBTHeadsetUnExpectedState)); //can't initialize while we're reconfiguring
	iInitializeRequestStatus = &aStatus;
	*iInitializeRequestStatus = KRequestPending;
	
	//this will cause a full initialization if the address has changed
	iGAVDPStateMachine->SetBTAddress(aRemoteAddress);
	
	TGAVDPState state(TGAVDPState::EGAVDPOpen);
	TInt err = iGAVDPStateMachine->ChangeState(state);
	if (err)
		{
		User::RequestComplete(iInitializeRequestStatus, err);
		}	
	}


/**
Procedure to cancel a GAVDP initialization sequence on an A2DP headset
*/		
EXPORT_C void CA2dpBTHeadsetAudioInterface::CancelInitialize()
	{
	if (iInitializeRequestStatus)
		{
		if (*iInitializeRequestStatus == KRequestPending)//make sure there is a pending request to cancel
			{
			iGAVDPStateMachine->CancelChangeState();
			}
		}
	}
	

/**
Handles the state change GAVDPIdle->GAVDPOpen
This state change usually occurs as a result to an Initialize call
Can also occur due to an error condition resulting in a GAVDP state machine reset 
*/
void CA2dpBTHeadsetAudioInterface::ProcessGAVDPStateChangeIdleToOpen(TInt aError)
	{
	__ASSERT_DEBUG((aError||iGAVDPStateMachine->State()==TGAVDPState::EGAVDPOpen||
	iGAVDPStateMachine->State()==TGAVDPState::EGAVDPIdle),Panic(EA2dpBTHeadsetUnExpectedState)); //if there is no error then current state should be open
	
	if (!aError)
		{
		SetSniffMode(ETrue);
		iPaused = EFalse;
		iReconfiguring = EFalse;
		if (iRTPStreamer) //if this is true then an error has occured somewhere
			{//and we need to reset the streamer and close the socket
			delete iRTPStreamer;
			iRTPStreamer = NULL;
			iGAVDPStateMachine->BearerSocket().Close();
			}
		}

	if (iInitializeRequestStatus)
		{
		if (*iInitializeRequestStatus == KRequestPending)
			{
			User::RequestComplete(iInitializeRequestStatus, aError);
			}
		}
	}
	

/**
Handles the state change GAVDPOpen->GAVDPStreaming
This state change occurs as a result to an OpenDevice call 
This can get called twice, once if a reconfiguration is needed prior 
to going in the streaming state where the state machine should still be in the open state
and a second time when the state machine is in the streaming state
*/	
void CA2dpBTHeadsetAudioInterface::ProcessGAVDPStateChangeOpenToStreaming(TInt aError)
	{
	if (iOpenDeviceRequestStatus)
		{
		iOpenDeviceError = aError;
		if (*iOpenDeviceRequestStatus == KRequestPending)
			{
			if (iReconfiguring)
				{//should be in the OPEN state if we are reconfiguring
				__ASSERT_DEBUG((iGAVDPStateMachine->State() == TGAVDPState::EGAVDPOpen),Panic(EA2dpBTHeadsetUnExpectedState));
				iReconfiguring = EFalse;
				if (!iOpenDeviceError)
					{
					//move GAVDP state to streaming
					TGAVDPState state(TGAVDPState::EGAVDPStreaming);
					iOpenDeviceError = iGAVDPStateMachine->ChangeState(state);
					}
				if (iOpenDeviceError)
					{
					SetSniffMode(ETrue); //go back to sniff mode
					User::RequestComplete(iOpenDeviceRequestStatus, iOpenDeviceError);
					}
				}//if (iReconfiguring)
			else
				{//callback must be in response to a streaming change state
				__ASSERT_DEBUG((aError||iGAVDPStateMachine->State()==TGAVDPState::EGAVDPStreaming),Panic(EA2dpBTHeadsetUnExpectedState)); //if there is no error then current state should be open
				if (!iOpenDeviceError)
					{
					__ASSERT_DEBUG((iGAVDPStateMachine->State()==TGAVDPState::EGAVDPStreaming),Panic(EA2dpBTHeadsetUnExpectedState));
					delete iRTPStreamer;
					iRTPStreamer = NULL;
					TRAP(iOpenDeviceError,iRTPStreamer = CActiveRTPStreamer::NewL(iGAVDPStateMachine->BearerSocket(),*this));
					if (!iOpenDeviceError)
						{//create codec
						iAudioSettingsHaveChanged = EFalse;
						iRTPStreamer->SetAudioConfiguration(*iA2dpCodecConfiguration);
						//if we have not set the data type ourselves then get iDataType
						//from the code configuration
						if (iDataType == KMMFFourCCCodeNULL)
							{
							iDataType = iA2dpCodecConfiguration->HeadsetCodecDataType();
							if (iDataType == KMMFFourCCCodeSBC)
								{//then the audio interfacef code will be pcm16
								iDataType = KMMFFourCCCodePCM16;
								}
							}
						if (iDataType == KMMFFourCCCodePCM16)
							{//then an SBC codec is required
							const TUid KSbcCodecUid = { KMmfUidCodecPCM16ToSBC };
							TRAP(iOpenDeviceError,iCodec = CMMFCodec::NewL(KSbcCodecUid));
							if (!iOpenDeviceError)
								{
								TUid configType;
								configType.iUid = KMmfUidSBCConfigure;
								TSBCFrameParameters& SBCFrameParameters = iA2dpCodecConfiguration->UpdateLocalSBCCodecConfiguration();
								//use a package buffer for codec config to keep flexability should
								//it be possible to use other codecs rather than SBC in future.
								TPckgBuf<TSBCFrameParameters> SBCFrameParametersBuf(SBCFrameParameters);
								TRAP(iOpenDeviceError,iCodec->ConfigureL(configType, SBCFrameParametersBuf));
								iRTPStreamer->SetCodec(*iCodec);
								}
							}
						}//if (!iOpenDeviceError)
					}//if (!iOpenDeviceError)
				if ((iOpenDeviceError) && (iGAVDPStateMachine->State()==TGAVDPState::EGAVDPStreaming))
					{
					//there was an error so wind back state to EGAVDPOpen
					//don't complete the OpenDevice request status till we've wound back
					TGAVDPState state(TGAVDPState::EGAVDPOpen);
					TInt iOpenDeviceError = iGAVDPStateMachine->ChangeState(state);
					//if the above fails there's not much we can do
					}
				else
					{
					//if we get to here then we should either be in the streaming state with no errors
					//or in the open state with an error - either way we complete the OpenDevice request status
					User::RequestComplete(iOpenDeviceRequestStatus, iOpenDeviceError);
					if (iOpenDeviceError)
						{
						SetSniffMode(ETrue);
						}
					}
				}//else  if not (iReconfiguring)
			}//	if (*iOpenRequestStatus == KRequestPending)
		}//if (iOpenRequestStatus)
	}


/*
Handles the state change GAVDPStreaming->GAVDPOpen
The transition from streaming to open can be caused by one of two ways
1 - A call to CloseDevice
2 - A call to Open device where there was an error after the transition to streaming
*/	
void CA2dpBTHeadsetAudioInterface::ProcessGAVDPStateChangeStreamingToOpen(TInt aError)
	{
	__ASSERT_DEBUG((aError||iGAVDPStateMachine->State()==TGAVDPState::EGAVDPOpen),Panic(EA2dpBTHeadsetUnExpectedState)); //if there is no error then current state should be open
	
	//only have streamer and codec in streaming state
	delete iRTPStreamer;
	iRTPStreamer = NULL;
	delete iCodec;
	iCodec = NULL;
	
	if (iCloseDeviceRequestStatus)
		{//state change is in response to a close device
		if (*iCloseDeviceRequestStatus == KRequestPending)
			{
			User::RequestComplete(iCloseDeviceRequestStatus, aError);
			}
		}
	else if (iOpenDeviceRequestStatus)
		{
		//it must be a failed OpenDevice where the state has been wound back to EGAVDPOpen
		__ASSERT_DEBUG((iGAVDPStateMachine->State() == TGAVDPState::EGAVDPOpen),Panic(EA2dpBTHeadsetUnExpectedState));
		//the following ASSERT_DEBUG should be present but is commented out as the RVCT 
		//compiler generates a warning
		//__ASSERT_DEBUG((iOpenDeviceError), EA2dpBTHeadsetUnExpectedState);
		if (*iOpenDeviceRequestStatus == KRequestPending)
			{
			User::RequestComplete(iOpenDeviceRequestStatus, iOpenDeviceError);
			}
		}
	SetSniffMode(ETrue);
	}

/**
Callback by the GAVDP state machine when a GAVDP state change has been completed

@param aInitialState The TGAVDPState prior to the start of the state change
@param aCurrentState The current TGAVDPState of the GAVDP state machine
@param aError standard Symbian error code.  
*/
void CA2dpBTHeadsetAudioInterface::GAVDPStateChangeComplete(const TGAVDPState& aInitialState, TInt aError)
	{	
	switch(aInitialState.State())
		{
		case TGAVDPState::EGAVDPIdle:
			ProcessGAVDPStateChangeIdleToOpen(aError);
			break;
		case TGAVDPState::EGAVDPOpen:
			ProcessGAVDPStateChangeOpenToStreaming(aError);
			break;
		case TGAVDPState::EGAVDPStreaming:
			ProcessGAVDPStateChangeStreamingToOpen(aError);	
			break;
		default:
			Panic(EA2dpBTHeadsetUnExpectedState);
			break;
		}
	}


/**
Returns a list of the supported datatypes for audio that can be sent to the headset.
The list is obtained from the headset, so the headset need to have been Initialized first
	
@param  aSupportedDataTypes
The array of supported data types that will be filled in by this function.
The supported data types are in the form of an array
of TFourCC codes. Any existing entries in the array will be overwritten on 
calling this function. 
@return standard Symbian error code.   
KErrNotReady if headset has not been initialized.
*/
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::GetSupportedDataTypes(RArray<TFourCC>& aSupportedDataTypes) const
	{
	aSupportedDataTypes.Reset(); //clear any existing data in aSupportedDataTypes array
	TInt err = KErrNone;
	RArray<TUsableSEP>& usableSEPs = iGAVDPStateMachine->UsableSEPs();
	TUint numberOfUsableSEPS = usableSEPs.Count();
	if (numberOfUsableSEPS)
		{
		TUint i;
		TUsableSEP SEP;
		//iterate through the list of usable SEPs looking for one that supports
		//the requested data type
		for (i=0;  i<usableSEPs.Count(); i++)
			{
			SEP = usableSEPs[i];
			//both the headset and the A2dpBTHeadsetAudioInterface need to
			//support the datatype so check we also support the datatype
			//currently only SBC & MP3 data types are supported
			if ((SEP.iDataType == KMMFFourCCCodeSBC)||(SEP.iDataType == KMMFFourCCCodeMP3))
				{
				err = aSupportedDataTypes.Append(SEP.iDataType);
				}
			if (err)
				{
				break;
				}
			if (SEP.iDataType == KMMFFourCCCodeSBC)
				{//then we also support pcm16
				err = aSupportedDataTypes.Append(KMMFFourCCCodePCM16);
				if (err)
					{
					break;
					}
				}
			}
		}
	else if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPIdle)
		{//no usable SEPs available so can't yet get supported data types
		err = KErrNotReady;
		}
		
	if (err)
		{
		aSupportedDataTypes.Reset();
		}
	return err;
	}
	

/**
Returns the sample rates supported by the headset.
In the case of SBC the supported values are obtained from the headset.
In the case of mp3,AAC & ATRAC 3 the values are not obtained from
the headset as the underlying bluetoothav code does not have a defined structure
for containing these values, therefore the mandatory values are returned
	
@param  aSupportedDiscreteRates
The array of supported sample rates that will be filled in by this function.
The supported data types are in the form of an array
of TFourCC codes. Any existing entries in the array will be overwritten on 
calling this function. 
@param aSupportedRateRanges
To cover the case where headsets support a range of sample rates
@return standard Symbian error code.   
KErrNotReady if headset has not been initialized.
*/	
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::GetSupportedSampleRates(RArray<TUint>& aSupportedDiscreteRates, RArray<TRange>& aSupportedRateRanges) const
	{
	TInt error = KErrNone;
	aSupportedDiscreteRates.Reset();
	aSupportedRateRanges.Reset();
	
	//get the codec capabilites from the GAVDP state machine
	TAvdtpMediaCodecCapabilities* codecCaps = NULL;
	error = iGAVDPStateMachine->CodecCaps(codecCaps);

	if (!error)
		{
		error = TA2dpCodecCapabilityParser::GetSupportedSampleRates(*codecCaps,aSupportedDiscreteRates);
		}
	if (error)
		{
		aSupportedDiscreteRates.Reset();
		}
	
	return error;
	}
	

/**
Returns the number of channels supported by the headset.
In the case of SBC the supported values are obtained from the headset.
In the case of mp3,AAC & ATRAC 3 the values are not obtained from
the headset as the underlying bluetoothav code does not have a defined structure
for containing these values, therefore the mandatory values are returned
	
@param  aSupportedChannels
The array of supported number of channels that will be filled in by this function.
Any existing entries in the array will be overwritten on 
calling this function. 
@param aStereoSupport
Additional parameter to specifiy the stereo support
@return standard Symbian error code.
@see TMMFStereoSupport
KErrNotReady if headset has not been initialized.
*/		
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::GetSupportedChannels(RArray<TUint>& aSupportedChannels, TMMFStereoSupport& aStereoSupport) const
	{
	aSupportedChannels.Reset();
	aStereoSupport = EMMFNone;
	
	TInt error = KErrNone;

	//get the codec capabilites from the GAVDP state machine
	TAvdtpMediaCodecCapabilities* codecCaps = NULL;
	error = iGAVDPStateMachine->CodecCaps(codecCaps);
	
	if (!error)
		{
		error = TA2dpCodecCapabilityParser::GetSupportedChannels(*codecCaps, aSupportedChannels, aStereoSupport);
		}
	if (error) 
		{
		aSupportedChannels.Reset();
		aStereoSupport = EMMFNone;
		}
	return error;
	}
	

/**
Sets the data type of the data sent to the CA2dpBTHeadsetAudioInterface
Note that data type specified here may not be identical to the data type
sent to the headset and hence the data type used in the the a2dp codec configuration.  
ie if the CA2dpBTHeadsetAudioInterface data type set by this method is pcm16
then the data type sent to the headset is SBC.
note that this method just sets the internal iDatatype member
the datatype cannot be changed on the fly, but only gets changed
when the GAVDP state machine is reconfigured in CA2dpBTHeadsetAudioInterface::OpenDevice()

@param  aSinkFourCC
        The 4CC code of the data to be supplied to this class instance.
        
@return System wide error code indicating if the function call was successful.
		KErrNotReady if the headset is not initialized
		KErrInUse if the headset is already streaming
		KErrNotSupported if the datatype is not supported
*/
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::SetDataType(const TFourCC& aDataType)
	{
	TInt error = KErrNone;
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPIdle)
		{
		return KErrNotReady;
		}
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPStreaming)
		{
		return KErrInUse; //don't allow this while we're streaming audio
		}
		
	//the requested data type is a non standard data type
	//so first check if there is a SEP that supports it
	RArray<TUsableSEP>& usableSEPs = iGAVDPStateMachine->UsableSEPs();
	TUint i;
	TUsableSEP SEP;
	TBool dataTypeSupported = EFalse;
	//iterate through the list of usable SEPs looking for one that supports
	//the requested data type
	for (i=0; i<usableSEPs.Count(); i++)
		{
		SEP = usableSEPs[i];
		if (SEP.iDataType == aDataType)
			{//one of the usable SEPs supports the requested data type
			dataTypeSupported = ETrue;
			iDataType = aDataType;
			if (aDataType != iA2dpCodecConfiguration->HeadsetCodecDataType())
				{
				iA2dpCodecConfiguration->SetHeadsetCodecDataType(aDataType);
				iAudioSettingsHaveChanged = ETrue;
				}
			break;
			}
		else if ((SEP.iDataType == KMMFFourCCCodeSBC) && (aDataType == KMMFFourCCCodePCM16))
			{
			dataTypeSupported = ETrue;
			iDataType = aDataType;
			if (iA2dpCodecConfiguration->HeadsetCodecDataType()!= KMMFFourCCCodeSBC)
				{
				iA2dpCodecConfiguration->SetHeadsetCodecDataType(KMMFFourCCCodeSBC);
				iAudioSettingsHaveChanged = ETrue;
				}
			break;
			}		
		}
	if (!dataTypeSupported)
		{
		error = KErrNotSupported;
		}		
	return error;
	}
	

/**
Sets the sample rate 

@param aSampleRate
@return System wide error code indicating if the function call was successful.
		KErrNotReady if the headset is not initialized
		KErrInUse if the headset is already streaming
		KErrNotSupported if the sample rate is not supported
*/	
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::SetSampleRate(TUint aSampleRate)
	{
	TInt error = KErrNone;
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPIdle)
		{
		return KErrNotReady;
		}
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPStreaming)
		{
		//don't allow this while we're streaming audio
		return KErrInUse; 
		}
	
	//check that the sample rate is supported
	RArray<TUint> supportedDiscreteRates;
	RArray<TRange> supportedRateRanges;
	GetSupportedSampleRates(supportedDiscreteRates, supportedRateRanges);
	if (supportedDiscreteRates.Find(aSampleRate) == KErrNotFound)
		{
		//in theory we should iterate through the ranges as well, however
		//SBC only suports discrete values so just return KErrNotSupported
		error = KErrNotSupported;
		}
	else
		{
		if (aSampleRate != iA2dpCodecConfiguration->SampleRate())
			{
			iA2dpCodecConfiguration->SetSampleRate(aSampleRate);
			iAudioSettingsHaveChanged = ETrue;
			}
		}
	supportedDiscreteRates.Close();
	supportedRateRanges.Close();
	return error;
	}
	

/**
Sets the number of channels 

@param aSampleChannels
@param aStereoSupport.  note that the aStereoSupport in the case of pcm16 refers to the output to the headset.
the data going for pcm16 is always either interleaved stereo pcm16 or mono
the aStereoSupport parameter is ignored for non stereo
@see TMMFStereoSupport
@return System wide error code indicating if the function call was successful.
		KErrNotReady if the headset is not initialized
		KErrInUse if the headset is already streaming
		KErrNotSupported if the datatype is not supported
*/	
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::SetChannels(TUint aChannels, TMMFStereoSupport aStereoSupport)
	{
	TInt error = KErrNone;
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPIdle)
		{
		return KErrNotReady;
		}
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPStreaming)
		{
		return KErrInUse; //don't allow this while we're streaming audio
		}

	//check that the number of channels is supported
	RArray<TUint> supportedChannels;
	TMMFStereoSupport stereoSupport;
	GetSupportedChannels(supportedChannels, stereoSupport);
	if (supportedChannels.Find(aChannels) == KErrNotFound)
		{
		error = KErrNotSupported;
		}	
	else if (aChannels == EMMFStereo)
		{
		//now check stereo support
		if ((aStereoSupport & stereoSupport) != aStereoSupport)
			{
			error = KErrNotSupported;
			}
		else if (iA2dpCodecConfiguration->StereoSupport() != aStereoSupport)
			{
			iA2dpCodecConfiguration->SetStereoSupport(aStereoSupport);
			iAudioSettingsHaveChanged = ETrue;
			}
		}
	if (!error)
		{
		if (iA2dpCodecConfiguration->Channels() != aChannels)
			{
			iA2dpCodecConfiguration->SetChannels(aChannels);
			iAudioSettingsHaveChanged = ETrue;
			}
		}
	return error;
	}

	
/**
Procedure to gets the headset into the GAVDP Streaming state where 
it is ready to accept audio data.  The procedure causes a reconfiguration
of the headset incase the settings have changed - and if they haven't then the 
reconfigure does nothing.
The GAVDPStateChangeComplete callback is called after the reconfigure
where it puts the GAVDP state machine in the streaming state
The GAVDPStateChangeComplete callback is called again when the headset device
is in the streaming state.

@param aStatus
*/
EXPORT_C void CA2dpBTHeadsetAudioInterface::OpenDevice(TRequestStatus& aStatus)
	{
	iOpenDeviceRequestStatus = &aStatus;
	*iOpenDeviceRequestStatus = KRequestPending;
	
	if (iGAVDPStateMachine->State() == TGAVDPState::EGAVDPStreaming)
		{
		//device is already open
		User::RequestComplete(iOpenDeviceRequestStatus, KErrNone);
		return;
		}
	
	//this isn't a local error variable as it is used in the GAVDPStateChangeComplete callback
	iOpenDeviceError = KErrNone;
	//take out of sniff mode - no point in checking error code as there is
	//not much that can be done
	SetSniffMode(EFalse);
	
	//first reconfigure if the configuration has changed
	//note that there is a subtle difference between the configuration of the 
	//CA2dpBTHeadsetAudioInterface and that of the GAVDP state machine
	//the configuration of the CA2dpBTHeadsetAudioInterface is the configuration
	//seen from the DevSound whereas the CGAVDPStateMachine is the configuration
	//as seen by the headset.  In most cases they would be identical, the only
	//current case where the differ is in the data type where the DevSound sees
	//the data tpye as pcm16 but the headset is SBC
	iOpenDeviceError = iGAVDPStateMachine->Reconfigure(iAudioSettingsHaveChanged);
	if (iOpenDeviceError)
		{
		User::RequestComplete(iOpenDeviceRequestStatus, iOpenDeviceError);
		}
	else
		{
		//the Reconfigure will result in a GAVDPStateChangeComplete callback
		iReconfiguring = ETrue;
		}
	}


/**
 Procedure to cancel a GAVDP initialization sequence on an A2DP headset
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::CancelOpenDevice()
	{
	if (iOpenDeviceRequestStatus)
		{
		if (*iOpenDeviceRequestStatus == KRequestPending)//make sure there is a pending request to cancel
			{
			iGAVDPStateMachine->CancelChangeState();
			}
		}
	}

/**
Procedure to put a headset that is in the GAVDP Streaming state back into
the GAVDP Open state
The GAVDPStateChangeComplete callback is called again when the headset device
is in the open state.

@param aStatus
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::CloseDevice(TRequestStatus& aStatus)
	{
	iCloseDeviceRequestStatus = &aStatus;
	*iCloseDeviceRequestStatus = KRequestPending;
	
	if (iGAVDPStateMachine->State() != TGAVDPState::EGAVDPStreaming)
		{
		//device isn't open
		User::RequestComplete(iCloseDeviceRequestStatus, KErrNone);
		return;
		}
	else
		{
		if (iRTPStreamer)
			{
			iRTPStreamer->Pause();
			}
		//note the callback ProcessGAVDPStateChangeStreamingToOpen
		//will put the BT link back in sniff mode
		TGAVDPState state(TGAVDPState::EGAVDPOpen);
		TInt err = iGAVDPStateMachine->ChangeState(state);
		if (err)
			{
			User::RequestComplete(iCloseDeviceRequestStatus, err);
			}
		}
	}

/**
Procedure to get the headset volume

@return volume in the range 0-255
*/	
EXPORT_C TUint CA2dpBTHeadsetAudioInterface::Volume() const
	{
	return 0;
	}
	

/**
Procedure to set the volume in the range 0-255
Note this procedure requires the AVRCP bluettooth profile
in order to change the volume which is not implemented

@param aVolume the volume in the range 0-255
*/	
EXPORT_C TInt CA2dpBTHeadsetAudioInterface::SetVolume(TUint /*aVolume*/)
	{
	return KErrNone;
	}
	

/**
This function causes the CA2dpBTHeadsetAudioInterface to send the audio data contained in aData to the initialized A2DP headset.
The data is buffered internally and CA2dpBTHeadsetAudioInterface may already be playing audio
when this method is called.
The TRequestStatus is completed when the CA2dpBTHeadsetAudioInterface  has finished with the audio data contained in aData.  
By 'finished' this does not mean finished as in physically finished playing the audio,
but to inform the client that:
a)	it can repopulate the buffer with new audio data.
b)	the  CA2dpBTHeadsetAudioInterface is ready to accept another buffer of audio data via a further call to PlayData()
(ie the CA2dpBTHeadsetAudioInterface is not expected to accept multiple buffers in succession prior to completing their
TRequestStatus as this would not provide the client with any indication of whether the
sound drivers internal buffers are full).

@param aStatus  The TRequestStatus completed when the sound driver has finished with the audio data contained in aData.
The TRequestStatus is set to KErrNone unless the request has been cancelled via CancelPlayData()
in which case the status will be set to KErrCancel. 
The TRequestStatus is completed as soon as possible and before the audio has been physically played
in order for the client to send further audio buffers before the buffers already in the sound driver
are played out. 
Prior to calling PlayData() the client would be expected to Initialize Configure and Open the CA2dpBTHeadsetAudioInterface
and register with the sound driver that it requires notification of error conditions via a call to NotifyPlayError().
	
@param aData  Buffer containing the audio data to be played.  
aData, is not owned or created by the sound driver but is owned by the client.  
*/
EXPORT_C void CA2dpBTHeadsetAudioInterface::PlayData(const TDesC8& aData, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG((aData.Length()), Panic(EA2dpBTHeadsetNoData));
	__ASSERT_DEBUG((iRTPStreamer), Panic(EA2dpBTHeadsetNoRTPStreamer));
	
	iRTPStreamer->Send(aData, aStatus);
	}
	
/**
Cancels the current request for PlayData() if currently active.
The currently active PlayData() completes the request status immediately with KErrCancel.
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::CancelPlayData()
	{
	__ASSERT_DEBUG((iRTPStreamer), Panic(EA2dpBTHeadsetNoRTPStreamer));
		
	iRTPStreamer->CancelLastSendBuffer();
	}
	

/**
Used to clear all internally buffered audio.  
This function does not complete any pending TRequestStatuses.  
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::FlushBuffer()
	{
	__ASSERT_DEBUG((iRTPStreamer), Panic(EA2dpBTHeadsetNoRTPStreamer));
	
	iRTPStreamer->FlushPendingSendBuffers();
	}

	
/**
Returns the total number of bytes of data sent in PlayData that have been
sent to the headset.  ie the number of bytes sent prior to any encoding by the SBC codec.
It is updated after each buffer has been sent to the headset. 
The BytesPlayed are not reset to 0 on an error condition.
It must be explicitly reset via a call to ResetBytesPlayed().
	
@param The number of bytes played
*/	
EXPORT_C TUint CA2dpBTHeadsetAudioInterface::BytesPlayed() const
	{
	TUint bytesPlayed = 0;
	if (iRTPStreamer)//don't ASSERT iRTPStreamer here as this API still has meaning with no streamer
		{
		bytesPlayed = iRTPStreamer->BytesSent();
		}
	return bytesPlayed;
	}


/**
Resets bytes played to 0.
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::ResetBytesPlayed()
	{
	if (iRTPStreamer)//don't ASSERT iRTPStreamer here as this API still has meaning with no streamer
		{
		iRTPStreamer->ResetBytesSent();
		}
	}
	

/**
Function to halt the sending of buffers to the headset.
Note that even if no buffers are being send via PlayData buffers
can still be sent to the headset as they are buffered internally
Calling PauseBuffer stops the internally buffered data being sent as well
as any buffers sent via PlayData
If the TRequestStatus from the previous PlayData() is outstanding then PausePlayBuffer()
does not complete the TRequestStatus.
The TRequestStatus will only be completed after ResumePlaying() is called.
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::PauseBuffer()
	{
	__ASSERT_DEBUG((iRTPStreamer), Panic(EA2dpBTHeadsetNoRTPStreamer));
	
	iRTPStreamer->Pause();
	iPaused = ETrue;
	}

	
/**
This resumes playing from where playing was halted from the previous call to PauseBuffer().
The next TRequestStatus (if active) to be completed, is the request status from the previous PlayData()
prior to calling PausePlayBuffer().
If PausePlayBuffer() has not been called prior to ResumePlaying(),
then this method does nothing.
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::ResumePlaying()
	{
	__ASSERT_DEBUG((iRTPStreamer), Panic(EA2dpBTHeadsetNoRTPStreamer));
	
	iRTPStreamer->Resume();
	iPaused = EFalse;
	}
	

/**
This function is used when the client is requesting notification of an error that occurs during playback.
The TRequestStatus passed in the NotifyPlayError() method is separate to the TRequestStatus passed into the PlayData() method,
and hence the client would typically have a separate active object for both the PlayData()
and NotifyPlayError() TRequestStatuses.
The NotifyPlayError() method is required because if an error occurs during playback,
the PlayData() TRequestStatus may not be active.
If a PlayData() request status is active then the error is reported back via the PlayData()
TRequestStatus and not via the NotifyPlayError() TRequestStatus.
If no PlayData() TRequestStatus is active then the error is reported back by completing the NotifyPlayError()
TRequestStatus with the appropriate error code.
Note that a call to CancelPlayData() should complete the PlayData() request status with KErrCancel
and should not complete the NotifyPlayError() TRequestStatus.

@param aStatus TRequestStatus completed when a play error occurs.
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::NotifyError(TRequestStatus& aStatus)
	{
	iNotifyRequestStatus= &aStatus;
	*iNotifyRequestStatus = KRequestPending;
	}
	

/**
Cancels the NotifyPlayError() TRequestStatus if currently active.
The TRequestStatus completes immediately with KErrCancel.
*/	
EXPORT_C void CA2dpBTHeadsetAudioInterface::CancelNotifyError()
	{
	User::RequestComplete(iNotifyRequestStatus, KErrCancel);
	}


/**
Callback from the RTPStreamer when an unexpected event occurs

@param aError
*/
void CA2dpBTHeadsetAudioInterface::RTPStreamerEvent(TInt aError)
	{
	//an error has occured while streaming
	__ASSERT_DEBUG((iRTPStreamer), Panic(EA2dpBTHeadsetNoRTPStreamer));
	//could also be EGAVDPIdle as we could have just received a GAVDPStateMachineEvent
	//which would reset the state to EGAVDPIdle
	__ASSERT_DEBUG((iGAVDPStateMachine->State() == TGAVDPState::EGAVDPStreaming) ||
	(iGAVDPStateMachine->State() == TGAVDPState::EGAVDPIdle), Panic(EA2dpBTHeadsetUnExpectedState));
	TGAVDPState state(TGAVDPState::EGAVDPIdle);
	TInt err = iGAVDPStateMachine->ChangeState(state);
	//the above returns an error code but there's not much we can do if it returns an error

	if (iNotifyRequestStatus)
		{
		if (*iNotifyRequestStatus == KRequestPending)
			{
			User::RequestComplete(iNotifyRequestStatus, aError);
			}
		}
	}


/**
Callback from the GAVDPStateMachine when an error event occurs

@param aError
*/	
void  CA2dpBTHeadsetAudioInterface::GAVDPStateMachineEvent(TInt aError)
	{
	//an error has occured - if an error occurs then it could be
	//because we've lost the connection to the headset eg because it is out of range
	//has been switched off etc there we assume that the connection has been
	//lost and put the GAVDP state back to idle
	//note we're simplifying things somewhat as some errors may not be a result 
	//of a loss of connection to the headset
	TGAVDPState state(TGAVDPState::EGAVDPIdle);
	TInt err = iGAVDPStateMachine->ChangeState(state);
	if (!iRTPStreamer)
		{//if we don't have an RTP streamer
		//then close the bearer socket.  If we do have an RTP streamer
		//then delay the close till after we have deleted the RTPStreamer in the
		//the GAVDPStateChangeComplete callback.  this is because
		//closing the bearer socket before the RTP streamer is deleted
		//can panic the RTP stack.
		iGAVDPStateMachine->BearerSocket().Close();
		}	
	if (iNotifyRequestStatus)
		{
		if (*iNotifyRequestStatus == KRequestPending)
			{
			User::RequestComplete(iNotifyRequestStatus, aError);
			}
		}
	}

	
/**
Callback from the GAVDPStateMachine when the headset has suspended the stream
*/
void  CA2dpBTHeadsetAudioInterface::GAVDPStateMachineStreamSuspendedByRemoteHeadset()
	{
	if (iRTPStreamer)
		{
		iRTPStreamer->Pause();
		}
	}


/**
Callback from the GAVDPStateMachine when the headset has resumed a suspended stream
*/	
void  CA2dpBTHeadsetAudioInterface::GAVDPStateMachineStreamResumedByRemoteHeadset()
	{
	if ((!iPaused)&&(iRTPStreamer))
		{
		iRTPStreamer->Resume();
		}
	}


/**
Callback from the GAVDPStateMachine when the headset has reconfigured the parameters
reconfigures codec with new configuration and inform RTP streamer

@return SymbianOS error code
*/
TInt  CA2dpBTHeadsetAudioInterface::GAVDPStateMachineReconfigureByRemoteHeadset()
	{
	//we're only allowing the headset to reconfigure for SBC SEP
	__ASSERT_DEBUG(iA2dpCodecConfiguration->HeadsetCodecDataType() == KMMFFourCCCodeSBC, Panic(	EA2dpBTHeadsetUnexpectedDataType));
	__ASSERT_DEBUG(iCodec,Panic(EA2dpBTHeadsetNoCodec));
	TUid configType;
	TInt err = KErrNone;
	configType.iUid = KMmfUidSBCConfigure;
	TSBCFrameParameters& SBCFrameParameters = iA2dpCodecConfiguration->UpdateLocalSBCCodecConfiguration();
	TPckgBuf<TSBCFrameParameters> SBCFrameParametersBuf(SBCFrameParameters);
	TRAP(err,iCodec->ConfigureL(configType, SBCFrameParametersBuf)); //update codec with new configuration
	iRTPStreamer->SetAudioConfiguration(*iA2dpCodecConfiguration); //and tell RTP streamer	
	return err;
	}


/**
Function to set/release the BT link into low power sniff mode

@internalComponent
@return Error code, this is used for debugging, it's not checked
by the calling code as if SetSniffMode fails there's not much that can be
done in the calling code
*/
TInt CA2dpBTHeadsetAudioInterface::SetSniffMode(TBool aSniffMode)
	{
	TInt err = KErrNone;
	
	if (!iGAVDPStateMachine)
		{
		return KErrNotReady;
		}
		
	if (iGAVDPStateMachine->BTAddress() == TBTDevAddr(0))
		{
		return KErrNotReady;
		}

	err = iBTPhysicalLinkAdapter.Open(iSocketServer, iGAVDPStateMachine->BTAddress());
	if (!err)
		{
		if (aSniffMode)
			{
			err = iBTPhysicalLinkAdapter.AllowLowPowerModes(ESniffMode);
			err = iBTPhysicalLinkAdapter.ActivateSniffRequester();
			}
		else
			{
			err = iBTPhysicalLinkAdapter.CancelLowPowerModeRequester();
			}
		iBTPhysicalLinkAdapter.Close();
		}

	return err;
	}
	

/**
This function should only used by the TSU_MMF_A2DPBLUETOOTH unit test harness
It allows the test harness to get at the MGavdpUser in order
to emulate actions from the headset

@return the GavdpState machine as a MGavdpUser mixin
*/	
EXPORT_C MGavdpUser* CA2dpBTHeadsetAudioInterface::TEST_MGavdpUser()
	{
	return static_cast<MGavdpUser*>(iGAVDPStateMachine);
	}
	

/**
This function should only used by the TSU_MMF_A2DPBLUETOOTH unit test harness
It allows the test harness to override the configuration of the remote headset
for the SBC codec with regard to sampling frequencies, block size; subbands and allocation method

@param
*/
EXPORT_C void CA2dpBTHeadsetAudioInterface::TEST_ForceRemoteSBCCodecConfiguration(const TSBCCodecCapabilities& aRemoteCodecConfiguration)
	{
	iA2dpCodecConfiguration->TEST_ForceRemoteSBCCodecConfiguration(aRemoteCodecConfiguration);
	}
