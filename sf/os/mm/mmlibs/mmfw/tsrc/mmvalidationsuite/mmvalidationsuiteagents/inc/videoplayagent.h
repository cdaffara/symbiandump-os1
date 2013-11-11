// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the MVS Agents for TechView
//

#ifndef MVSVIDEOPLAYAGENT_H
#define MVSVIDEOPLAYAGENT_H

#include <f32file.h>
#include <flogger.h>
#include <ecam.h>

#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfcontroller.h>

#include <videorecorder.h>
#include <videoplayer.h>

#include <mvs/agentsutility.h>


// forward declaration
#ifdef SYMBIAN_BUILD_GCE
class CVideoPlayerUtility2;
class MMMFSurfaceEventHandler;
#endif

/**
@publishedPartner
@prototype
@test

Provides Video playing functionality for the UI

*/
class CMVSVideoPlayAgent : public CBase, MVideoPlayerUtilityObserver, MMMFAudioResourceNotificationCallback
{
public:
	IMPORT_C static CMVSVideoPlayAgent* NewL(MMVSClientObserver& aObserver);
	IMPORT_C ~CMVSVideoPlayAgent();
	
	//opens an video clip for playback
    IMPORT_C void OpenFileL(const TDesC& aFileName);	
	IMPORT_C void OpenFileL(const TDesC& aFileName, TUid aControllerUid);
	
	//opens DRM protected source
	IMPORT_C void OpenFileL(TMMSource &aSource);
	
	IMPORT_C void OpenDesL(const TDesC8 &aDescriptor); //For future support
	
	IMPORT_C void Prepare();
	IMPORT_C void Close();
	IMPORT_C void Play();
	IMPORT_C void Play(const TTimeIntervalMicroSeconds &aStartPoint, const TTimeIntervalMicroSeconds &aEndPoint);
	IMPORT_C TInt Stop();
	IMPORT_C void PauseL();
		
    //Setter Functions	
	IMPORT_C void SetDisplayWindowL(RWsSession &aWs, CWsScreenDevice &aScreenDevice, RWindow &aWindow, const TRect &aWindowRect, const TRect &aClipRect);
	IMPORT_C void SetVideoFrameRateL(TReal32 aFramesPerSecond);
	IMPORT_C void SetVolumeL(TInt aVolume);
	IMPORT_C void SetBalanceL(TInt aBalance);
	IMPORT_C void SetPriorityL(TInt aPriority, TMdaPriorityPreference aPref);
	IMPORT_C void SetPositionL(const TTimeIntervalMicroSeconds &aPosition);
	IMPORT_C void SetRotationL(TVideoRotation aRotation);
	IMPORT_C void SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering);
	IMPORT_C void SetCropRegionL(const TRect &aCropRegion);
	IMPORT_C void SetVideoExtentL(const TRect& aVideoExtent);
	IMPORT_C void SetWindowClipRectL(const TRect& aWindowClipRect);	
	
	IMPORT_C void GetFrameL(TDisplayMode aDisplayMode);
	IMPORT_C void GetFrameL(TDisplayMode aDisplayMode, ContentAccess::TIntent aIntent);
	IMPORT_C TReal32 VideoFrameRateL();
	IMPORT_C void VideoFrameSizeL(TSize &aSize) ;
	IMPORT_C const TDesC8 &VideoFormatMimeType() ;
	IMPORT_C TInt VideoBitRateL() ; //No way to seek this information in HwDevice.
	IMPORT_C TInt AudioBitRateL() ; //This is done in devsound using the custom interface.
	IMPORT_C TFourCC AudioTypeL() ;
	IMPORT_C TBool AudioEnabledL() ;
	IMPORT_C TTimeIntervalMicroSeconds PositionL() ;
	IMPORT_C TTimeIntervalMicroSeconds DurationL() ;
	
	IMPORT_C TInt Volume() ;
	IMPORT_C void PriorityL(TInt &aPriority, TMdaPriorityPreference &aPref) ;
	IMPORT_C TInt Balance() ;
	
	IMPORT_C TVideoRotation RotationL() ;
	IMPORT_C void GetScaleFactorL(TReal32 &aWidthPercentage, TReal32 &aHeightPercentage, TBool &aAntiAliasFiltering) ;
	IMPORT_C void GetCropRegionL(TRect &aCropRegion) ;

	IMPORT_C TInt NumberOfMetaDataEntriesL();
    IMPORT_C void GetMetaDataArrayL(RPointerArray<CMMFMetaDataEntry>& aMetaData);
	IMPORT_C const CMMFControllerImplementationInformation &ControllerImplementationInformationL();

	IMPORT_C TMVSState GetState();

	IMPORT_C void SetVideoOutputL(RWsSession& aWs,
                                  CWsScreenDevice& aScreenDevice,
                                  RWindow& aWindow,
                                  TRect& aScreenRect,
                                  TRect& aClipRect);

    IMPORT_C void AddDisplayWindowL(RWsSession& aWs,
                                    CWsScreenDevice& aScreenDevice,
                                    RWindow& aWindow,
                                    const TRect& aScreenRect,
                                    const TRect& aClipRect);

    IMPORT_C void AddDisplayWindowL(RWsSession& aWs,
                                    CWsScreenDevice& aScreenDevice,
                                    RWindow& aWindow);

	IMPORT_C void AddDisplayL(RWsSession& aWs, TInt aDisplay, MMMFSurfaceEventHandler& aEventHandler);

    IMPORT_C void RemoveDisplayWindowL(RWindow& aWindow);
	IMPORT_C void RemoveDisplay(TInt aDisplay);

    IMPORT_C TInt MaxVolume();
    IMPORT_C TInt SetScreenNumber(TInt aScreenNumber);
    IMPORT_C TInt RegisterForNotification();
    IMPORT_C TInt CancelNotification();
    IMPORT_C TInt WillResumePlay();
    IMPORT_C TBool SupportVideoPlayerUtility2() const; 

	IMPORT_C void SetAutoScaleL(RWindow& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos);
	IMPORT_C void SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos);
	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	IMPORT_C void DisableSubtitlesL();
	IMPORT_C void EnableSubtitlesL();
	IMPORT_C void RenderSubtitle(const TRect& aRect);
	IMPORT_C TBool SubtitlesAvailable();
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	
public: 
	void MvpuoOpenComplete(TInt aError);
    void MvpuoPrepareComplete(TInt aError);
    void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
    void MvpuoPlayComplete(TInt aError);
    void MvpuoEvent(const TMMFEvent& aEvent);
    
    //From MMMFAudioResourceNotificationCallback
    void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);        
private:
	void ConstructL();
    CMVSVideoPlayAgent(MMVSClientObserver& aObserver);

private:
	CVideoPlayerUtility*    iVideoPlayer;	
#ifdef SYMBIAN_BUILD_GCE	
	CVideoPlayerUtility2*   iVideoPlayer2;	
#endif	

	MMVSClientObserver& iObserver;
	TMVSState iState;
	TMMFEvent iEvent;
	RFs iFs;
	RFile iFile;
	CWsScreenDevice* iScreen;
	RWindow* iWindow;
	RWsSession iWs;
	RWindowGroup iRootWindow;
    RFileLogger iFileLogger;
    TInt iScreenNumber;
    TBool iSupportVideoPlayerUtility2;
    CVideoPlayerUtility*    iCurrentVideoPlayer;
    RBuf    iFileName;
    TUid    iControllerUid;
    TBool iRequestNotify;
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    TBool iEnableSubtitlesOnAdd;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
};




#endif  // MVSVIDEOPLAYAGENT_H
