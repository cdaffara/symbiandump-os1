/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "T_CMMFDevSoundData.h"

/*@{*/
//Constants for HwDevice
const TInt KHwDeviceDefaultDataLength = 9;
const TInt KFourCCLength = 4;
//Supported Input Data Types
const TInt KFourCCMaxBitDisplacement = 24;
const TInt K8Bits = 8;
const TInt KTFourCC = 4;
/*@}*/

/*@{*/
//Command literals
_LIT(KCmdNewL,                         "NewL");
_LIT(KCmdNewLLoopL,                    "NewLLoopL");
_LIT(KCmdDestructor,                   "~");
_LIT(KCmdCapabilities,                 "Capabilities");
_LIT(KCmdConfig,                       "Config");
_LIT(KCmdMaxGain,                      "MaxGain");
_LIT(KCmdMaxVolume,                    "MaxVolume");
_LIT(KCmdGetPlayBalanceL,              "GetPlayBalanceL");
_LIT(KCmdGetRecordBalanceL,            "GetRecordBalanceL");
_LIT(KCmdGetSupportedInputDataTypesL,  "GetSupportedInputDataTypesL");
_LIT(KCmdGetSupportedOutputDataTypesL, "GetSupportedOutputDataTypesL");
_LIT(KCmdSamplesPlayed,                "SamplesPlayed");
_LIT(KCmdSamplesRecorded,              "SamplesRecorded");
_LIT(KCmdVolume,                       "Volume");
_LIT(KCmdGain,                         "Gain");
_LIT(KCmdFixedSequenceCount,           "FixedSequenceCount");
_LIT(KCmdFixedSequenceName,            "FixedSequenceName");
_LIT(KCmdSetConfigL,                   "SetConfigL");
_LIT(KCmdSetDTMFLengths,               "SetDTMFLengths");
_LIT(KCmdSetGain,                      "SetGain");
_LIT(KCmdSetPlayBalanceL,              "SetPlayBalanceL");
_LIT(KCmdSetPrioritySettings,          "SetPrioritySettings");
_LIT(KCmdSetRecordBalanceL,            "SetRecordBalanceL");
_LIT(KCmdSetToneRepeats,               "SetToneRepeats");
_LIT(KCmdSetVolume,                    "SetVolume");
_LIT(KCmdSetVolumeRamp,                "SetVolumeRamp");
_LIT(KCmdInitializeL,                  "InitializeL");
_LIT(KCmdPlayDTMFStringL,              "PlayDTMFStringL");
_LIT(KCmdPlayData,                     "PlayData");
_LIT(KCmdPlayDualToneL,                "PlayDualToneL");
_LIT(KCmdPlayInitL,                    "PlayInitL");
_LIT(KCmdPlayToneL,                    "PlayToneL");
_LIT(KCmdPlayToneSequenceL,            "PlayToneSequenceL");
_LIT(KCmdRecordData,                   "RecordData");
_LIT(KCmdRecordInitL,                  "RecordInitL");
_LIT(KCmdStop,                         "Stop");
_LIT(KCmdErrorConceal,                 "ErrorConceal");
_LIT(KCmdSetVolumeLoop,                "SetVolumeLoop");
/*@}*/

/*@{*/
//Utility literals
_LIT(KCmdUtilityReplaceFile,                  "ReplaceFile");
_LIT(KCmdUtilityOpenFile,                     "OpenFile");
_LIT(KCmdUtilityCloseFile,                    "CloseFile");
_LIT(KCmdUtilityFileStartPosition,            "FileStartPosition");
/*@}*/

/*@{*/
//INI Section name literals
_LIT(KGainValue,                       "Gain");
_LIT(KVolumeValue,                     "Volume");
_LIT(KLeftPercentage,                  "LeftPercentage");
_LIT(KRightPercentage,                 "RightPercentage");
_LIT(KSequenceNumber,                  "SequenceNumber");
_LIT(KRate,                            "Rate");
_LIT(KEncoding,                        "Encoding");
_LIT(KChannels,                        "Channels");
_LIT(KBufferSize,                      "BufferSize");
_LIT(KToneOnLength,                    "ToneOnLength");
_LIT(KToneOffLength,                   "ToneOffLength");
_LIT(KPause,                           "Pause");
_LIT(KPauseLength,                     "PauseLength");
_LIT(KPriority,                        "Priority");
_LIT(KPreference,                      "Preference");
_LIT(KPriorityState,                   "PriorityState");
_LIT(KRepeatCount,                     "RepeatCount");
_LIT(KRepeatTrailingSilence,           "RepeatTrailingSilence");
_LIT(KRampDuration,                    "RampDuration");
_LIT(KMode,                            "Mode");
_LIT(KHwDevice,                        "HwDevice");
_LIT(KFourCC,                          "FourCC");
_LIT(KFilename,                        "Filename");
_LIT(KDTMFString,                      "DTMFString");
_LIT(KFrequency,                       "Frequency");
_LIT(KFrequencyOne,                    "FrequencyOne");
_LIT(KFrequencyTwo,                    "FrequencyTwo");
_LIT(KDuration,                        "Duration");
_LIT(KLoop,                            "Loop");
_LIT(KAllowedError,                    "AllowedError");
_LIT(KVolumeList,                      "VolumeList");
_LIT(KVolumeLoop,                      "VolumeLoop");
/*@}*/

/*@{*/
//Literals for iEnumChannels
_LIT(KEMMFStereo, "EMMFStereo");
_LIT(KEMMFMono,   "EMMFMono");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CMMFDevSoundData::iEnumChannels[] =
	{
	{KEMMFMono,   EMMFMono},
	{KEMMFStereo, EMMFStereo}
	};

/*@{*/
//Literals for iEnumSampleRate
_LIT(KEMMFSampleRate8000Hz,  "EMMFSampleRate8000Hz");
_LIT(KEMMFSampleRate11025Hz, "EMMFSampleRate11025Hz");
_LIT(KEMMFSampleRate16000Hz, "EMMFSampleRate16000Hz");
_LIT(KEMMFSampleRate22050Hz, "EMMFSampleRate22050Hz");
_LIT(KEMMFSampleRate32000Hz, "EMMFSampleRate32000Hz");
_LIT(KEMMFSampleRate44100Hz, "EMMFSampleRate44100Hz");
_LIT(KEMMFSampleRate48000Hz, "EMMFSampleRate48000Hz");
_LIT(KEMMFSampleRate88200Hz, "EMMFSampleRate88200Hz");
_LIT(KEMMFSampleRate96000Hz, "EMMFSampleRate96000Hz");
_LIT(KEMMFSampleRate12000Hz, "EMMFSampleRate12000Hz");
_LIT(KEMMFSampleRate24000Hz, "EMMFSampleRate24000Hz");
_LIT(KEMMFSampleRate64000Hz, "EMMFSampleRate64000Hz");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CMMFDevSoundData::iEnumSampleRate[] =
	{
	{KEMMFSampleRate8000Hz,  EMMFSampleRate8000Hz},
	{KEMMFSampleRate11025Hz, EMMFSampleRate11025Hz},
	{KEMMFSampleRate16000Hz, EMMFSampleRate16000Hz},
	{KEMMFSampleRate22050Hz, EMMFSampleRate22050Hz},
	{KEMMFSampleRate32000Hz, EMMFSampleRate32000Hz},
	{KEMMFSampleRate44100Hz, EMMFSampleRate44100Hz},
	{KEMMFSampleRate48000Hz, EMMFSampleRate48000Hz},
	{KEMMFSampleRate88200Hz, EMMFSampleRate88200Hz},
	{KEMMFSampleRate96000Hz, EMMFSampleRate96000Hz},
	{KEMMFSampleRate12000Hz, EMMFSampleRate12000Hz},
	{KEMMFSampleRate24000Hz, EMMFSampleRate24000Hz},
	{KEMMFSampleRate64000Hz, EMMFSampleRate64000Hz}
	};

/*@{*/
//Literal for iEnumMode
_LIT(KEMMFStateIdle,             "EMMFStateIdle");
_LIT(KEMMFStatePlaying,          "EMMFStatePlaying");
_LIT(KEMMFStateTonePlaying,      "EMMFStateTonePlaying");
_LIT(KEMMFStateRecording,        "EMMFStateRecording");
_LIT(KEMMFStatePlayingRecording, "EMMFStatePlayingRecording");
_LIT(KEMMFStateConverting,       "EMMFStateConverting");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CMMFDevSoundData::iEnumMode[] =
	{
	{KEMMFStateIdle,             EMMFStateIdle},
	{KEMMFStatePlaying,          EMMFStatePlaying},
	{KEMMFStateRecording,        EMMFStateRecording},
	{KEMMFStateTonePlaying,      EMMFStateTonePlaying},
	{KEMMFStatePlayingRecording, EMMFStatePlayingRecording},
	{KEMMFStateConverting,       EMMFStateConverting}
	};

/*@{*/
//Literals for iEnumInitializeLType
_LIT(KEInitializeState,          "EInitializeState");
_LIT(KEInitializeHwDeviceMode,   "EInitializeHwDeviceMode");
_LIT(KEInitializeFourCCMode,     "EInitializeFourCCMode");
/*@}*/

enum TMMFInitializeMode
	{
	EInitializeState,
	EInitializeHwDeviceMode,
	EInitializeFourCCMode
	};

const CDataWrapperBase::TEnumEntryTable CT_CMMFDevSoundData::iEnumInitializeLType[] =
	{
	{KEInitializeState,        EInitializeState},
	{KEInitializeHwDeviceMode, EInitializeHwDeviceMode},
	{KEInitializeFourCCMode,   EInitializeFourCCMode}
	};

/**
 * Two phase constructor
 *
 * @leave   system wide error
 */
CT_CMMFDevSoundData* CT_CMMFDevSoundData::NewL()
	{
	CT_CMMFDevSoundData* ret = new (ELeave) CT_CMMFDevSoundData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Private constructor. First phase construction
 */
CT_CMMFDevSoundData::CT_CMMFDevSoundData()
	:
	iDevSound(NULL),
	iPrioritySettings(),
	iCapabilities(),
	iVolumeValue(0),
	iGainValue(0),
	iLeftPercentage(0),
	iRightPercentage(0),
	iFs(),
	iToneFile(),
	iAudiofile(),
	iBuffer(NULL),
	iLastSample(0),
	iPaused(EFalse),
	iErrorConceal(EFalse),
	iEndFile(EFalse),
	iToneSequence(),
	iRecordErrorIndex(0),
	iPlayErrorIndex(0),
	iToneErrorIndex(0),
    iInitializeErrorIndex(0)
	{
	}

/**
 * Second phase construction
 * @internalComponent
 * @return  N/A
 * @pre     None
 * @post    None
 * @leave   system wide error
 */
void CT_CMMFDevSoundData::ConstructL()
	{
	iFs.Connect();
	}

/**
 * Public destructor
 */
CT_CMMFDevSoundData::~CT_CMMFDevSoundData()
	{
	iFs.Close();
	DestroyData();
	}

/**
 * Helper Destructor
 */
void CT_CMMFDevSoundData::DestroyData()
	{
	if(iDevSound)
		{
		delete iDevSound;
		iDevSound = NULL;
		}
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return  pointer to the object that the data wraps
 */
TAny* CT_CMMFDevSoundData::GetObject()
	{
	return iDevSound;
	}

/**
 * Process a command read from the Ini file
 * @param aCommand          - The command to process
 * @param aSection          - The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex  - Command index dor async calls to returns errors to
 * @return TBool            - ETrue if the command is process
 * @leave                   - system wide error
 */
TBool CT_CMMFDevSoundData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;

	if (aCommand == KCmdNewL)
		{
		DoCmdNewL();
		}
	else if(aCommand == KCmdNewLLoopL)
		{
		DoCmdNewLLoopL(aSection);
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else if (aCommand == KCmdUtilityReplaceFile)
		{
		DoCmdUtilityReplaceFile(aSection);
		}
	else if (aCommand == KCmdUtilityOpenFile)
		{
		DoCmdUtilityOpenFile(aSection);
		}
	else if (aCommand == KCmdUtilityCloseFile)
		{
		DoCmdUtilityCloseFile();
		}
	else if (aCommand == KCmdUtilityFileStartPosition)
		{
		DoCmdUtilityFileStartPosition();
		}
	else if (aCommand == KCmdCapabilities)
		{
		DoCmdCapabilities();
		}
	else if (aCommand == KCmdConfig)
		{
		DoCmdConfig();
		}
	else if (aCommand == KCmdMaxGain)
		{
		DoCmdMaxGain(aSection);
		}
	else if (aCommand == KCmdMaxVolume)
		{
		DoCmdMaxVolume(aSection);
		}
	else if (aCommand == KCmdGetPlayBalanceL)
		{
		DoCmdGetPlayBalanceL(aSection);
		}
	else if (aCommand == KCmdGetRecordBalanceL)
		{
		DoCmdGetRecordBalanceL(aSection);
		}
	else if (aCommand == KCmdGetSupportedInputDataTypesL)
		{
		DoCmdGetSupportedInputDataTypesL();
		}
	else if (aCommand == KCmdGetSupportedOutputDataTypesL)
		{
		DoCmdGetSupportedOutputDataTypesL();
		}
	else if (aCommand == KCmdSamplesPlayed)
		{
		DoCmdSamplesPlayed();
		}
	else if (aCommand == KCmdSamplesRecorded)
		{
		DoCmdSamplesRecorded();
		}
	else if (aCommand == KCmdVolume)
		{
		DoCmdVolume(aSection);
		}
	else if (aCommand == KCmdGain)
		{
		DoCmdGain(aSection);
		}
	else if (aCommand == KCmdFixedSequenceCount)
		{
		DoCmdFixedSequenceCount();
		}
	else if (aCommand == KCmdFixedSequenceName)
		{
		DoCmdFixedSequenceName(aSection);
		}
	else if (aCommand == KCmdSetConfigL)
		{
		DoCmdSetConfigL(aSection);
		}
	else if (aCommand == KCmdSetDTMFLengths)
		{
		DoCmdSetDTMFLengths(aSection);
		}
	else if (aCommand == KCmdSetGain)
		{
		DoCmdSetGain(aSection);
		}
	else if (aCommand == KCmdSetPlayBalanceL)
		{
		DoCmdSetPlayBalanceL(aSection);
		}
	else if (aCommand == KCmdSetPrioritySettings)
		{
		DoCmdSetPrioritySettings(aSection);
		}
	else if (aCommand == KCmdSetRecordBalanceL)
		{
		DoCmdSetRecordBalanceL(aSection);
		}
	else if (aCommand == KCmdSetToneRepeats)
		{
		DoCmdSetToneRepeats(aSection);
		}
	else if (aCommand == KCmdSetVolume)
		{
		DoCmdSetVolume(aSection);
		}
	else if (aCommand == KCmdSetVolumeLoop)
		{
		DoCmdSetVolumeLoop(aSection);
		}
	else if (aCommand == KCmdSetVolumeRamp)
		{
		DoCmdSetVolumeRamp(aSection);
		}
	else if (aCommand == KCmdInitializeL)
		{
		DoCmdInitializeL(aSection,aAsyncErrorIndex);
		}
	else if (aCommand == KCmdPlayDTMFStringL)
		{
		DoCmdPlayDTMFStringL(aSection, aAsyncErrorIndex);
		}
	else if (aCommand == KCmdPlayData)
		{
		DoCmdPlayData();
		}
	else if (aCommand == KCmdPlayDualToneL)
		{
		DoCmdPlayDualToneL(aSection, aAsyncErrorIndex);
		}
	else if (aCommand == KCmdPlayInitL)
		{
		DoCmdPlayInitL(aAsyncErrorIndex);
		}
	else if (aCommand == KCmdPlayToneL)
		{
		DoCmdPlayToneL(aSection, aAsyncErrorIndex);
		}
	else if (aCommand == KCmdPlayToneSequenceL)
		{
		DoCmdPlayToneSequenceL(aSection,aAsyncErrorIndex);
		}
	else if (aCommand == KCmdRecordData)
		{
		DoCmdRecordData();
		}
	else if (aCommand == KCmdRecordInitL)
		{
		DoCmdRecordInitL(aAsyncErrorIndex);
		}
	else if (aCommand == KCmdStop)
		{
		DoCmdStop(aSection);
		}
	else if(aCommand == KCmdErrorConceal)
		{
		DoCmdUtilityErrorConceal();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret=EFalse;
		}
	return ret;
	}

/**
 * Handles initialization completion event.
 * CMMFDevSound object calls this function when its InitializeL() function
 * completes.
 * @param  aError - Error code. KErrNone if successful. Other values are possible
 *                  indicating a problem initializing CMMFDevSound object.
 * @return void
 */

void CT_CMMFDevSoundData::InitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::InitializeComplete"));
	if(KErrNone != aError)
		{
		ERR_PRINTF2(_L("InitializeComplete callback failed with error %d"), aError);
		SetAsyncError(iInitializeErrorIndex, aError);
        iInitializeErrorIndex = 0;
		}
	if(Outstanding())
	{
    DecOutstanding();
	}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::InitializeComplete"));
	}

/**
 * It's needed for playing o converting
 * @param aBuffer - Buffer into which data should be read. The amount of data that is
 *                  needed is specified in CMMFBuffer::RequestSize().
 */
void CT_CMMFDevSoundData::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
		SetBlockResult(EFail);
		}
	else
		{
		TBool dataOk = ETrue;
		iBuffer = aBuffer;
		iBuffer->SetLastBuffer(EFalse);
		CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
		if (iPaused)
			{
			iPaused=EFalse;
			TInt error = iAudiofile.Read(iLastSample,buffer->Data());
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), error);
				SetBlockResult(EFail);
				dataOk = EFalse;
				}
			}
		else
			{
			TInt error = iAudiofile.Read(buffer->Data());
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), error);
				SetBlockResult(EFail);
				dataOk =EFalse;
				}
			}
		if(dataOk)
			{
			if (buffer->Data().Length() != buffer->RequestSize())
				{
				iBuffer->SetLastBuffer(ETrue);
				}
			if (iErrorConceal)
				{
				iErrorConceal=EFalse;
				buffer->Data().Zero();
				INFO_PRINTF1(_L("buffer->Data().Zero()"));
				}
			PlayData();
			TInt pos=0;
			iLastSample=iAudiofile.Seek(ESeekCurrent ,pos);
			INFO_PRINTF2(_L("Number of samples played so far: %d"), pos);
			iLastSample=pos;
			}
		}
	}

/**
* Handles play completion or cancel event.
*
* CMMFDevSound object calls this function when an attempt to play audio sample
* has completed, successfully or otherwise.
*
*   @param  aError - Error code. The status of playback. KErrUnderflow playing of the
*                    audio sample is complete. KErrAccessDenied the sound device is in
*                    use by another higher priority client.
*/
void CT_CMMFDevSoundData::PlayError(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::PlayError"));
	if(Outstanding())
	{
    DecOutstanding();
	}
	if(aError != KErrNone && aError != KErrUnderflow)
	{
   	ERR_PRINTF2(_L("Playback completed with error %d"), aError);
		SetAsyncError(iPlayErrorIndex, aError);	
		iPlayErrorIndex = 0; 
	}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::PlayError"));
	}


/**
 *  Handles tone play completion event.
 *
 *
 *  CMMFDevSound object calls this function when an attempt to play tone has
 *  completed, successfully or otherwise.
 *
 *  The following are the play tone functions; PlayToneL(), PlayDMTFStringL(),
 *  PlayToneSequenceL(), and PlayFixedSequenceL().
 *
 *  @param  aError -  Error code. The status of tone playback. KErrUnderflow playing of
 *                    the tone is complete. KErrAccessDenied the sound device is in use by
 *                    another higher priority client. KErrCancel playing of the audio
 *                    sample is stopped by DevSound client another higher priority client.
 *
 */

void CT_CMMFDevSoundData::ToneFinished(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::ToneFinished"));
	if(Outstanding())
	{
    DecOutstanding();
	}
	if(aError != KErrNone)
	{
   	ERR_PRINTF2(_L("Tone finished with error %d"), aError);
		SetAsyncError(iToneErrorIndex, aError);
	  iToneErrorIndex = 0;	
	}
	iToneFile.Close();
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::ToneFinished"));
	}

/**
 * Handles CMMFDevSound object's data request event.
 *
 * CMMFDevSound object calls this function when the buffer, aBuffer gets filled
 * while recording or converting. The observer should notify CMMFDevSound
 * object as quickly as possible after data in the buffer is processed by
 * calling RecordData(), otherwise the implementation might callback
 * the function RecordError() on derived class object with error code KErrOverflow.
 *
 * @param  aBuffer - Buffer containing processed (recorded or converted) data. The amount
 *                   of data that is available is specified in CMMFBuffer::RequestSize().
 */

void CT_CMMFDevSoundData::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
		SetBlockResult(EFail);
		}
	else
		{
		iBuffer = aBuffer;
		CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
		TInt error = iAudiofile.Write(buffer->Data());
		if(error == KErrNone)
			{
			if (!aBuffer->LastBuffer())
				{
				RecordData();
				}
			else{
				Stop();
				}
			}
		else
			{
			ERR_PRINTF2(_L("iFile.Write() returned the error %d"), error);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Stops the ongoing operation (Play, Record, TonePlay, Convert).
 * This function should be synchronous and invoke no callbacks through MDevSoundObserver.
 */
void CT_CMMFDevSoundData::Stop()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::Stop()"));
	iDevSound->Stop();
	if(Outstanding())
		{
		DecOutstanding();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::Stop()"));
	}

/**
 * Handles record completion or cancel event.
 *
 * CMMFDevSound object calls this function when an attempt to record audio sample
 * has completed, successfully or otherwise.
 *
 * @param  aError - Error code. The status of recording. KErrOverflow audio devices
 *                  runs out of internal buffer. KErrAccessDenied the sound device is
 *                  in use by another higher priority client.
 *
 */

void CT_CMMFDevSoundData::RecordError(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::RecordError"));
	if(aError != KErrNone)
	{
   ERR_PRINTF2(_L("Record failed with error %d"), aError); 
   SetAsyncError(iRecordErrorIndex , aError);	
	 iRecordErrorIndex = 0;	
	}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::RecordError"));
	}


/**
 * Handles conversion completion or cancel event.
 *
 * CMMFDevSound object calls this function when an attempt to convert data from
 * source format to destination format has completed, successfully or otherwise.
 *
 * @param  aError - Error code. KErrCancel conversion operation is cancelled. KErrNone
 *                  conversion is complete. Other values are possible indicating a
 *                  problem converting data.
 */

void CT_CMMFDevSoundData::ConvertError(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::ConvertError"));
	ERR_PRINTF1(_L("Invalid Callback call"));
	if (aError == KErrNone)
		{
		INFO_PRINTF1(_L("Data conversion completed successfully."));
		}
	if (aError == KErrCancel)
		{
		INFO_PRINTF1(_L("Data conversion operation was cancelled."));
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::ConvertError"));
	}

/**
 * Handles device event.
 *
 * CMMFDevSound object calls this function when a message is received from the
 * audio hardware device.
 *
 * @param   aMessageType - Defines the type of message. Used to determine how to
 *                         interpret the contents of aMsg.
 * @param   aMsg         - Message that is packed in the Descriptor format.
 */


void CT_CMMFDevSoundData::DeviceMessage(TUid aMessageType, const TDesC8& aMsg)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DeviceMessage"));
	INFO_PRINTF2(_L("Message type: %d"), aMessageType.iUid);
	INFO_PRINTF2(_L("Message type: %S"), &aMsg);
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DeviceMessage"));
	}


/**
 * Create a new or Replace the existing file
 *
 */
void CT_CMMFDevSoundData::DoCmdUtilityReplaceFile(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CMMFDevSoundData::DoCmdUtilityReplaceFile()"));
	TPtrC fileName;
	TBool dataOk = ETrue;
	if (!GetStringFromConfig(aSection, KFilename, fileName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFilename);
		SetBlockResult(EFail);
		dataOk =EFalse;
		}
	if(dataOk)
		{
		TInt error = iAudiofile.Replace(iFs, fileName, EFileRead);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Could not open input file. Error = %d"), error);
			SetError(error);
			}
		}

	INFO_PRINTF1(_L("*END* CT_CMMFDevSoundData::DoCmdUtilityReplaceFile()"));
	}

/**
 * Open a file
 */
void CT_CMMFDevSoundData::DoCmdUtilityOpenFile(const TTEFSectionName& aSection)
	{

	INFO_PRINTF1(_L("*START* CT_CMMFDevSoundData::DoCmdUtilityOpenFile()"));
	TPtrC fileName;
	TBool dataOk = ETrue;
	if (!GetStringFromConfig(aSection, KFilename, fileName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFilename);
		SetBlockResult(EFail);
		dataOk =EFalse;
		}
	if(dataOk)
		{
		TInt error = iAudiofile.Open(iFs, fileName, EFileRead);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Could not open input file. Error = %d"), error);
			iFs.Close();
			SetError(error);
			}
		}
	INFO_PRINTF1(_L("*END* CT_CMMFDevSoundData::DoCmdUtilityOpenFile()"));
	}

/**
 * Close the opened file
 */
void CT_CMMFDevSoundData::DoCmdUtilityCloseFile()
	{
	INFO_PRINTF1(_L("*START* CT_CMMFDevSoundData::DoCmdUtilityCloseFile()"));
	iAudiofile.Close();
	INFO_PRINTF1(_L("*END* CT_CMMFDevSoundData::DoCmdUtilityCloseFile()"));
	}

/**
 * Place the the pointer file at the begining
 *
 */
void CT_CMMFDevSoundData::DoCmdUtilityFileStartPosition()
	{
	INFO_PRINTF1(_L("*START* CT_CMMFDevSoundData::DoCmdUtilityFileStartPosition()"));
	TInt filePosition = 0;
	TBool dataOk = ETrue;
	TInt error = iAudiofile.Seek(ESeekStart, filePosition);
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Could not set file to the start position. Error = %d"), error);
		SetError(error);
		dataOk =EFalse;
		}
	if(dataOk)
		{
		INFO_PRINTF2(_L("File Position: %d"), filePosition);
		}
	INFO_PRINTF1(_L("*END* CT_CMMFDevSoundData::DoCmdUtilityFileStartPosition()"));
	}


/**
 * Creates "n" instance of CMMFDevSound
 * @param aSection - Section to read from the ini file
 * @return void
 */
void CT_CMMFDevSoundData::DoCmdNewLLoopL(const TTEFSectionName&aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdNewLLoopL"));
	CMMFDevSound* devSound = NULL;
	TBool dataOk = ETrue;
	TInt loop= 0;
	if (!GetIntFromConfig(aSection, KLoop, loop))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLoop);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt allowedError=0;
	if ( !GetIntFromConfig(aSection, KAllowedError, allowedError) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAllowedError);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		for(TInt i=0; i<loop; i++)
			{
			TRAPD(error,devSound = CMMFDevSound::NewL());
			if(error == KErrNone)
				{
				INFO_PRINTF2(_L("Instance %d of DevSound was created"),i);
				CleanupStack::PushL(devSound);
				iObjectsDevSound.AppendL(devSound);
				CleanupStack::Pop(devSound);
				}
			else if(error == allowedError)
				{
				INFO_PRINTF2(_L(" Allowing Error: %d"), error);
				ERR_PRINTF2(_L(" Could not create CMMFDevSound due to error: %d"), error);
				i = loop;
				}
			else if((error != KErrNone) && (error!=allowedError))
				{
				ERR_PRINTF2(_L("> Could not create CMMFDevSound: %d"), error);
				SetError(error);
				}
			}
		iObjectsDevSound.ResetAndDestroy();
		INFO_PRINTF2(_L("%d DevSound instances were deleted"),loop);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdNewLLoopL"));
	}


/**
 * Create an instance of CMMFDevSound
 */
void CT_CMMFDevSoundData::DoCmdNewL()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdNewL"));
	DestroyData();
	TRAPD(error,iDevSound = CMMFDevSound::NewL());
	if(error!= KErrNone)
		{
		ERR_PRINTF2(_L("> Could not create CMMFDevSound: %d"), error);
		SetError(error);
		}
	else
		{
		iPaused= EFalse;
		iErrorConceal = EFalse;
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdNewL"));
	}
/**
 * Destroy an instance of CMMFDevSound
 */

void CT_CMMFDevSoundData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdDestroyData"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdDestroyData"));
	}

/**
 * Show the supported Audio settings ie. encoding, sample rates, mono/stereo operation, buffer
 * size, etc.
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdCapabilities()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdCapabilities"));
	iCapabilities = iDevSound->Capabilities();
	INFO_PRINTF5(_L("Supported Audio settings: buffsize=%d, channels=0x%x, encoding=0x%x and rate=0x%x"),
							iCapabilities.iBufferSize,
							iCapabilities.iChannels,
							iCapabilities.iEncoding,
							iCapabilities.iRate);
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdCapabilities"));
	}


/**
 * Shows the current device configuration.
 * @params none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdConfig()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdConfig"));
	TMMFCapabilities currentConfiguration = iDevSound->Config();
	INFO_PRINTF5(_L("Current device configuration: buffsize=%d, channels=0x%x, encoding=0x%x and rate=0x%x"),
			currentConfiguration.iBufferSize,
			currentConfiguration.iChannels,
			currentConfiguration.iEncoding,
			currentConfiguration.iRate);
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdConfig"));
	}


/**
 * Shows an integer representing the maximum gain the device supports.
 * This is the maximum value which can be passed to CMMFDevSound::SetGain.
 * @param aSection - The section to read the params from the ini file
 * @return none
 */

void CT_CMMFDevSoundData::DoCmdMaxGain(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdMaxGain"));
	TBool dataOk = ETrue;
	TInt expectedGainValue=0;
	if (!GetIntFromConfig(aSection, KGainValue, expectedGainValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KGainValue);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iGainValue = iDevSound->MaxGain();
		INFO_PRINTF2(_L("Retrieved maximum gain value: %d"), iGainValue);
		if (expectedGainValue != iGainValue)
			{
			ERR_PRINTF3(_L("Expected gain value %d is different from retrieved gain value %d"), expectedGainValue, iGainValue);
			SetBlockResult(EFail);
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdMaxGain"));
	}

/**
 * Shows an integer representing the maximum volume device supports.
 * This is the maximum value which can be passed to CMMFDevSound::SetVolume.
 * @param aSection - Section to read params from the ini file
 * @return none
 */

void CT_CMMFDevSoundData::DoCmdMaxVolume(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdMaxVolume"));
	TBool dataOk = ETrue;
	TInt expectedVolumeValue=0;
	if(!GetIntFromConfig(aSection, KVolumeValue, expectedVolumeValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVolumeValue);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iVolumeValue = iDevSound->MaxVolume();
		INFO_PRINTF2(_L("Retrieved maximum volume value: %d"), iVolumeValue);
		if(expectedVolumeValue != iVolumeValue)
			{
			ERR_PRINTF3(_L("Expected volume value %d is different from retrieved volume value %d"), expectedVolumeValue, iVolumeValue);
			SetBlockResult(EFail);
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdMaxVolume"));
	}

/**
 * Shows the speaker balance set for playing.
 * @param aSection - Section to read params from the ini file
 * @return none
 */

void CT_CMMFDevSoundData::DoCmdGetPlayBalanceL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdGetPlayBalanceL"));
	TBool dataOk = ETrue;
	TInt expectedLeftPercentage=0;
	if (!GetIntFromConfig(aSection, KLeftPercentage, expectedLeftPercentage))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLeftPercentage);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt expectedRightPercentage=0;
	if (!GetIntFromConfig(aSection, KRightPercentage, expectedRightPercentage))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRightPercentage);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TRAPD(error, iDevSound->GetPlayBalanceL(iLeftPercentage, iRightPercentage));
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("GetPlayBalanceL left with error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF3(_L("> Retrieved play balance values are left: %d, right:%d"), iLeftPercentage, iRightPercentage);
			if (expectedLeftPercentage !=iLeftPercentage)
				{
				ERR_PRINTF3(_L("Expected left percentage value %d is different from retrieved value %d"), expectedLeftPercentage, iLeftPercentage);
				SetBlockResult(EFail);
				}
			if (expectedRightPercentage !=iRightPercentage)
				{
				ERR_PRINTF3(_L("Expected right percentage value %d is different from retrieved value %d"), expectedRightPercentage, iRightPercentage);
				SetBlockResult(EFail);
				}
			}
		}

	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdGetPlayBalanceL"));
	}


/**
 * Shows the microphone gain balance set for recording.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdGetRecordBalanceL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdGetRecordBalanceL"));
	TBool dataOk = ETrue;
	TInt expectedLeftPercentage=0;
	if(!GetIntFromConfig(aSection, KLeftPercentage, expectedLeftPercentage))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLeftPercentage);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt expectedRightPercentage=0;
	if(!GetIntFromConfig(aSection, KRightPercentage, expectedRightPercentage))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRightPercentage);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TRAPD(error, iDevSound->GetRecordBalanceL(iLeftPercentage, iRightPercentage));
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("GetRecordBalanceL left with error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF3(_L("> Retrieved record balance values are left: %d, right:%d"), iLeftPercentage, iRightPercentage);
			if(expectedLeftPercentage !=iLeftPercentage)
				{
				ERR_PRINTF3(_L("Expected left percentage value %d is different from retrieved value %d"), expectedLeftPercentage, iLeftPercentage);
				SetBlockResult(EFail);
				}
			if(expectedRightPercentage !=iRightPercentage)
				{
				ERR_PRINTF3(_L("Expected right ercentage value %d is different from retrieved value %d"), expectedRightPercentage, iRightPercentage);
				SetBlockResult(EFail);
				}
			}

		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdGetRecordBalanceL"));
	}

/**
 * Shows a list of the supported input datatypes that can be sent to DevSound for playing audio.
 * The datatypes returned are those that the DevSound supports given the priority settings passed
 * in aPrioritySettings.
 * @params none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdGetSupportedInputDataTypesL()
	{
	TUid outputUid= {0x0};
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdGetSupportedInputDataTypesL"));
	RArray< TFourCC > supportedDataTypes;
	TRAPD(error, iDevSound->GetSupportedInputDataTypesL(supportedDataTypes, iPrioritySettings));
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("GetSupportedInputDataTypesL left with error %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported input datatypes that can be sent to DevSound for playing audio."));
		for (TInt i=0; i<supportedDataTypes.Count(); i++)
			{
			TFourCC fourCC = supportedDataTypes[i];
			TBuf<4> name;
			for(TInt j=0; j<= KFourCCMaxBitDisplacement; j+=K8Bits)
				{
				name.Append((TUint8)(fourCC.FourCC() >> j));
				}
			INFO_PRINTF3(_L("Supported Input Data types: 0x%x  %S "), fourCC.FourCC(), &name);
			UtilityFourCCToHwDeviceUidL(name,EDecoder, outputUid);
			if(outputUid.iUid != 0)
				{
				INFO_PRINTF3(_L("\nHwDevice Uid=0x%08x for %S"),outputUid.iUid,&name);
				}
			else
				{
				INFO_PRINTF1(_L("Not Found"));
				}
			}
		supportedDataTypes.Reset();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdGetSupportedInputDataTypesL"));
	}

/**
 * Shows a list of the supported output dataypes that can be received from DevSound for
 * recording audio.  The datatypes returned are those that the DevSound supports given the priority
 * settings passed in aPrioritySettings.
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdGetSupportedOutputDataTypesL()
	{
	TUid outputUid= {0x0};
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdGetSupportedOutputDataTypesL"));
	RArray< TFourCC > supportedDataTypes;
	TRAPD(error, iDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, iPrioritySettings));
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("GetSupportedOutputDataTypesL left with error %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF1(_L("List of the supported output dataypes that can be received from DevSound for recording audio."));
		for (TInt i=0; i<supportedDataTypes.Count(); i++)
			{
			TFourCC fourCC = supportedDataTypes[i];
			TBuf<4> name;
			for(TInt j=0; j<=KFourCCMaxBitDisplacement; j+=K8Bits)
				{
				name.Append((TUint8)(fourCC.FourCC() >> j));
				}
			INFO_PRINTF3(_L("Supported Output Data types: 0x%x  %S "), fourCC.FourCC(), &name);
			UtilityFourCCToHwDeviceUidL(name, EEncoder, outputUid);
			if(outputUid.iUid != 0)
				{
				INFO_PRINTF3(_L("\nHwDevice Uid=0x%08x for %S"),outputUid.iUid,&name);
				}
			else
				{
				INFO_PRINTF1(_L("Not Found"));
				}
			}
		supportedDataTypes.Reset();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdGetSupportedOutputDataTypesL"));
	}

/**
 * Shows the number of samples played
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSamplesPlayed()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSamplesPlayed"));
	INFO_PRINTF2(_L("Number of samples played so far: %d"),iDevSound->SamplesPlayed());
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSamplesPlayed"));
	}

/**
 * Shows the number of samples recorded
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSamplesRecorded()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSamplesRecorded"));
	INFO_PRINTF2(_L("Number of samples recorded so far: %d"), iDevSound->SamplesRecorded());
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSamplesRecorded"));
	}

/**
 * Shows an integer representing the current volume.
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdVolume(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdVolume"));
	TBool dataOk = ETrue;
	TInt expectedVolumeValue=0;
	if(!GetIntFromConfig(aSection, KVolumeValue, expectedVolumeValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVolumeValue);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iVolumeValue = iDevSound->Volume();
		INFO_PRINTF2(_L("Retrieved volume value: %d"), iVolumeValue);
		if(expectedVolumeValue != iVolumeValue)
			{
			ERR_PRINTF3(_L("Expected volume value %d is different from retrieved volume value %d"), expectedVolumeValue, iVolumeValue);
			SetBlockResult(EFail);
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdVolume"));
	}

/**
 * Shows an integer representing the current gain.
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdGain(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdGain"));
	TBool dataOk = ETrue;
	TInt expectedGainValue=0;
	if(!GetIntFromConfig(aSection, KGainValue, expectedGainValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KGainValue);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iGainValue = iDevSound->Gain();
		INFO_PRINTF2(_L("Retrieved gain value: %d"), iGainValue);
		if(expectedGainValue != iGainValue)
			{
			ERR_PRINTF3(_L("Expected gain value %d is different from retrieved gain value %d"), expectedGainValue, iGainValue);
			SetBlockResult(EFail);
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdGain"));
	}

/**
 * Shows the number of available pre-defined tone sequences
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdFixedSequenceCount()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdFixedSequenceCount"));
	INFO_PRINTF2(_L("Number of available pre-defined tone sequences: %d"), iDevSound->FixedSequenceCount());
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdFixedSequenceCount"));
	}

/**
 * Shows the name assigned to a specific pre-defined tone sequence.
 * This is the number of the fixed sequence supported by DevSound by default.
 * @param aSection -  Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdFixedSequenceName(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdFixedSequenceName"));
	TBool dataOk =ETrue;
	TInt sequenceNumber=0;
	if(!GetIntFromConfig(aSection, KSequenceNumber, sequenceNumber))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSequenceNumber);
		SetBlockResult(EFail);
		dataOk =EFalse;
		}
	if(dataOk)
		{
		INFO_PRINTF2(_L("Name assigned to a specific pre-defined tone sequence returned: %S"),
					&iDevSound->FixedSequenceName(sequenceNumber));
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdFixedSequenceName"));
	}


/**
 * Configure CMMFDevSound object with the settings in aConfig.
 * Use this to set sampling rate, encoding and mono/stereo.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetConfigL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetConfigL"));

	TInt rate=0;
	INFO_PRINTF1(_L("To use default values run Capabilities before SetConfigL"));
	if (!GetEnumFromConfig(aSection, KRate, iEnumSampleRate, rate))
		{
		INFO_PRINTF2(_L("Rate value was not overwritten, using default value %d."), iCapabilities.iRate);
		}
	else
		{
		iCapabilities.iRate = rate;
		}

	TInt encoding=0;
	if (!GetIntFromConfig(aSection, KEncoding, encoding))
		{
		INFO_PRINTF2(_L("Encoding value was not overwritten, using default value %d."), iCapabilities.iEncoding);
		}
	else
		{
		iCapabilities.iEncoding = encoding;
		}

	TInt channels=0;
	if (!GetEnumFromConfig(aSection, KChannels, iEnumChannels, channels))
		{
		INFO_PRINTF2(_L("Channels value was not overwritten, using default value %d ."), iCapabilities.iChannels);
		}
	else
		{
		iCapabilities.iChannels = channels;
		}

	TInt bufferSize=0;
	if (!GetIntFromConfig(aSection, KBufferSize, bufferSize))
		{
		INFO_PRINTF2(_L("BufferSize value was not overwritten, using default value %d"), iCapabilities.iBufferSize);
		}
	else
		{
		iCapabilities.iBufferSize = bufferSize;
		}
	TRAPD(error,iDevSound->SetConfigL(iCapabilities))
	if(error != KErrNone)
		{
		ERR_PRINTF2(_L("SetConfigL left with error %d"), error);
		SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetConfigL"));
	}


/**
 * Defines the duration of tone on, tone off and tone pause to be used during the
 * DTMF tone playback operation.
 * Supported only during tone playing.
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetDTMFLengths(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetDTMFLengths"));
	TInt time=0;
	TBool dataOk = ETrue;
	if (!GetIntFromConfig(aSection, KToneOnLength, time))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KToneOnLength);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TTimeIntervalMicroSeconds32 toneOnLength(time);

	if (!GetIntFromConfig(aSection, KToneOffLength, time))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KToneOffLength);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TTimeIntervalMicroSeconds32 toneOffLength(time);

	if (!GetIntFromConfig(aSection, KPauseLength, time))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPauseLength);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TTimeIntervalMicroSeconds32 pauseLength(time);
	if(dataOk)
		{
		iDevSound->SetDTMFLengths(toneOnLength, toneOffLength, pauseLength);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetDTMFLengths"));
	}


/**
 * Changes the current recording gain to a specified value.
 * The gain can be changed before or during recording and is effective
 * immediately.
 * @params aSection - Section to read param from the ini file
 */
void CT_CMMFDevSoundData::DoCmdSetGain(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetGain"));
	TInt gainValue = iGainValue;
	if(!GetIntFromConfig(aSection, KGainValue, gainValue))
		{
		INFO_PRINTF1(_L("The Gain value was not found in the ini file, using default value"));
		}
	INFO_PRINTF2(_L("Setting gain value to %d"), gainValue);
	iDevSound->SetGain(gainValue);
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetGain"));
	}

/**
 * Sets the speaker balance for playing.
 * The speaker balance can be changed before or during playback and is
 * effective immediately.
 * @param aSection - The section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetPlayBalanceL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetPlayBalanceL"));
	TInt leftPercentage = iLeftPercentage;
	if(!GetIntFromConfig(aSection, KLeftPercentage, leftPercentage))
		{
		INFO_PRINTF1(_L("The Left percentage was not found in the ini file, using default value"));
		}
	TInt rightPercentage = iRightPercentage;
	if(!GetIntFromConfig(aSection, KRightPercentage, rightPercentage))
		{
		INFO_PRINTF1(_L("The Right percentage was not found in the ini file, using default value"));
		}
	INFO_PRINTF3(_L("Setting play balance to left: %d, right: %d values"), leftPercentage, rightPercentage);
	TRAPD(error, iDevSound->SetPlayBalanceL(leftPercentage, rightPercentage));
	if(error != KErrNone)
		{
		ERR_PRINTF2(_L("SetPlayBalanceL left with error %d"), error);
		SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetPlayBalanceL"));
	}

/**
 * Defines the priority settings that should be used for this instance.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetPrioritySettings(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetPrioritySettings"));
	TBool dataOk = ETrue;
	if(!GetIntFromConfig(aSection, KPriority, iPrioritySettings.iPriority))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPriority);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt preference=0;
	if (!GetHexFromConfig(aSection, KPreference, preference))
		{
		ERR_PRINTF2(_L("%S parameter was not found in INI file"), &KPreference);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	iPrioritySettings.iPref = (TMdaPriorityPreference)preference;
	TInt state=0;
	if (!GetEnumFromConfig(aSection, KPriorityState, iEnumMode, state))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPriorityState);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	iPrioritySettings.iState = (TMMFState)state;
	if(dataOk)
		{
		iDevSound->SetPrioritySettings(iPrioritySettings);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetPrioritySettings"));
	}

/**
 * Sets the microphone gain balance for recording.
 * The microphone gain balance can be changed before or during recording and
 * is effective immediately.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetRecordBalanceL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetRecordBalanceL"));
	TInt leftPercentage = iLeftPercentage;
	if (!GetIntFromConfig(aSection, KLeftPercentage, leftPercentage))
		{
		INFO_PRINTF1(_L("The Left value was not found in the ini file, using default value"));
		}
	TInt rightPercentage = iRightPercentage;
	if (!GetIntFromConfig(aSection, KRightPercentage, rightPercentage))
		{
		INFO_PRINTF1(_L("The Right value was not found in the ini file, using default value"));
		}
	INFO_PRINTF3(_L("Setting record balance to left: %d, right: %d values"), leftPercentage, rightPercentage);
	TRAPD(error, iDevSound->SetRecordBalanceL(leftPercentage, rightPercentage));
	if (KErrNone != error)
		{
		ERR_PRINTF2(_L("SetRecordBalanceL left with error %d"), error);
		SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetRecordBalanceL"));
	}

/**
 * Defines the number of times the audio is to be repeated during the tone
 * playback operation.
 * A period of silence can follow each playing of a tone. The tone playing can
 * be repeated indefinitely.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetToneRepeats(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetToneRepeats"));
	TInt repeatCount=0;
	TBool dataOk = ETrue;
	if (!GetIntFromConfig(aSection, KRepeatCount, repeatCount))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRepeatCount);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TInt tempValue=0;
	if (!GetIntFromConfig(aSection, KRepeatTrailingSilence, tempValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRepeatTrailingSilence);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TTimeIntervalMicroSeconds repeatTrailingSilence(tempValue);

	if(dataOk)
		{
		iDevSound->SetToneRepeats(repeatCount, repeatTrailingSilence);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetToneRepeats"));
	}

/**
 * Defines the period over which the volume level is to rise smoothly from
 * nothing to the normal volume level.
 * The function is only available while the tone is playing.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetVolume(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetVolume"));
	TInt volume = iVolumeValue;
	if(!GetIntFromConfig(aSection, KVolumeValue, volume))
		{
		INFO_PRINTF1(_L("The Volume value was not found in the ini file, using default value"));
		}
	INFO_PRINTF2(_L("Setting volume value to %d"), volume);
	iDevSound->SetVolume(volume);
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetVolume"));
	}

/**
 * Defines the loop over which the volume level is to rise smoothly from
 * nothing to the normal volume level.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetVolumeLoop(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetVolumeLoop"));
	TInt volLoop=0;
	TBool dataOk = ETrue;
	if (!GetIntFromConfig(aSection, KVolumeLoop, volLoop))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVolumeLoop);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TPtrC parVolume;
		TInt volumeValue=0;
		RPointerArray<HBufC> volumeList;
		for (TInt i=0; i<volLoop; i++)
			{
			if (!GetArrayRectFromConfig(aSection, KVolumeList, volumeList))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVolumeList);
				volumeList.ResetAndDestroy();
				SetBlockResult(EFail);
				dataOk = EFalse;
				}
			if(dataOk)
				{
				for (TInt j=0; j<volumeList.Count(); j++)
					{
					parVolume.Set(*volumeList[j]);
					TLex lex(parVolume);
					TInt error = lex.Val(volumeValue);
					if (error != KErrNone)
						{
						ERR_PRINTF2(_L("Obtaining volume value failed with error %d"), error);
						SetError(error);
						volumeList.ResetAndDestroy();
						}
					else
						{
						INFO_PRINTF3(_L("Setting volume number %d in the iteration number %d"), j, i);
						iDevSound->SetVolume(volumeValue);
						}
					}
				}
			}
		volumeList.ResetAndDestroy();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetVolumeLoop"));
	}

/**
 * Defines the period over which the volume level is to rise smoothly from
 * nothing to the normal volume level.
 * The function is only available while the tone is playing.
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdSetVolumeRamp(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdSetVolumeRamp"));
	TInt tempValue=0;
	TBool dataOk = ETrue;
	if (!GetIntFromConfig(aSection, KRampDuration, tempValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRampDuration);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TTimeIntervalMicroSeconds rampDuration(tempValue);

	if(dataOk)
		{
		iDevSound->SetVolumeRamp(rampDuration);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdSetVolumeRamp"));
	}

/**
 * Initializes DevSound object for the mode aMode for processing audio data
 * with hardware device aHWDev and supporting FourCC.
 * Too Initializes CMMFDevSound object to play and record PCM16 raw audio data
 * with sampling rate of 8 KHz.
 * @param aSection - Section to read params from the ini file
 */
void CT_CMMFDevSoundData::DoCmdInitializeL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdInitializeL"));
	TBool dataOk = ETrue;
	TInt mode=0;
	if (!GetEnumFromConfig(aSection, KMode, iEnumMode, mode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMode);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	else
		{
		TInt hwDeviceInt=0;
		if (!GetHexFromConfig(aSection, KHwDevice, hwDeviceInt))
			{
			INFO_PRINTF2(_L("%S parameter was not found in INI file"), &KHwDevice);
			TPtrC fourCCIniString;
			if (!GetStringFromConfig(aSection, KFourCC, fourCCIniString))
				{
				INFO_PRINTF2(_L("%S parameter was not found in INI file"), &KFourCC);
				if (dataOk)
					{
					INFO_PRINTF1(_L("Initializing DevSound with overload: void InitializeL(MDevSoundObserver &aDevSoundObserver, TMMFState aMode)"));
					TRAPD(error, iDevSound->InitializeL(*this, (TMMFState)mode));
					if(KErrNone != error)
						{
						ERR_PRINTF2(_L("InitializeL(aMDevSoundObserver, mode) left with error %d"), error);
						SetError(error);
						}
					}
				}
			else
				{
				if (fourCCIniString.Length() > KTFourCC)
					{
					ERR_PRINTF2(_L("FourCC obtained from INI file exceeded the valid length in %d"), fourCCIniString.Length());
					SetBlockResult(EFail);
					dataOk = EFalse;
					}
				if (dataOk)
					{
					TBuf<256> fourCCString;
					fourCCString.Copy(fourCCIniString);
					while (fourCCString.Length() < KTFourCC)
						{
						fourCCString.Insert(0, _L(" "));
						}
					TFourCC fourCC;
					fourCC = TFourCC(fourCCString[3] << 24 | fourCCString[2] << 16 | fourCCString[1] << 8 | fourCCString[0]);
					INFO_PRINTF1(_L("Initializing DevSound with overload: void InitializeL(MDevSoundObserver &aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode)"));
					TRAPD(error, iDevSound->InitializeL(*this, fourCC, (TMMFState)mode));
					if(KErrNone!=error)
						{
						ERR_PRINTF2(_L("InitializeL(aMDevSoundObserver, fourCC, mode) left with error %d"), error);
						ERR_PRINTF2(_L(" %S Not supported in this device"), &fourCCString);
						SetError(error);
						SetBlockResult(EFail);
						}
					}
				}
			}
		else
			{
			if (dataOk)
				{
				TUid hwDevice;
				hwDevice.iUid = hwDeviceInt;
				INFO_PRINTF1(_L("Initializing DevSound with overload: void InitializeL(MDevSoundObserver &aDevSoundObserver, TUid aHWDev, TMMFState aMode)"));
				TRAPD(error, iDevSound->InitializeL(*this, hwDevice, (TMMFState)mode));
				if(KErrNone != error)
					{
					ERR_PRINTF2(_L("InitializeL(aMDevSoundObserver, hwDevice, mode) left with error %d"), error);
					SetError(error);
					}
				}
			}
		}

	if (dataOk)
		{
		IncOutstanding();
        iInitializeErrorIndex = aAsyncErrorIndex;
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdInitializeL"));
	}


/**
 * Initializes the audio device and starts playing the DTMF string aDTMFString.
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdPlayDTMFStringL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdPlayDTMFStringL"));
	TBool dataOk = ETrue;
	TPtrC dtmfString;
	if (!GetStringFromConfig(aSection, KDTMFString, dtmfString) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDTMFString);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TRAPD( error, iDevSound->PlayDTMFStringL(dtmfString) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("Play DTMF string failed with error: %d "), error);
			SetError(error);
			}
		else
			{
			iToneErrorIndex = aAsyncErrorIndex;
			IncOutstanding();
			}
		}

	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdPlayDTMFStringL"));
	}

/**
 * Plays data in the buffer at the current volume.
 * @param
 * @return
 */
void CT_CMMFDevSoundData::DoCmdPlayData()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdPlayData"));
	PlayData();
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdPlayData"));
	}

/**
 * Helper for the DoCmdPlayData command
 */
void CT_CMMFDevSoundData::PlayData()
	{
	INFO_PRINTF1(_L("CT_CMMFDevSoundData::PlayData()"));
	iDevSound->PlayData();
	}

/**
 * Initializes audio device and starts playing a dual tone.
 * The generated tone consists of two sine waves of different frequencies summed together.
 * Dual Tone is played with the specified frequencies and for the specified duration.
 * @param aSection - Section to read params from the ini file.
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdPlayDualToneL(const TTEFSectionName& aSection , const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdPlayDualToneL"));
	TBool dataOk = ETrue;
	TInt frequencyOne=0;
	if (!GetIntFromConfig(aSection, KFrequencyOne, frequencyOne) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFrequencyOne);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TInt frequencyTwo=0;
	if (!GetIntFromConfig(aSection, KFrequencyTwo, frequencyTwo) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFrequencyTwo);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TInt duration=0;
	if (!GetIntFromConfig(aSection, KDuration, duration) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDuration);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TRAPD( error, iDevSound->PlayDualToneL(frequencyOne, frequencyTwo, duration) );
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("Play dual tone failed with error %d"), error);
			SetError(error);
			}
		else
			{
			iToneErrorIndex =  aAsyncErrorIndex;
			IncOutstanding();
			}
		}

	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdPlayDualToneL"));
	}


/**
 * Initializes the audio device and starts the play process.
 *
 * This function queries and acquires the audio policy before initializing audio device. If there was
 * an error during policy initialization, PlayError() function will be called on    the observer with
 * error code KErrAccessDenied, otherwise BufferToBeFilled() function will be called with a buffer
 * reference. After reading data into the buffer reference passed, the client should call PlayData() to play data.
 *
 * The amount of data that can be played is specified in CMMFBuffer::RequestSize(). Any data that is read into
 * buffer beyond this size will be ignored.
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdPlayInitL(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdPlayInitL"));
	TRAPD(error, iDevSound->PlayInitL());
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("PlayInitL left with error %d"), error);
		SetError(error);
		}
	else
		{
		iPlayErrorIndex =  aAsyncErrorIndex;
		IncOutstanding();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdPlayInitL"));
	}

/**
 * Initializes the audio device and starts playing a tone. The tone is played with the
 * frequency and duration specified.
 * @param aSection - Section to read params from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdPlayToneL(const TTEFSectionName& aSection , const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdPlayToneL"));
	TBool dataOk = ETrue;
	TInt frequency=0;
	if (!GetIntFromConfig(aSection, KFrequency, frequency))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFrequency);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TInt duration=0;
	if (!GetIntFromConfig(aSection, KDuration, duration))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDuration);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TRAPD(error, iDevSound->PlayToneL(frequency, duration));
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Play tone failed with error %d"), error);
			SetError(error);
			}
		else
			{
			iToneErrorIndex =  aAsyncErrorIndex;
			IncOutstanding();
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdPlayToneL"));
	}

/**
 * Initializes the audio device and starts playing a tone sequence.
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdPlayToneSequenceL(const TTEFSectionName& aSection , const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdPlayToneSequenceL"));
	TBool dataOk = ETrue;
	TPtrC toneFileParameter;
	if (!GetStringFromConfig(aSection, KFilename, toneFileParameter))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFilename);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TInt error = iToneFile.Open(iFs, toneFileParameter, EFileRead);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Open file failed with error %d"), error);
			SetError(EFail);
			}
		else
			{
			error = iToneFile.Read(iToneSequence);
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("Read file failed with error %d"), error);
				SetError(EFail);
				}
			else
				{
				TRAP(error, iDevSound->PlayToneSequenceL(iToneSequence));
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("Play tone sequence failed with error %d"), error);
					SetError(error);
					}
				else
					{
				  iToneErrorIndex =  aAsyncErrorIndex;
					IncOutstanding();
					}
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdPlayToneSequenceL"));
	}

/**
 * Contine the process of recording.
 *
 * Once the buffer is filled with recorded  data, the Observer gets a reference to the buffer along
 * with the callback function BufferToBeEmptied(). After processing the buffer (copying over to a
 * different buffer or writing to file) the client should call this function to continue the
 * recording process.
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdRecordData()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdRecordData"));
	RecordData();
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdRecordData"));
	}

/**
 * Helper method to DoCmdRecordData command
 */
void CT_CMMFDevSoundData::RecordData()
	{
	iDevSound->RecordData();
	}

/**
 * Initializes audio device and starts the recording process.
 *
 * This command queries and acquires the audio policy before initializing audio device. If there
 * was an error during policy initialization, RecordError() function will be called on the observer
 * with error code KErrAccessDenied, otherwise BufferToBeEmptied()  function will be called with a
 * buffer reference. This buffer contains recorded  or encoded data. After processing data in the
 * buffer reference passed, the client should call RecordData() to continue recording process.
 *
 * The amount of data that is available is specified in CMMFBuffer::RequestSize().
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdRecordInitL(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdRecordInitL"));
	iRecordErrorIndex = aAsyncErrorIndex;	
	TRAPD(error, iDevSound->RecordInitL());
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("RecordInitL left with error %d"), error);
		SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdRecordInitL"));
	}


/**
 * Conceal an error
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdUtilityErrorConceal()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdErrorConceal"));
	iErrorConceal=ETrue;
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdErrorConceal"));
	}

/**
 * Stop an audio file
 * @param none
 * @return none
 */
void CT_CMMFDevSoundData::DoCmdStop(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevSoundData::DoCmdStop"));
	TBool pause;
	if(!GetBoolFromConfig(aSection,KPause,pause))
		{
		INFO_PRINTF1(_L("The DoCmdStop command will be execute"));
		pause = EFalse;
		}
	iPaused=pause;
	if(iPaused)
		{
		INFO_PRINTF1(_L("a Pause was required"));
		}
	Stop();
	INFO_PRINTF1(_L("*END*CT_CMMFDevSoundData::DoCmdStop"));
	}




/**
 * Utility for get the HwDevice for play and recording
 */

void CT_CMMFDevSoundData::UtilityFourCCToHwDeviceUidL(const TDesC& aFourCC, THwDeviceType aType, TUid& aHwDeviceUid)
	{
	TPtrC8 fourCCPtr(0,0);
	TPtrC8 fourCCP16Ptr(0,0);
	RImplInfoPtrArray impArray;
	CleanupResetAndDestroyPushL(impArray);
	REComSession::ListImplementationsL(TUid::Uid(KMmfUidPluginInterfaceHwDevice), impArray);
	TBuf<KHwDeviceDefaultDataLength> datatype;
	for(TInt i = 0; i < impArray.Count(); ++i)
		{
		CImplementationInformation& entry = *(impArray[i]);
		// Validate lenght
		if (entry.DataType().Length() == KHwDeviceDefaultDataLength)
			{
			INFO_PRINTF2(_L("Implementation UID=0x%08x"),entry.ImplementationUid());
			datatype.Copy(entry.DataType());
			INFO_PRINTF2(_L("Default data (fourCCs)=%S"), &datatype);
			// Extract FourCC
			if(aType == EDecoder)
				{
				fourCCPtr.Set(entry.DataType().Left(KFourCCLength));
				fourCCP16Ptr.Set(entry.DataType().Right(KFourCCLength));
				}
			else if (aType == EEncoder)
				{
				fourCCPtr.Set(entry.DataType().Right(KFourCCLength));
				fourCCP16Ptr.Set(entry.DataType().Left(KFourCCLength));
				}
			// Compare FourCC
			TBuf16<4> foundFourCC;
			foundFourCC.Copy(fourCCPtr);
			if(aFourCC.CompareF(foundFourCC) == 0)
				{
				aHwDeviceUid = entry.ImplementationUid();
				break;
				}
			}
		}
	CleanupStack::PopAndDestroy(&impArray);
	REComSession::FinalClose();
	}
