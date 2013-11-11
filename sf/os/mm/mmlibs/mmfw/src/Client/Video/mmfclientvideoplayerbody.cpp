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


#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfdrmcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include "mmfvideocallback.h"
#include "VideoPlayerBody.h"
#include "mmfclientvideocommon.h"
#include "mmfclientutility.h"
#include "mediaclientvideotrace.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfvideoenums.h>
#endif

#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#include <surfaceeventhandler.h>
#endif

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include <mmf/common/mmfpaniccodes.h>
#include "mmfsubtitleutility.h"
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

using namespace ContentAccess;

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

#ifdef _DEBUG
void VPUDebugPanicOrLeaveL(TInt aVPUPanicCode, TInt aLeaveCode)
	{
	_LIT(KMMFVideoPlayerUtilityPanicCategory, "MMFVideoPlayUtil");
	User::Panic(KMMFVideoPlayerUtilityPanicCategory, aVPUPanicCode);
	User::Leave(aLeaveCode); // added for leavescan
	}

#else
void VPUDebugPanicOrLeaveL(TInt /*aVPUPanicCode*/, TInt aLeaveCode)
	{
	User::Leave(aLeaveCode);
	}
#endif //_DEBUG

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

CVideoPlayerUtility::CBody* CVideoPlayerUtility::CBody::NewL(CVideoPlayerUtility* aParent,
															 MVideoPlayerUtilityObserver& aObserver,
															 TInt aPriority,
															 TInt aPref,
															 RWsSession& aWs,
															 CWsScreenDevice& aScreenDevice,
															 RWindowBase& aWindow,
															 const TRect& aScreenRect,
															 const TRect& aClipRect)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::NewL (CVPU1)++");
	CBody* self = new(ELeave) CBody(aParent, aObserver, aScreenDevice.GetScreenNumber(), aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL(aWs, aScreenDevice, aWindow, aScreenRect, aClipRect);
	CleanupStack::Pop();
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::NewL (CVPU1)--");
	return self;
	}

#ifdef SYMBIAN_BUILD_GCE
CVideoPlayerUtility::CBody* CVideoPlayerUtility::CBody::NewL(CVideoPlayerUtility2* aParent,
															 MVideoPlayerUtilityObserver& aObserver,
															 TInt aPriority,
															 TInt aPref)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::NewL (CVPU2)++");
	CBody* self = new(ELeave) CBody(aParent, aObserver, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::NewL (CVPU2)--");
	return self;
	}
#endif // SYMBIAN_BUILD_GCE

CVideoPlayerUtility::CBody::~CBody()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::~CBody++");
	Close();
	
	delete iControllerImplementationInformation;
	delete iDirectScreenAccess;
	delete iFindAndOpenController;
	delete iControllerEventMonitor;
	delete iFrameCallback;
	delete iAsyncCallback;
	delete iMimeType;
	delete iFrameBitmap;
	if (iFbsSessionConnected)
		iFbsSession.Disconnect();
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::~CBody--");
	}

void CVideoPlayerUtility::CBody::Close()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Close++");
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	DisableSubtitles();
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	if (iAsyncCallback)
		iAsyncCallback->Cancel();
	if (iFrameCallback)
		iFrameCallback->Cancel();
	if (iFindAndOpenController)
		iFindAndOpenController->Close();
	if (iControllerEventMonitor)
		iControllerEventMonitor->Cancel();
	if(iControllerImplementationInformation)
		{
		delete iControllerImplementationInformation;
		iControllerImplementationInformation = NULL;
		}

#ifdef SYMBIAN_BUILD_GCE
	iActiveDisplays.ResetAndDestroy();
	iSurfaceId = TSurfaceId::CreateNullId();
#endif

	iController.Close();	
	iControllerUid = KNullUid;
	iControllerOpen = EFalse;
	iDirectScreenAccessAbort = EFalse;
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Close--");
	}

CVideoPlayerUtility::CBody::CBody(CVideoPlayerUtility* aParent,
								  MVideoPlayerUtilityObserver& aObserver,
								  TInt aScreenNumber,
								  TInt aPriority,
								  TInt aPref) : 
	iVideoControllerCustomCommands(iController),
	iVideoPlayControllerCustomCommands(iController),
	iAudioPlayDeviceCustomCommands(iController),
	iAudioRecordDeviceCustomCommands(iController),
	iVideoDRMExtCustomCommands(iController),
	iDRMCustomCommands(iController),
	iVideoPlayControllerExtCustomCommands(iController),	
	iNotificationRegistrationCommands(iController),
#ifdef SYMBIAN_BUILD_GCE
	iVideoPlaySurfaceSupportCustomCommands(iController),
#endif
	iArnEventHolder(KNullUid),
	iVideoSetInitScreenCustomCommands(iController),
	iObserver(aObserver),
	iState(EStopped),
	iParent(aParent),
	iScreenNumber(aScreenNumber)
#ifdef SYMBIAN_BUILD_GCE
	,iGlobalScaleWidth(100.0f),
	iGlobalScaleHeight(100.0f),
	iGlobalAutoScaleType(EAutoScaleBestFit),  // Really need some platform specific way of defining this. Not everyone will want it.
	iGlobalHorizPos(EHorizontalAlignCenter),
	iGlobalVertPos(EVerticalAlignCenter)
#endif
	{
	iPrioritySettings.iPriority = aPriority;
	iPrioritySettings.iPref = aPref;
	}

#ifdef SYMBIAN_BUILD_GCE
CVideoPlayerUtility::CBody::CBody(CVideoPlayerUtility2* aParent,
				MVideoPlayerUtilityObserver& aObserver,
				TInt aPriority,
				TInt aPref) : 
	iVideoControllerCustomCommands(iController),
	iVideoPlayControllerCustomCommands(iController),
	iAudioPlayDeviceCustomCommands(iController),
	iAudioRecordDeviceCustomCommands(iController),
	iVideoDRMExtCustomCommands(iController),
	iDRMCustomCommands(iController),
	iVideoPlayControllerExtCustomCommands(iController),
	iNotificationRegistrationCommands(iController),
	iVideoPlaySurfaceSupportCustomCommands(iController),
	iArnEventHolder(KNullUid),
	iVideoSetInitScreenCustomCommands(iController),
	iObserver(aObserver),
	iState(EStopped),
	iParent(aParent),
	iUsingVPU2(ETrue),
	iGlobalScaleWidth(100.0f),
	iGlobalScaleHeight(100.0f),
	iGlobalHorizPos(EHorizontalAlignCenter),
	iGlobalVertPos(EVerticalAlignCenter)
	{
	iPrioritySettings.iPriority = aPriority;
	iPrioritySettings.iPref = aPref;
	}

void CVideoPlayerUtility::CBody::ConstructL()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::ConstructL1++");
	CommonConstructL();
	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
	iFindAndOpenController->SetSurfaceMode(iUsingVPU2, &iVideoPlaySurfaceSupportCustomCommands);
	iFindAndOpenController->Configure(KUidMediaTypeVideo, iPrioritySettings, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	iFindAndOpenController->ConfigureController(iController, *iControllerEventMonitor, CMMFFindAndOpenController::EPlayback);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::ConstructL1--");
	}

#endif // SYMBIAN_BUILD_GCE

void CVideoPlayerUtility::CBody::ConstructL(RWsSession& aWs,
						CWsScreenDevice& aScreenDevice,
						RWindowBase& aWindow,
						const TRect& aWindowRect,
						const TRect& aClipRect)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::ConstructL2++");
	CommonConstructL();
	
	SetDisplayWindowL(aWs, aScreenDevice, aWindow, aWindowRect, aClipRect);
	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
#ifdef SYMBIAN_BUILD_GCE	
	iFindAndOpenController->SetSurfaceMode(iUsingVPU2, &iVideoPlaySurfaceSupportCustomCommands);
	
	// If we open a controller with surface support we may render to surfaces.  We need to store these
	// so we can add the display as a surface rendering target.

	iWs = &aWs;
	iDisplayId = aScreenDevice.GetScreenNumber();
	iWindow = &aWindow;
	iWindowRect = aWindowRect;
	iClipRect = aClipRect;	

#endif
	iFindAndOpenController->Configure(KUidMediaTypeVideo, iPrioritySettings, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	iFindAndOpenController->ConfigureController(iController, *iControllerEventMonitor, CMMFFindAndOpenController::EPlayback);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::ConstructL2--");
	}

void CVideoPlayerUtility::CBody::CommonConstructL()
	{
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	iMimeType = HBufC8::NewL(KMaxMimeTypeLength);
	iAsyncCallback = new (ELeave) CMMFVideoPlayerCallback(iObserver);
	iFrameCallback = new (ELeave) CMMFVideoPlayerCallback(iObserver);
	User::LeaveIfError(iFbsSession.Connect());
	iFbsSessionConnected = ETrue;
	}

void CVideoPlayerUtility::CBody::Reset() 
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Reset++");
	Close();
	
	// reset all state variables
	iEventOpenReceived = EFalse;
	iCallbackOpenReceived = EFalse;
	iOpenError = KErrNone;
#ifdef SYMBIAN_BUILD_GCE
	iGraphicsSurfaceSupported = EFalse;
#endif
	iControllerUid = KNullUid;
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Reset--");
	}

void CVideoPlayerUtility::CBody::SetAndUpdateWindow()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetAndUpdateWindow++");
	if (iOpenError == KErrNone)	
		{
		// Set the display window and update display region if the controller doesn't support graphics surface. 		
		iOpenError = iVideoPlayControllerCustomCommands.SetDisplayWindow(iWindowRect, iClipRect);
		}

	if (iOpenError == KErrNone && iDirectScreenAccess && iDirectScreenAccess->DrawingRegion())
		{
		iOpenError = iVideoPlayControllerCustomCommands.UpdateDisplayRegion(*iDirectScreenAccess->DrawingRegion());
		}
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetAndUpdateWindow--");
	}

void CVideoPlayerUtility::CBody::MfaocComplete(		
	TInt& aError, 
	RMMFController*	/*aController*/,
	TUid aControllerUid, 
	TMMFMessageDestination* /*aSourceHandle*/, 
	TMMFMessageDestination* /*aSinkHandle*/)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::MfaocComplete++");
    DEBUG_PRINTF3("CVideoPlayerUtility::CBody::MfaocComplete - aError %d, aControllerUid 0x%X", aError, aControllerUid.iUid);
	iCallbackOpenReceived = ETrue;

	// save the error in iOpenError -
	// unless HandleEvent(KMMFEventCategoryVideoOpenComplete) 
	// has reported an error already
	if (iOpenError == KErrNone)
		iOpenError = aError;
	
	if (iOpenError == KErrNone)
		{
#ifdef SYMBIAN_BUILD_GCE
		// Check if the graphics surfaces is supported
		TInt err = CheckSurfaceSupported();

		DEBUG_PRINTF4("CVideoPlayerUtility::CBody::MfaocComplete - Checked surfaces supported - Err %d, Surfaces supported %d, Using VPU2 %d", err, iGraphicsSurfaceSupported, iUsingVPU2);
	
		if (!iGraphicsSurfaceSupported)
			{
			if (err != KErrNone && iUsingVPU2)
				{
				// Set KErrNotSupported if the controller doesn't support graphics surface and 
				// the open source is using CVideoPlayerUtility2
				iOpenError = err;
				}
			else
				{
				SetAndUpdateWindow();
				}
			}
#else
		SetAndUpdateWindow();
#endif // SYMBIAN_BUILD_GCE	
	
		// now that the controller is open
		if (iOpenError == KErrNone)
			{
			iControllerOpen = ETrue;
			iControllerUid = aControllerUid;

#ifdef SYMBIAN_BUILD_GCE		
			// We are using a controller that supports graphics surfaces but are not in surface mode.  
			// Call AddDisplay so we can render to surfaces using the CVideoUtility API.
			if (iGraphicsSurfaceSupported && !iUsingVPU2)
				{
				if (iDirectScreenAccess)
					{
		 			iDirectScreenAccess->Cancel();
					}
				
		        // When using surfaces for CVPU we use DoAddDisplayWindowL() which requires clip rectangle and video extent
		        // as arguments. Video extent is not supported by CVPU so aWindowRect is used instead. This function
		        // assumes these args are relative to the window. However they are relative to the display and must be
		        // converted.
		        TRect windowRectRelativeToWindow;
		        TRect clipRectRelativeToWindow;     
		        ConvertFromRelativeToDisplayToRelativeToWindow(*iWindow, iWindowRect, iClipRect, windowRectRelativeToWindow, clipRectRelativeToWindow);

				TRAP(iOpenError, DoAddDisplayWindowL(*iWs, iDisplayId, *iWindow, clipRectRelativeToWindow, windowRectRelativeToWindow, NULL));				
				}
#endif		
			}
		// If an error occurred in any of the above, try for next controller if present
		// in the selection list.
		else
			{
			if(iFindAndOpenController->ControllerIndex() < (iFindAndOpenController->ControllerCount())-1)
				{
				aError = iOpenError;
				
				if (iAsyncCallback)
					{
					iAsyncCallback->Cancel();
					}
		
				if (iControllerEventMonitor)
					{
					iControllerEventMonitor->Cancel();
					}
				iOpenError = KErrNone;
					
				return;
				}
				
			Close();
			}
		}

	if(iOpenError != KErrNone && !iControllerOpen)
		{
		if (iFindAndOpenController)	
			{
			//if try next controller was called when specific controller uid was 
			//given, skip the searching and report error.
			if(!iFindAndOpenController->StopTryLoadController()) 
				{
				if( !(aError == KErrNoMemory || aError == KErrCancel ) ) 
					{
					if(iFindAndOpenController->ControllerIndex() < (iFindAndOpenController->ControllerCount())-1)
						{
						aError = iOpenError;   // Actually tries to load controller
						iOpenError = KErrNone;
						return;
						}
					}
				}
			}
		}
	
	// if we've already received the open complete event
	// call the client now (otherwise wait for it)
	if (iEventOpenReceived || iOpenError != KErrNone)
		{
		// stop a subsequent HandleEvent(KMMFEventCategoryVideoOpenComplete) 
		// from issuing another callback to client
		iCallbackOpenReceived = EFalse;
		iAsyncCallback->Signal(iOpenError, CMMFVideoPlayerCallback::EOpenCompleteEvent);
		}
	
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::MfaocComplete--");
	}

void CVideoPlayerUtility::CBody::OpenFileL(const TDesC& aFileName, TUid aControllerUid)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenFileL (filename)++");
	TMMFileSource filesource(aFileName, KDefaultContentObject, EPlay);
	OpenFileL(filesource, aControllerUid);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenFileL (filename)--");
	}
	
void CVideoPlayerUtility::CBody::OpenFileL(const RFile& aFile, TUid aControllerUid)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenFileL (filehandle)++");
	RFile& file = const_cast<RFile&>(aFile);
	TMMFileHandleSource filehandlesource(file, KDefaultContentObject, EPlay);
	OpenFileL(filehandlesource, aControllerUid);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenFileL (filehandle)--");
	}

void CVideoPlayerUtility::CBody::OpenFileL(const TMMSource& aSource, TUid aControllerUid)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenFileL (source)++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::OpenFileL - aControllerUid 0x%X", aControllerUid.iUid);
	// Make sure we are closed
	Reset();

	iFindAndOpenController->ConfigureSourceSink(
		aSource,
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));

#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	iFindAndOpenController->SetInitScreenNumber(iScreenNumber, &iVideoSetInitScreenCustomCommands);
#endif

	if (aControllerUid != KNullUid) 
		{
		iFindAndOpenController->OpenByControllerUid(aControllerUid);
		}
	else
		{
		iFindAndOpenController->OpenByFileSource(aSource);
		}

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenFileL (source)--");
	}
	
//
// This method launches and initializes plugin controller based on the stream 
// source header passed in the descriptor buffer.
//
void CVideoPlayerUtility::CBody::OpenDesL(const TDesC8& aDescriptor, TUid aControllerUid)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenDesL++");
	// Make sure we are closed
	Reset();

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSource, CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
	
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	iFindAndOpenController->SetInitScreenNumber(iScreenNumber, &iVideoSetInitScreenCustomCommands);
#endif
		
	if (aControllerUid!=KNullUid)
		{
		iFindAndOpenController->OpenByControllerUid(aControllerUid);
		}
	else
		{
		iFindAndOpenController->OpenByDescriptor(aDescriptor);
		}
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenDesL--");
	}
	
void CVideoPlayerUtility::CBody::OpenUrlL(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType, TUid aControllerUid)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenUrlL++");
	// Make sure we are closed
	Reset();

	CBufFlat* urlCfgBuffer = NULL;
    CMMFFindAndOpenController::GetConfigUrlL(urlCfgBuffer, aUrl, aIapId);

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
	
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	iFindAndOpenController->SetInitScreenNumber(iScreenNumber, &iVideoSetInitScreenCustomCommands);
#endif
		
	if (aControllerUid!=KNullUid)
		{
		iFindAndOpenController->OpenByControllerUid(aControllerUid);
		}
	else
		iFindAndOpenController->OpenByUrl(aUrl, aIapId, aMimeType);

	delete urlCfgBuffer;

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::OpenUrlL--");
	}

void CVideoPlayerUtility::CBody::Play()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Play++");
	TInt err = iController.Prime();
	if (err == KErrNone)
		err = iController.Play();
	if (err != KErrNone)
	    {
	    iAsyncCallback->Signal(err,CMMFVideoPlayerCallback::EPlayCompleteEvent);
	    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::Play (exit 1)-- - Err %d", err);
	    return;
	    }
	iState = EPlaying;
	//If Audio Resource Notification request is pending - attempt now
	if(iArnEventHolder != KNullUid)
		{
		err = iNotificationRegistrationCommands.RegisterAsClient(iArnEventHolder, iNotificationDataHolder);
		iArnEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		if(err != KErrNone)
			{
			iAsyncCallback->Signal(err, CMMFVideoPlayerCallback::EPlayCompleteEvent);
			}
		}
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::Play-- - Err %d", err);
	}

void CVideoPlayerUtility::CBody::Play(const TTimeIntervalMicroSeconds& aStartTime, const TTimeIntervalMicroSeconds& aEndTime)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Play++");
    DEBUG_PRINTF3("CVideoPlayerUtility::CBody::Play - aStartTime %ld, aEndTime %ld", aStartTime.Int64(), aEndTime.Int64());
	TInt err = iController.Prime();
	if (err == KErrNone)
	err = iVideoPlayControllerCustomCommands.Play(aStartTime, aEndTime);
	if (err != KErrNone)
	    {
	    iAsyncCallback->Signal(err, CMMFVideoPlayerCallback::EPlayCompleteEvent);
		DEBUG_PRINTF2("CVideoPlayerUtility::CBody::Play (exit1)-- - Err %d", err);
	    return;
	    }
	iState = EPlaying;
	//If Audio Resource Notification request is pending - attempt now
	if(iArnEventHolder != KNullUid)
		{
		err = iNotificationRegistrationCommands.RegisterAsClient(iArnEventHolder, iNotificationDataHolder);			
		iArnEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		if(err != KErrNone)
			{
			iAsyncCallback->Signal(err, CMMFVideoPlayerCallback::EPlayCompleteEvent);
			}
		}
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::Play-- - Err %d", err);
	}


TInt CVideoPlayerUtility::CBody::Stop()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::Stop++");
	TInt err = iController.Stop();
	iState = EStopped;
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::Stop-- - Err %d", err);
	return err;
	}

void CVideoPlayerUtility::CBody::PauseL()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::PauseL++");
	User::LeaveIfError(iController.Pause());
	iState = EPaused;
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::PauseL--");
	}

void CVideoPlayerUtility::CBody::SetVolumeL(TInt aVolume)
	{
	User::LeaveIfError(iAudioPlayDeviceCustomCommands.SetVolume(aVolume));
	}

void CVideoPlayerUtility::CBody::PrepareDSAL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::PrepareDSAL++");
	CDirectScreenAccess* old = iDirectScreenAccess;
	iDirectScreenAccess = CDirectScreenAccess::NewL(aWs,aScreenDevice,aWindow,*this);
	delete old;
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::PrepareDSAL - Starting");
	iDirectScreenAccess->StartL();
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::PrepareDSAL--");
	}

void CVideoPlayerUtility::CBody::SetDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice,
							RWindowBase& aWindow, const TRect& aWindowRect,
							const TRect& aClipRect)
	{	
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetDisplayWindowL++");
    DEBUG_PRINTF4("CVideoPlayerUtility::CBody::SetDisplayWindowL - aScreenDevice num %d, width %d, height %d", aScreenDevice.GetScreenNumber(), aScreenDevice.SizeInPixels().iWidth, aScreenDevice.SizeInPixels().iHeight);
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetDisplayWindowL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::SetDisplayWindowL - aWindow abs pos %d,%d - width %d, height %d", aWindow.AbsPosition().iX, aWindow.AbsPosition().iY, aWindow.Size().iWidth, aWindow.Size().iHeight);
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::SetDisplayWindowL - aWindowRect %d,%d - %d,%d", aWindowRect.iTl.iX, aWindowRect.iTl.iY, aWindowRect.iBr.iX, aWindowRect.iBr.iY);
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::SetDisplayWindowL - aClipRect %d,%d - %d,%d", aClipRect.iTl.iX, aClipRect.iTl.iY, aClipRect.iBr.iX, aClipRect.iBr.iY);

    iWindowRect = aWindowRect;
	iClipRect = aClipRect;
	
#ifdef SYMBIAN_BUILD_GCE
	// If called from CVideoPlayerUtility2, fail with KErrNotSupport. Otherwise, if the controller supports 
	// the graphics surfaces, it will render to graphics surfaces. If the controller doesn't support 
	// the graphics surfaces, it will use direct screen access.
	if (iUsingVPU2)
		{
		User::Leave(KErrNotSupported);
		}
	// If the controller does not support GCE or the source has not been opened, start new DSA.
	if (!iGraphicsSurfaceSupported)
		{
		PrepareDSAL(aWs, aScreenDevice, aWindow);
		}

	if (!iGraphicsSurfaceSupported && iControllerOpen)
		{
		// Set display window and update region, if controller is open
		User::LeaveIfError(iVideoPlayControllerCustomCommands.SetDisplayWindow(iWindowRect,iClipRect));
		User::LeaveIfError(iVideoPlayControllerCustomCommands.UpdateDisplayRegion(*iDirectScreenAccess->DrawingRegion()));
		}
		
	if (iGraphicsSurfaceSupported && iControllerOpen)
		{
		// When the controller supports graphics surfaces, need to stop direct screen access.
 		if (iDirectScreenAccess)
 			{
 			iDirectScreenAccess->Cancel();
  			AbortNow(RDirectScreenAccess::ETerminateCancel);
 			}
		
		// When using surfaces for CVPU we use DoAddDisplayWindowL() which requires clip rectangle and video extent
		// as arguments. Video extent is not supported by CVPU so aWindowRect is used instead. This function
		// assumes these args are relative to the window. However they are relative to the display and must be
		// converted.
		TRect windowRectRelativeToWindow;
		TRect clipRectRelativeToWindow;		
		ConvertFromRelativeToDisplayToRelativeToWindow(aWindow, aWindowRect, aClipRect, windowRectRelativeToWindow, clipRectRelativeToWindow);

		// check if display for window already exists and if so do an update else create a new display
		TRAPD(err, CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow));
		if (err == KErrNone)
		    {
		    DoUpdateDisplayWindowL(aWs, aWindow, clipRectRelativeToWindow, windowRectRelativeToWindow, NULL);
		    }
		else
		    {
		    DoAddDisplayWindowL(aWs, aScreenDevice.GetScreenNumber(), aWindow, clipRectRelativeToWindow, windowRectRelativeToWindow, NULL);
		    }
		}
#else
	PrepareDSAL(aWs, aScreenDevice, aWindow);
	
	if (iControllerOpen)
		{
		// Set display window and update region, if controller is open
		User::LeaveIfError(iVideoPlayControllerCustomCommands.SetDisplayWindow(iWindowRect,iClipRect));
		User::LeaveIfError(iVideoPlayControllerCustomCommands.UpdateDisplayRegion(*iDirectScreenAccess->DrawingRegion()));
		if(iDirectScreenAccessAbort)
			{
			User::LeaveIfError(iVideoPlayControllerCustomCommands.DirectScreenAccessEvent(EResumeDSA));
			iDirectScreenAccessAbort = EFalse;
			}
		}
#endif
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetDisplayWindowL--");
	}

TReal32 CVideoPlayerUtility::CBody::VideoFrameRateL() const
	{
	TReal32 framerate = 0;
	User::LeaveIfError(iVideoControllerCustomCommands.GetFrameRate(framerate));
	return framerate;
	}


void CVideoPlayerUtility::CBody::VideoFrameSizeL(TSize& aSize) const
	{
	User::LeaveIfError(iVideoControllerCustomCommands.GetVideoFrameSize(aSize));
	}

TInt CVideoPlayerUtility::CBody::Volume() const
	{
	TInt vol = 0;
	iAudioPlayDeviceCustomCommands.GetVolume(vol);
	return vol;
	}


void CVideoPlayerUtility::CBody::SetBalanceL(TInt aBalance)
	{
	User::LeaveIfError(iAudioPlayDeviceCustomCommands.SetBalance(aBalance));
	}


TInt CVideoPlayerUtility::CBody::Balance() const
	{
	TInt bal = 0;
	iAudioPlayDeviceCustomCommands.GetBalance(bal);
	return bal;
	}

void CVideoPlayerUtility::CBody::SetRotationL(TVideoRotation aRotation)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetRotationL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetRotationL - aRotation %d", aRotation);

#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported)
		{
		User::LeaveIfError(iVideoPlayControllerCustomCommands.SetRotation(aRotation));
		}
	else
		{
		// Rotation setting is not sent to controller when graphics surfaces are used.
		// If the surface has been created, perform rotation with the help of graphics surfaces;
		// otherwise, just store the rotation info.

		TInt count = iActiveDisplays.Count();
		
		for (TInt i = 0; i < count; ++i)
			{
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
			CMediaClientVideoDisplayBody* display = iActiveDisplays[i];
			RArray<CMediaClientVideoDisplayBody::TWindowData>& windows = display->Windows();
			TInt windowcount = windows.Count();	
			for (TInt i = 0; i < windowcount; ++i)
				{
				CMediaClientVideoDisplayBody::TWindowData& window = windows[i];		
				if (window.iRotation != aRotation)
					{
					// update config only if setting has actually changed
					display->SetRotationL(*window.iWindow, aRotation, iCropRegion);

					if (iSubtitleUtility)
						{
						TMMFSubtitleWindowConfig config;
						GetSubtitleConfigFromWindowData(window, config);

						iSubtitleUtility->UpdateSubtitleConfig(config);
						}
					}
				}
#else

			iActiveDisplays[i]->SetRotationL(aRotation, iCropRegion);
#endif//SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
			}

		iGlobalRotation = aRotation;

		}
#else 
	User::LeaveIfError(iVideoPlayControllerCustomCommands.SetRotation(aRotation));
#endif // SYMBIAN_BUILD_GCE

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetRotationL--");
	}

TVideoRotation CVideoPlayerUtility::CBody::RotationL() const
	{
	TVideoRotation rot = EVideoRotationNone;
#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported)
		{		
		User::LeaveIfError(iVideoPlayControllerCustomCommands.GetRotation(rot));		
		}
	else 
		{
		// Rotation setting is not retrieved from controller when graphics surfaces are used.
		rot = iGlobalRotation;
		}
#else
	User::LeaveIfError(iVideoPlayControllerCustomCommands.GetRotation(rot));
#endif // SYMBIAN_BUILD_GCE
	return rot;
	}

void CVideoPlayerUtility::CBody::SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetScaleFactorL");
    DEBUG_PRINTF4("CVideoPlayerUtility::CBody::SetScaleFactorL - aWidthPercentage %f, aHeightPercentage %f, aAntiAliasFiltering %d", aWidthPercentage, aHeightPercentage, aAntiAliasFiltering);

#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported)
		{
		User::LeaveIfError(iVideoPlayControllerCustomCommands.SetScaleFactor(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering));
		}
	else
		{
		// Scale factor  setting is not sent to controller when graphics surfaces are used.
		// If the surface has been created, perform scale factor with the help of graphics surfaces.
		iAntiAliasFiltering = aAntiAliasFiltering;
		iGlobalScaleWidth = aWidthPercentage;
		iGlobalScaleHeight = aHeightPercentage;
		iGlobalAutoScaleType = EAutoScaleNone;

		TInt count = iActiveDisplays.Count();
		
		for (TInt i = 0; i < count; ++i)
			{
			iActiveDisplays[i]->SetScaleFactorL(aWidthPercentage, aHeightPercentage, iCropRegion);
			}
		}
#else
	User::LeaveIfError(iVideoPlayControllerCustomCommands.SetScaleFactor(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering));
#endif	// SYMBIAN_BUILD_GCE

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetScaleFactorL--");
	}

void CVideoPlayerUtility::CBody::GetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering) const
	{
#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported)
		{
		User::LeaveIfError(iVideoPlayControllerCustomCommands.GetScaleFactor(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering));
		}
	else
		{
		// Scale factor setting is not retrieved from controller when graphics surfaces are used.
		
		aWidthPercentage = iGlobalScaleWidth;
		aHeightPercentage = iGlobalScaleHeight;	
		
		aAntiAliasFiltering = iAntiAliasFiltering;
		}
#else
	User::LeaveIfError(iVideoPlayControllerCustomCommands.GetScaleFactor(aWidthPercentage, aHeightPercentage, aAntiAliasFiltering));	
#endif // SYMBIAN_BUILD_GCE
	}

void CVideoPlayerUtility::CBody::SetCropRegionL(const TRect& aCropRegion)
	{
#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported)
		{
		User::LeaveIfError(iVideoPlayControllerCustomCommands.SetCropRegion(aCropRegion));
		}
	else
		{
		// Crop region setting is not sent to controller when graphics surfaces are used.
		// If the surface has been created, perform crop region with the help of graphics surfaces;
		// otherwise, just store the crop region info.
		if (aCropRegion != iCropRegion)
			{
			iCropRegion = aCropRegion;
			User::LeaveIfError(SetAllBackgroundSurfaces());
			}
		}
#else
	User::LeaveIfError(iVideoPlayControllerCustomCommands.SetCropRegion(aCropRegion));
#endif // SYMBIAN_BUILD_GCE
	}

void CVideoPlayerUtility::CBody::GetCropRegionL(TRect& aCropRegion) const
	{
#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported )
		{
		User::LeaveIfError(iVideoPlayControllerCustomCommands.GetCropRegion(aCropRegion));
		}
	else
		{
		// Crop rectangle setting is not retrieved from controller when graphics surfaces are used.
		aCropRegion = iCropRegion;
		}
#else
	User::LeaveIfError(iVideoPlayControllerCustomCommands.GetCropRegion(aCropRegion));
#endif // SYMBIAN_BUILD_GCE
	}

void CVideoPlayerUtility::CBody::Prepare()
	{
	TInt error = iVideoPlayControllerCustomCommands.Prepare();
	if (error)
		iObserver.MvpuoPrepareComplete(error);
	}

const TDesC8& CVideoPlayerUtility::CBody::VideoFormatMimeType() const
	{
	TPtr8 des = iMimeType->Des();
	TInt err = iVideoControllerCustomCommands.GetVideoMimeType(des);
	if (!err)
		return *iMimeType;
	else
		return KNullDesC8;
	}
											 
void CVideoPlayerUtility::CBody::RegisterForVideoLoadingNotification(MVideoLoadingObserver& aObserver)
	{
	iVideoLoadingObserver = &aObserver;
	}

TInt CVideoPlayerUtility::CBody::NumberOfMetaDataEntriesL() const
	{
	TInt num = 0;
	User::LeaveIfError(iController.GetNumberOfMetaDataEntries(num));
	return num;
	}


CMMFMetaDataEntry* CVideoPlayerUtility::CBody::MetaDataEntryL(TInt aMetaDataIndex) const
	{
	return iController.GetMetaDataEntryL(aMetaDataIndex);
	}

void CVideoPlayerUtility::CBody::SetPriorityL(TInt aPriority, TInt aPref)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetPriorityL++");
    DEBUG_PRINTF3("CVideoPlayerUtility::CBody::SetPriorityL - aPriority %d, aPref %d", aPriority, aPref);

    iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;

	TInt err = iController.SetPrioritySettings(iPrioritySettings);
	if ((err == KErrNone) || (err == KErrNotReady))
		{
		iFindAndOpenController->Configure(KUidMediaTypeVideo, iPrioritySettings, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
		}
	else
		{
		User::Leave(err);
		}
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetPriorityL--");
	}

void CVideoPlayerUtility::CBody::PriorityL(TInt & aPriority, TMdaPriorityPreference &aPref) const 
	{
	aPriority = iPrioritySettings.iPriority;
	aPref = TMdaPriorityPreference(iPrioritySettings.iPref);
	}


void CVideoPlayerUtility::CBody::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetPositionL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetPositionL - aPosition %ld", aPosition.Int64());
	User::LeaveIfError(iController.SetPosition(aPosition));
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetPositionL--");
	}


TTimeIntervalMicroSeconds CVideoPlayerUtility::CBody::DurationL() const
	{
	TTimeIntervalMicroSeconds duration;
	User::LeaveIfError(iController.GetDuration(duration));
	return duration;
	}

TTimeIntervalMicroSeconds CVideoPlayerUtility::CBody::PositionL() const
	{
	TTimeIntervalMicroSeconds position;
	User::LeaveIfError(iController.GetPosition(position));
	return position;
	}

TInt CVideoPlayerUtility::CBody::MaxVolume() const
	{
	TInt maxVol = 0;
	iAudioPlayDeviceCustomCommands.GetMaxVolume(maxVol);
	return maxVol;
	}

void CVideoPlayerUtility::CBody::GetFrameL(TDisplayMode aDisplayMode, TBool aUseIntentAPI, TIntent aIntent)
	{
	delete iFrameBitmap;
	iFrameBitmap = NULL;
	iFrameBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iFrameBitmap->Create(TSize(0,0),aDisplayMode));

	iFrameCallback->SetFrame(*iFrameBitmap);

	if (aUseIntentAPI)
		{
		iVideoDRMExtCustomCommands.GetFrame(*iFrameBitmap, aIntent, iFrameCallback->ActiveStatus());
		}
	else
		{
		iVideoPlayControllerCustomCommands.GetFrame(*iFrameBitmap, iFrameCallback->ActiveStatus());
		}
	}

MMMFDRMCustomCommand* CVideoPlayerUtility::CBody::GetDRMCustomCommand()
	{
	// XXX: check controller supports MMMFDRMCustomCommandImplementor
	TInt error = iDRMCustomCommands.EvaluateIntent(ContentAccess::EPeek);
	if (error==KErrNone)
		{
		return static_cast<MMMFDRMCustomCommand*>(&iDRMCustomCommands);
		}
	else
		{
		return NULL;
		}
	}
	
TInt CVideoPlayerUtility::CBody::VideoBitRateL() const
	{
	TInt bitRate;
	User::LeaveIfError(iVideoControllerCustomCommands.GetVideoBitRate(bitRate));
	return bitRate;
	}

TInt CVideoPlayerUtility::CBody::AudioBitRateL() const
	{
	TInt bitRate;
	User::LeaveIfError(iVideoControllerCustomCommands.GetAudioBitRate(bitRate));
	return bitRate;
	}

TBool CVideoPlayerUtility::CBody::AudioEnabledL() const
	{
	TBool enabled;
	User::LeaveIfError(iVideoPlayControllerCustomCommands.GetAudioEnabled(enabled));
	return enabled;
	}
	
	
void CVideoPlayerUtility::CBody::RefreshFrameL()
	{
	User::LeaveIfError(iVideoPlayControllerCustomCommands.RefreshFrame());
	}

TFourCC CVideoPlayerUtility::CBody::AudioTypeL() const
	{
	TFourCC codec;
	User::LeaveIfError(iVideoControllerCustomCommands.GetAudioCodec(codec));
	return codec;
	}


void CVideoPlayerUtility::CBody::HandleEvent(const TMMFEvent& aEvent)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::HandleEvent++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::HandleEvent - Event type 0x%X", aEvent.iEventType);

    if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		// if we haven't had a MfaocComplete() callback yet,
		// we need to delay the call back to the client
		// because the source/sink will not have been opened yet.
		iEventOpenReceived = ETrue;
		if (iOpenError == KErrNone)
			iOpenError = aEvent.iErrorCode;
		if (iCallbackOpenReceived)
			iObserver.MvpuoOpenComplete(aEvent.iErrorCode);
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
		{
		iObserver.MvpuoPrepareComplete(aEvent.iErrorCode);
		}
	else if (aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
		{
		iObserver.MvpuoPlayComplete(aEvent.iErrorCode);
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoLoadingStarted)
		{
		if (iVideoLoadingObserver)
			iVideoLoadingObserver->MvloLoadingStarted();
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoLoadingComplete)
		{
		if (iVideoLoadingObserver)
			iVideoLoadingObserver->MvloLoadingComplete();
		}
	else if(aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		if(iAudioResourceNotificationCallBack != NULL)
			{
			TBuf8<TMMFAudioConfig::KNotificationDataBufferSize> notificationData;
			if (KErrNone != iNotificationRegistrationCommands.GetResourceNotificationData(aEvent.iEventType, notificationData))
				{
				notificationData.SetLength(0);
				}
			iAudioResourceNotificationCallBack->MarncResourceAvailable(aEvent.iEventType, notificationData);	
			}
		}
#ifdef SYMBIAN_BUILD_GCE
	else if(aEvent.iEventType == KMMFEventCategoryVideoSurfaceCreated)
		{
		TInt error = SurfaceCreated();
			
		if (error != KErrNone)	
			{
			// Send error to the client
			TMMFEvent generalError(KMMFEventCategoryVideoPlayerGeneralError, error);
			iObserver.MvpuoEvent(generalError);	
			}
		}
	else if(aEvent.iEventType == KMMFEventCategoryVideoSurfaceParametersChanged)
		{
		TInt error = SurfaceParametersChanged();
				
		if (error != KErrNone)	
			{
			// Send error to the client
			TMMFEvent generalError(KMMFEventCategoryVideoPlayerGeneralError, error);
			iObserver.MvpuoEvent(generalError);	
			}
		}		
	else if(aEvent.iEventType == KMMFEventCategoryVideoRemoveSurface)
		{
		// Check surface is created
		TInt error = RemoveSurface(ETrue);
			
		if (error != KErrNone)
			{
			// Send error to the client
			TMMFEvent generalError(KMMFEventCategoryVideoPlayerGeneralError, error);
			iObserver.MvpuoEvent(generalError);	
			}
		}
#endif // SYMBIAN_BUILD_GCE	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	else if(aEvent.iEventType == KMMFEventCategoryVideoSubtitleCrpReady)
		{
		if (iSubtitleUtility)
			{
			// subtitle has not been disabled before event arrive
			RWindow* window = FindWindowWithWsHandle(iActiveDisplays, aEvent.iErrorCode);
			if (window)
				{
				// window has not been removed before event arrive
				iSubtitleUtility->HandleCrpReady(*window);
				}
			}
		}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	else 
		// Pass on all unrecognised events to the client
		iObserver.MvpuoEvent(aEvent);

    DEBUG_PRINTF("CVideoPlayerUtility::CBody::HandleEvent--");
	}

#ifdef SYMBIAN_BUILD_GCE
TInt CVideoPlayerUtility::CBody::SurfaceCreated()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SurfaceCreated++");

    TInt count = iActiveDisplays.Count();
	TBool replaceSurface = !(iSurfaceId.IsNull());
	TSurfaceId oldSurfaceId(iSurfaceId);
	
	// first remove surface if one already in use
	if(replaceSurface)
		{
		for (TInt i = 0; i < count; i++)
			{
			CMediaClientVideoDisplayBody* display = iActiveDisplays[i];
			display->RemoveSurface(EFalse);
			}
		iSurfaceId = TSurfaceId::CreateNullId();
		}
	
	TSurfaceId surfaceId;
	TRect cropRect;
	TVideoAspectRatio aspectRatio;
	
	TInt error = iVideoPlaySurfaceSupportCustomCommands.GetSurfaceParameters(surfaceId, 
										cropRect, 
										aspectRatio);
	if(error != KErrNone)
		{
		if(replaceSurface)
				{
				// ignore error
				iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved(oldSurfaceId);
				}
		return error;
		}
	
	// loop through all displays, if any fail continue and report error later
	TInt error2;
	for (TInt i = 0; i < count; ++i)
		{
		error2 = iActiveDisplays[i]->SurfaceCreated(surfaceId, cropRect, aspectRatio, iCropRegion);
		if(error2 != KErrNone)
			{
			error = error2;
			}
		}
	
	// now store surface details
	iSurfaceId = surfaceId;
	iSurfaceCropRect = cropRect;
	iAspectRatio = aspectRatio;
	
	// if surface already existed tell video adaptation it is no longer in use. Video adaptation
	// will remove the surface when it receives this call therefore the following code must be done at the
	// end of this function.
	if (replaceSurface)
        {
        error2 = iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved(oldSurfaceId);
        if (error2 != KErrNone)
            {
            error = error2;
            }
        }

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::SurfaceCreated--");
	return error;
	}

TInt CVideoPlayerUtility::CBody::SurfaceParametersChanged()
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SurfaceParametersChanged++");
	if (iSurfaceId.IsNull())
		{
		return KErrNotSupported;
		}
	
	TSurfaceId surfaceId;
	TRect cropRect;
	TVideoAspectRatio aspectRatio;
	
	TInt error = iVideoPlaySurfaceSupportCustomCommands.GetSurfaceParameters(surfaceId, 
										cropRect, 
										aspectRatio);
	if (error != KErrNone)
		{
		return error;
		}
	
	if (iSurfaceId != surfaceId)
		{
		return KErrInUse;
		}

	TInt count = iActiveDisplays.Count();
	TInt error2 = KErrNone;
	for (TInt i = 0; i < count; ++i)
		{
		error2 = iActiveDisplays[i]->SurfaceParametersChanged(surfaceId, cropRect, aspectRatio);
		
		// Save the error for the first failure only
		if ((error2 != KErrNone) && (error == KErrNone))
			{
			error = error2;
			}
		}
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SurfaceParametersChanged-- - Error %d", error);
    return error;
	}
		
TInt CVideoPlayerUtility::CBody::SetAllBackgroundSurfaces()
	{
	TInt count = iActiveDisplays.Count();
	TInt err = KErrNone;
	
	for (TInt i = 0; i < count; ++i)
		{
		CMediaClientVideoDisplayBody* display = iActiveDisplays[i];
		err = display->RedrawWindows(iCropRegion);
			
		if (err != KErrNone)
			{
			break;
			}
		}
	return err;
	}

TInt CVideoPlayerUtility::CBody::RemoveSurface(TBool aControllerEvent)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::RemoveSurface++");
	if (iSurfaceId.IsNull())
		{
		return KErrNotFound;
		}
	
	TInt count = iActiveDisplays.Count();
	for (TInt i = 0; i < count; i++)
		{
		iActiveDisplays[i]->RemoveSurface(aControllerEvent);
		}
	
	TInt error = iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved(iSurfaceId);

	iSurfaceId = TSurfaceId::CreateNullId();
	
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::RemoveSurface-- - Error %d", error);
	return error;
	}
	
#endif // SYMBIAN_BUILD_GCE

void CVideoPlayerUtility::CBody::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	if (iControllerOpen)
		{
		iVideoPlayControllerCustomCommands.DirectScreenAccessEvent(EAbortDSA);
		iDirectScreenAccessAbort = ETrue;
		}
	}

void CVideoPlayerUtility::CBody::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/) 
	{
	__ASSERT_ALWAYS(iDirectScreenAccess, User::Panic(_L("iDirectScreenAccess is NULL"), KErrArgument)); // should always be valid if we have a callback from it
	TRAPD( err, iDirectScreenAccess->StartL());

	if (iControllerOpen)
		{
		if (err == KErrNone)
			{
			iVideoPlayControllerCustomCommands.UpdateDisplayRegion(*iDirectScreenAccess->DrawingRegion());
			iVideoPlayControllerCustomCommands.DirectScreenAccessEvent(EResumeDSA);
			}
		}
	}

TInt CVideoPlayerUtility::CBody::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
	
TInt CVideoPlayerUtility::CBody::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
	
void CVideoPlayerUtility::CBody::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
void CVideoPlayerUtility::CBody::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}
	
void CVideoPlayerUtility::CBody::SetVideoFrameRateL(TReal32 aFramesPerSecond)
	{
	User::LeaveIfError(iVideoControllerCustomCommands.SetFrameRate(aFramesPerSecond));
	}

const CMMFControllerImplementationInformation& CVideoPlayerUtility::CBody::ControllerImplementationInformationL()
	{
	if (!iControllerImplementationInformation)
		{
		if (iControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iControllerUid);
		}
	return *iControllerImplementationInformation;
	}
	
void CVideoPlayerUtility::CBody::GetVideoLoadingProgressL(TInt& aPercentageProgress)
	{
	User::LeaveIfError(iVideoPlayControllerCustomCommands.GetLoadingProgress(aPercentageProgress));
	}

void CVideoPlayerUtility::CBody::StopDirectScreenAccessL()
	{
#ifdef SYMBIAN_BUILD_GCE
	if (iUsingVPU2)
		{
		User::Leave(KErrNotSupported);
		}

	if (!iGraphicsSurfaceSupported)
		{
		__ASSERT_ALWAYS(iDirectScreenAccess, User::Panic(_L("iDirectScreenAccess is NULL"), KErrArgument));
		iDirectScreenAccess->Cancel();
		AbortNow(RDirectScreenAccess::ETerminateCancel);
		}
#else
	__ASSERT_ALWAYS(iDirectScreenAccess, User::Panic(_L("iDirectScreenAccess is NULL"), KErrArgument));
	iDirectScreenAccess->Cancel();
	AbortNow(RDirectScreenAccess::ETerminateCancel);
#endif // SYMBIAN_BUILD_GCE
	}
	
void CVideoPlayerUtility::CBody::StartDirectScreenAccessL()
	{
#ifdef SYMBIAN_BUILD_GCE
	if (iUsingVPU2)
		{
		User::Leave(KErrNotSupported);
		}

	if (!iGraphicsSurfaceSupported)
		{
		// ETerminateCancel is a dummy value that is ignored by Restart()
		Restart(RDirectScreenAccess::ETerminateCancel);
		}
#else
	// ETerminateCancel is a dummy value that is ignored by Restart()
	Restart(RDirectScreenAccess::ETerminateCancel);
#endif  // SYMBIAN_BUILD_GCE
	}

//registers for notification when audio resource is available.
TInt CVideoPlayerUtility::CBody::RegisterAudioResourceNotification( MMMFAudioResourceNotificationCallback& aCallback, 
																	TUid aNotificationEventUid, 
																	const TDesC8& aNotificationRegistrationData)
	{
	//if not valid notification event id, return;
	if(aNotificationEventUid != KMMFEventCategoryAudioResourceAvailable)
		{
		return KErrNotSupported;
		}
		
	iAudioResourceNotificationCallBack = &aCallback;
	TInt err = iNotificationRegistrationCommands.RegisterAsClient(aNotificationEventUid, aNotificationRegistrationData);
	if(err == KErrNotReady)
		{
		//save the request - so that registration can be attempted on play
		iArnEventHolder = 	aNotificationEventUid;
		iNotificationDataHolder = aNotificationRegistrationData;
		return KErrNone;
		}
	iArnEventHolder = KNullUid;
	iNotificationDataHolder = KNullDesC8;
	return err;
	}

//Cancels the registration for audio resource notification.
TInt CVideoPlayerUtility::CBody::CancelRegisterAudioResourceNotification(TUid aNotificationEventId)
	{
	//if not valid notification event id, return;
	if(aNotificationEventId != KMMFEventCategoryAudioResourceAvailable)
		{
		return KErrNotSupported;
		}
	TInt err = iNotificationRegistrationCommands.CancelRegisterAsClient(aNotificationEventId);
	if(err == KErrNotReady)
		{
		if(iArnEventHolder == KNullUid)	
			{
			return KErrCancel;
			}
		//iArnEventHolder has data to re-attempt registration ;
		//but reattempt is not made(which is done in play);hence return KErrNone
		iArnEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		return KErrNone;
		}
	return err;
	}

//Waits for the client to resume the play even after the default timer expires.
TInt CVideoPlayerUtility::CBody::WillResumePlay()
	{
	return iNotificationRegistrationCommands.WillResumePlay();
	}

// This method will fail with KErrNotSupported when using CVideoPlayerUtility2, otherwise
// set the screen number.
TInt CVideoPlayerUtility::CBody::SetInitScreenNumber(TInt aScreenNumber)
	{
#ifdef SYMBIAN_BUILD_GCE
	// If the method is called from CVideoPlayerUtility2, return KErrNotSupport
	if (iUsingVPU2)
		{
		return KErrNotSupported;
		}
#endif
		
#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
	iScreenNumber = aScreenNumber;
	return KErrNone;
#else
	aScreenNumber = aScreenNumber;//added to remove the warning
	return KErrNotSupported;
#endif
	}

#ifdef SYMBIAN_BUILD_GCE
//-------------------------------------------------------------------------------

void CVideoPlayerUtility::CBody::AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, 
							RWindow& aWindow, const TRect& aVideoExtent, 
							const TRect& aWindowClipRect)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::AddDisplayWindowL++");
    DEBUG_PRINTF4("CVideoPlayerUtility::CBody::AddDisplayWindowL - aScreenDevice num %d, width %d, height %d", aScreenDevice.GetScreenNumber(), aScreenDevice.SizeInPixels().iWidth, aScreenDevice.SizeInPixels().iHeight);
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::AddDisplayWindowL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::AddDisplayWindowL - aWindow abs pos %d,%d - width %d, height %d", aWindow.AbsPosition().iX, aWindow.AbsPosition().iY, aWindow.Size().iWidth, aWindow.Size().iHeight);
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::AddDisplayWindowL - aVideoExtent %d,%d - %d,%d", aVideoExtent.iTl.iX, aVideoExtent.iTl.iY, aVideoExtent.iBr.iX, aVideoExtent.iBr.iY);
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::AddDisplayWindowL - aWindowClipRect %d,%d - %d,%d", aWindowClipRect.iTl.iX, aWindowClipRect.iTl.iY, aWindowClipRect.iBr.iX, aWindowClipRect.iBr.iY);

    // set window and get display ID for the window
	TRect windowRect = TRect(aWindow.Size()); 
	
	// Check the rectangle is contained completely within the window
	if (!RectContains(windowRect, aWindowClipRect))
		{
		// NOTE: TRect::Contains() is not used for comparison here as point located on the right hand 
		//       side or bottom is considered to be outside the rectangle, which is not the desirable 
		//       behaviour in this case
		User::Leave(KErrArgument);
		}
		
	DoAddDisplayWindowL(aWs, aScreenDevice.GetScreenNumber(), aWindow, aWindowClipRect, aVideoExtent, &aWindow);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::AddDisplayWindowL--");
	}
	
void CVideoPlayerUtility::CBody::AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::AddDisplayWindowL++");
    DEBUG_PRINTF4("CVideoPlayerUtility::CBody::AddDisplayWindowL - aScreenDevice num %d, width %d, height %d", aScreenDevice.GetScreenNumber(), aScreenDevice.SizeInPixels().iWidth, aScreenDevice.SizeInPixels().iHeight);
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::AddDisplayWindowL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::AddDisplayWindowL - aWindow abs pos %d,%d - width %d, height %d", aWindow.AbsPosition().iX, aWindow.AbsPosition().iY, aWindow.Size().iWidth, aWindow.Size().iHeight);

    DoAddDisplayWindowL(aWs, aScreenDevice.GetScreenNumber(), aWindow, TRect(aWindow.Size()), TRect(aWindow.Size()), &aWindow);

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::AddDisplayWindowL--");
	}

void CVideoPlayerUtility::CBody::DoAddDisplayWindowL(RWsSession& aWs, TInt aDisplayId, RWindowBase& aWindow,
								const TRect& aClipRect, const TRect& aVideoExtent, RWindow* aWindow2)
	{
	iWs = &aWs;

	// check opening the source is complete and the client has been recieved an MvpuoOpenComplete() callback
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}

	// make sure window isn't already added on another display
	TRAPD(err, CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow));
	if (err != KErrNotFound)
		{
		// If err is something else but KErrNone leave
		User::LeaveIfError(err);
		// Window is already in use
		User::Leave(KErrInUse);
		}

	TInt pos = iActiveDisplays.FindInOrder(aDisplayId, CMediaClientVideoDisplayBody::CompareByDisplay);
	CMediaClientVideoDisplayBody* display = NULL;

	if (pos == KErrNotFound)
		{
		if(iSurfaceId.IsNull())
			{
			display = CMediaClientVideoDisplayBody::NewL(aDisplayId, ETrue);
			}
		else
			{
			display = CMediaClientVideoDisplayBody::NewL(aDisplayId, iSurfaceId, iSurfaceCropRect, iAspectRatio, ETrue);
			}
		CleanupStack::PushL(display);
		iActiveDisplays.InsertInOrderL(display, CMediaClientVideoDisplayBody::Compare);
		CleanupStack::Pop(display);
		}
	else 
		{
		User::LeaveIfError(pos);
		display = iActiveDisplays[pos];
		}

	display->AddDisplayWindowL(&aWindow, aClipRect, iCropRegion, aVideoExtent, iGlobalScaleWidth, iGlobalScaleHeight,
								iGlobalRotation, iGlobalAutoScaleType, iGlobalHorizPos, iGlobalVertPos, aWindow2);

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	if (iSubtitleUtility)
		{
		// subtitles were enabled already, so enable subtitles on this window
		TMMFSubtitleWindowConfig config;
		config.iWindowId = aWindow.WsHandle();
		config.iWindowClipRect = aClipRect;
		config.iDisplayMode = aWindow.DisplayMode();
		config.iRotation = iGlobalRotation;
		iSubtitleUtility->AddSubtitleConfig(config); // ignore error from add subtitle config because the window can still display video properly
		}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	}

void CVideoPlayerUtility::CBody::DoUpdateDisplayWindowL(RWsSession& aWs, RWindowBase& aWindow,
                                const TRect& aClipRect, const TRect& aVideoExtent, RWindow* aWindow2)
    {
    iWs = &aWs;

    // check opening the source is complete and the client has been recieved an MvpuoOpenComplete() callback
    if (!iControllerOpen)
        {
        User::Leave(KErrNotReady);
        }

    CMediaClientVideoDisplayBody* display = NULL;
    TRAPD(err, display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow));
    User::LeaveIfError(err);

    display->UpdateDisplayWindowL(&aWindow, aClipRect, iCropRegion, aVideoExtent, iGlobalScaleWidth, iGlobalScaleHeight,
                                iGlobalRotation, iGlobalAutoScaleType, iGlobalHorizPos, iGlobalVertPos, aWindow2);

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    if (iSubtitleUtility)
        {
        // subtitles were enabled already, so update subtitles on this window
        TMMFSubtitleWindowConfig config;
        config.iWindowId = aWindow.WsHandle();
        config.iWindowClipRect = aClipRect;
        config.iDisplayMode = aWindow.DisplayMode();
        config.iRotation = iGlobalRotation;
        iSubtitleUtility->UpdateSubtitleConfig(config); // ignore error from add subtitle config because the window can still display video properly
        }
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    }

void CVideoPlayerUtility::CBody::RemoveDisplayWindow(RWindowBase& aWindow)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::RemoveDisplayWindow++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::RemoveDisplayWindow - aWindow WsHandle 0x%X", aWindow.WsHandle());

    CMediaClientVideoDisplayBody* display = NULL;
	TRAPD(err, display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow));
	
	if (err == KErrNone)
		{		
		display->RemoveDisplayWindow(aWindow);
	
		if (!display->IsUsed())

			{
			TInt pos = iActiveDisplays.Find(display);
			iActiveDisplays.Remove(pos);
			delete display;
			}

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
		if (iSubtitleUtility)
			{
			// subtitles are enabled, so remove window config, ignore error
			iSubtitleUtility->RemoveSubtitleConfig(aWindow.WsHandle());
			}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
		}	
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::RemoveDisplayWindow--");
	}

void CVideoPlayerUtility::CBody::AddDisplayL(TInt aDisplay, MMMFSurfaceEventHandler& aEventHandler)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::AddDisplayL");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::AddDisplayL - aDisplay %d", aDisplay);
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}
	
	TInt err = iActiveDisplays.FindInOrder(aDisplay, CMediaClientVideoDisplayBody::CompareByDisplay);
	
	if (err == KErrNotFound)
		{
		CMediaClientVideoDisplayBody* display;
		if(iSurfaceId.IsNull())
			{
			display = CMediaClientVideoDisplayBody::NewL(aDisplay, ETrue);
			}
		else
			{
			display = CMediaClientVideoDisplayBody::NewL(aDisplay, iSurfaceId, iSurfaceCropRect, iAspectRatio, ETrue);
			}
		CleanupStack::PushL(display);
		iActiveDisplays.InsertInOrderL(display, CMediaClientVideoDisplayBody::Compare);
		CleanupStack::Pop(display);
		display->AddDisplayL(aEventHandler);
		}
	else
		{
		User::LeaveIfError(err);
		iActiveDisplays[err]->AddDisplayL(aEventHandler);
		}
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::AddDisplayL--");
	}

void CVideoPlayerUtility::CBody::RemoveDisplay(TInt aDisplay)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::RemoveDisplay++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::RemoveDisplay - aDisplay %d", aDisplay);
	TInt pos = iActiveDisplays.FindInOrder(aDisplay, CMediaClientVideoDisplayBody::CompareByDisplay);
	
	if (pos >= KErrNone)
		{
		CMediaClientVideoDisplayBody* disp = iActiveDisplays[pos];
		disp->RemoveDisplay();
		
		if (!disp->HasWindows())
			{
			iActiveDisplays.Remove(pos);
			delete disp;
			}
		}
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::RemoveDisplay--");
	}

void CVideoPlayerUtility::CBody::ConvertFromRelativeToDisplayToRelativeToWindow(
            const RWindowBase& aWindow,
            const TRect& aFromWindowRect,
            const TRect& aFromClipRect,
            TRect& aToWindowRect,
            TRect& aToClipRect)
    {
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::ConvertFromRelativeToDisplayToRelativeToWindow++");
    TPoint windowOrigin = aWindow.AbsPosition();
    
    // window rect
    aToWindowRect.iTl.iX = aFromWindowRect.iTl.iX - windowOrigin.iX;
    aToWindowRect.iTl.iY = aFromWindowRect.iTl.iY - windowOrigin.iY;
    aToWindowRect.iBr.iX = aFromWindowRect.iBr.iX - windowOrigin.iX;
    aToWindowRect.iBr.iY = aFromWindowRect.iBr.iY - windowOrigin.iY;
    
    // clip rect
    aToClipRect.iTl.iX = aFromClipRect.iTl.iX - windowOrigin.iX;
    aToClipRect.iTl.iY = aFromClipRect.iTl.iY - windowOrigin.iY;
    aToClipRect.iBr.iX = aFromClipRect.iBr.iX - windowOrigin.iX;
    aToClipRect.iBr.iY = aFromClipRect.iBr.iY - windowOrigin.iY;

    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::ConvertFromRelativeToDisplayToRelativeToWindow - New Window rect %d,%d - %d,%d", aToWindowRect.iTl.iX, aToWindowRect.iTl.iY, aToWindowRect.iBr.iX, aToWindowRect.iBr.iY);
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::ConvertFromRelativeToDisplayToRelativeToWindow - New Clip rect %d,%d - %d,%d", aToClipRect.iTl.iX, aToClipRect.iTl.iY, aToClipRect.iBr.iX, aToClipRect.iBr.iY);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::ConvertFromRelativeToDisplayToRelativeToWindow--");
    }

void CVideoPlayerUtility::CBody::SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetVideoExtentL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetVideoExtentL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::SetVideoExtentL - aVideoExtent %d,%d - %d,%d", aVideoExtent.iTl.iX, aVideoExtent.iTl.iY, aVideoExtent.iBr.iX, aVideoExtent.iBr.iY);

    // check opening the source is complete and the client has been recieved an MvpuoOpenComplete() callback
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}
		
	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
	display->SetVideoExtentL(aWindow, aVideoExtent, iCropRegion);

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetVideoExtentL--");
	}

void CVideoPlayerUtility::CBody::SetWindowClipRectL(const RWindowBase& aWindow, const TRect& aWindowClipRect)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetWindowClipRectL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetWindowClipRectL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CVideoPlayerUtility::CBody::SetWindowClipRectL - aWindowClipRect %d,%d - %d,%d", aWindowClipRect.iTl.iX, aWindowClipRect.iTl.iY, aWindowClipRect.iBr.iX, aWindowClipRect.iBr.iY);

    // check opening the source is complete and the client has been recieved an MvpuoOpenComplete() callback
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}

	TRect winRect(aWindow.Size());

	if (!RectContains(winRect, aWindowClipRect))
		{
		User::Leave(KErrArgument);
		}
		
	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	RArray<CMediaClientVideoDisplayBody::TWindowData>& windows = display->Windows();
	TInt pos = windows.Find(aWindow.WsHandle(), CMediaClientVideoDisplayBody::TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);
	
	CMediaClientVideoDisplayBody::TWindowData& currentWin = windows[pos];
	if (currentWin.iClipRect != aWindowClipRect)
		{
		// update config only if setting has actually changed
		display->SetWindowClipRectL(aWindow, aWindowClipRect, iCropRegion);

		if (iSubtitleUtility)
			{
			TMMFSubtitleWindowConfig config;
			GetSubtitleConfigFromWindowData(currentWin, config);

			iSubtitleUtility->UpdateSubtitleConfig(config);
			}
		}
#else
	display->SetWindowClipRectL(aWindow, aWindowClipRect, iCropRegion);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetWindowClipRectL--");
	}

// Check if the controller supports the graphics surface. Has to be called after resource opened.
TInt CVideoPlayerUtility::CBody::CheckSurfaceSupported()
	{
	TInt err = iFindAndOpenController->SurfaceSupported();
		
	if (err == KErrNone)
		{
		iGraphicsSurfaceSupported = ETrue;
		}
	else 
		{
		iGraphicsSurfaceSupported = EFalse;
		}
		
	return err;
	}

TBool CVideoPlayerUtility::CBody::RectContains(const TRect& aLeft, const TRect& aRight)
	{
	return !(aLeft.iTl.iX > aRight.iTl.iX || 
			aLeft.iTl.iY > aRight.iTl.iY ||
	    	aLeft.iBr.iX < aRight.iBr.iX || 
	    	aLeft.iBr.iY < aRight.iBr.iY);
	}
	
	
void CVideoPlayerUtility::CBody::SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType)
	{
	SetAutoScaleL(aWindow, aScaleType, EHorizontalAlignCenter, EVerticalAlignCenter);
	}
	
void CVideoPlayerUtility::CBody::SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetAutoScaleL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetAutoScaleL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF4("CVideoPlayerUtility::CBody::SetAutoScaleL - aScaleType %d, aHorizPos %d, aVertPos %d", aScaleType, aHorizPos, aVertPos);

    if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}
		
	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
	display->SetAutoScaleL(aWindow, aScaleType, aHorizPos, aVertPos, iCropRegion);

	DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetAutoScaleL--");
	}

void CVideoPlayerUtility::CBody::SetRotationL(const RWindowBase& aWindow, TVideoRotation aRotation)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetRotationL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetRotationL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetRotationL - aRotation %d", aRotation);

    if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}

	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	RArray<CMediaClientVideoDisplayBody::TWindowData>& windows = display->Windows();
	TInt pos = windows.Find(aWindow.WsHandle(), CMediaClientVideoDisplayBody::TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);
	
	CMediaClientVideoDisplayBody::TWindowData& currentWin = windows[pos];
	if (currentWin.iRotation != aRotation)
		{
		display->SetRotationL(aWindow, aRotation, iCropRegion);

		if (iSubtitleUtility)
			{
			TMMFSubtitleWindowConfig config;
			GetSubtitleConfigFromWindowData(currentWin, config);

			iSubtitleUtility->UpdateSubtitleConfig(config);
			}
		}
#else
	display->SetRotationL(aWindow, aRotation, iCropRegion);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetRotationL--");
	}
	
TVideoRotation CVideoPlayerUtility::CBody::RotationL(const RWindowBase& aWindow) const
	{
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		}
	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
	return display->RotationL(aWindow);
	}
	
void CVideoPlayerUtility::CBody::SetScaleFactorL(const RWindowBase& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetScaleFactorL++");
    DEBUG_PRINTF2("CVideoPlayerUtility::CBody::SetScaleFactorL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF3("CVideoPlayerUtility::CBody::SetScaleFactorL - aWidthPercentage %f, aHeightPercentage %f", aWidthPercentage, aHeightPercentage);
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		
		}
	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
	display->SetScaleFactorL(aWindow, aWidthPercentage, aHeightPercentage, iCropRegion);
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetScaleFactorL--");
	}
	
void CVideoPlayerUtility::CBody::GetScaleFactorL(const RWindowBase& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage) const
	{
	if (!iControllerOpen)
		{
		User::Leave(KErrNotReady);
		
		}
	CMediaClientVideoDisplayBody* display = CMediaClientVideoDisplayBody::FindDisplayWithWindowL(iActiveDisplays, aWindow);
	display->GetScaleFactorL(aWindow, aWidthPercentage, aHeightPercentage);
	}

void CVideoPlayerUtility::CBody::SetExternalDisplaySwitchingL(TInt aDisplay, TBool aControl)
    {
    TInt pos = iActiveDisplays.FindInOrder(aDisplay, CMediaClientVideoDisplayBody::CompareByDisplay);
    
    User::LeaveIfError(pos);
    
    iActiveDisplays[pos]->SetExternalDisplaySwitchingL(aControl);
    }

#endif // SYMBIAN_BUILD_GCE

void CVideoPlayerUtility::CBody::SetPlayVelocityL(TInt aVelocity)
	{
	// Leave if Open is not yet called on controller.
	if(!iEventOpenReceived)
		{
		User::Leave(KErrNotReady);
		}
	
	User::LeaveIfError(iVideoPlayControllerExtCustomCommands.SetPlayVelocity(aVelocity));	
	}

const TInt KDefaultPlayVelocity = 100;

TInt CVideoPlayerUtility::CBody::PlayVelocityL() const
	{
	TInt velocity;
	TInt error;

	// Leave if Open is not yet called.
	if( !iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}
	
	error = iVideoPlayControllerExtCustomCommands.PlayVelocity(velocity);
	/* if customcommand is not implemented by controller pluggin return default value(100) */
	if (KErrNotSupported == error)
		{
		velocity = KDefaultPlayVelocity;
		}
	else
		{
		User::LeaveIfError(	error );	
		}
	return velocity;
	}
void CVideoPlayerUtility::CBody::StepFrameL(TInt aStep)
	{
	//Leave if not in paused state. This functionality is supported only in paused state.
	if( (!iEventOpenReceived ) || ( EPaused != iState ) )
		{
		User::Leave(KErrNotReady);
		}

	User::LeaveIfError(iVideoPlayControllerExtCustomCommands.StepFrame(aStep));
	}
void CVideoPlayerUtility::CBody::GetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCapabilities) const
	{
	TInt error;

	// Leave if Open is not yet called on controller.
	if( !iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}
	
	error = iVideoPlayControllerExtCustomCommands.GetPlayRateCapabilities(aCapabilities);
	/* if customcommand is not implemented by controller pluggin return not supported */
	if (KErrNotSupported == error)
		{
		aCapabilities.iPlayBackward = EFalse;
		aCapabilities.iPlayForward = EFalse;
		aCapabilities.iStepBackward = EFalse;
		aCapabilities.iStepForward = EFalse;
		}
	else
		{
		User::LeaveIfError(error);
		}	
	}
void CVideoPlayerUtility::CBody::SetVideoEnabledL(TBool aVideoEnabled)
	{
	// Leave if Open is not yet called or not in stopped state.
	if( (!iEventOpenReceived ) || ( EStopped != iState ) )
		{
		User::Leave(KErrNotReady);
		}
	
	User::LeaveIfError(iVideoPlayControllerExtCustomCommands.SetVideoEnabled(aVideoEnabled));
	}
TBool CVideoPlayerUtility::CBody::VideoEnabledL() const
	{
	TBool videoEnabled;
	TInt error;

	// Leave if Open is not yet called.
	if( !iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}
	
	error = iVideoPlayControllerExtCustomCommands.VideoEnabled(videoEnabled);
	/* if customcommand is not implemented by controller pluggin return default value ETrue */
	if (KErrNotSupported == error)
		{
		videoEnabled = ETrue;
		}
	else
		{
		User::LeaveIfError(error);
		}	
	return videoEnabled;
	}
void CVideoPlayerUtility::CBody::SetAudioEnabledL(TBool aAudioEnabled)
	{
	// Leave if Open is not yet called or not in stopped state.
	if( (!iEventOpenReceived ) || ( EStopped != iState ) )
		{
		User::Leave(KErrNotReady);
		}
	
	User::LeaveIfError(iVideoPlayControllerExtCustomCommands.SetAudioEnabled(aAudioEnabled));
	}
void CVideoPlayerUtility::CBody::SetAutoScaleL(TAutoScaleType aScaleType)
	{
	SetAutoScaleL(aScaleType, EHorizontalAlignCenter, EVerticalAlignCenter);
	}
void CVideoPlayerUtility::CBody::SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetAutoScaleL++");
    DEBUG_PRINTF4("CVideoPlayerUtility::CBody::SetAutoScaleL - aScaleType %d, aHorizPos %d, aVertPos %d", aScaleType, aHorizPos, aVertPos);

    // Leave if Open is not yet called.
	if(!iEventOpenReceived )
		{
		User::Leave(KErrNotReady);
		}	

#ifdef SYMBIAN_BUILD_GCE
	if (!iGraphicsSurfaceSupported)
		{
		User::LeaveIfError(iVideoPlayControllerExtCustomCommands.SetAutoScale(aScaleType, aHorizPos, aVertPos));
		}
	else
		{
		// Auto scale setting is not sent to controller when graphics surfaces are used.
		// If the surface has been created, perform auto scale with the help of graphics surfaces;
		// otherwise, just store the auto scale info.
		iGlobalAutoScaleType = aScaleType;
		iGlobalHorizPos = aHorizPos;
		iGlobalVertPos = aVertPos;
		
		TInt count = iActiveDisplays.Count();
		
		for (TInt i = 0; i < count; ++i)
			{
			iActiveDisplays[i]->SetAutoScaleL(aScaleType, aHorizPos, aVertPos, iCropRegion);
			}
		}
#else
	User::LeaveIfError(iVideoPlayControllerExtCustomCommands.SetAutoScale(aScaleType, aHorizPos, aVertPos));
#endif // SYMBIAN_BUILD_GCE
    DEBUG_PRINTF("CVideoPlayerUtility::CBody::SetAutoScaleL--");
	}

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
TBool CVideoPlayerUtility::CBody::SubtitlesAvailable()
	{
	if (!iControllerOpen)
		{
		return EFalse;
		}

	return CMMFSubtitleUtility::SubtitlesAvailable(iController);
	}

void CVideoPlayerUtility::CBody::EnableSubtitlesL()
	{
	// Check if video file and controller is opened
	__ASSERT_ALWAYS(iControllerOpen, VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilityFileNotOpened, KErrNotReady));
	// Check if subtitles are already enabled
	__ASSERT_ALWAYS(!iSubtitleUtility, User::Leave(KErrInUse));
	// Check if display window is added
	TBool windowsAdded = EFalse;
	for (TInt i = iActiveDisplays.Count()-1; (i >= 0 && windowsAdded == EFalse); --i)
		{
		CMediaClientVideoDisplayBody* display = iActiveDisplays[i];
		windowsAdded = display->HasWindows();
		}
	__ASSERT_ALWAYS(windowsAdded, VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilityNoWindowAdded, KErrNotReady));


	iSubtitleUtility = CMMFSubtitleUtility::NewL(iController, *iWs);
	TInt err = iSubtitleUtility->EnableSubtitles();
	if (KErrNone != err)
		{
		delete iSubtitleUtility;
		iSubtitleUtility = NULL;
		if (KErrNotSupported == err || KErrNotFound == err)
			{
			// controller does not support subtitles or subtitle source not found,
			// panic client because they should have called SubtitlesAvailable first
			VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilitySubtitleNotSupported, err);
			}
		
		// leave in release mode or any other error
		User::Leave(err);
		}

	TBool subtitleEnabled = EFalse;
	TInt count = iActiveDisplays.Count();
	
	// add the windows in the orders that they were added
	for (TInt i = 0; i < count; ++i)
		{
		CMediaClientVideoDisplayBody* display = iActiveDisplays[i];
		err = EnableSubtitles(*display);
		
		if (KErrNone == err)
			{
			subtitleEnabled = ETrue;
			}
		}

	// no subtitle window was enabled successfully, enable subtitle failed
	if (!subtitleEnabled)
		{
		DisableSubtitles();	// disable subtitle on controller side
		User::Leave(err); // Leave with error returned by last EnableSubtitles call
		}
	}

void CVideoPlayerUtility::CBody::DisableSubtitles()
	{
	if (iSubtitleUtility)
		{
		iSubtitleUtility->DisableSubtitles();
		
		delete iSubtitleUtility;
		iSubtitleUtility = NULL;
		}
	}

TArray<TLanguage> CVideoPlayerUtility::CBody::SupportedSubtitleLanguagesL()
	{
	// Check if subtitles have been enabled
	__ASSERT_ALWAYS(iSubtitleUtility, VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilitySubtitleNotEnabled, KErrNotReady));

	return iSubtitleUtility->SupportedSubtitleLanguagesL();
	}

TLanguage CVideoPlayerUtility::CBody::SubtitleLanguageL()
	{
	// Check if subtitles have been enabled
	__ASSERT_ALWAYS(iSubtitleUtility, VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilitySubtitleNotEnabled, KErrNotReady));

	return iSubtitleUtility->SubtitleLanguage();
	}

void CVideoPlayerUtility::CBody::SetSubtitleLanguageL(TLanguage aLanguage)
	{
	// Check if subtitles have been enabled
	__ASSERT_ALWAYS(iSubtitleUtility, VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilitySubtitleNotEnabled, KErrNotReady));

	TInt err = iSubtitleUtility->SetSubtitleLanguage(aLanguage);
	if (KErrNotSupported == err)
		{
		// panic on debug because client should call SupportedSubtitleLanguagesL() first
		VPUDebugPanicOrLeaveL(EMMFVideoPlayerUtilitySubtitleLanguageNotSupported, KErrNotSupported);
		}
	User::LeaveIfError(err);
	}

void CVideoPlayerUtility::CBody::RedrawSubtitle(RWindow& aWindow, const TRect &aRect)
	{
	if (iSubtitleUtility)
		{
		// subtitle is enabled
		RWindow* window = FindWindowWithWsHandle(iActiveDisplays, aWindow.WsHandle());
		if (window) // check that window was added for video playback
			{
			iSubtitleUtility->RedrawSubtitle(aWindow, aRect);
			}
		}
	// else ignore the redraw request
	}

RWindow* CVideoPlayerUtility::CBody::FindWindowWithWsHandle(const RPointerArray<CMediaClientVideoDisplayBody>& aDisplays, TInt aWsHandle)
	{
	TInt count = aDisplays.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		CMediaClientVideoDisplayBody* display = aDisplays[i];
		RArray<CMediaClientVideoDisplayBody::TWindowData>& windows = display->Windows();
		TInt pos = windows.Find(aWsHandle, CMediaClientVideoDisplayBody::TWindowData::CompareByWsHandle);
		if (pos >= 0)
			{
			return windows[pos].iWindow2;
			}
		}
		
	return NULL;
	}

void CVideoPlayerUtility::CBody::GetSubtitleConfigFromWindowData(CMediaClientVideoDisplayBody::TWindowData& aWindowData, TMMFSubtitleWindowConfig& aConfig)
	{
	aConfig.iWindowId = aWindowData.iWindow->WsHandle();
	aConfig.iWindowClipRect = aWindowData.iClipRect;
	aConfig.iDisplayMode = aWindowData.iWindow->DisplayMode();
	aConfig.iRotation = aWindowData.iRotation;
	
	}
TInt CVideoPlayerUtility::CBody::EnableSubtitles(CMediaClientVideoDisplayBody& aDisplay)
	{
	TBool windowAdded = EFalse;
	RArray<CMediaClientVideoDisplayBody::TWindowData> windows = aDisplay.Windows();
	
 	TInt count = windows.Count();
	TInt err = KErrNone;	
	// add the windows in the orders that they are added
	for (TInt i = 0; i < count; ++i)
		{
		err = AddSubtitleConfig(windows[i]);
		if (KErrNone == err)
			{
			windowAdded = ETrue;
			}
		}

	// Return the error code returned by the last AddSubtitleConfig call.
	if (!windowAdded)
		{
		return err;
		}
	
	return KErrNone;
	}

TInt CVideoPlayerUtility::CBody::AddSubtitleConfig(CMediaClientVideoDisplayBody::TWindowData& aWindowData)
	{
	ASSERT(iSubtitleUtility);
	TMMFSubtitleWindowConfig config;
	GetSubtitleConfigFromWindowData(aWindowData, config);
	
	return iSubtitleUtility->AddSubtitleConfig(config);
	}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

