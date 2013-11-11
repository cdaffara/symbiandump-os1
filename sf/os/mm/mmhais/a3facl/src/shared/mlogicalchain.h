/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef MLOGICALCHAIN_H
#define MLOGICALCHAIN_H

#include <e32base.h>
#include <a3f/a3fbase.h>

// CLASS FORWARD

class MMMFAudioDataSupplier;
class MMMFAudioDataConsumer;
class MMMFBufferSource;
class MMMFBufferSink;
class MAudioGainControl;
class MAudioCodecAdaptationObserver;
class MStreamPositionControl;
class MCustomInterfaceSupport;
class MConfigurationHelper;
class MStreamBufferControl;


enum TMMRCMessageType
	{
	ENoMessage = 0x0000,
	ERegisterStreamObserver = 0x0001, 
	ERegisterCodecObserver = 0x0002, 
	ERegisterGainObserver = 0x0004, 

	EComponentCreation = 0x0010,
	EComponentDestruction = 0x0020,
	EComponentAlterationGain = 0x0040,
	EComponentAlterationCodec = 0x0080,
	EStreamStateChange = 0x0100,
	EConfigurationRequest = 0x0200
	};


const TUid KUidAudioCodecObserver = {0x102834BF};


/*
Interface that represent the logical chain 
@class MLogicalChain
*/

class MLogicalChain
	{
public:
	/**
	Set the type of message to be processed at the adaptation side
	@ see TMMRCMessageType
	@param the type of message to be processed
	*/
	virtual void SetMessageType(TMMRCMessageType aMessageType) = 0;

	/**
	Set the type of message to be processed at the adaptation side
	@ see TMMRCMessageType
	@param the type of message to be processed
	*/
	virtual void ResetMessage() = 0;

	/**
	Returns the type of message to be processed at the adaptation side
	@ see TMMRCMessageType
	@return the type of message to be processed
	*/
	virtual TUint MessageType() = 0;


	/**
	Returns the number of processing units in the current logical chain 
	@return the number of processing units in the current logical chain 
	*/
	virtual TInt AudioProcessingUnitsCount() = 0;

	/**
	Returns the UID type of the processing unit of the logical chain 
	This function allows the adaptation side know extra details for logical components
	@param aIndex indicates the creation order index
	@return the uid type of the processing unit
	*/
	virtual TUid AudioProcessingUnitUid(TInt aIndex) = 0;

	/**
	Returns the logical chain  state
	@return the logical chain state
	*/
	virtual TAudioState StreamState() = 0;

	/**
	Set the logical chain state
	@param the logical chain state
	*/
	virtual void SetStreamState(TAudioState aState) = 0;

	/**
	Returns the logical codec format
	@return the logical codec format
	*/
	virtual TUid CodecFormat() = 0;


	/**
	Returns the logical codec sample rate
	@return the logical codec sample rate
	*/
	virtual TInt GetSampleRate() = 0;

	/**
	Gets the mode – e.g. mono/stereo. 
	@return aMode
	*/
	virtual TUid GetMode() = 0;

	/**
	Returns the priority from the stream
	@return the the priority from the stream
	*/
	virtual TInt Priority() = 0;

	/**
	Returns the ramp time parameters
	*/
	virtual void GetVolumeRampParameters(TUid& aRampOperation, TTimeIntervalMicroSeconds& aRampDuration) = 0;

	/**
	Copies the main configuration parameters of the logical chain
	@see TLogicalChainConfiguration
	@param the logical chain from where the parameter will be copied
	*/
	virtual void CopySettings(const MLogicalChain& aChain) = 0;

	/**
	Creates a copy from the current object 
	@return a copy from the current logical chain the ownership is transferred
	*/
	virtual MLogicalChain* CloneL() = 0;

	/**
	Deletes the logical chain cloned
	*/
	virtual void Release() = 0;

	/**
	Deletes the logical chain cloned
	*/
	virtual TAny* GetComponent(TUid aType) = 0;

	// TODO: Remove those temporary workarounds for data connection
	// When the MMRC server is fully implemented and 
	// the Control mechanisms & Observation interface extension mechanisms are available
	virtual void SetAdaptationSource(MMMFBufferSource& aSource) = 0;
	virtual void SetAdaptationSink(MMMFBufferSink& aSink) = 0;
	virtual void SetAdaptationGainControl(MAudioGainControl& aGain) = 0;
	virtual void SetStreamPositionControl(MStreamPositionControl& aControl) = 0;
	virtual void SetCustomInterfaceProvider(MCustomInterfaceSupport& aControl) = 0;
	virtual void SetAdaptationStream(MConfigurationHelper& aCodec) = 0;
	virtual void SetStreamBufferControl(MStreamBufferControl& aStreamBufferControl) =  0;
	};

#endif // MLOGICALCHAIN_H
