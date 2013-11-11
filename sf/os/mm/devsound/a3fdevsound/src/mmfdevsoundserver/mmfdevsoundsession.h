// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFDEVSOUNDSESSION_H
#define MMFDEVSOUNDSESSION_H

#include <e32base.h>
#include <e32std.h>
#include <e32msgqueue.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <mmf/server/sounddevice.h>
#include <a3f/mmfdevsoundcustominterfaceextensions.h>
#include "mmfaudioclientserver.h"
#include "mmfdevsoundadaptation.h"

#include <mmf/server/mmfdevsoundcustominterface.h>
#include "MmfDevSoundCIDeMuxUtility.h"
#include <mmf/common/mcustominterface.h>

// FORWARD DECLARATIONS
class CMMFDevSoundServer;
class CMMFDevSoundAdaptation;
class MGlobalProperties;

/**
Panic category and codes that MMFDevSoundSession raises on the client
*/
_LIT(KMMFDevSoundSessionPanicCategory, "MMFDevSoundSession");

enum TMMFDevSoundSessionPanicCodes
	{
	EMsgQueueFailedToSendMsg =1,
	EQueueRequestsFailedToAppend,
	EUnexpectedAsyncOpCompleteHandlingCI,
	ERequestBeingServicedMismatch
	};

// CLASS DECLARATION

/*
	This class is wrapper for RMmfIpcMessage
	The main purpose of this class is to hold a messages and 
	keep the status of the process started with the request
	This is not related to message status itself (if was completed or not)
	
	@lib MmfDevSoundServer.lib
	@since 
*/
class TMMFDevSoundRequest
	{
public:

	enum TA3FDevSoundRequestType
		{ 
		EQuery_Synchronous,			// Message completed immediately, doesn't involve commit cycle, can be serviced even when a commit operation is pending
		EQuery_Asynchronous,		// Message is not completed until operation is completed, doesn't involve commit cycle
		EConfigure_Synchronous,		// Message completed immediately, doesn't involve commit cycle, can be serviced even when a commit operation is pending
		EConfigure_Asynchronous,	// Message is not completed until operation is completed, cannot be serviced when a commit operation is pending
		EAction_Asynchronous,		// Message is not completed until operation is completed, cannot be serviced when a commit operation is pending
		EAction_PseudoAsynchronous,	// Message is completed, but all the incoming asynchronous request that involves commit are enqueued 
		EOther_Synchronous,			// Message completed immediately, doesn't involve commit cycle, can be serviced even when a commit operation is pending
		EBufferExchangeRelated,		// Message completed immediately, doesn't involve commit cycle, can be serviced even when a commit operation is pending
		ECustomInterfacesRelated,	// Message completed immediately, doesn't involve commit cycle, can be serviced even when a commit operation is pending
		EUndefinedType,
		ECallBackType,
		ESessionEvents,
		};	

public:

	TMMFDevSoundRequest();
	TMMFDevSoundRequest(TInt aIsCallBack);
	TMMFDevSoundRequest(const TMMFDevSoundRequest& aRequest);
	void SetMessage(const RMmfIpcMessage& aMessage);
	void SetMessageCallback();
	TBool operator==(const TMMFDevSoundRequest& aRequest) const;
	const RMmfIpcMessage& Message();
	TInt Function() const;

	TA3FDevSoundRequestType Type() const;
	TInt IsCallBack() const;

	void Complete(TInt aError);

private:
	TA3FDevSoundRequestType ResolveType();

private:
	TBool			iMessageCompleted;

	/**	
	The encapsulated request message	
	*/
	RMmfIpcMessage iMessage;

	/**
	The classification for this request
	*/
	TA3FDevSoundRequestType		iRequestType;

	TInt			iCallBackPF;
	};

// CLASS DECLARATION

/*
	Server-side session implementation.
	This class handles all the request from RMmfDevSoundProxy.
	This classes uses MmfDevSoundAdaptation library and forwards all the reqeusts
	to it. The main purpose of this class is to handle client server
	communication.
	
	@lib MmfDevSoundServer.lib
	@since 
*/
class CMMFDevSoundSession : public CMmfIpcSession,
							public MDevSoundAdaptationObserver,
							public MMMFDevSoundCustomInterfaceDeMuxInterface,
							public MCustomInterface,
							public MMMFDevSoundCustomInterfaceTarget
							
	{
			
public:  // Constructors and destructor	

	/**
		Constructs, and returns a pointer to, a new CMMFDevSoundSession
		object.
		Leaves on failure.
		@param MGlobalProperties a reference to global properties such as FourCC to format converter
		@return CMMFDevSoundSession* A pointer to newly created object.
	*/
	static CMMFDevSoundSession* NewL(MGlobalProperties& aGlobalProperties);

	/**
		Destructor.
	*/
	~CMMFDevSoundSession();

public: // New functions

	/**
		Called by Client/Server framework when a new session is created.
		Leaves on failure.
		@since 
		@param const CMmfIpcServer& aServer A constant reference to server
			creating this session.
		@return void
	*/
	void CreateL(const CMmfIpcServer& aServer);

	/**
		Called by Client/Server framework when service request is made by the
		corresponding RMmfDevSoundProxy.object.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return void
	*/
	void ServiceL(const RMmfIpcMessage& aMessage);

	/**
		Function to handle DevSound session id request.
		@since 
		@return TInt A integer assigned to this DevSound server.
	*/
	TInt DevSoundSessionId() {return iDevSoundSessionId;};

	// TODO: Review if the parameter should be kept or not
	void SendEventToClient(/*TMMFAudioServerEvent& aEvent*/);

	/**
		Method to service PostOpen request - 2nd phase async open.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPostOpenL(const RMmfIpcMessage& aMessage);

	/**
		Method to service Initialize1L request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoInitialize1L(const RMmfIpcMessage& aMessage);

	/**
		Method to service Initialize1L request that has already completed, but
		not finished due to a pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedInitialize1L();

	/**
		Method to service Initialize2L request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoInitialize2L(const RMmfIpcMessage& aMessage);

	/**
		Method to service Initialize2L request that has already completed, but
		not finished due to a pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedInitialize2L();

	/**
		Method to service Initialize3L request.
		Leaves on failure.
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoInitialize3L(const RMmfIpcMessage& aMessage);
	/**
		Method to service Initialize4L request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoInitialize4L(const RMmfIpcMessage& aMessage);
	
	/**
		Method to service Initialize4L request that has already completed, but
		not finished due to a pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedInitialize4L();

	/**
		Method to service CancelInitialize request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoCancelInitializeL(const RMmfIpcMessage& aMessage);

	/**
		Method to service capabilities query request from
		RMmfDevSoundProxy.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoCapabilitiesL(const RMmfIpcMessage& aMessage);

	/**
		Method to service current configuration query request from
		RMmfDevSoundProxy.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoConfigL(const RMmfIpcMessage& aMessage);

	/**
		Method to service set current configuration request from
		RMmfDevSoundProxy.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetConfigL(const RMmfIpcMessage& aMessage);

	/**
		Method to service maximum volume query request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoMaxVolumeL(const RMmfIpcMessage& aMessage);

	/**
		Method to service current volume level query request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoVolumeL(const RMmfIpcMessage& aMessage);

	/**
		Method to service set current volume level request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetVolumeL(const RMmfIpcMessage& aMessage);

	/**
		Method to service maximum gain query request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoMaxGainL(const RMmfIpcMessage& aMessage);

	/**
		Method to service current gain level query request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoGainL(const RMmfIpcMessage& aMessage);

	/**
		Method to service set current gain level request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetGainL(const RMmfIpcMessage& aMessage);

	/**
		Method to service current play balance level query request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoGetPlayBalanceL(const RMmfIpcMessage& aMessage);

	/**
		Method to service set current play balance level request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetPlayBalanceL(const RMmfIpcMessage& aMessage);

	/**
		Method to service current record balance level query request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoGetRecordBalanceL(const RMmfIpcMessage& aMessage);

	/**
		Method to service set current record balance level request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetRecordBalanceL(const RMmfIpcMessage& aMessage);

	/**
		Method to service initialize DevSound to play request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayInitL(const RMmfIpcMessage& aMessage);

	/**
		Method to service initialize DevSound to play request that has already completed,
		but not finished due to a pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedPlayInitL();

	/**
		Method to service initialize DevSound to record request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoRecordInitL(const RMmfIpcMessage& aMessage);

	/**
		Method to service initialize DevSound to record request that has already completed,
		but not finished due to a pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedRecordInitL();

	/**
		Method to service signal DevSound to playing current buffer request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayDataL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to continue record request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoRecordDataL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to stop ongoing operation request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoStopL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to temporarily stop ongoing
		operation request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPauseL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to play simple tone  operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayToneL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to play simple tone operation
		request that has already completed, but not finished due to a
		pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedPlayToneL();

	/**
		Method to service signal DevSound to play dual tone  operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayDualToneL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to play dual tone operation
		request that has already completed, but not finished due to a
		pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedPlayDualToneL();

	/**
		Method to service signal DevSound to play DTMFString operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayDTMFStringL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to play DTMFString operation
		request that has already completed, but not finished due to a
		pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedPlayDTMFStringL();

	/**
		Method to service signal DevSound to play tone sequence operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayToneSequenceL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to play tone sequence operation
		request that has already completed, but not finished due to a
		pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedPlayToneSequenceL();

	/**
		Method to service signal DevSound to play fixed sequence operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPlayFixedSequenceL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to play fixed sequence operation
		request that has already completed, but not finished due to a
		pre-emption clash during its commit cycle(s).
		Leaves on failure.
		@return void
	*/
	void DoAlreadyCompletedPlayFixedSequenceL();

	/**
		Method to service signal DevSound to initilize DTMF String operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetDTMFLengthsL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to set volume ramp operation
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetVolumeRampL(const RMmfIpcMessage& aMessage);

	/**
		Method to service query DevSound to return supported input data types
		operation request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoGetSupportedInputDataTypesL(const RMmfIpcMessage& aMessage);

	/**
		Method to service query DevSound to copy FourCC array data
		operation request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoCopyFourCCArrayDataL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querying samples recorded so far.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSamplesRecordedL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querying samples played so far.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSamplesPlayedL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request to set tone repeats.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetToneRepeatsL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request to set priority settings.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoSetPrioritySettingsL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querrying fixed sequence name.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoFixedSequenceNameL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querrying fixed sequence count.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoFixedSequenceCountL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querrying supported output data types.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoGetSupportedOutputDataTypesL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querrying specifics of the TBF data
			 buffer.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoBufferToBeFilledDataL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querrying specifics of the TBE data
			buffer.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoBufferToBeEmptiedDataL(const RMmfIpcMessage& aMessage);

	/**

		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoEmptyBuffersL(const RMmfIpcMessage& aMessage);

	/**
		Method to service the request querrying preparation for close session
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoPrepareCloseL(const RMmfIpcMessage& aMessage);

	
	TBool DoRegisterAsClientL(const RMmfIpcMessage& aMessage);
	TBool DoCancelRegisterAsClientL(const RMmfIpcMessage& aMessage);
	TBool DoGetResourceNotificationDataL(const RMmfIpcMessage& aMessage);
	TBool DoWillResumePlayL(const RMmfIpcMessage& aMessage);
	TBool DoSetClientThreadInfoL(const RMmfIpcMessage& aMessage);

	
	/**
		Method to get the current play time from the audio renderer
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoGetTimePlayedL(const RMmfIpcMessage& aMessage);
	
	/**
		Method to query if True Pause is supported by current codec format
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoQueryResumeSupportedL(const RMmfIpcMessage& aMessage);

	/**
		Method to service signal DevSound to resume playback, recording or tone playing
		request.
		Leaves on failure.
		@since 
		@param const RMmfIpcMessage& aMessage A reference to message object
			containing request attributes.
		@return ETrue if the request is serviced completely else EFalse.
	*/
	TBool DoResumeL(const RMmfIpcMessage& aMessage);

	/**
		Initializes to raw audio data PCM16 and Sampling Rate of 8 KHz.
		On completion of Initialization, calls InitializeComplete() on
		aDevSoundObserver.
		Leaves on failure.
		@since 
		@param MDevSoundAdaptationObserver& aDevSoundObserver A reference to DevSound
			Observer instance.
		@param TMMFState aMode Mode for which this object will be used.
		@return void
	*/
	void InitializeL(MDevSoundAdaptationObserver& aDevSoundObserver,
									TMMFState aMode);

	/**
		Initializes DevSound object for the mode aMode for processing audio
		data with hardware device aHWDev.
		On completion of Initialization, calls InitializeComplete() on
		aDevSoundObserver.
		Leaves on failure.
		@since 
		@param MDevSoundAdaptationObserver& aDevSoundObserver A reference to DevSound
			Observer instance.
		@param TUid aHWDev The CMMFHwDevice implementation identifier.
		@param TMMFState aMode The mode for which this object will be used
		@return void
	*/
	void InitializeL(MDevSoundAdaptationObserver& aDevSoundObserver,
					TUid aHWDev,
					TMMFState aMode);

	/**
		Initializes DevSound object for the mode aMode for processing audio
		data with hardware device supporting FourCC aDesiredFourCC.
		Leaves on failure.
		@since 
		@param MDevSoundAdaptationObserver& aDevSoundObserver A reference to
			the DevSound adaptation observer instance.
		@param TFourCC aDesiredFourCC The CMMFHwDevice implementation FourCC
			code.
		@param TMMFState aMode The mode for which this object will be used
		@return KErrNone if successfull, else corresponding error code
		@return void
	*/
	void InitializeL(MDevSoundAdaptationObserver& aDevSoundObserver,
					TFourCC aDesiredFourCC,
					TMMFState aMode);

	/**
		Returns the supported Audio settings ie. encoding, sample rates,
		mono/stereo operation, buffer size etc..
		@since 
		@return TMMFCapabilities The device settings.
	*/
	TMMFCapabilities Capabilities();

	/**
		Returns the current device configuration.
		@since 
		@return TMMFCapabilities The device settings.
	*/
	TMMFCapabilities Config() const;

	/**
		Configure CMMFDevSound object with the settings in aConfig. Use this
		to set sampling rate, encoding and mono/stereo.
		Leaves on failure.
		@since 
		@param const TMMFCapabilities& aConfig The attribute values to which
			CMMFDevSound object will be configured to.
		@return void
	*/
	void SetConfigL(const TMMFCapabilities& aCaps);

	/**
		Returns an integer representing the maximum volume device supports.
		This is the maximum value which can be passed to
		CMMFDevSound::SetVolume.
		@since 
		@return TInt The maximum volume. This value is platform dependent but
			is always greater than or equal to one.
	*/
	TInt MaxVolume();

	/**
		Returns an integer representing the current volume.
		@since 
		@return TInt The current volume level.
	*/
	TInt Volume();

	/**
		Changes the current playback volume to a specified value. The volume
		can be changed before or during playback and is effective immediately.
		@since 
		@param TInt aVolume The volume setting. This can be any value from 0
			to the value returned by a call to
			CMMFDevSound::MaxVolume(). If the volume is not
			within this range, the volume is automatically set
			to minimum or maximum value based on the value
			that is being passed. Setting a zero value mutes
			the sound. Setting the maximum value results in
			the loudest possible sound.
		@return void
	*/
	void SetVolume(TInt aVolume);

	/**
		Returns an integer representing the maximum gain the device supports.
		This is the maximum value which can be passed to CMMFDevSound::SetGain
		@since 
		@return TInt The maximum gain. This value is platform dependent but is
			always greater than or equal to one.
	*/
	TInt MaxGain();

	/**
		Returns an integer representing the current gain.
		@since 
		@return TInt The current gain level.
	*/
	TInt Gain();

	/**
		Changes the current recording gain to a specified value. The gain can
		be changed before or during recording and is effective immediately.
		@since 
		@param TInt aGain The gain setting. This can be any value from zero to
			the value returned by a call to
			CMMFDevSound::MaxGain(). If the volume
			is not within this range, the gain is automatically
			set to minimum or maximum value based on the value
			that is being passed. Setting a zero value mutes the
			sound. Setting the maximum value results in the
			loudest possible sound.
		@return void
	*/
	void SetGain(TInt aGain);

	/**
		Returns the speaker balance set for playing.
		Leaves on failure.
		@since 
		@param TInt &aLeftPercentage On return contains the left speaker
			volume percentage.
		@param TInt &aRightPercentage On return contains the right speaker
			volume percentage.
		@return void
	*/
	void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
		Sets the speaker balance for playing. The speaker balance can be
		changed before or during playback and is effective immediately.
		Leaves on failure.
		@since 
		@param TInt aLeftPercentage The left speaker volume percentage. This
			can be any value from zero to 100. Setting
			a zero value mutes the sound on left
			speaker.
		@param TInt aRightPercentage The right speaker volume percentage.
			This can be any value from zero to 100.
			Setting a zero value mutes the sound on
			right speaker.
		@return void
	*/
	void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage);

	/**
		Returns the microphone gain balance set for recording.
		Leaves on failure.
		@since 
		@param TInt &aLeftPercentage On return contains the left microphone
			gain percentage.
		@param TInt &aRightPercentage On return contains the right microphone
			 gain percentage.
		@return void
	*/
	void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	/**
		Sets the microphone balance for recording. The microphone balance can
		be changed before or during recording and is effective immediately.
		Leaves on failure.
		@since 
		@param TInt aLeftPercentage The left microphone gain percentage. This
		can be any value from zero to 100. Setting
			a zero value mutes the sound from left
			microphone.
		@param TInt aRightPercentage The right microphone gain percentage.
			This can be any value from zero to 100.
			Setting a zero value mutes the sound from
			right microphone.
		@return void
	*/
	void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage);

	/**
		Initializes the audio device and starts the play process. This
		function queries and acquires the audio policy before initializing
		audio device. If there was an error during policy initialization,
		PlayError() function will be called on the observer with error code
		KErrAccessDenied, otherwise BufferToBeFilled() function will be called
		with a buffer reference. After reading data into the buffer reference
		passed, the client should call PlayData() to play data.
		The amount of data that can be played is specified in
		CMMFBuffer::RequestSize(). Any data that is read into buffer beyond
		this size will be ignored.
		Leaves on failure.
		@since 
		@return void
	*/
	void PlayInitL();

	/**
		Initializes the audio device and starts the record process. This
		function queries and acquires the audio policy before initializing
		audio device. If there was an error during policy initialization,
		RecordError() function will be called on the observer with error code
		KErrAccessDenied, otherwise BufferToBeEmptied() function will be called
		with a buffer reference. This buffer contains recorded or encoded
		data. After processing data in the buffer reference passed, the client
		should call RecordData() to continue recording process.
		The amount of data that is available is specified in
		CMMFBuffer::RequestSize().
		Leaves on failure.
		@since 
		@return void
	*/
	void RecordInitL();

	/**
		Plays data in the buffer at the current volume.
		The client should fill the buffer with audio data before calling this
		function. The observer gets a reference to the buffer along with the
		callback function BufferToBeFilled(). When playing of the audio sample
		is complete, successfully or otherwise, the function PlayError() on
		the observer is called.
		The last buffer of the audio stream being played should have the last
		buffer flag set using CMMFBuffer::SetLastBuffer(TBool). If a
		subsequent attempt to play the clip is made, this flag will need
		resetting by the client.
		@return void
	*/
	void PlayData();

	/**
		Contine the process of recording.
		Once the buffer is filled with recorded data, the Observer gets a
		reference to the buffer along with the callback function
		BufferToBeEmptied(). After processing the buffer (copying over to a
		different buffer or writing to file) the client should call this
		function to continue the recording process.
		@return void
	*/
	void RecordData();

	/**
		Stops the ongoing operation (Play, Record, TonePlay).
		@since 
		@return void
	*/
	void Stop();

	/**
		Temporarily Stops the ongoing operation (Play, Record, TonePlay).
		@since 
		@return void
	*/
	void Pause();

	/**
		Returns the Sample recorded so far
		@since 
		@return TInt Returns the samples recorded.
	*/
	TInt SamplesRecorded();

	/**
		Returns the Sample played so far
		@since 
		@return TInt Returns the samples played.
	*/
	TInt SamplesPlayed();

	/**
		Initializes the audio device and starts playing a tone. The tone is
		played with the frequency and duration specified.
		Leaves on failure.
		@since 
		@param TInt aFrequency The frequency at which the tone will be played.
		@param const TTimeIntervalMicroSeconds &aDuration The period over
		which the tone will be played. A zero value causes the no tone
		to be played.
		@return void
	*/
	void PlayToneL(TInt aFrequency,
				const TTimeIntervalMicroSeconds& aDuration);

	/**
		Initializes audio device and starts playing a dual tone. Dual Tone is
		played with the specified frequencies and for the specified duration.
		Leaves on failure.
		@since 
		@param TInt aFrequencyOne The first frequency of dual tone.
		@param TInt aFrequencyTwo The second frequency of dual tone.
		@param const TTimeIntervalMicroSeconds &aDuration The period over
			which the tone will be played. A zero value causes the no tone
			to be played.
		@return void
	*/
	void PlayDualToneL(TInt aFrequencyOne,
					TInt aFrequencyTwo,
					const TTimeIntervalMicroSeconds& aDuration);

	/**
		Initializes the audio device and starts playing the DTMF string
		aDTMFString.
		Leaves on failure.
		@since 
		@param const TDesC &aDTMFString The DTMF sequence in a descriptor.
		@return void
	*/
	void PlayDTMFStringL(const TDesC& aDTMFString);

	/**
		Initializes the audio device and starts playing a tone sequence.
		Leaves on failure.
		@since 
		@param const TDesC8 &aData The tone sequence in a descriptor.
		@return void
	*/
	void PlayToneSequenceL(const TDesC8& aData);

	/**
		Initializes the audio device and starts playing the specified
		pre-defined tone sequence.
		Leaves on failure.
		@since 
		@param TInt aSequenceNumber The index identifying the specific
			pre-defined tone sequence. Index values are relative to zero.
			This can be any value from zero to the value returned by a call
			to FixedSequenceCount() - 1. The function raises a panic if the
			sequence number is not within this range.
		@return void
	*/
	void PlayFixedSequenceL(TInt aSequenceNumber);

	/**
		Defines the number of times the audio is to be repeated during the
		tone playback operation. A period of silence can follow each playing
		of a tone. The tone playing can be repeated indefinitely
		@since 
		@param TInt aRepeatCount The number of times the tone, together with
			the trailing silence, is to be repeated. If this is set to
			KMdaRepeatForever, then the tone, together with the trailing
			silence, is repeated indefinitely or until Stop() is called.
			If this is set to zero, then the tone is not repeated.
		@param const TTimeIntervalMicroSeconds &aRepeatTrailingSilence An
			interval of silence which will be played after each tone.
			Supported only during tone playing.
		@return void
	*/
	void SetToneRepeats(
			TInt aRepeatCount,
			const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);

	/**
		Defines the duration of tone on, tone off and tone pause to be used
		during the DTMF tone playback operation.
		Supported only during tone playing.
		@since 
		@param TTimeIntervalMicroSeconds32 &aToneOnLength The period over
			which the tone will be played. If this is set to zero, then the
			tone is not played.
		@param TTimeIntervalMicroSeconds32 &aToneOffLength The period over
			which the no tone will be played.
		@param TTimeIntervalMicroSeconds32 &aPauseLength The period over which
			the tone playing will be paused.
		@return void
	*/
	void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
					TTimeIntervalMicroSeconds32& aToneOffLength,
					TTimeIntervalMicroSeconds32& aPauseLength);

	/**
		Defines the period over which the volume level is to rise smoothly
		from nothing to the normal volume level.
		The function is only available before playing.
		@since 
		@param const TTimeIntervalMicroSeconds &aRampDuration The period over
			which the volume is to rise. A zero value causes the tone
			sample to be played at the normal level for the full duration
			of the playback. A value, which is longer than the duration of
			the tone sample means that the sample never reaches its normal
			volume level.
		@return void
	*/
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	/**
		Defines the priority settings that should be used for this instance.
		@since 
		@param const TMMFPrioritySettings &aPrioritySettings A class type
			representing the client's priority, priority preference and
			state
		@return void
	*/
	void SetPrioritySettings(TMMFPrioritySettings& aPrioritySettings);

	/**
		Returns a pointer reference to custom interface implementation.
		Note: Implementation from MCustomInterface.
		@since 
		@return   "TAny"
			Custom interface implementation. The exact type of pointer is
			dependent on the custom interface implemenation.
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId);

	/**
		Returns the number of available pre-defined tone sequences.
		This is the number of fixed sequence supported by DevSound by default.
		@since 
		@return TInt  The fixed sequence count. This value is implementation
			dependent.
	*/
	TInt FixedSequenceCount();

	/**
		Returns the name assigned to a specific pre-defined tone sequence.
		This is the number of the fixed sequence supported by DevSound by
		default.
		The function raises a panic if sequence number specified is invalid.
		@since 
		@param TInt aSequenceNumber The index identifying the specific
			pre-defined tone sequence. Index values are relative to zero.
			This can be any value from zero to the value returned by a call
			to CMdaAudioPlayerUtility::FixedSequenceCount() - 1. The
			function raises a panic if sequence number is not within this
			range.
		@return const TDesC & A reference to a Descriptor containing the fixed
			sequence name indexed by aSequenceNumber.
	*/
	const TDesC& FixedSequenceName(TInt aSequenceNumber);

	/**
		Returns a list of the supported input datatypes that can be sent to
		DevSound for playing audio. The datatypes returned are those that the
		DevSound supports given the priority settings passed in
		aPrioritySettings. Note that if no supported data types are found this
		does not constitute failure, the function will return normally with no
		entries in aSupportedDataTypes.
		@since 
		@param RArray< TFourCC > &aSupportedDataTypes The array of supported
			data types that will be filled in by this function. The
			supported data types of the DevSound are in the form of an
			array of TFourCC codes. Any existing entries in the array will
			be overwritten on calling this function. If no supported data
			types are found given the priority settings, then the
			aSupportedDatatypes array will have zero entries.
		@param const TMMFPrioritySettings &aPrioritySettings The priority
			settings used to determine the supported datatypes. Note this
			does not set the priority settings. For input datatypes the
			iState member of the priority settings would be expected to be
			either EMMFStatePlaying or EMMFStatePlayingRecording. The
			priority settings may effect the supported datatypes depending
			on the audio routing.
		@return void
	*/
	void GetSupportedInputDataTypesL(
		RArray<TFourCC>& aSupportedDataTypes,
		const TMMFPrioritySettings& aPrioritySettings) const;

	/**
		Returns a list of the supported output dataypes that can be received
		from DevSound for recording audio. The datatypes returned are those
		that the DevSound supports given the priority settings passed in
		aPrioritySettings. Note that if no supported data types are found this
		does not constitute failure, the function will return normally with no
		entries in aSupportedDataTypes.
		@since 
		@param RArray< TFourCC > &aSupportedDataTypes The array of supported
			data types that will be filled in by this function. The
			supported datatypes of the DevSound are in the form of an array
			of TFourCC codes. Any existing entries in the array will be
			overwritten on calling this function. If no supported datatypes
			are found given the priority settings, then the
			aSupportedDatatypes array will have zero entries.
		@param const TMMFPrioritySettings &aPrioritySettings The priority
			settings used to determine the supported data types. Note this
			does not set the priority settings. For output data types the
			iState member of the priority settings would expected to be
			either EMMFStateRecording or EMMFStatePlayingRecording. The
			priority settings may effect the supported datatypes depending
			on the audio routing.
		@return void
	*/
	void GetSupportedOutputDataTypesL(
		RArray<TFourCC>& aSupportedDataTypes,
		const TMMFPrioritySettings& aPrioritySettings) const;

	/**
		MDevSoundAdaptationObserver callback.
		This is called when DevSound initialization is complete.
		@since 
		@param TInt aError KErrNone if successfull, else corresponding error
			code
		@return void
	*/
	void InitializeComplete(TInt aError);

	/**
		MDevSoundAdaptationObserver callback.
		This is called when tone play back is complete.
		@since 
		@param TInt aError KErrNone if successfull, else corresponding error
			code
		@return void
	*/
	void ToneFinished(TInt aError);

	/**
		MDevSoundAdaptationObserver callback.
		This is called when more data is needed during digital audio playback.
		@since 
		@param CMMFBuffer* aBuffer A pointer to buffer where data is needed.
		@return void
	*/
	void BufferToBeFilled(CMMFBuffer* aBuffer);

	/**
		MDevSoundAdaptationObserver callback.
		This is called when there is error during playback.
		@since 
		@param TInt aError KErrUnderflow if last buffer is played back, else
			corresponding error code
		@return void
	*/
	void PlayError(TInt aError);

	/**
		MDevSoundAdaptationObserver callback.
		This is called when data is available during digital audio recording.
		@since 
		@param CMMFBuffer* aBuffer A pointer to buffer where recorded data is
			available.
		@return void
	*/
	void BufferToBeEmptied(CMMFBuffer* aBuffer);

	/**
		MDevSoundAdaptationObserver callback.
		This is called when there is error during recording.
		@since 
		@param TInt aError Corresponding error code
		@return void
	*/
	void RecordError(TInt aError);

	/**
		MDevSoundAdaptationObserver callback.
		This is called when there is a message from audio device.
		@since 
		@param TUid aMessageType A Uid identifying message type.
		@param const TDesC8& aMsg A reference to constant descriptor
			containing packed message.
		@return void
	*/
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

	
	// from MMMFDevSoundCustomInterfaceDeMuxPlugin
	/**
		MDevSoundAdaptationObserver callback.
		Underlying interface has been (or is about to be) deleted.
		Implementations will generally cancel outstanding messages
	
		@param	aInterfaceId	Uid of the Interface which has been deleted
	*/

	void InterfaceDeleted(TUid aInterfaceId);

	/**
		MDevSoundAdaptationObserver callback.
		Not used.
		@since 
		@param const TMMFEvent& aEvent Not used
		@return void
	*/
	void SendEventToClient(const TMMFEvent& aEvent);

	void DoProcessingFinished();
	void DoProcessingError();
	
	/**
		MDevSoundAdaptationObserver callback.
		Underlying physical adaptation has sent a callback that will result either on a commit
		and need to scheduled or in the case of RecordPauseComplete, completes the message. 
		@since 
		@param aType the callback type
		@param aError KErrNone if successful, else corresponding error
			code
	*/
	void CallbackFromAdaptorReceived(TInt aType, TInt aError);
	
	
	/**
		MDevSoundAdaptationObserver callback.
		Underlying physical adaptation has sent a callback indicating that a preemption process 
	 	has started, any incomming commands will be queued during the preemption.  

		@since 
	 */
	void PreemptionStartedCallbackReceived();

	
	/**
		MDevSoundAdaptationObserver callback.
		Underlying physical adaptation has sent a callback indicating that a preemption process
		has finished. Queued commands during preemption can now be processed. 
		@since 
		@param	aCanStartNewOperation	If EFalse indicates that the operation has more than a cycle
	 */
	void PreemptionFinishedCallbackReceived(TBool aCanStartNewOperation);

	// from MDevSoundAdaptationObserver
	TBool AdaptorControlsContext() const;
	void PreemptionClash();
	void PreemptionClashWithStateChange();
    void NotifyError(TInt aError);

	/**
		MDevSoundAdaptationObserver callback.
		Indicates that a low layer operation completion
		@since 
		@param	aError	The status of operation in progress
		@param	aCanStartNewOperation	If EFalse indicates that the operation has more than a cycle
	*/
	void AsynchronousOperationComplete(TInt aError, TBool aCanStartNewOperation);


	// from MMMFDevSoundCustomInterfaceDeMuxInterface
	TInt DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf);
	void DoCloseSlaveL(TInt aHandle);
	TInt DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage);
	TInt DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage); 
	void DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage);
	void DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage);
	

private: // Functions

	/**
		Returns a object reference to CMMFObjectContainer.
		@since 
		@return CMMFObjectContainer& A reference to CMMFObjectContainer
			implemenation.
	*/
	CMMFObjectContainer& MMFObjectContainerL();

	/**
		Returns a object reference to CMMFObjectContainer.
		@since 
		@return CMMFCustomCommandParserBase& aParser A reference to
			Custom command parser implemenation.
		@return void
	*/
	void AddCustomCommandParserL(CMMFCustomCommandParserBase& aParser);

	/**
		Method to set client configuration information to DevSound Adaptation.
		Leaves on failure.
		@since 
		@return void
	*/
	void DoSetClientConfigL();

	/**
		Equivalent of DoSetClientConfigL() that returns error
		@return system-wide error
	*/
	TInt DoSetClientConfig();

	/**
		Method to create global chunk
		@since 
		@param TMMFDevSoundProxyHwBufPckg& - buffer information container
		@param CMMFDataBuffer* - data buffer pointer
		@return void
	*/
	TInt CreateChunk(TMMFDevSoundProxyHwBufPckg& aBufPckg, TInt aRequestedSize);


	// New internal methods
	/*
	Services the first request at the FIFO
	*/
	void DoServiceRequestL(const RMmfIpcMessage& aMessage);

	/*
	Services the first request of queue for a pseudo asynchronous function that has already completed,
	but needs to be re-applied again due to pre-emption clash.
	*/
	void DoServiceAlreadyCompletedRequestL(const TInt aFunction);

	/*
	Handles the first request of queue for a pseudo asynchronous function that has already completed,
	but needs to be re-applied again due to pre-emption clash.
	*/
	void HandleAlreadyCompletedRequest();

	/*
	Services the first request at the FIFO
	*/
	void DoServiceNextRequestL();

	/*
	Completes the message 
	@param aReason the error to complete message
	@param aOperationComplete if is true also this message will be removed from the queue
		if is false just the message will be completed but will be keeped at the queue
	*/
	void CompleteRequest(TInt aReason);

	/*
	Adds a message to the FIFO
	*/
	void EnqueueRequest(const RMmfIpcMessage& aMessage);

	/*
	Removes the message from the FIFO
	 */
	void DequeueRequest();
	


	/*
	Flush event queue - called as part of Stop() sequence
	 */
	void FlushEventQueue();

	/*
	Filter certain events from message queue 
	*/
	void FilterQueueEvent(TMMFDevSoundProxyRequest aRequest);
	
	/*
	Called by a server when it receives a disconnect message for the session.
	Overrided in order to perform asynchronous cleanup actions
	these actions must end with a call to the base class implementation of this method
	which will delete the session object and complete the disconnect message
	*/
	void Disconnect(const RMessage2& aMessage);
	
	//panic function	
	void Panic(TMMFDevSoundSessionPanicCodes aCode);
	
	/*
	Used to send a stop call when error in buffer
	*/
	void BufferErrorEvent();

	/*
	 Removes all entries from iQueuedRequests except for
	 disconnect requests.
	 */
	void FlushQueuedRequests();
	
	/*
	 Helper functions, used to guard against accessing the equivalent
	 functions on bad message handles during a disconnection. These
	 methods should be used whenever a Read or Write is made outside the 
	 context of DoServiceRequestL().
	 */
	TInt MessageRead(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult);
	TInt MessageRead(const RMmfIpcMessage& aMessage, TInt aParam, TDes16& aResult);
	TInt MessageWrite(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue);

protected:

	/**
		C++ default constructor.
	*/
	CMMFDevSoundSession();
	/**
		By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL(MGlobalProperties& aGlobalProperties);
	
private:
	TBool DoCustomCommandL(const RMmfIpcMessage& aMessage);
	MMMFDevSoundCustomInterfaceDeMuxPlugin* InterfaceFromUid(TUid aUid);
	
	static TInt AsyncQueueStartCallback(TAny* aPtr);
	void AsyncQueueStartCallback();

	void ResetNotifiedError();
	TInt NotifiedError() const;

	TBool NeedToQueue() const;

protected:	// Data
	CMMFDevSoundAdaptation* iAdapter;

private:	// Data
	// A number representing the session id
	TInt iDevSoundSessionId;
	// Reference to play buffer
	CMMFDataBuffer* iBufferPlay;
	// Reference to record buffer
	CMMFDataBuffer* iBufferRecord;
	// Array to hold supported input/output data types
	RArray<TFourCC> iArray;
	// Buffer to hold DTMFString to be played
	HBufC* iDtmfString;
	// Buffer to hold tone sequence to be played
	HBufC8* iToneSeqBuf;


	// Replaces the messages / AO handlers
	RMsgQueue<TMMFDevSoundQueueItem> iMsgQueue;
	// data store for 2-stage asynch message passing
	TMMFDevSoundProxyHwBufPckg iHwBufPckgFill;
	// data store for 2-stage asynch message passing
	TMMFDevSoundProxyHwBufPckg iHwBufPckgEmpty;

	// Array of custom interface pairs
	RArray<TMMFDevSoundCustomInterfaceDeMuxData> iCustomInterfaceArray;

	//Capabilities from DevSoundAdaptor	
	TMMFCapabilities iDevSoundCapabilities;
	// 
	CMMFDevSoundCIDeMuxUtility* iDeMuxUtility;
	// CI Extension
	MDevSoundCIServerExtension* iCIExtension;

	// Handling async requests
	RArray<TMMFDevSoundRequest> iQueuedRequests;
	TMMFDevSoundRequest			iRequestBeingServiced;
	TBool						iOperationCompletePending;
	TBool                       iHandlingExtdCI;
	TBool						iPlayErrorOccured;
	
	//Check if the call of SetClientConfigL was alredy made
	TBool						iSetClientConfigApplied;
	
	// Chunk for use between DevSound client and server
	RChunk iChunk;
	TBool iForceSendOfChunkHandle;
	
	CActiveSchedulerWait*		iClosingWait;
	CAsyncCallBack*				iAsyncQueueStart;

	TInt iRedoFunction;
	TMMFDevSoundProxySettingsPckg iCachedClientData;
	TInt iSeqNum;
	TBool iPreemptionClash;
	TInt iNotifiedError;
	TBool iDisconnecting;

	};

#endif // MMFDEVSOUNDSESSION_H

// End of File
