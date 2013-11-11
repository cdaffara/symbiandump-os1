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



#if (!defined __T_MMDATA_MDA_AUDIO_RECORDER_UTILITY_H__)
#define __T_MMDATA_MDA_AUDIO_RECORDER_UTILITY_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <mdaaudiosampleeditor.h>

const TInt	KMdaAudioRecorderBufferSize=1024;
const TInt 	KMaxdataFromText = 255;

// Temp fix for conditional building and testing of UseSharedHeap
#if (defined SYMBIAN_SWI_POST_INSTALL_REVOCATION)
#define CMDA_AUDIO_RECORDER_UTILITY_USED_SHARED_HEAP
#endif

/**
CMdaAudioRecorderUtility API test commands
@internalComponent
*/
class CT_MMDataMdaAudioRecorderUtility : public CDataWrapperBase, private MMdaObjectStateChangeObserver, private MMMFAudioResourceNotificationCallback, private MAudioLoadingObserver
	{
public:
	~CT_MMDataMdaAudioRecorderUtility();
	static CT_MMDataMdaAudioRecorderUtility*	NewL();

	virtual TAny*	GetObject();
	virtual void	SetObjectL(TAny* aObject);
	virtual void	DisownObjectL();
	virtual TBool	DoCommandL(	const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	inline RFs&     FileServer();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMDataMdaAudioRecorderUtility();

	void	ConstructL();

private:
	inline void		DoCmdDestructor();
	inline void		DoCmdAddMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdAudioPlayControllerImplementationInformationL();
	inline void		DoCmdAudioRecorderControllerImplementationInformationL();
	inline void		DoCmdCancelRegisterAudioResourceNotification(const TDesC& aSection);
	inline void		DoCmdClearPlayWindow();
	inline void		DoCmdClose();
	inline void		DoCmdCropFromBeginningL();
	inline void		DoCmdCropL();
	inline void		DoCmdDestinationBitRateL(const TDesC& aSection);
	inline void		DoCmdDestinationDataTypeL(const TDesC& aSection);
	inline void		DoCmdDestinationFormatL(const TDesC& aSection);
	inline void		DoCmdDestinationNumberOfChannelsL(const TDesC& aSection);
	inline void		DoCmdDestinationSampleRateL(const TDesC& aSection);
	inline void		DoCmdDuration(const TDesC& aSection);
	inline void		DoCmdGetAudioLoadingProgressL(const TDesC& aSection);
	inline void		DoCmdGetDRMCustomCommand(const TDesC& aSection);
	inline void		DoCmdGetGain(const TDesC& aSection);
	inline void		DoCmdGetMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdGetNumberOfMetaDataEntries(const TDesC& aSection);
	inline void		DoCmdGetPlaybackBalance(const TDesC& aSection);
	inline void		DoCmdGetRecordBalance(const TDesC& aSection);
	inline void		DoCmdGetSupportedBitRatesL(const TDesC& aSection);
	inline void		DoCmdGetSupportedDestinationDataTypesL(const TDesC& aSection);
	inline void		DoCmdGetSupportedNumberOfChannelsL(const TDesC& aSection);
	inline void		DoCmdGetSupportedSampleRatesL(const TDesC& aSection);
	inline void		DoCmdGetVolume(const TDesC& aSection);
	inline void		DoCmdMaxGain(const TDesC& aSection);
	inline void		DoCmdMaxVolume(const TDesC& aSection);
	inline void		DoCmdNewL(const TDesC& aSection);
	inline void		DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdOpenL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdOpenUrlL(const TDesC& aSection);
	inline void		DoCmdPlayControllerCustomCommandAsyncL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdPlayControllerCustomCommandSyncL(const TDesC& aSection);
	inline void		DoCmdPlayL(const TInt aAsyncErrorIndex);
	inline void		DoCmdPosition(const TDesC& aSection);
	inline void		DoCmdRecordControllerCustomCommandAsyncL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdRecordControllerCustomCommandSyncL(const TDesC& aSection);
	inline void		DoCmdRecordL(const TInt aAsyncErrorIndex);
	inline void		DoCmdRecordTimeAvailable(const TDesC& aSection);
	inline void		DoCmdRegisterAudioResourceNotificationL(const TDesC& aSection);
	inline void		DoCmdRegisterForAudioLoadingNotification();
	inline void		DoCmdRemoveMetaDataEntry(const TDesC& aSection);
	inline void		DoCmdReplaceMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdSetAudioDeviceMode(const TDesC& aSection);
	inline void		DoCmdSetDestinationBitRateL(const TDesC& aSection);
	inline void		DoCmdSetDestinationDataTypeL(const TDesC& aSection);
	inline void		DoCmdSetDestinationFormatL(const TDesC& aSection);
	inline void		DoCmdSetDestinationNumberOfChannelsL(const TDesC& aSection);
	inline void		DoCmdSetDestinationSampleRateL(const TDesC& aSection);
	inline void		DoCmdSetGain(const TDesC& aSection);
	inline void		DoCmdSetMaxWriteLength(const TDesC& aSection);
	inline void		DoCmdSetPlayWindow(const TDesC& aSection);
	inline void		DoCmdSetPlaybackBalance(const TDesC& aSection);
	inline void		DoCmdSetPosition(const TDesC& aSection);
	inline void		DoCmdSetPriority(const TDesC& aSection);
	inline void		DoCmdSetRecordBalance(const TDesC& aSection);
	inline void		DoCmdSetRepeats(const TDesC& aSection);
	inline void		DoCmdSetThreadPriorityPlayback(const TDesC& aSection);
	inline void		DoCmdSetThreadPriorityRecord(const TDesC& aSection);
	inline void		DoCmdSetVolume(const TDesC& aSection);
	inline void		DoCmdSetVolumeRamp(const TDesC& aSection);
	inline void		DoCmdSourceBitRateL(const TDesC& aSection);
	inline void		DoCmdState(const TDesC& aSection);
	inline void		DoCmdStop();
	inline void		DoCmdUseSharedHeap();
	inline void		DoCmdWillResumePlay();

	//	MT_MMActiveCallback implementation
 	virtual void			RunL(CActive* aActive, const TInt aAsyncErrorIndex);
 	virtual void			DoCancel(CActive* aActive);
 	inline void				RunCustomCommandAsyncL(const TInt aAsyncErrorIndex);
 	inline void				DoCancelCustomCommandAsync();

	//	Helper methods
	void			DestroyData();
	void			OpenFile(const TDesC& aCommand, const TDesC& aSection);
	TBool			GetFormatL(const TDesC& aFormatStr, TMdaClipFormat* &aMdaClipFormat, const TDesC& aSection);
	TBool 			ConvertToTDeviceMode(const TDesC& aDesc, CMdaAudioRecorderUtility::TDeviceMode& aDevicemode);
	const 	TDesC& 	GetState(const TInt& aState);
	TBool			GetCodecL(const TDesC& aCodecStr, TMdaPackage* &aMdaPackage);
	void			GetAudioSettingsL(const TDesC& aAudioSettingsStr, const TDesC& aSection, TMdaPackage* &aMdaPackage);

	//MMdaObjectStateChangeObserver implementation
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
    virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
    virtual void MaloLoadingStarted();
	virtual void MaloLoadingComplete();

private:
	CMdaAudioRecorderUtility*	iRecorderUtility;
	CActiveCallback*			iActiveCustomCommandAsync;
	TInt 						iAsyncErrorIndex;
   	TUid						iRecorderControllerUid;
	TUid						iRecorderFormatUid;
   	TUid						iPlayerControllerUid;
	TUid						iPlayerFormatUid;
	TBool						iFileOpened;
	RFs 						iFs;
	RFile 						iFile;
	HBufC8*						iBuffer;
	TPtr8*						iBufferPtr;
	TInt						iBufferSize;
	TBuf8<KMaxdataFromText>		iDataFromText;
	TBool						iCustomCommandData;
	TBool						iRecording;
	};

#include "T_MMDataMdaAudioRecorderUtility.inl"

#endif /* __T_MMDATA_MDA_AUDIO_RECORDER_UTILITY_STEP_H__ */
