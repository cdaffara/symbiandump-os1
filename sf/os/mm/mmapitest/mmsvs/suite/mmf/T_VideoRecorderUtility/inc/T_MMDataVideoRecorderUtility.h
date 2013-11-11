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



#if (!defined __T_MM_DATA_VIDEO_RECORDER_UTILITY_H__)
#define __T_MM_DATA_VIDEO_RECORDER_UTILITY_H__

//	User includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <videorecorder.h>
#include <fbs.h>

/**
 * Test Active Notification class
 *
 */
class CT_MMDataVideoRecorderUtility : public CDataWrapperBase, private MVideoRecorderUtilityObserver
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_MMDataVideoRecorderUtility*	NewL();

	/**
	* Public destructor
	*/
	~CT_MMDataVideoRecorderUtility();

	//	CTPDataWithDictionaryAccess implementation
	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMDataVideoRecorderUtility();

	void					ConstructL();

private:
	inline void		DoCmdDestructor();
	inline void		DoCmdAddMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdAudioBitRateL(const TDesC& aSection);
	inline void		DoCmdAudioEnabledL(const TDesC& aSection);
	inline void		DoCmdAudioTypeL(const TDesC& aSection);
	inline void		DoCmdClose();
	inline void		DoCmdControllerImplementationInformationL();
	inline void		DoCmdCustomCommandAsync(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdCustomCommandSync(const TDesC& aSection);
	inline void		DoCmdDurationL(const TDesC& aSection);
	inline void		DoCmdGainL(const TDesC& aSection);
	inline void		DoCmdGetPriorityL(const TDesC& aSection);
	inline void		DoCmdGetSupportedAudioTypesL();
	inline void		DoCmdGetSupportedVideoTypesL();
	inline void		DoCmdGetVideoFrameSizeL(const TDesC& aSection);
	inline void		DoCmdMaxGainL(const TDesC& aSection);
	inline void		DoCmdMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdNewL(const TDesC& aSection);
	inline void		DoCmdNumberOfMetaDataEntriesL(const TDesC& aSection);
	inline void		DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdOpenUrlL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void		DoCmdPauseL();
	inline void		DoCmdPrepare(const TInt aAsyncErrorIndex);
	inline void		DoCmdRecord(const TInt aAsyncErrorIndex);
	inline void		DoCmdRecordTimeAvailable(const TDesC& aSection);
	inline void		DoCmdRemoveMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdReplaceMetaDataEntryL(const TDesC& aSection);
	inline void		DoCmdSetAudioBitRateL(const TDesC& aSection);
	inline void		DoCmdSetAudioEnabledL(const TDesC& aSection);
	inline void		DoCmdSetAudioTypeL(const TDesC& aSection);
	inline void		DoCmdSetGainL(const TDesC& aSection);
	inline void		DoCmdSetMaxClipSizeL(const TDesC& aSection);
	inline void		DoCmdSetPriorityL(const TDesC& aSection);
	inline void		DoCmdSetVideoBitRateL(const TDesC& aSection);
	inline void		DoCmdSetVideoFrameRateL(const TDesC& aSection);
	inline void		DoCmdSetVideoFrameSizeL(const TDesC& aSection);
	inline void		DoCmdSetVideoTypeL(const TDesC& aSection);
	inline void		DoCmdStop();
	inline void		DoCmdVideoBitRateL(const TDesC& aSection);
	inline void		DoCmdVideoFormatMimeTypeL(const TDesC& aSection);
	inline void		DoCmdVideoFrameRateL(const TDesC& aSection);

	//	MVideoRecorderUtilityObserver implementation
	virtual void			MvruoOpenComplete(TInt aError);
	virtual void			MvruoPrepareComplete(TInt aError);
	virtual void			MvruoRecordComplete(TInt aError);
	virtual void			MvruoEvent(const TMMFEvent &aEvent);

	//	MActiveCallback implementation
	virtual void			RunL(CActive* aActive, TInt aIndex);
 	virtual void			DoCancel(CActive* aActive, TInt aIndex);
	inline void				RunActiveL(TInt aIndex);
	inline void				DoCancelActive();

 	//	Helper methods
 	void					DestroyData();
 	void					DeleteFile(const TDesC& aSection);

	//	Helper
	void					ReadCameraHandleL(const TDesC& aSection, TInt& aCameraHandle);

private:
	CVideoRecorderUtility*	iVideoRecorderUtility;
	CActiveCallback*		iActiveCallback;
	HBufC8*					iBuffer;
	TPtr8*					iBufferPtr;
	TInt					iBufferSize;
	RFile					iFile;
	TBool					iFileOpened;
	TUid					iVideoControllerUid;
	TUid					iVideoFormatUid;
	RArray<TFourCC>			iSupportedAudioTypes;
	/** For Asynchronous methods withound CActive **/
	TInt 					iApiAsyncErrorIndex;
	TBool					iRecordStarted;

	/** RFs class instance for additional purpoces */
	RFs						iFs;
	};

#endif /* __T_MM_DATA_VIDEO_RECORDER_UTILITY_H__ */
