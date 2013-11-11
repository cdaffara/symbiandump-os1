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



#if (!defined __T_MM_DATA_VIDEO_PLAYER_UTILITY_H__)
#define __T_MM_DATA_VIDEO_PLAYER_UTILITY_H__

//	EPOC includes
#include <videoplayer.h>

//	User Includes
#include "DataWrapperBase.h"

const TInt KMaxdataFromText = 255;

/**
CVideoPlayerUtility API test commands
@internalComponent
*/
class CT_MMDataVideoPlayerUtility : public CDataWrapperBase,
									private MVideoPlayerUtilityObserver,
									private MVideoLoadingObserver
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_MMDataVideoPlayerUtility*	NewL();

	/**
	* Public destructor
	*/
	~CT_MMDataVideoPlayerUtility();

	virtual TAny*	GetObject();
	virtual void	SetObjectL(TAny* aObject);
	virtual void	DisownObjectL();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMDataVideoPlayerUtility();

	void	ConstructL();

private:
	inline	void			DoCmdConstructor(const TDesC& aSection);
	inline	void			DoCmdOpenFileL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline	void			DoCmdOpenDesL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline	void			DoCmdOpenUrlL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline	void			DoCmdPrepare(const TInt aAsyncErrorIndex);
	inline	void			DoCmdClose();
	inline	void			DoCmdPlay(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline	void			DoCmdStop();
	inline	void			DoCmdPauseL();
	inline	void			DoCmdSetPriorityL(const TDesC& aSection);
	inline	void			DoCmdPriorityL(const TDesC& aSection);
	inline	void			DoCmdSetDisplayWindowL(const TDesC& aSection);
	inline	void			DoCmdRegisterForVideoLoadingNotification();
	inline	void			DoCmdGetVideoLoadingProgressL(const TDesC& aSection);
	inline	void			DoCmdGetFrameL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline	void			DoCmdRefreshFrameL();
	inline	void			DoCmdVideoFrameRateL(const TDesC& aSection);
	inline	void			DoCmdSetVideoFrameRateL(const TDesC& aSection);
	inline	void			DoCmdVideoFrameSizeL(const TDesC& aSection);
	inline	void			DoCmdVideoFormatMimeTypeL(const TDesC& aSection);
	inline	void			DoCmdVideoBitRateL(const TDesC& aSection);
	inline	void			DoCmdAudioBitRateL(const TDesC& aSection);
	inline	void			DoCmdAudioTypeL(const TDesC& aSection);
	inline	void			DoCmdAudioEnabledL(const TDesC& aSection);
	inline	void			DoCmdSetPositionL(const TDesC& aSection);
	inline	void			DoCmdPositionL(const TDesC& aSection);
	inline	void			DoCmdDurationL(const TDesC& aSection);
	inline	void			DoCmdSetVolumeL(const TDesC& aSection);
	inline	void			DoCmdVolume(const TDesC& aSection);
	inline	void			DoCmdMaxVolume(const TDesC& aSection);
	inline	void			DoCmdSetBalanceL(const TDesC& aSection);
	inline	void			DoCmdBalance(const TDesC& aSection);
	inline	void			DoCmdSetRotationL(const TDesC& aSection);
	inline	void			DoCmdRotationL(const TDesC& aSection);
	inline	void			DoCmdSetScaleFactorL(const TDesC& aSection);
	inline	void			DoCmdGetScaleFactorL(const TDesC& aSection);
	inline	void			DoCmdSetCropRegionL(const TDesC& aSection);
	inline	void			DoCmdGetCropRegionL(const TDesC& aSection);
	inline	void			DoCmdNumberOfMetaDataEntriesL(const TDesC& aSection);
	inline	void			DoCmdMetaDataEntryL(const TDesC& aSection);
	inline	void			DoCmdControllerImplementationInformationL();
	inline	void			DoCmdCustomCommandSyncL(const TDesC& aSection);
	inline	void			DoCmdCustomCommandASyncL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline	void			DoCmdGetDRMCustomCommand();
	inline	void			DoCmdStopDirectScreenAccessL();
	inline	void			DoCmdStartDirectScreenAccessL();
	inline	void 			DoCmdDestructor();

	//	MVideoPlayerUtilityObserver implementation
	virtual void			MvpuoOpenComplete(TInt aError);
	virtual void			MvpuoPrepareComplete(TInt aError);
	virtual void			MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
	virtual void			MvpuoPlayComplete(TInt aError);
	virtual void			MvpuoEvent(const TMMFEvent &aEvent);

	//	MVideoLoadingObserver implementation
	virtual void 			MvloLoadingStarted();
	virtual void 			MvloLoadingComplete();

	//	Helper methods
	void					DestroyData();
	void					SetDefaultVideoControllerL();

	//	MT_MMActiveCallback implementation
 	virtual void			RunL(CActive* aActive, TInt aIndex);
 	virtual void			DoCancel(CActive* aActive);
 	inline void				RunCustomCommandAsyncL(TInt aIndex);
 	inline void				DoCancelCustomCommandAsync();

private:
	CVideoPlayerUtility*	iVideoPlayerUtility;
	TInt 					iApiAsyncErrorIndex;

	// vars neccessary for CVideoPlayerUtility
	RWsSession				iWsSession;
	RWsSession				iInvalidWsSession;
	TBool					iWsSessionConnected;
	CWsScreenDevice* 		iScrDevice;
	CWsScreenDevice* 		iInvalidScrDevice;
	RWindowGroup*			iWindowGroup;
	RWindow*				iWindow;
	RWindow*				iInvalidWindow;
	CActiveCallback*		iActiveCustomCommandAsync;
	TBuf8<KMaxdataFromText>	iDataFromText;
	TUid					iVideoControllerUid;
	TUid					iVideoFormatUid;

	RFs						iFs;
	TBool					iFileOpened;
	RFile					iFile;
	HBufC8*					iBuffer;
	TPtr8*					iBufferPtr;
	};

#endif /* __T_VIDEO_PLAYER_UTILITY_STEP_H__ */
