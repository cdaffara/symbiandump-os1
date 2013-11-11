
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file TESTPLAYER.h
*/

#ifndef TESTPLAYER_H__
#define TESTPLAYER_H__

#include "TSI_MMFVCLNT.h"

/**
 * Load and initialise a video file.
 *
 * @class CTestVclntVideoFile
 *
 * Req. under test REQ172.2.4           REQ172.2.7        REQ172.2.10
 */
class CTestVclntVideoFile : public CTestMmfVclntStep , public MVideoPlayerUtilityObserver
	{
public:
	CTestVclntVideoFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	static CTestVclntVideoFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	static CTestVclntVideoFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aVideo);
	// MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	TInt iError;
	const TBool iPlay;						// Does this test require playing the video
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of video
	};


/**
 * Load and initialise an video descriptor.
 *
 * @class CTestVclntVideoDes
 *
 * Req. under test REQ172.2.4        REQ172.2.8        REQ172.2.10
 */
class CTestVclntVideoDes : public CTestMmfVclntStep , public MVideoPlayerUtilityObserver
	{
public:
	CTestVclntVideoDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay) ;
	static CTestVclntVideoDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	static CTestVclntVideoDes* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
	// MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);

protected:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	HBufC8* iVideo;						// Buffer for holding video descriptor data
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	TInt iError;						// Stores error values for testing
	const TBool iPlay;					// Does this test require playing the video
	} ;

/**
 * Open video form a URL and play.
 *
 * @class CTestVclntVideoUrl
 *
 */
class CTestVclntVideoUrl : public CTestMmfVclntStep , public MVideoPlayerUtilityObserver
	{
public:
	CTestVclntVideoUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	static CTestVclntVideoUrl* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	static CTestVclntVideoUrl* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
	// MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);	

protected:
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	TInt iError;						// Stores error values for testing
	const TBool iPlay;					// Does this test require playing the video
	HBufC8* iVideo;						// Buffer for holding video descriptor data
	};

/**
 * Set and enquire framerate
 *
 * @class CTestVclntEnqFrameRate
 *
 * Req. under test REQ172.2.28      
 */
class CTestVclntEnqFrameRate : public CTestVclntVideoFile
	{
public:
	static CTestVclntEnqFrameRate* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aFrameRate);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntEnqFrameRate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aFrameRate);

private:
	TInt iFrameRate;
	};


/**
 * set position and play form this position.
 * 
 * @class CTestVclntPosition
 *
 * Req. under test REQ172.2.13 REQ172.2.14
 */
class CTestVclntPosition : public CTestVclntVideoFile
	{
public:
	static CTestVclntPosition* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntPosition(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition);

private:
	TTimeIntervalMicroSeconds iPosition;
	};

/**
 * Video priority
 * 
 * @class CTestVclntPriority
 *
 * Req. under test REQ172.2.25
 */
class CTestVclntPriority : public CTestVclntVideoFile
	{
public:
	static CTestVclntPriority* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aPriority);

private:
	TInt iPriority;
	};


/**
 * Video duration
 * 
 * @class CTestVclntDuration
 *
 * Req. under test REQ172.2.15
 */
class CTestVclntDuration : public CTestVclntVideoFile
	{
public:
	static CTestVclntDuration* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aDuration);
	};


/**
 * Video Volume
 * 
 * @class CTestVclntVolume
 *
 * Req. under test REQ172.2.16        REQ172.2.17        REQ172.2.18
 */
class CTestVclntVolume : public CTestVclntVideoFile
	{
public:
	static CTestVclntVolume* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aVolume);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntVolume(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aVolume);

private:
	TInt iVolume;
	};


/**
 * Video open, close and then open again.
 * 
 * @class CTestVclntCloseOpen
 *
 * Req. under test REQ172.2.4        REQ172.2.5       REQ172.2.6
 */
class CTestVclntCloseOpen : public CTestVclntVideoFile
	{
public:
	static CTestVclntCloseOpen* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};



/**
 * Video pause.
 * 
 * @class CTestVclntPause
 *
 * Req. under test REQ172.2.10     REQ172.2.11
 */
class CTestVclntPause : public CTestVclntVideoFile
	{
public:
	static CTestVclntPause* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};


/**
 * Video pause.
 * 
 * @class CTestVclntBalance
 *
 * Req. under test REQ172.2.30       REQ172.2.31
 */
class CTestVclntBalance : public CTestVclntVideoFile
	{
public:
	static CTestVclntBalance* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntBalance(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aBalance);

private:
	TInt iBalance;
	};



/**
 * Video set play window and delete play window.
 * 
 * @class CTestVclntPlayWindow
 *
 * Req. under test REQ172.2.21          REQ172.2.22
 */
class CTestVclntPlayWindow : public CTestVclntVideoFile
	{
public:
	static CTestVclntPlayWindow* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntPlayWindow(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd);

private:
	TTimeIntervalMicroSeconds iStart;
	TTimeIntervalMicroSeconds iEnd;
	};


/**
 * Video get meta data.
 * 
 * @class CTestVclntMeta
 *
 * Req. under test REQ172.2.20
 */
class CTestVclntMeta : public CTestVclntVideoFile
	{
public:
	static CTestVclntMeta* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);

private:
	TTimeIntervalMicroSeconds iStart;
	TTimeIntervalMicroSeconds iEnd;
	};


/**
 * Video get framesize.
 * 
 * @class CTestVclntFrameSize
 *
 * Req. under test REQ172.2.27
 */
class CTestVclntFrameSize : public CTestVclntVideoFile
	{
public:
	static CTestVclntFrameSize* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aFrameSize);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntFrameSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aFrameSize);

private:
	TInt iFrameSize;
	};


/**
 * Video get mime type.
 * 
 * @class CTestVclntMimeType
 *
 * Req. not mapped
 */
class CTestVclntMimeType : public CTestVclntVideoFile
	{
public:
	static CTestVclntMimeType* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntMimeType(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);

	};


/**
 * Video rebuffering.
 * 
 * @class CTestVclntRebuffering
 *
 * Req. not mapped
 */
class CTestVclntRebuffering : public CTestVclntVideoFile, public MVideoLoadingObserver
	{
public:
	static CTestVclntRebuffering* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
	void MvloLoadingStarted();
	void MvloLoadingComplete();
private:
	CTestVclntRebuffering(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);

	};

/**
 * Video get framesize.
 * 
 * @class CTestVclntRepeat
 *
 * Req. under test REQ172.2.23
 */
class CTestVclntRepeat : public CTestVclntVideoFile
	{
public:
	static CTestVclntRepeat* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aRepeat);
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntRepeat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aRepeat);

private:
	TInt iRepeat;
	};



/**
 * Video Delete object while playing
 * 
 * @class CTestVclntDelete
 *
 * Req. under test REQ172.2.4           REQ172.2.7        REQ172.2.10
 */
class CTestVclntDelete : public CTestVclntVideoFile
	{
public:
	static CTestVclntDelete* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
	CTestVclntDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	};


#endif
