// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef VIDEOPLAYERBODY_H
#define VIDEOPLAYERBODY_H

#include <fbs.h>
#include <w32std.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#endif

#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mda/common/base.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include "mmf/common/mmcaf.h"
#include "mmfclientutility.h"

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include "mmfsubtitleutility.h"
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

#ifdef SYMBIAN_BUILD_GCE
#include "mediaclientvideodisplaybody.h"
#endif

class CMMFVideoPlayerCallback;
class MMMFDRMCustomCommand;
class CVideoPlayerUtility;
class MMMFSurfaceEventHandler;
#ifdef SYMBIAN_BUILD_GCE
	class TWindowData;
	class CMediaClientVideoDisplayBody;
#endif // SYMBIAN_BUILD_GCE
NONSHARABLE_CLASS( CVideoPlayerUtility::CBody ): public CBase, 
								   				 public MDirectScreenAccess, 
								   				 public MMMFControllerEventMonitorObserver,
								   				 public MMMFFindAndOpenControllerObserver

	{

public:
	~CBody();

	static CBody* NewL(CVideoPlayerUtility* aParent,
					   MVideoPlayerUtilityObserver& aObserver,
					   TInt aPriority,
					   TInt aPref,
					   RWsSession& aWs,
					   CWsScreenDevice& aScreenDevice,
					   RWindowBase& aWindow,
					   const TRect& aScreenRect,
					   const TRect& aClipRect);

#ifdef SYMBIAN_BUILD_GCE
	static CBody* NewL(CVideoPlayerUtility2* aParent,
					   MVideoPlayerUtilityObserver& aObserver,
					   TInt aPriority,
					   TInt aPref);
#endif
	void OpenFileL(const TDesC& aFileName,TUid aControllerUid = KNullUid);
	void OpenFileL(const RFile& aFile, TUid aControllerUid = KNullUid);


	void OpenFileL(const TMMSource& aSource, TUid aControllerUid = KNullUid);

	void OpenDesL(const TDesC8& aDescriptor,TUid aControllerUid = KNullUid);
	void OpenUrlL(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType=KNullDesC8, TUid aControllerUid = KNullUid);

	void Prepare();

	void Close();

	void Play();
	void Play(const TTimeIntervalMicroSeconds& aStartPoint, const TTimeIntervalMicroSeconds& aEndPoint);
	TInt Stop();
	void PauseL();

	void SetPriorityL(TInt aPriority, TInt aPref);
	void PriorityL(TInt& aPriority, TMdaPriorityPreference& aPref) const;

	void SetDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow,
						   const TRect& aScreenRect, const TRect& aClipRect);

	void RegisterForVideoLoadingNotification(MVideoLoadingObserver& aObserver);

	void GetFrameL(TDisplayMode aDisplayMode, TBool aUseIntentAPI = EFalse, ContentAccess::TIntent aIntent = ContentAccess::EPeek );
	void RefreshFrameL();

	TReal32 VideoFrameRateL() const;
	void VideoFrameSizeL(TSize& aSize) const;
	const TDesC8& VideoFormatMimeType() const;
	TInt VideoBitRateL() const;

	TInt AudioBitRateL() const;
	TFourCC AudioTypeL() const;
	TBool AudioEnabledL() const;

	void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	TTimeIntervalMicroSeconds PositionL() const;
	TTimeIntervalMicroSeconds DurationL() const;

	void SetVolumeL(TInt aVolume);
	TInt Volume() const;
	TInt MaxVolume() const;
	void SetBalanceL(TInt aBalance);
	TInt Balance()const;

	void SetRotationL(TVideoRotation aRotation);
	TVideoRotation RotationL() const;
	void SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering);
	void GetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering) const;
	void SetCropRegionL(const TRect& aCropRegion);
	void GetCropRegionL(TRect& aCropRegion) const;

	TInt NumberOfMetaDataEntriesL() const;
	CMMFMetaDataEntry* MetaDataEntryL(TInt aIndex) const;
	
	void GetVideoLoadingProgressL(TInt& aPercentageProgress);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	
	void SetVideoFrameRateL(TReal32 aFramesPerSecond);
	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

	MMMFDRMCustomCommand* GetDRMCustomCommand();

	void StopDirectScreenAccessL();
	void StartDirectScreenAccessL();
	
	TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback, TUid aNotificationEventUid, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
	TInt WillResumePlay();

	TInt SetInitScreenNumber(TInt aScreenNumber);

    void SetPlayVelocityL(TInt aVelocity);
    TInt PlayVelocityL() const;
    void StepFrameL(TInt aStep);
    void GetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities) const;
    void SetVideoEnabledL(TBool aVideoEnabled);
    TBool VideoEnabledL() const;
    void SetAudioEnabledL(TBool aAudioEnabled);
    void SetAutoScaleL(TAutoScaleType aScaleType);
    void SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos);
		
#ifdef SYMBIAN_BUILD_GCE
	void AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, 
							RWindow& aWindow, const TRect& aVideoExtent, 
							const TRect& aWindowClipRect);
	void AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindow& aWindow);
 	void RemoveDisplayWindow(RWindowBase& aWindow);	
 	void SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent);	
	void SetWindowClipRectL(const RWindowBase& aWindow, const TRect& aWindowClipRect);
    
   	void SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType);
    void SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos);

	void SetRotationL(const RWindowBase& aWindow, TVideoRotation aRotation);
	TVideoRotation RotationL(const RWindowBase& aWindow) const;
	void SetScaleFactorL(const RWindowBase& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage);
	void GetScaleFactorL(const RWindowBase& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage) const;
	void AddDisplayL(TInt aDisplay, MMMFSurfaceEventHandler& aEventHandler);
	void RemoveDisplay(TInt aDisplay);
	void ConvertFromRelativeToDisplayToRelativeToWindow(
	        const RWindowBase& aWindow,
	        const TRect& aFromWindowRect,
	        const TRect& aFromClipRect,
	        TRect& aToWindowRect,
	        TRect& aToClipRect);
	void SetExternalDisplaySwitchingL(TInt aDisplay, TBool aControl);
#endif

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	TBool SubtitlesAvailable();
	void EnableSubtitlesL();
	void DisableSubtitles();
	TArray<TLanguage> SupportedSubtitleLanguagesL();
	TLanguage SubtitleLanguageL();
	void SetSubtitleLanguageL(TLanguage aLanguage);
	void RedrawSubtitle(RWindow& aWindow, const TRect &aRect);
	static RWindow* FindWindowWithWsHandle(const RPointerArray<CMediaClientVideoDisplayBody>& aDisplays, TInt aWsHandle);
	TInt EnableSubtitles(CMediaClientVideoDisplayBody& aDisplay);
	TInt AddSubtitleConfig(CMediaClientVideoDisplayBody::TWindowData& aWindowData);
	void GetSubtitleConfigFromWindowData(CMediaClientVideoDisplayBody::TWindowData& aWindowData, TMMFSubtitleWindowConfig& aConfig);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

private:
	void ConstructL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow,
					const TRect& aWindowRect, const TRect& aClipRect);
	CBody(CVideoPlayerUtility* aParent,
		  MVideoPlayerUtilityObserver& aObserver,
		  TInt aScreenNumber,
		  TInt aPriority=EMdaPriorityNormal,
		  TInt aPref=EMdaPriorityPreferenceTimeAndQuality);

	void CommonConstructL();
	void SetAndUpdateWindow();
	void PrepareDSAL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow);

#ifdef SYMBIAN_BUILD_GCE	
	CBody(CVideoPlayerUtility2* aParent,
		  MVideoPlayerUtilityObserver& aObserver,
		  TInt aPriority=EMdaPriorityNormal,
		  TInt aPref=EMdaPriorityPreferenceTimeAndQuality);

	void ConstructL();
	TInt CheckSurfaceSupported();
	TBool RectContains(const TRect& aLeft, const TRect& aRight);
	void DoAddDisplayWindowL(RWsSession& aWs, TInt aDisplay, RWindowBase& aWindow,
								const TRect& aClipRect, const TRect& aVideoExtent, RWindow* aWindow2);
	void DoUpdateDisplayWindowL(RWsSession& aWs, RWindowBase& aWindow,
	              const TRect& aClipRect, const TRect& aVideoExtent, RWindow* aWindow2);
	TInt SurfaceCreated();
	TInt SurfaceParametersChanged();
	TInt RemoveSurface(TBool aControllerEvent);
	TInt SetAllBackgroundSurfaces();

#endif // SYMBIAN_BUILD_GCE
	
	// from MMMFControllerEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);

	//from MDirectScreenAccess
	virtual void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	virtual void Restart(RDirectScreenAccess::TTerminationReasons aReason);

	// from MMMFFindAndOpenControllerObserver 
	virtual void MfaocComplete(
		TInt& aError, 
		RMMFController* aController, 
		TUid aControllerUid, 
		TMMFMessageDestination* aSourceHandle, 
		TMMFMessageDestination* aSinkHandle);


	void OpenComplete(TInt aError);
	void FrameReady(TInt aError);
	void PlayComplete(TInt aError);
	void Reset();

private:
	//Source and sink handle info
	TMMFMessageDestination iSourceHandle;
	TMMFMessageDestination iSinkHandle;

	RMMFController iController;
	RMMFVideoControllerCustomCommands iVideoControllerCustomCommands;
	RMMFVideoPlayControllerCustomCommands iVideoPlayControllerCustomCommands;
	RMMFAudioPlayDeviceCustomCommands iAudioPlayDeviceCustomCommands;
	RMMFAudioRecordDeviceCustomCommands iAudioRecordDeviceCustomCommands;
	RMMFVideoDRMExtCustomCommands iVideoDRMExtCustomCommands;
	RMMFDRMCustomCommands iDRMCustomCommands;
	RMMFVideoPlayControllerExtCustomCommands iVideoPlayControllerExtCustomCommands;
	RMMFResourceNotificationCustomCommands iNotificationRegistrationCommands;
	MMMFAudioResourceNotificationCallback* iAudioResourceNotificationCallBack;
#ifdef SYMBIAN_BUILD_GCE
	RMMFVideoPlaySurfaceSupportCustomCommands iVideoPlaySurfaceSupportCustomCommands;
#endif	
	//for audio resource notification
	TUid iArnEventHolder;
	TBuf8<TMMFAudioConfig::KNotificationDataBufferSize> iNotificationDataHolder;
	
	RMMFVideoSetInitScreenCustomCommands iVideoSetInitScreenCustomCommands;
	MVideoPlayerUtilityObserver& iObserver;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	CVideoPlayerUtility::TMMFVideoPlayerState iState;
	TTimeIntervalMicroSeconds iDuration;
	TMMFPrioritySettings iPrioritySettings;
	
	MVideoLoadingObserver *iVideoLoadingObserver;
	CDirectScreenAccess * iDirectScreenAccess;
	HBufC8* iMimeType;
	CMMFVideoPlayerCallback* iAsyncCallback;
	CMMFVideoPlayerCallback* iFrameCallback;
	CMMFControllerImplementationInformation* iControllerImplementationInformation;
	TUid iControllerUid;

	CVideoPlayerUtility* iParent;
	CFbsBitmap* iFrameBitmap;
	RFbsSession iFbsSession;

	TRect iWindowRect;
	TRect iClipRect;
	TBool iControllerOpen;
	TBool iFbsSessionConnected;

	// utility class to find and open a suitable controller asynchronously
	CMMFFindAndOpenController* iFindAndOpenController;

	// received a KMMFEventCategoryVideoOpenComplete event
	TBool iEventOpenReceived;
	
	// received a MfaocComplete() callback
	TBool iCallbackOpenReceived;
	
	TInt iOpenError;
	TInt iScreenNumber;

	// flag indicating DSA abort in StopDirectScreenAccess() - to be used in SetDisplayWindowL
	TBool iDirectScreenAccessAbort;	
		
#ifdef SYMBIAN_BUILD_GCE

	TSurfaceId iSurfaceId;
	
	//video graphics surface supported
	TBool iGraphicsSurfaceSupported;
	TBool iUsingVPU2;
			
	// store scale factor
	TReal32 iGlobalScaleWidth;
	TReal32 iGlobalScaleHeight;
	TBool iAntiAliasFiltering;
	TRect iCropRegion;
	TVideoRotation iGlobalRotation;
	TRect iSurfaceCropRect;
	TVideoAspectRatio iAspectRatio;
	// for storing window added during NewL,
	// i.e. DSA
	RWsSession* iWs;
	TInt iDisplayId;
	RWindowBase* iWindow;
			
	RPointerArray<CMediaClientVideoDisplayBody> iActiveDisplays;

	// Autoscale info
	TAutoScaleType iGlobalAutoScaleType;
	TInt iGlobalHorizPos;
	TInt iGlobalVertPos;
#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	CMMFSubtitleUtility* iSubtitleUtility;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	friend class CMMFVideoPlayerCallback;
	friend class CTestStepUnitMMFVidClient;
	friend class CTestView;
	friend class CVideoPlayerUtility;
	
#ifdef SYMBIAN_BUILD_GCE
	friend class CVideoPlayerUtility2;
#endif
	};


#endif // VIDEOPLAYERBODY_H
