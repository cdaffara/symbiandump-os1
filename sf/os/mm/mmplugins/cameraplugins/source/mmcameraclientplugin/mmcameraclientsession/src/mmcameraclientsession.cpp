// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <f32file.h>

#include "mmcameraclientsession.h"
#include "mmdirectviewfinder.h"

#include <graphics/surfaceconfiguration.h>
#include "w32std.h"

/**
 * First-phase constructor
 */
CMMCameraClientSession::CMMCameraClientSession():iCameraObserver2(NULL),
									 iReserved(EFalse),
									 iPoweredUp(EFalse),
									 iPrepareCompleted(EFalse),
									 iCollaborativeClient(EFalse)
	{
	}

/**
 * NewL() factory function
 */
CMMCameraClientSession* CMMCameraClientSession::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CMMCameraClientSession;
	}

/**
 * Destructor
 */
CMMCameraClientSession::~CMMCameraClientSession()
	{
	delete iCameraPowerHandler;
	delete iCameraAccessHandler;

	iHashMap.Close();
	iCameraSession.Close();
	}


//
// from CCameraPlugin
//

/**
 * Derived from CCameraPlugin. Second Phase contruction
 *
 * @see CCameraPlugin
 */
void CMMCameraClientSession::Construct2L(MCameraObserver& /*aObserver*/, TInt /*aCameraIndex*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Derived from CCameraPlugin. Second Phase contruction for collaborative clients
 *
 * @see CCameraPlugin
 */
void CMMCameraClientSession::Construct2DupL(MCameraObserver& /*aObserver*/, TInt /*aCameraHandle*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Derived from CCameraPlugin. Second Phase contruction
 *
 * @see CCameraPlugin
 */
void CMMCameraClientSession::Construct2L(MCameraObserver2& aObserver, TInt aCameraIndex, TInt aPriority)
	{
	if (aCameraIndex > CamerasAvailable())
		{
		User::Leave(KErrNotSupported);
		}

	iCameraIndex = aCameraIndex;
	iPriority = aPriority;
	iCameraObserver2 = &aObserver;

	//initialize server
	//create session
	User::LeaveIfError(iCameraSession.Connect());

	//open camera
	TOpenCamera openCam;

	openCam.iCameraIndex = iCameraIndex;
	openCam.iPriority = iPriority;
	openCam.iHandle = KECamHandleNotKnown;
	openCam.iMMCapability = ETrue;  //could be changed.
	openCam.iCollaborativeClient = EFalse;

	TOpenCameraPckg openCamBuf(openCam);

	User::LeaveIfError(iCameraSession.SendMessage(ECamOpenCamera, openCamBuf));
					// since this call is synchronous, providing reference to local
					//  varialbe is not a problem.
												   
	iCameraAccessHandler = CMMCameraAccessHandler::NewL(this, CActive::EPriorityStandard);
	iCameraPowerHandler = CMMCameraPowerHandler::NewL(this, CActive::EPriorityStandard);
	}

/**
 * Derived from CCameraPlugin. Second Phase contruction for collaborative clients
 *
 * @see CCameraPlugin
 */
void CMMCameraClientSession::Construct2DupL(MCameraObserver2& /*aObserver*/, TInt /*aCameraHandle*/)
	{
	User::Leave(KErrNotSupported);	
	}


//
// from CCamera itself
//

/**
 * Retrieves information about the currectly reserved camera device
 *
 * @see CCamera::CameraInfo()
 */
void CMMCameraClientSession::CameraInfo(TCameraInfo& /*aInfo*/) const
	{ 
	return;
	}

/**
 * Reserves the camera
 *
 * @see CCamera::Reserve()
 */
void CMMCameraClientSession::Reserve()
	{
	iCameraAccessHandler->Reserve();
	}

/**
 * Relinquishes control of the camera device
 *
 * @see CCamera::Release()
 */
void CMMCameraClientSession::Release()
	{
	iCameraSession.SendMessage(ECamCameraAccessControl, ECameraRelease);
	iPoweredUp = EFalse;
	}

/**
 * Powers on the camera device
 *
 * @see CCamera::PowerOn()
 */
void CMMCameraClientSession::PowerOn()
	{
	iCameraPowerHandler->PowerOn();
	}

/**
 * Powers down the camera device
 *
 * @see CCamera::PowerOff()
 */
void CMMCameraClientSession::PowerOff()
	{	
	iCameraSession.SendMessage(ECamPowerCamera, ECameraPowerOff);
	iPoweredUp = EFalse;
	}

/**
 * Retrieves the handle of the currently reserved camera
 *
 * @see CCamera::Handle()
 */
TInt CMMCameraClientSession::Handle()
	{
	TCameraHandle handle;
	TCameraHandlePckg pckg(handle);

	iCameraSession.SendRxMessage(ECamCameraHandle, pckg);

	handle = pckg();
	return handle.iHandle;
	}

/**
 * Sets the optical zoom level
 *
 * @see CCamera::SetZoomFactorL()
 */
void CMMCameraClientSession::SetZoomFactorL(TInt aZoomFactor)
	{
	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EZoomFactor, aZoomFactor));
	}

/**
 *  Retrieves current optical zoom level
 *
 * @see CCamera::ZoomFactor()
 */
TInt CMMCameraClientSession::ZoomFactor() const
	{
	TCameraZoom zoomFactor;
	TCameraZoomPckg pckg(zoomFactor);
	iCameraSession.SendRxMessage(ECamGetParameter, EZoomFactor, pckg);
	zoomFactor = pckg();
	return zoomFactor.iZoom;
	}

/**
 * Sets the digital zoom level
 *
 * @see CCamera::SetDigitalZoomFactorL()
 */
void CMMCameraClientSession::SetDigitalZoomFactorL(TInt aDigitalZoomFactor)
	{
	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EDigitalZoomFactor, aDigitalZoomFactor));
	}

/**
 * Retrieves the current digital zoom level
 *
 * @see CCamera::DigitalZoomFactor()
 */
TInt CMMCameraClientSession::DigitalZoomFactor() const
	{
	TCameraDigitalZoom digitalZoomFactor;
	TCameraDigitalZoomPckg pckg(digitalZoomFactor);
	iCameraSession.SendRxMessage(ECamGetParameter, EDigitalZoomFactor, pckg);
	digitalZoomFactor = pckg();
	return digitalZoomFactor.iDigitalZoom;
	}

/**
 * Sets the contrast level
 *
 * @see CCamera::SetContrastL()
 */
void CMMCameraClientSession::SetContrastL(TInt aContrast)
	{
	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EContrast, aContrast));
	}

/**
 * Retrieves the current contrast level
 *
 * @see CCamera::Contrast()
 */
TInt CMMCameraClientSession::Contrast() const
	{
	TCameraContrast contrast;
	TCameraContrastPckg pckg(contrast);
	iCameraSession.SendRxMessage(ECamGetParameter, EContrast, pckg);
	contrast = pckg();
	return contrast.iContrast;
	}

/**
 * Sets the brightness level
 *
 * @see CCamera::SetBrightnessL()
 */
void CMMCameraClientSession::SetBrightnessL(TInt aBrightness)
	{
	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EBrightness, aBrightness));
	}

/**
 * Retrieves the current brightness level
 *
 * @see CCamera::Brigthness()
 */
TInt CMMCameraClientSession::Brightness() const
	{
	TCameraBrightness brightness;
	TCameraBrightnessPckg pckg(brightness);
	iCameraSession.SendRxMessage(ECamGetParameter, EBrightness, pckg);
	brightness = pckg();
	return brightness.iBrightness;
	}

/**
 * Sets the flash level
 *
 * @see CCamera::SetFlashL()
 */
void CMMCameraClientSession::SetFlashL(TFlash aFlash)
	{
	TCameraFlash flash;
	flash.iFlash = aFlash;
	TCameraFlashPckg pckg(flash);

	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EFlash, pckg));
	}

/**
 * Retrieves the current flash level
 *
 * @see CCamera::Flash()
 */
CCamera::TFlash CMMCameraClientSession::Flash() const
	{
	TCameraFlash flash;
	TCameraFlashPckg pckg(flash);
	
	iCameraSession.SendRxMessage(ECamGetParameter, EFlash, pckg);
	flash = pckg();
	return flash.iFlash;
	}

/**
 * Sets the exposure level
 *
 * @see CCamera::SetExposureL()
 */
void CMMCameraClientSession::SetExposureL(TExposure aExposure)
	{
	TCameraExposure exposure;
	exposure.iExposure = aExposure;
	TCameraExposurePckg pckg(exposure);

	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EExposure, pckg));
	}

/**
 * Retrieves the current exposure level
 *
 * @see CCamera::Exposure()
 */
CCamera::TExposure CMMCameraClientSession::Exposure() const
	{
	TCameraExposure exposure;
	TCameraExposurePckg pckg(exposure);
	
	iCameraSession.SendRxMessage(ECamGetParameter, EExposure, pckg);
	exposure = pckg();
	return exposure.iExposure;
	}

/**
 * Sets the white balance level
 *
 * @see CCamera::SetWhiteBalance()
 */
void CMMCameraClientSession::SetWhiteBalanceL(TWhiteBalance aWhiteBalance)
	{
	TCameraWhiteBalance whiteBalance;
	whiteBalance.iWhiteBalance = aWhiteBalance;
	TCameraWhiteBalancePckg pckg(whiteBalance);

	User::LeaveIfError(iCameraSession.SendMessage(ECamSetParameter, EWhiteBalance, pckg));
	}

/**
 * Retrieves the current white balance level
 *
 * @see CCamera::WhiteBalance()
 */
CCamera::TWhiteBalance CMMCameraClientSession::WhiteBalance() const
	{
	TCameraWhiteBalance whiteBalance;
	TCameraWhiteBalancePckg pckg(whiteBalance);
	
	iCameraSession.SendRxMessage(ECamGetParameter, EWhiteBalance, pckg);
	whiteBalance = pckg();
	return whiteBalance.iWhiteBalance;	
	}

/**
 * Starts client direct viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StartViewFinderDirectL()
 */
void CMMCameraClientSession::StartViewFinderDirectL(RWsSession& /*aWs*/, CWsScreenDevice& /*aScreenDevice*/, RWindowBase& /*aWindow*/, TRect& /*aScreenRect*/)
	{
	User::Leave(KErrNotSupported);	
	}

/**
 * Starts client direct viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StartViewFinderDirectL()
 */
void CMMCameraClientSession::StartViewFinderDirectL(RWsSession& /*aWs*/, CWsScreenDevice& /*aScreenDevice*/, RWindowBase& /*aWindow*/, TRect& /*aScreenRect*/, TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Starts client viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StartViewFinderBitmapsL()
 */
void CMMCameraClientSession::StartViewFinderBitmapsL(TSize& /*aSize*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Starts client viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StartViewFinderBitmapsL()
 */
void CMMCameraClientSession::StartViewFinderBitmapsL(TSize& /*aSize*/, TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);	
	}

/**
 * Starts client viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StartViewFinderL()
 */
void CMMCameraClientSession::StartViewFinderL(CCamera::TFormat /*aImageFormat*/, TSize& /*aSize*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Starts client viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StartViewFinderL()
 */
void CMMCameraClientSession::StartViewFinderL(CCamera::TFormat /*aImageFormat*/, TSize& /*aSize*/, TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);	
	}

/**
 * Stops client viewfinder.
 * Not supported in this implementation.
 * 
 * @see CCamera::StopViewFinder()
 */
void CMMCameraClientSession::StopViewFinder()
	{
	return;
	}

/**
 * Queries if viewfinder is active.
 * Not supported in this implementation.
 * 
 * @see CCamera::ViewFinderActive()
 */
TBool CMMCameraClientSession::ViewFinderActive() const
	{
	return EFalse;
	}

/**
 * Turns on/off viewfinder mirrorring.
 * Not supported in this implementation.
 * 
 * @see CCamera::SetViewFinderMirrorL()
 */
void CMMCameraClientSession::SetViewFinderMirrorL(TBool /*aMirror*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Queries whether viewfinder mirrorring is on or off.
 * Not supported in this implementation.
 * 
 * @see CCamera::ViewFinderMirrorL()
 */
TBool CMMCameraClientSession::ViewFinderMirror() const
	{
	return EFalse;
	}

/**
 * Prepares image capture.
 * 
 * @see CCamera::PrepareImageCapture()
 */
void CMMCameraClientSession::PrepareImageCaptureL(CCamera::TFormat /*aImageFormat*/, TInt /*aSizeIndex*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Prepares image capture.
 * 
 * @see CCamera::PrepareImageCapture()
 */
void CMMCameraClientSession::PrepareImageCaptureL(CCamera::TFormat /*aImageFormat*/, TInt /*aSizeIndex*/, const TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Captures image.
 * 
 * @see CCamera::CaptureImage()
 */
void CMMCameraClientSession::CaptureImage()
	{
	return;
	}

/**
 * Cancels any ongoing image capture.
 * 
 * @see CCamera::CaptureImage()
 */
void CMMCameraClientSession::CancelCaptureImage()
	{
	return;
	}

/**
 * Enumerates image capture sizes.
 * 
 * @see CCamera::EnumerateCaptureSizes()
 */
void CMMCameraClientSession::EnumerateCaptureSizes(TSize& /*aSize*/, TInt /*aSizeIndex*/, CCamera::TFormat /*aFormat*/) const
	{
	return;
	}

/**
 * Prepares video capture.
 * 
 * @see CCamera::PrepareVideoCaptureL()
 */
void CMMCameraClientSession::PrepareVideoCaptureL(CCamera::TFormat /*aFormat*/, TInt /*aSizeIndex*/, TInt /*aRateIndex*/, TInt /*aBuffersToUse*/, TInt /*aFramesPerBuffer*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Prepares video capture.
 * 
 * @see CCamera::PrepareVideoCaptureL()
 */
void CMMCameraClientSession::PrepareVideoCaptureL(CCamera::TFormat /*aFormat*/, TInt /*aSizeIndex*/, TInt /*aRateIndex*/, TInt /*aBuffersToUse*/, TInt /*aFramesPerBuffer*/, const TRect& /*aClipRect*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
 * Starts video capture.
 * 
 * @see CCamera::StartVideoCapture()
 */
void CMMCameraClientSession::StartVideoCapture()
	{
	return;
	}

/**
 * Stops video capture.
 * 
 * @see CCamera::StopVideoCapture()
 */
void CMMCameraClientSession::StopVideoCapture()
	{
	return;
	}

/**
 * Queries whether video capture is active.
 * 
 * @see CCamera::VideoCaptureActive()
 */
TBool CMMCameraClientSession::VideoCaptureActive() const
	{
	return EFalse;	
	}

/**
 * Enumerates video frame sizes.
 * 
 * @see CCamera::EnumerateVideoFrameSizes()
 */
void CMMCameraClientSession::EnumerateVideoFrameSizes(TSize& /*aSize*/, TInt /*aSizeIndex*/, CCamera::TFormat /*aFormat*/) const
	{
	return;
	}

/**
 * Enumerates video frame rates.
 * 
 * @see CCamera::EnumerateVideoFrameRates()
 */
void CMMCameraClientSession::EnumerateVideoFrameRates(TReal32& /*aRate*/, TInt /*aRateIndex*/, CCamera::TFormat /*aFormat*/, TInt /*aSizeIndex*/, TExposure /*aExposure*/) const
	{
	return;
	}

/**
 * Retrieve the frame size.
 * 
 * @see CCamera::GetFrameSize()
 */
void CMMCameraClientSession::GetFrameSize(TSize& /*aSize*/) const
	{
	return;	
	}

/**
 * Retrieve the frame rate.
 * 
 * @see CCamera::FrameRate()
 */
TReal32 CMMCameraClientSession::FrameRate() const
	{
	return static_cast<TReal32>(0);
	}

/**
 * Retrieve the number of buffers currently in use.
 * 
 * @see CCamera::BuffersInUse()
 */
TInt CMMCameraClientSession::BuffersInUse() const
	{
	return 0;
	}

/**
 * Retrieve the number of frames per buffer.
 * 
 * @see CCamera::FramesPerBuffer()
 */
TInt CMMCameraClientSession::FramesPerBuffer() const
	{
	return 0;	
	}

/**
 * Set the Jpeg quality.
 * 
 * @see CCamera::SetJpegQuality()
 */
void CMMCameraClientSession::SetJpegQuality(TInt /*aQuality*/)
	{
	return;
	}

/**
 * Retrieve the Jpeg quality.
 * 
 * @see CCamera::JpegQuality()
 */
TInt CMMCameraClientSession::JpegQuality() const
	{
	return 0;	
	}

/**
 * Retrieves a custom interface based on the id passed in by the client.
 * 
 * @see CCamera::CustomInterface()
 */
TAny* CMMCameraClientSession::CustomInterface(TUid aInterface)
	{
	switch (aInterface.iUid)
		{
		// The framework retrieves KECamMCameraV2DirectViewFinderUidValue followed by KECamMCameraBaseV2DirectViewFinderUidValue
		// We take care to initialise iDirectViewFinder in the first case and use it in the subsequent call
		case KECamMCameraV2DirectViewFinderUidValue:
			{
			ASSERT(!iDirectViewFinder);
			TRAPD(err, iDirectViewFinder = CMMDirectViewFinder::NewL(*this));
			if (err != KErrNone)
				{
				return NULL;
				}

			return static_cast<MCameraV2DirectViewFinder*>(iDirectViewFinder);
	        }

		case KECamMCameraBaseV2DirectViewFinderUidValue:
			{
			ASSERT(iDirectViewFinder);
			iVF = static_cast<MCameraViewFinder*>(iDirectViewFinder);
			//delete iDirectViewFinder;
			iDirectViewFinder = NULL;
			return iVF;
	        }

		default:
			{
			return NULL;
			}
		}
	}

//
// From CCameraInfoPlugin
//
CMMCameraClientSessionInfo* CMMCameraClientSessionInfo::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CMMCameraClientSessionInfo;
	}

CMMCameraClientSessionInfo::~CMMCameraClientSessionInfo()
	{
	}

/**
 * Retrieves the number of cameras available.
 * 
 * @see CCameraInfoPlugin::CamerasAvailable()
 */
TInt CMMCameraClientSessionInfo::CamerasAvailable()
	{
	// One-off connection to the server
	RMMCameraSession cameraSession;
	TInt error = cameraSession.Connect();
	if (error != KErrNone)
		{
		return 0;
		}

	TCamerasAvailable info;
	TCamerasAvailablePckg pckg(info);

	error = cameraSession.SendMessage(ECamQueryCamerasAvailable, pckg);
	cameraSession.Close();
	if (error != KErrNone)
		{
		return 0;
		}

	info = pckg();
	return info.iCameraCount;
	}

CMMCameraClientSessionInfo::CMMCameraClientSessionInfo()
	{
	}



void FileDependencyUtil::CheckFileDependencyL()
	{	
	RFs fsSession;
  	RFile file;
    CleanupClosePushL(fsSession);
  	User::LeaveIfError(fsSession.Connect());
    TInt err = file.Open(fsSession, KMMCameraPluginName, EFileRead);
  	file.Close();
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
  	CleanupStack::PopAndDestroy(&fsSession);
	}


//
// CLASS CMMCameraAccessHandler
//

CMMCameraAccessHandler::CMMCameraAccessHandler(CMMCameraClientSession* aPlugin, TInt aPriority): CActive(aPriority), 
																					   iPlugin(aPlugin)
	{
	}

CMMCameraAccessHandler* CMMCameraAccessHandler::NewL(CMMCameraClientSession* aPlugin, TInt aPriority)
	{
	CMMCameraAccessHandler* self = new (ELeave) CMMCameraAccessHandler(aPlugin, aPriority);
	CActiveScheduler::Add(self);
	return self;
	}

CMMCameraAccessHandler::~CMMCameraAccessHandler()
	{
	Cancel();
	}

void CMMCameraAccessHandler::Reserve()
	{
	if(!IsActive())
		{
		// Reserve the camera
		iPlugin->iCameraSession.ReceiveMessage(ECamCameraAccessControl, ECameraReservedNotification, iStatus);
		SetActive();
		}
	else
		{
		// Reserve has been called again before first call has completed so notify the client
		TECAMEvent ecamEvent(KUidECamEventReserveComplete, KErrInUse);
		iPlugin->iCameraObserver2->HandleEvent(ecamEvent);
		}
	}

void CMMCameraAccessHandler::RunL()
	{ 
	TInt ret = iStatus.Int();

	if(!iPlugin->iReserved)
		{
		if(KErrNone == ret)
			{
			iPlugin->iReserved = ETrue;
			if(!IsActive())
				{
				// Continue monitoring for overthrow messages should a higher priority client reserve the same camera index
				iPlugin->iCameraSession.ReceiveMessage(ECamCameraAccessControl, ECameraOverthrowNotification, iStatus);
				SetActive();
				}
			}

		// notify the client that reserve has completed, successfully or otherwise
		TECAMEvent ecamEvent(KUidECamEventReserveComplete, ret);	
		iPlugin->iCameraObserver2->HandleEvent(ecamEvent);
		}
	else
		{
		iPlugin->iReserved = EFalse;

		if (ret == KErrNone)
			{
			// Client has been overthrown by a second client with higher priority
			TECAMEvent ecamEvent(KUidECamEventCameraNoLongerReserved, ret);
			iPlugin->iCameraObserver2->HandleEvent(ecamEvent);		
			}
		}
	}

void CMMCameraAccessHandler::DoCancel()
	{
	if(!iPlugin->iReserved)
		{
		iPlugin->iCameraSession.SendMessage(ECamCameraAccessControl, ECameraCancelReservedNotification);

		// Notify client that ongoing reserve request was cancelled
		TECAMEvent ecamEvent(KUidECamEventReserveComplete, KErrCancel);
		iPlugin->iCameraObserver2->HandleEvent(ecamEvent);
		}
	else
		{
		iPlugin->iCameraSession.SendMessage(ECamCameraAccessControl, ECameraCancelOverthrowNotification);
		iPlugin->iReserved = EFalse;
		}
	}


//
//
// CLASS CMMCameraPowerHandler
//

CMMCameraPowerHandler::CMMCameraPowerHandler(CMMCameraClientSession* aPlugin, TInt aPriority): CActive(aPriority), 
																					 iPlugin(aPlugin)
	{
	}

CMMCameraPowerHandler* CMMCameraPowerHandler::NewL(CMMCameraClientSession* aPlugin, TInt aPriority)
	{
	CMMCameraPowerHandler* self = new (ELeave) CMMCameraPowerHandler(aPlugin, aPriority);
	CActiveScheduler::Add(self);
	return self;
	}

CMMCameraPowerHandler::~CMMCameraPowerHandler()
	{
	Cancel();
	}

void CMMCameraPowerHandler::PowerOn()
	{ 
	if(!IsActive())
		{
		iPlugin->iCameraSession.ReceiveMessage(ECamPowerCamera, ECameraPowerOnNotification, iStatus);
		SetActive();
		}
	else
		{
		// PowerOn has been called again before first call has completed so notify the client
		TECAMEvent ecamEvent(KUidECamEventPowerOnComplete, KErrAlreadyExists);	
		iPlugin->iCameraObserver2->HandleEvent(ecamEvent);	
		}
	}

void CMMCameraPowerHandler::RunL()
	{ 
	TInt ret = iStatus.Int();

	if(!iPlugin->iPoweredUp)
		{
		if (ret == KErrNone)
			{
			iPlugin->iPoweredUp = ETrue;
			}
		}

	TECAMEvent ecamEvent(KUidECamEventPowerOnComplete, ret);	
	iPlugin->iCameraObserver2->HandleEvent(ecamEvent);
	}

void CMMCameraPowerHandler::DoCancel()
	{
	if(!iPlugin->iPoweredUp)
		{
		iPlugin->iCameraSession.SendMessage(ECamPowerCamera, ECameraCancelPowerOnNotification);
		} 
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidMMCameraClientSession, CMMCameraClientSession::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidMMCameraClientSessionInfo, CMMCameraClientSessionInfo::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
