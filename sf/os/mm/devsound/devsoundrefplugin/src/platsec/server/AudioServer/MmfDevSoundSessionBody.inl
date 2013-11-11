// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/server/mmfswcodecwrapper.h> //for SwCodecWrapper CustomInterfaces
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
#include <mmf/server/mmfhwdevicesetup.h>
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
#include "MmfDevSoundServer.h"
#include "MmfDevSoundSessionBody.h"
/*
 *
 *	Initializes DevSound object for the mode aMode for processing audio data
 *	with hardware device aHWDev.
 *
 *	On completion of Initialization, the observer will be notified via call back
 *	InitializeComplete().
 *
 *	Leaves on failure.
 *  
 *	@param	"MDevSoundObserver& aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"TUid aHWDev"
 *			CMMFHwDevice implementation identifier.
 *
 *	@param	"TMMFState aMode"
 *			Mode for which this object will be used.
 *
 */
 
 #ifdef _DEBUG
_LIT(KMMFDevSoundSessionCategory, "MMFDevSoundSessionCategory");
inline void Panic(TInt aError)
	{
	User::Panic(KMMFDevSoundSessionCategory, aError); 
	}
enum TMMFDevSoundSessionPanics
	{
	TMMFDevSoundSessionPolicyNotInvalidated
	};
#endif
 
inline void CMMFDevSoundSvrImp::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)
	{
	TInt initError = KErrNone;
	iDevSoundObserver = &aDevSoundObserver;

	if (aMode == EMMFStateIdle)
		User::Leave(KErrNotSupported);

	iMode= static_cast<TMMFDevSoundState> (aMode);
	iHasPolicy = EFalse;

	if (iMode == EMMFDevSoundStateRecording)
		{//DEF037912 incase the recording capabilities differ from play
		User::LeaveIfError(InitializeFormat(iRecordFormatsSupported, iRecordFormat));
		}


	iDevSoundObserver = &aDevSoundObserver;
	iHwDeviceID.iUid = aHWDev.iUid;
    if(iCMMFHwDevice)
		{
		delete iCMMFHwDevice;
		iHwDeviceBuffer = NULL; // buffer is deleted by HwDevice delete
		iPlayCustomInterface = NULL; //custom interfaces are also invalid
		iRecordCustomInterface = NULL;
		iTimePlayedCustomInterface = NULL;
		}
    
    iCMMFHwDevice = NULL;

	// Load the HwDevice implementation from ECom
    TRAP(initError,iCMMFHwDevice = CMMFHwDevice::NewL(aHWDev));//trap it to get debug print

	if (initError == KErrNone)
		{
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
		if (iFourCCSet)
			{
			MMdfHwDeviceSetup* setup = reinterpret_cast<MMdfHwDeviceSetup*>(iCMMFHwDevice->CustomInterface(KUidHwDeviceSetupInterface)); 
			if (setup!=NULL)
				{
				setup->SetDataTypesL(iSrcFourCC, iDestFourCC);	
				}
			}
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
		iDevInfo.iHwDeviceObserver = this;
		initError = iCMMFHwDevice->Init(iDevInfo);
		if (initError == KErrNone) 
			{
			TUid playCustomInterfaceUid;
			playCustomInterfaceUid.iUid = KMmfPlaySettingsCustomInterface;
			TUid recordCustomInterfaceUid;
			recordCustomInterfaceUid.iUid = KMmfRecordSettingsCustomInterface;
			iPlayCustomInterface = 
				(MPlayCustomInterface*)iCMMFHwDevice->CustomInterface(playCustomInterfaceUid);
			if (!iPlayCustomInterface) 
				{//DEF40443 need to check custom interface has been created
				initError = KErrNoMemory;//it won't if there is no memory
				}
			else
				{
				iRecordCustomInterface = 
					(MRecordCustomInterface*)iCMMFHwDevice->CustomInterface(recordCustomInterfaceUid);
				if (!iRecordCustomInterface) 
					initError = KErrNoMemory;
				}
			}
		}

	// in the InitializeComplete() call we will signal the observer that the 
	// custom interface may have changed
	iDevSoundObserver->InitializeComplete(initError);  

	if (initError)
		{
		User::Leave(initError);
		}
	__ASSERT_DEBUG(!(iHasPolicy&&(iMode == EMMFDevSoundStatePlaying)), Panic(TMMFDevSoundSessionPolicyNotInvalidated));
	}

/*
 *
 *	Initializes DevSound object for the mode aMode for processing audio data
 *	with hardware device supporting FourCC aDesiredFourCC.
 *
 *	On completion of Initialization, the observer will be notified via call back
 *	InitializeComplete().
 *
 *	Leaves on failure.
 *  
 *	@param	"MDevSoundObserver& aDevSoundObserver"
 *			A reference to DevSound Observer instance.
 *
 *	@param	"TFourCC aDesiredFourCC"
 *			CMMFHwDevice implementation FourCC.
 *
 *	@param	"TMMFState aMode"
 *			Mode for which this object will be used.
 *
 */
inline void CMMFDevSoundSvrImp::InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode)
	{
	// to get HW Uid from the FourCC
	RImplInfoPtrArray plugInArray;
	TFourCC KPCM16FourCC(' ','P','1','6');
	_LIT(KNullString, "");

	TUid hwDevicePluginInterface = {KMmfUidPluginInterfaceHwDevice};
	TUid implUid = {0};  
	
	CleanupResetAndDestroyPushL( plugInArray );
	// Get the implementation UID based on the FourCC and mode.
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	TInt err = KErrNone;
	RArray<TFourCC> dataTypes;
	CleanupClosePushL(dataTypes);
	
	TRAP(err, iDevSoundUtil->SeekCodecPluginsL(dataTypes, aMode, EFalse));
	// if we find a new codec plugin
	TBool found = EFalse;
	if (!err && dataTypes.Find(aDesiredFourCC)!=KErrNotFound)
		{
		TRAP(err, iDevSoundUtil->FindHwDeviceAdapterL(hwDevicePluginInterface, plugInArray));
		if (err == KErrNone)
			{
			if (aMode == EMMFDevSoundStatePlaying)
				{//destination four CC is pcm16
				iFourCCSet = ETrue;
				iSrcFourCC = aDesiredFourCC;
				iDestFourCC = KPCM16FourCC;
				}
			else if (aMode == EMMFDevSoundStateRecording)
				{//source fourCC is pcm16
				iFourCCSet = ETrue;
				iSrcFourCC = KPCM16FourCC;
				iDestFourCC = aDesiredFourCC;
				}
			found = ETrue;
			}
		}
	if (!found)
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
		{
		plugInArray.ResetAndDestroy();
		if (aMode == EMMFDevSoundStatePlaying)
			{//destination four CC is pcm16
			iFourCCSet = ETrue;
			iSrcFourCC = aDesiredFourCC;
			iDestFourCC = KPCM16FourCC;
			iDevSoundUtil->SeekUsingFourCCL(hwDevicePluginInterface, plugInArray, aDesiredFourCC, KPCM16FourCC, KNullString);
			}
		else if (aMode == EMMFDevSoundStateRecording)
			{//source fourCC is pcm16
			iFourCCSet = ETrue;
			iSrcFourCC = KPCM16FourCC;
			iDestFourCC = aDesiredFourCC;
			iDevSoundUtil->SeekUsingFourCCL(hwDevicePluginInterface, plugInArray, KPCM16FourCC, aDesiredFourCC, KNullString);
			}
		else 
			{
			User::Leave(KErrNotSupported);//invalid aMode cant set 4CC for tone
			}
		}
	

	if(plugInArray.Count() == 0)
		{ // couldn't find Decoder only implementation, try to get Decoder/Encoder
		iDevSoundUtil->SeekUsingFourCCL(hwDevicePluginInterface, plugInArray, aDesiredFourCC, aDesiredFourCC, KNullString);
		if(plugInArray.Count() == 0)
			User::Leave(KErrNotSupported);
		}
	implUid = plugInArray[0]->ImplementationUid(); // Just pick the first in the list

#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
	CleanupStack::PopAndDestroy(2, &plugInArray) ;  //pluginArray, dataTypes
#else
	CleanupStack::PopAndDestroy() ;  //pluginArray
#endif // SYMBIAN_MULTIMEDIA_CODEC_API
	// If we made it here, there we have found implementation UID
	InitializeL(aDevSoundObserver, implUid, aMode);
	__ASSERT_DEBUG(!(iHasPolicy&&(iMode == EMMFDevSoundStatePlaying)), Panic(TMMFDevSoundSessionPolicyNotInvalidated)); 
	}

/*
 *
 *	Returns the supported Audio settings.
 *  
 *	@return	"TMMFCapabilities"
 *			Device settings.
 *
 */
inline TMMFCapabilities CMMFDevSoundSvrImp::Capabilities()
	{
	return iDeviceCapabilities;
	}

/*
 *
 *	Returns the current audio settings.
 *  
 *	@return	"TMMFCapabilities"
 *			Device settings.
 *
 */
inline TMMFCapabilities CMMFDevSoundSvrImp::Config() const
	{
	return iDeviceConfig;
	}

/*
 *
 *	Returns an integer representing the maximum volume.
 *
 *	This is the maximum value which can be passed to CMMFDevSoundProxy::SetVolume.
 *  
 *	@return	"TInt"
 *			The maximum volume. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
inline TInt CMMFDevSoundSvrImp::MaxVolume()
	{
	return iPlayFormatsSupported().iMaxVolume;
	}

/*
 *
 *	Returns an integer representing the current volume.
 * 
 *	@return	"TInt"
 *			The current volume level.
 *
 */
inline TInt CMMFDevSoundSvrImp::Volume()
	{
	return iVolume;
	}

/*
 *
 *	Returns an integer representing the maximum gain.
 *
 *	This is the maximum value which can be passed to CMMFDevSoundProxy::SetGain.
 * 
 *	@return	"TInt"
 *			The maximum gain. This value is platform dependent but is always
 *			greater than or equal to one.
 *
 */
inline TInt CMMFDevSoundSvrImp::MaxGain()
	{
	return iRecordFormatsSupported().iMaxVolume;//uses iMaxVolume for iMaxGain
	}

/*
 *
 *	Returns an integer representing the current gain.
 *
 *	@return	"TInt"
 *			The current gain level.
 *
 */
inline TInt CMMFDevSoundSvrImp::Gain()
	{
	return iGain;
	}

/*
 *
 *	Returns the speaker balance set for playing.
 *
 *	Leaves on failure.
 *
 *	@param	"TInt& aLeftPrecentage"
 *			On return contains the left speaker volume percentage.
 *
 *	@param	"TInt& aRightPercentage"
 *			On return contains the right speaker volume percentage.
 *
 */
inline void CMMFDevSoundSvrImp::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	aLeftPercentage = iLeftPlayBalance;
	aRightPercentage = iRightPlayBalance;
	}

/*
 *
 *	Returns the microphone gain balance set for recording.
 *
 *	Leaves on failure.
 *
 *	@param	"TInt& aLeftPercentage"
 *			On return contains the left microphone gain percentage.
 *
 *	@param	"TInt& aRightPercentage"
 *			On return contains the right microphone gain percentage.
 *
 */
inline void CMMFDevSoundSvrImp::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	{
	aLeftPercentage = iLeftRecordBalance;
	aRightPercentage = iRightRecordBalance;
	}

/*
 *
 *	Contine the process of recording. Once the buffer is filled with recorded
 *	data, the Observer gets reference to buffer along with callback
 *	BufferToBeEmptied(). After processing the buffer (copying over to a
 *	different buffer or writing to file) the client should call this
 *	method to continue recording process.
 *
 */
inline TBool CMMFDevSoundSvrImp::RecordData(const RMmfIpcMessage& aMessage)
	{
	ASSERT(iDevSoundObserver);
	if(iAudioPolicyPrioritySettings.iState != EMMFStateRecordData)
		{
		PanicClient(aMessage, EMMFDevSoundRecordDataWithoutInitialize);
		return EFalse;
		}
	// Checkes if the client has a UserEnvironment capability
	if (!aMessage.HasCapability(ECapabilityUserEnvironment))
		{
		iDevSoundObserver->RecordError(KErrPermissionDenied);
		return ETrue;
		}
	if(iCMMFHwDevice)
		{
		if ((iMode == EMMFDevSoundStateRecording) && iHasPolicy)
			{
			iHwDeviceBuffer->Data().SetLength(iHwDeviceBuffer->RequestSize());
			TInt error = iCMMFHwDevice->ThisHwBufferEmptied(*iHwDeviceBuffer);
			if(error != KErrNone)
				{
				Error(error);
				iCMMFHwDevice->Stop();
				}

			}
		}
	return ETrue;
	}

/*
 *
 *	Defines the number of times the audio is to be repeated during the tone
 *	playback operation.
 *
 *	A period of silence can follow each playing of tone. The tone playing can
 *	be repeated indefinitely.
 *
 *	@param	"TInt aRepeatCount"
 *			The number of times the tone, together with the trailing silence,
 *			is to be repeated. If this is set to KMdaRepeatForever, then the
 *			tone, together with the trailing silence, is repeated indefinitely
 *			or until Stop() is called. If this is set to zero, then the tone is
 *			not repeated.
 *
 *			Supported only during tone playing.
 *
 */
inline void CMMFDevSoundSvrImp::SetToneRepeats(TInt aRepeatCount,
				const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	{
	iRepeatCount = aRepeatCount;
	iRepeatTrailingSilence = aRepeatTrailingSilence;
	}

/*
 *
 *	Defines the priority settings that should be used for this instance.
 *
 *	@param	"const TMMFPrioritySettings& aPrioritySettings"
 *			An class type representing the client's priority, priority 
 *			preference and state.
 *
 */
inline void CMMFDevSoundSvrImp::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	iAudioPolicyPrioritySettings.iPref = aPrioritySettings.iPref;
	iAudioPolicyPrioritySettings.iPriority = aPrioritySettings.iPriority;
	}

// Currently only support bitrate custom interface
inline TAny* CMMFDevSoundSvrImp::CustomInterface(TUid aInterfaceId)
	{
	TAny* ptr = NULL;
	if (iCMMFHwDevice)
		{
		ptr = iCMMFHwDevice->CustomInterface(aInterfaceId);	
		}
		
	return ptr;	
	}

/*
 *
 *	Returns the number of available pre-defined tone sequences.
 *
 *	This is the number of fixed sequence supported by DevSound by default.
 *
 *	@return	"TInt"
 *			The fixed sequence count. This value is implementation dependent
 *			but is always greater than or equal to zero.
 *
 */
inline TInt CMMFDevSoundSvrImp::FixedSequenceCount()
	{
	return iFixedSequences->Count();
	}

/*
 *
 *	Returns the name assigned to a specific pre-defined tone sequence.
 *
 *	This is the number of fixed sequence supported by DevSound by default.
 *
 *	The function raises a panic if sequence number specified invalid.
 *
 *	@return	"TDesC&"
 *			A reference to a Descriptor containing the fixed sequence
 *			name indexed by aSequenceNumber.
 *
 *	@param	"TInt aSequenceNumber"
 *			The index identifying the specific pre-defined tone sequence. Index
 *			values are relative to zero.
 *			This can be any value from zero to the value returned by a call to
 *			FixedSequenceCount() - 1.
 *			The function raises a panic if sequence number is not within this
 *			range.
 *
 *	@see	FixedSequenceCount()
 *
 */
inline const TDesC& CMMFDevSoundSvrImp::FixedSequenceName(TInt aSequenceNumber)
	{
	ASSERT((aSequenceNumber >= 0)&&(aSequenceNumber < iFixedSequences->Count()));
	return iDevSoundUtil->FixedSequenceName(aSequenceNumber);
	}

/*
 *
 *	Sets Id for this instance of DevSound
 *
 *	@param	"TInt aDevSoundId"
 *			Integer value assigned by Audio Policy Server
 *
 */
inline void CMMFDevSoundSvrImp::SetDevSoundId(TInt aDevSoundId)
	{
	iDevSoundInfo.iDevSoundId = aDevSoundId;
	}

/*
 *
 *	Returns information about this DevSound instance.
 *
 *	This method is used by Audio Policy Server to make audio policy decisions.
 * 
 *	@return	"TMMFDevSoundinfo"
 *			A reference to TMMFDevSoundinfo object holding the current settings
 *			of this DevSound instance.
 *
 */
inline TMMFDevSoundInfo CMMFDevSoundSvrImp::DevSoundInfo()
	{
 	return iDevSoundInfo;
	}


/*
 *	Updates the total bytes played.
 *
 */
inline void CMMFDevSoundSvrImp::UpdateBytesPlayed()
	{
	if (iPlayCustomInterface)
		iPlayedBytesCount = iPlayCustomInterface->BytesPlayed();
	}
