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


#ifndef T_CMMFDEVSOUND_DATA_H
#define T_CMMFDEVSOUND_DATA_H

//User includes
#include "DataWrapperBase.h"

//Epoc Includes
#include <sounddevice.h> //CMMFDevSound

/**
 * This class tests the CMMFDevSound API's
 */

enum THwDeviceType
	{
	EDecoder,
	EEncoder
	};

class CT_CMMFDevSoundData : public CDataWrapperBase, public MDevSoundObserver
	{
public:
	~CT_CMMFDevSoundData();
	static  CT_CMMFDevSoundData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_CMMFDevSoundData();
	void ConstructL();

private:
	void UtilityFourCCToHwDeviceUidL(const TDesC& aFourCC, THwDeviceType aType, TUid& aHwDeviceUid);
	void DestroyData();
	void Stop();
	void PlayData();
	void RecordData();
	
	/** From MDevSoundObserver */
	virtual void InitializeComplete(TInt aError);
	virtual void ToneFinished(TInt aError);
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError);
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);
	virtual void RecordError(TInt aError);
	virtual void ConvertError(TInt aError);
	virtual void DeviceMessage(TUid uid, const TDesC8& aMsg);

protected:
	void DoCmdNewL();
	void DoCmdNewLLoopL(const TTEFSectionName&aSection);
	void DoCmdDestructor();
	void DoCmdCapabilities();
	void DoCmdConfig();
	void DoCmdMaxGain(const TTEFSectionName& aSection);
	void DoCmdMaxVolume(const TTEFSectionName& aSection);
	void DoCmdGetPlayBalanceL(const TTEFSectionName& aSection);
	void DoCmdGetRecordBalanceL(const TTEFSectionName& aSection);
	void DoCmdGetSupportedInputDataTypesL();
	void DoCmdGetSupportedOutputDataTypesL();
	void DoCmdSamplesPlayed();
	void DoCmdSamplesRecorded();
	void DoCmdVolume(const TTEFSectionName& aSection);
	void DoCmdGain(const TTEFSectionName& aSection);
	void DoCmdFixedSequenceCount();
	void DoCmdFixedSequenceName(const TTEFSectionName& aSection);
	void DoCmdSetConfigL(const TTEFSectionName& aSection);
	void DoCmdSetDTMFLengths(const TTEFSectionName& aSection);
	void DoCmdSetGain(const TTEFSectionName& aSection);
	void DoCmdSetPlayBalanceL(const TTEFSectionName& aSection);
	void DoCmdSetPrioritySettings(const TTEFSectionName& aSection);
	void DoCmdSetRecordBalanceL(const TTEFSectionName& aSection);
	void DoCmdSetToneRepeats(const TTEFSectionName& aSection);
	void DoCmdSetVolume(const TTEFSectionName& aSection);
	void DoCmdSetVolumeRamp(const TTEFSectionName& aSection);
	void DoCmdInitializeL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void DoCmdPlayDTMFStringL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void DoCmdPlayData();
	void DoCmdPlayDualToneL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void DoCmdPlayInitL(const TInt aAsyncErrorIndex);
	void DoCmdPlayToneL(const TTEFSectionName& aSection , const TInt aAsyncErrorIndex);
	void DoCmdPlayToneSequenceL(const TTEFSectionName& aSection , const TInt aAsyncErrorIndex);
	void DoCmdRecordData();
	void DoCmdRecordInitL(const TInt aAsyncErrorIndex);
	void DoCmdStop(const TTEFSectionName& aSection);
	void DoCmdSetVolumeLoop(const TTEFSectionName& aSection);

	void DoCmdUtilityErrorConceal();
	void DoCmdUtilityReplaceFile(const TTEFSectionName& aSection);
	void DoCmdUtilityOpenFile(const TTEFSectionName& aSection);
	void DoCmdUtilityCloseFile();
	void DoCmdUtilityFileStartPosition();

private:

	/**
	 * Wrapped object
	 */
	CMMFDevSound* iDevSound;
	/**
	 * Array to store DevSound objects
	 */
	RPointerArray<CMMFDevSound> iObjectsDevSound;
	/**
	 * Prority settings
	 */
	TMMFPrioritySettings iPrioritySettings;
	/**
	 * Capabilities
	 */
	TMMFCapabilities iCapabilities;
	/**
	 * Store the volume value
	 */
	TInt iVolumeValue;
	/**
	 * Store the gain value
	 */
	TInt iGainValue;
	/**
	 * Left percentage
	 */
	TInt iLeftPercentage;
	/**
	 * Right percentage
	 */
	TInt iRightPercentage;
	/**
	 * File Server
	 */
	RFs iFs;
	/**
	 * File for tones
	 */
	RFile iToneFile;
	/**
	 * File for audio
	 */
	RFile iAudiofile;
	/**
	 * Buffer to be filled or to review if it's empty
	 */
	CMMFBuffer* iBuffer;
	/**
	 * Last sample of audio file
	 */
	TInt iLastSample;
	/**
	 * for control the pause (DoCmdPause)
	 */
	TBool iPaused;
	/**
	 * Conceal the error
	 */
	TBool iErrorConceal;
	/**
	 * End of file
	 */
	TBool iEndFile;
	/**
	 * Buffer to be filled with tone sequence
	 */
	TBuf8<1024> iToneSequence;
	/**
	 * Enum for channels
	 */
	static const TEnumEntryTable iEnumChannels[];
	/**
	 * Enum for sample rate
	 */
	static const TEnumEntryTable iEnumSampleRate[];
	/**
	 * Enum for the mode
	 */
	static const TEnumEntryTable iEnumMode[];
	/**
	 * Enum for the priorities
	 */
	static const TEnumEntryTable iEnumPriority[];
	/**
	 * Enum for the priority preference
	 */
	static const TEnumEntryTable iEnumPriorityPreference[];
	/**
	 * Enum for initialize(FourCC, HwDevice) the mode and state
	 */
	static const TEnumEntryTable iEnumInitializeLType[];
	/**
	 * Async Error Index of RecordError.
	 */
	TInt iRecordErrorIndex;
		/**
	 * Async Error Index of PlayError.
	 */
	TInt iPlayErrorIndex;
		/**
	 * Async Error Index of ToneError.
	 */
	TInt iToneErrorIndex;
    		/**
	 * Async Error Index of InitializeError.
	 */
	TInt iInitializeErrorIndex;
	
	};

#endif // T_CMMFDEVSOUND_DATA_H
