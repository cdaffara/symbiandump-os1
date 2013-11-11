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



#if (!defined __T_MDA_AUDIO_PLAYER_UTILITY_H__)
#define __T_MDA_AUDIO_PLAYER_UTILITY_H__

// User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <mdaaudiosampleplayer.h>
#include <mda/client/utility.h>


const TInt KMaxdataFromText = 255;

/**
 * Test Active Notification class
 *
 */
class CT_MMDataMdaAudioPlayerUtility :	public	CDataWrapperBase,
										private MMdaAudioPlayerCallback,
										private MAudioLoadingObserver,
										private MMMFAudioResourceNotificationCallback
	{
public:
	~CT_MMDataMdaAudioPlayerUtility();
	static CT_MMDataMdaAudioPlayerUtility*	NewL();

	virtual TAny*	GetObject();
	virtual void	SetObjectL(TAny* aObject);
	virtual void	DisownObjectL();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void			DestroyData();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMDataMdaAudioPlayerUtility();

	void	ConstructL();

private:
	//	MTPActiveCallback implementation
	void RunL(CActive* aActive, TInt aIndex );
	void DoCancel(CActive* aActive);

	inline void			RunCustomCommandAsyncL( const TInt aAsyncErrorIndex );
	inline void			DoCancelCustomCommandAsync();

	//	MMdaAudioPlayerCallback implementation
	virtual void 		MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds &aDuration );
	virtual void 		MapcPlayComplete( TInt aError );

	//	MMMFAudioResourceNotificationCallback implementation
	virtual void		MarncResourceAvailable( TUid aNotificationEventId, const TDesC8& aNotificationData );

	//	MAudioLoadingObserver implementation
	virtual void 		MaloLoadingStarted();
	virtual void 		MaloLoadingComplete();

	//	Commands
	inline void 		DoCmdDestructor();
	inline void 		DoCmdDelay();
	inline void 		DoCmdNewL( const TDesC& aSection );
	inline void 		DoCmdNewFilePlayerL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdNewDesPlayerReadOnlyL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdNewDesPlayerL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdOpenDesL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdOpenUrlL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdOpenFileL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdClose();
	inline void 		DoCmdPlay( const TInt aAsyncErrorIndex );
	inline void 		DoCmdPause();
	inline void 		DoCmdWillResumePlay();
	inline void 		DoCmdStop();
	inline void 		DoCmdSetVolume( const TDesC& aSection );
	inline void 		DoCmdSetBalance( const TDesC& aSection );
	inline void 		DoCmdSetPosition( const TDesC& aSection );
	inline void 		DoCmdMaxVolume( const TDesC& aSection );
	inline void 		DoCmdGetVolume( const TDesC& aSection );
	inline void 		DoCmdGetBalance( const TDesC& aSection );
	inline void 		DoCmdGetPosition( const TDesC& aSection );
	inline void 		DoCmdDuration( const TDesC& aSection );
	inline void 		DoCmdSetPlayWindow( const TDesC& aSection );
	inline void 		DoCmdClearPlayWindow();
	inline void 		DoCmdSetRepeats( const TDesC& aSection );
	inline void 		DoCmdSetVolumeRamp( const TDesC& aSection );
	inline void 		DoCmdGetBitRate( const TDesC& aSection );
	inline void 		DoCmdGetMetaDataEntryL( const TDesC& aSection );
	inline void 		DoCmdGetNumberOfMetaDataEntries( const TDesC& aSection );
	inline void 		DoCmdRegisterAudioResourceNotificationL( const TDesC& aSection );
	inline void 		DoCmdCancelRegisterAudioResourceNotification( const TDesC& aSection );
	inline void 		DoCmdRegisterForAudioLoadingNotification();
	inline void 		DoCmdSetPriority( const TDesC& aSection );
	inline void 		DoCmdSetThreadPriority( const TDesC& aSection );
	inline void 		DoCmdGetAudioLoadingProgressL( const TDesC& aSection );
	inline void 		DoCmdGetDRMCustomCommand( const TDesC& aSection );
	inline void 		DoCmdControllerImplementationInformationL();
	inline void 		DoCmdCustomCommandSyncL( const TDesC& aSection );
	inline void 		DoCmdCustomCommandAsyncL( const TDesC& aSection, const TInt aAsyncErrorIndex );
	inline void 		DoCmdUseSharedHeap();

private:
	CMdaAudioPlayerUtility*		iPlayerUtility;
	CActiveCallback*			iActiveCustomCommandAsync;
	CMdaServer*					iServer;
	TInt						iAsyncErrorIndex;

	RFs							iFs;
	RFile						iFile;
	TBool						iFileOpened;

	HBufC8*						iBuffer;
	TPtr8*						iBufferPtr;
	TBuf8<KMaxdataFromText>		iDataFromText;

	TUid						iAudioControllerUid;
	TUid						iAudioFormatUid;

	TBool						iPlaying;
	};

#endif /* __T_MDA_AUDIO_PLAYER_UTILITY_H__ */
